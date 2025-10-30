#include "tuberepository.h"
#include <cmath>

TubeRepository::TubeRepository()
    : db(DatabaseManager::getInstance())
{
}

TubeRepository::~TubeRepository()
{
}

int64_t TubeRepository::saveTube(const Tube& tube, int verId, int64_t previousTubeId)
{
    qDebug() << "TubeRepository::saveTube - Starting tube save operation, verId:" << verId;
    qDebug() << "Previous tube ID:" << previousTubeId;

    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return -1;
    }

    if (!validateTubeData(tube)) {
        setLastError("Tube data validation failed");
        return -1;
    }

    if (!db.beginTransaction()) {
        setLastError("Failed to begin transaction: " + db.getLastError());
        return -1;
    }

    // Создаем новую запись трубки
    int64_t tubeId = insertTubeRecord(verId);
    if (tubeId == -1) {
        db.rollbackTransaction();
        setLastError("Failed to insert tube record: " + lastError);
        return -1;
    }
    qDebug() << "Tube record inserted with id:" << tubeId;

    // Маппинги для связей между версиями
    std::map<int, int64_t> sectionIndexToNewId;  // index -> new section_id
    std::map<int, int64_t> sectionIndexToOldId;  // index -> old section_id
    std::map<PointKey, int64_t> pointIdsMap;
    std::map<int64_t, int64_t> oldPointIdToNewPointId;  // old point_id -> new point_id

    // Если это не первая версия, загружаем старые ID
    if (previousTubeId != -1) {
        if (!loadOldSectionIds(previousTubeId, sectionIndexToOldId)) {
            db.rollbackTransaction();
            setLastError("Failed to load old section IDs");
            return -1;
        }
    }

    // Сохраняем сечения
    if (!saveSectionsWithVersioning(tubeId, tube, verId, previousTubeId,
                                    pointIdsMap, sectionIndexToNewId,
                                    sectionIndexToOldId, oldPointIdToNewPointId)) {
        db.rollbackTransaction();
        setLastError("Failed to save sections: " + lastError);
        return -1;
    }
    qDebug() << "All sections saved successfully. Total points:" << pointIdsMap.size();

    // Сохраняем сегменты
    if (!saveSegmentsWithVersioning(tubeId, tube, verId, previousTubeId,
                                    pointIdsMap, oldPointIdToNewPointId)) {
        db.rollbackTransaction();
        setLastError("Failed to save segments: " + lastError);
        return -1;
    }
    qDebug() << "All segments saved successfully";

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        setLastError("Failed to commit transaction: " + db.getLastError());
        return -1;
    }

    qDebug() << "TubeRepository::saveTube - Tube saved successfully with id:" << tubeId;
    return tubeId;
}

bool TubeRepository::loadOldSectionIds(int64_t tubeId, std::map<int, int64_t>& sectionIndexToId)
{
    QString query = QString(
                        "SELECT id, index FROM section WHERE tube_id = %1 ORDER BY index"
                        ).arg(tubeId);

    QSqlQuery result = db.executeQuery(query);

    while (result.next()) {
        int64_t sectionId = result.value(0).toLongLong();
        int index = result.value(1).toInt();
        sectionIndexToId[index] = sectionId;
    }

    return true;
}

bool TubeRepository::saveSectionsWithVersioning(
    int64_t tubeId,
    const Tube& tube,
    int verId,
    int64_t previousTubeId,
    std::map<PointKey, int64_t>& pointIdsMap,
    std::map<int, int64_t>& sectionIndexToNewId,
    std::map<int, int64_t>& sectionIndexToOldId,
    std::map<int64_t, int64_t>& oldPointIdToNewPointId)
{
    qDebug() << "Saving" << tube.getSectionCount() << "sections with versioning";

    for (size_t i = 0; i < tube.getSectionCount(); ++i) {
        const Section& section = tube.getSection(static_cast<int>(i + 1));
        int sectionIndex = section.sectionIndex;

        // Получаем старый section_id если есть
        int64_t oldSectionId = -1;
        if (previousTubeId != -1 && sectionIndexToOldId.find(sectionIndex) != sectionIndexToOldId.end()) {
            oldSectionId = sectionIndexToOldId[sectionIndex];
        }

        // Вставляем новое сечение
        int64_t newSectionId = insertSectionWithPast(tubeId, section, verId, sectionIndex, oldSectionId);
        if (newSectionId == -1) {
            setLastError(QString("Failed to insert section %1: %2")
                             .arg(sectionIndex).arg(lastError));
            return false;
        }

        sectionIndexToNewId[sectionIndex] = newSectionId;

        // Обновляем старое сечение, добавляя ссылку на будущее
        if (oldSectionId != -1) {
            QString updateQuery = QString(
                                      "UPDATE section SET future_section_id = %1 WHERE id = %2"
                                      ).arg(newSectionId).arg(oldSectionId);
            db.executeQuery(updateQuery);
        }

        qDebug() << "Section" << sectionIndex << "inserted with id:" << newSectionId
                 << "(old id:" << oldSectionId << ")";

        // Сохраняем точки с версионностью
        if (!savePointsWithVersioning(newSectionId, oldSectionId, section, verId,
                                      pointIdsMap, oldPointIdToNewPointId)) {
            setLastError(QString("Failed to save points for section %1: %2")
                             .arg(sectionIndex).arg(lastError));
            return false;
        }
    }

    return true;
}

int64_t TubeRepository::insertSectionWithPast(
    int64_t tubeId,
    const Section& section,
    int verId,
    int index,
    int64_t pastSectionId)
{
    Point3D center = section.getCenter();

    QString query;
    if (pastSectionId == -1) {
        query = QString(
                    "INSERT INTO section (tube_id, ver_id, index, x_cen, y_cen, z_cen) "
                    "VALUES (%1, %2, %3, %4, %5, %6) "
                    "RETURNING id"
                    ).arg(tubeId).arg(verId).arg(index).arg(center.x).arg(center.y).arg(center.z);
    } else {
        query = QString(
                    "INSERT INTO section (tube_id, ver_id, index, x_cen, y_cen, z_cen, past_section_id) "
                    "VALUES (%1, %2, %3, %4, %5, %6, %7) "
                    "RETURNING id"
                    ).arg(tubeId).arg(verId).arg(index).arg(center.x).arg(center.y).arg(center.z).arg(pastSectionId);
    }

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get section id after insert");
    return -1;
}

bool TubeRepository::savePointsWithVersioning(
    int64_t newSectionId,
    int64_t oldSectionId,
    const Section& section,
    int verId,
    std::map<PointKey, int64_t>& pointIdsMap,
    std::map<int64_t, int64_t>& oldPointIdToNewPointId)
{
    qDebug() << "Saving" << section.getPointCount() << "points for section id:" << newSectionId;

    // Загружаем старые точки если есть
    std::map<int, int64_t> oldPointIndices;  // index_in_section -> point_id
    if (oldSectionId != -1) {
        QString query = QString(
                            "SELECT id, index_in_section FROM point "
                            "WHERE section_id = %1 AND index_in_section IS NOT NULL "
                            "ORDER BY index_in_section"
                            ).arg(oldSectionId);

        QSqlQuery result = db.executeQuery(query);
        while (result.next()) {
            int64_t pointId = result.value(0).toLongLong();
            int index = result.value(1).toInt();
            oldPointIndices[index] = pointId;
        }
    }

    for (size_t i = 0; i < section.getPointCount(); ++i) {
        const Point3D& point = section.getPoint(static_cast<int>(i + 1));
        int indexInSection = static_cast<int>(i + 1);

        // Получаем старый point_id если есть
        int64_t oldPointId = -1;
        if (oldPointIndices.find(indexInSection) != oldPointIndices.end()) {
            oldPointId = oldPointIndices[indexInSection];
        }

        // Вставляем новую точку
        int64_t newPointId = insertPointWithPast(newSectionId, point, verId, indexInSection, oldPointId);
        if (newPointId == -1) {
            setLastError(QString("Failed to insert point %1 in section: %2")
                             .arg(indexInSection).arg(lastError));
            return false;
        }

        // Обновляем старую точку
        if (oldPointId != -1) {
            QString updateQuery = QString(
                                      "UPDATE point SET future_point_id = %1 WHERE id = %2"
                                      ).arg(newPointId).arg(oldPointId);
            db.executeQuery(updateQuery);

            oldPointIdToNewPointId[oldPointId] = newPointId;
        }

        PointKey key = makePointKey(point);
        pointIdsMap[key] = newPointId;

        qDebug() << "  Point" << indexInSection << "inserted with id:" << newPointId
                 << "(old id:" << oldPointId << ")"
                 << "coords: (" << point.x << "," << point.y << "," << point.z << ")";
    }

    return true;
}

int64_t TubeRepository::insertPointWithPast(
    int64_t sectionId,
    const Point3D& point,
    int verId,
    int indexInSection,
    int64_t pastPointId)
{
    QString query;

    if (indexInSection == -1) {
        // Интерполированная точка
        if (pastPointId == -1) {
            query = QString(
                        "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z) "
                        "VALUES (%1, %2, NULL, %3, %4, %5) "
                        "RETURNING id"
                        ).arg(sectionId).arg(verId).arg(point.x).arg(point.y).arg(point.z);
        } else {
            query = QString(
                        "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z, past_point_id) "
                        "VALUES (%1, %2, NULL, %3, %4, %5, %6) "
                        "RETURNING id"
                        ).arg(sectionId).arg(verId).arg(point.x).arg(point.y).arg(point.z).arg(pastPointId);
        }
    } else {
        // Обычная точка сечения
        if (pastPointId == -1) {
            query = QString(
                        "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z) "
                        "VALUES (%1, %2, %3, %4, %5, %6) "
                        "RETURNING id"
                        ).arg(sectionId).arg(verId).arg(indexInSection).arg(point.x).arg(point.y).arg(point.z);
        } else {
            query = QString(
                        "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z, past_point_id) "
                        "VALUES (%1, %2, %3, %4, %5, %6, %7) "
                        "RETURNING id"
                        ).arg(sectionId).arg(verId).arg(indexInSection)
                        .arg(point.x).arg(point.y).arg(point.z).arg(pastPointId);
        }
    }

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get point id after insert");
    return -1;
}

bool TubeRepository::saveSegmentsWithVersioning(
    int64_t tubeId,
    const Tube& tube,
    int verId,
    int64_t previousTubeId,
    const std::map<PointKey, int64_t>& pointIdsMap,
    const std::map<int64_t, int64_t>& oldPointIdToNewPointId)
{
    qDebug() << "Saving" << tube.getSegmentCount() << "segments with versioning";

    // Загружаем старые сегменты если есть
    std::map<int, int64_t> oldSegmentIndices;  // index -> segment_id
    if (previousTubeId != -1) {
        QString query = QString(
                            "SELECT id, index FROM segment WHERE tube_id = %1 ORDER BY index"
                            ).arg(previousTubeId);

        QSqlQuery result = db.executeQuery(query);
        while (result.next()) {
            int64_t segmentId = result.value(0).toLongLong();
            int index = result.value(1).toInt();
            oldSegmentIndices[index] = segmentId;
        }
    }

    for (size_t i = 0; i < tube.getSegmentCount(); ++i) {
        const Segment& segment = tube.getSegment(static_cast<int>(i + 1));
        int segmentIndex = segment.segmentIndex;

        // Получаем старый segment_id если есть
        int64_t oldSegmentId = -1;
        if (oldSegmentIndices.find(segmentIndex) != oldSegmentIndices.end()) {
            oldSegmentId = oldSegmentIndices[segmentIndex];
        }

        // Вставляем новый сегмент
        int64_t newSegmentId = insertSegmentWithPast(tubeId, segment, verId, segmentIndex, oldSegmentId);
        if (newSegmentId == -1) {
            setLastError(QString("Failed to insert segment %1: %2")
                             .arg(segmentIndex).arg(lastError));
            return false;
        }

        // Обновляем старый сегмент
        if (oldSegmentId != -1) {
            QString updateQuery = QString(
                                      "UPDATE segment SET future_segment_id = %1 WHERE id = %2"
                                      ).arg(newSegmentId).arg(oldSegmentId);
            db.executeQuery(updateQuery);
        }

        qDebug() << "Segment" << segmentIndex << "inserted with id:" << newSegmentId
                 << "(old id:" << oldSegmentId << ")";

        // Сохраняем рёбра
        if (!saveEdgesWithVersioning(newSegmentId, oldSegmentId, segment, verId,
                                     pointIdsMap, oldPointIdToNewPointId)) {
            setLastError(QString("Failed to save edges for segment %1: %2")
                             .arg(segmentIndex).arg(lastError));
            return false;
        }
    }

    return true;
}

int64_t TubeRepository::insertSegmentWithPast(
    int64_t tubeId,
    const Segment& segment,
    int verId,
    int index,
    int64_t pastSegmentId)
{
    QString query;
    if (pastSegmentId == -1) {
        query = QString(
                    "INSERT INTO segment (tube_id, ver_id, index) "
                    "VALUES (%1, %2, %3) "
                    "RETURNING id"
                    ).arg(tubeId).arg(verId).arg(index);
    } else {
        query = QString(
                    "INSERT INTO segment (tube_id, ver_id, index, past_segment_id) "
                    "VALUES (%1, %2, %3, %4) "
                    "RETURNING id"
                    ).arg(tubeId).arg(verId).arg(index).arg(pastSegmentId);
    }

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get segment id after insert");
    return -1;
}

bool TubeRepository::saveEdgesWithVersioning(
    int64_t newSegmentId,
    int64_t oldSegmentId,
    const Segment& segment,
    int verId,
    const std::map<PointKey, int64_t>& pointIdsMap,
    const std::map<int64_t, int64_t>& oldPointIdToNewPointId)
{
    qDebug() << "Saving" << segment.getConnectingEdgeCount() << "edges for segment id:" << newSegmentId;

    // Загружаем старые рёбра если есть
    std::map<int, std::pair<int64_t, std::pair<int64_t, int64_t>>> oldEdges;
    // index -> (edge_id, (start_point_id, end_point_id))

    if (oldSegmentId != -1) {
        QString query = QString(
                            "SELECT id, index, start_point_id, end_point_id FROM edge "
                            "WHERE segment_id = %1 ORDER BY index"
                            ).arg(oldSegmentId);

        QSqlQuery result = db.executeQuery(query);
        while (result.next()) {
            int64_t edgeId = result.value(0).toLongLong();
            int index = result.value(1).toInt();
            int64_t startPointId = result.value(2).toLongLong();
            int64_t endPointId = result.value(3).toLongLong();
            oldEdges[index] = std::make_pair(edgeId, std::make_pair(startPointId, endPointId));
        }
    }

    for (size_t i = 0; i < segment.getConnectingEdgeCount(); ++i) {
        const Edge& edge = segment.getConnectingEdge(static_cast<int>(i + 1));
        int edgeIndex = edge.getIndex();

        // Находим ID точек для нового ребра
        int64_t startPointId = findPointId(edge.getStartPoint(), pointIdsMap);
        int64_t endPointId = findPointId(edge.getEndPoint(), pointIdsMap);

        // Если точки не найдены, это ошибка (все точки должны быть уже созданы)
        if (startPointId == -1 || endPointId == -1) {
            qDebug() << "ERROR: Edge points not found in pointIdsMap for edge" << edgeIndex;
            return false;
        }

        // Получаем старое ребро если есть
        int64_t oldEdgeId = -1;
        if (oldEdges.find(edgeIndex) != oldEdges.end()) {
            oldEdgeId = oldEdges[edgeIndex].first;
        }

        // Вставляем новое ребро
        int64_t newEdgeId = insertEdgeWithPast(newSegmentId, edge, verId, edgeIndex,
                                               startPointId, endPointId, oldEdgeId);
        if (newEdgeId == -1) {
            setLastError(QString("Failed to insert edge %1: %2")
                             .arg(edgeIndex).arg(lastError));
            return false;
        }

        // Обновляем старое ребро
        if (oldEdgeId != -1) {
            QString updateQuery = QString(
                                      "UPDATE edge SET future_edge_id = %1 WHERE id = %2"
                                      ).arg(newEdgeId).arg(oldEdgeId);
            db.executeQuery(updateQuery);
        }

        qDebug() << "  Edge" << edgeIndex << "inserted with id:" << newEdgeId
                 << "(old id:" << oldEdgeId << ")"
                 << "(start_point_id:" << startPointId << ", end_point_id:" << endPointId << ")";
    }

    return true;
}

int64_t TubeRepository::insertEdgeWithPast(
    int64_t segmentId,
    const Edge& edge,
    int verId,
    int index,
    int64_t startPointId,
    int64_t endPointId,
    int64_t pastEdgeId)
{
    QString query;
    if (pastEdgeId == -1) {
        query = QString(
                    "INSERT INTO edge (segment_id, ver_id, index, start_point_id, end_point_id, beg_sz) "
                    "VALUES (%1, %2, %3, %4, %5, 100) "
                    "RETURNING id"
                    ).arg(segmentId).arg(verId).arg(index).arg(startPointId).arg(endPointId);
    } else {
        query = QString(
                    "INSERT INTO edge (segment_id, ver_id, index, start_point_id, end_point_id, beg_sz, past_edge_id) "
                    "VALUES (%1, %2, %3, %4, %5, 100, %6) "
                    "RETURNING id"
                    ).arg(segmentId).arg(verId).arg(index).arg(startPointId).arg(endPointId).arg(pastEdgeId);
    }

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get edge id after insert");
    return -1;
}

int64_t TubeRepository::insertTubeRecord(int verId)
{
    QString query = QString(
                        "INSERT INTO tube (ver_id, len) "
                        "VALUES (%1, 0) "
                        "RETURNING id"
                        ).arg(verId);

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get tube id after insert");
    return -1;
}

bool TubeRepository::saveSections(int64_t tubeId, const Tube& tube, int verId,
                                  std::map<PointKey, int64_t>& pointIdsMap)
{
    qDebug() << "Saving" << tube.getSectionCount() << "sections";

    for (size_t i = 0; i < tube.getSectionCount(); ++i) {
        const Section& section = tube.getSection(static_cast<int>(i + 1));

        int64_t sectionId = insertSection(tubeId, section, verId, section.sectionIndex);
        if (sectionId == -1) {
            setLastError(QString("Failed to insert section %1: %2")
                             .arg(section.sectionIndex).arg(lastError));
            return false;
        }

        qDebug() << "Section" << section.sectionIndex << "inserted with id:" << sectionId;

        if (!savePoints(sectionId, section, verId, pointIdsMap)) {
            setLastError(QString("Failed to save points for section %1: %2")
                             .arg(section.sectionIndex).arg(lastError));
            return false;
        }
    }

    return true;
}

int64_t TubeRepository::insertSection(int64_t tubeId, const Section& section,
                                      int verId, int index)
{
    Point3D center = section.getCenter();

    QString query = QString(
                        "INSERT INTO section (tube_id, ver_id, index, x_cen, y_cen, z_cen) "
                        "VALUES (%1, %2, %3, %4, %5, %6) "
                        "RETURNING id"
                        ).arg(tubeId)
                        .arg(verId)
                        .arg(index)
                        .arg(center.x)
                        .arg(center.y)
                        .arg(center.z);

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get section id after insert");
    return -1;
}

bool TubeRepository::savePoints(int64_t sectionId, const Section& section, int verId,
                                std::map<PointKey, int64_t>& pointIdsMap)
{
    qDebug() << "Saving" << section.getPointCount() << "points for section id:" << sectionId;

    for (size_t i = 0; i < section.getPointCount(); ++i) {
        const Point3D& point = section.getPoint(static_cast<int>(i + 1));
        int indexInSection = static_cast<int>(i + 1);

        int64_t pointId = insertPoint(sectionId, point, verId, indexInSection);
        if (pointId == -1) {
            setLastError(QString("Failed to insert point %1 in section: %2")
                             .arg(indexInSection).arg(lastError));
            return false;
        }

        PointKey key = makePointKey(point);
        pointIdsMap[key] = pointId;

        qDebug() << "  Point" << indexInSection << "inserted with id:" << pointId
                 << "coords: (" << point.x << "," << point.y << "," << point.z << ")"
                 << "pointIndex:" << point.pointIndex;
    }

    return true;
}

int64_t TubeRepository::insertPoint(int64_t sectionId, const Point3D& point,
                                    int verId, int indexInSection)
{
    QString query;

    if (indexInSection == -1) {
        query = QString(
                    "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z) "
                    "VALUES (%1, %2, NULL, %3, %4, %5) "
                    "RETURNING id"
                    ).arg(sectionId)
                    .arg(verId)
                    .arg(point.x)
                    .arg(point.y)
                    .arg(point.z);
    } else {
        query = QString(
                    "INSERT INTO point (section_id, ver_id, index_in_section, x, y, z) "
                    "VALUES (%1, %2, %3, %4, %5, %6) "
                    "RETURNING id"
                    ).arg(sectionId)
                    .arg(verId)
                    .arg(indexInSection)
                    .arg(point.x)
                    .arg(point.y)
                    .arg(point.z);
    }

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get point id after insert");
    return -1;
}

bool TubeRepository::saveSegments(int64_t tubeId, const Tube& tube, int verId,
                                  const std::map<PointKey, int64_t>& pointIdsMap)
{
    qDebug() << "Saving" << tube.getSegmentCount() << "segments";

    for (size_t i = 0; i < tube.getSegmentCount(); ++i) {
        const Segment& segment = tube.getSegment(static_cast<int>(i + 1));

        int64_t segmentId = insertSegment(tubeId, segment, verId, segment.segmentIndex);
        if (segmentId == -1) {
            setLastError(QString("Failed to insert segment %1: %2")
                             .arg(segment.segmentIndex).arg(lastError));
            return false;
        }

        qDebug() << "Segment" << segment.segmentIndex << "inserted with id:" << segmentId;

        if (!saveEdges(segmentId, segment, verId, pointIdsMap)) {
            setLastError(QString("Failed to save edges for segment %1: %2")
                             .arg(segment.segmentIndex).arg(lastError));
            return false;
        }
    }

    return true;
}

int64_t TubeRepository::insertSegment(int64_t tubeId, const Segment& segment,
                                      int verId, int index)
{
    QString query = QString(
                        "INSERT INTO segment (tube_id, ver_id, index) "
                        "VALUES (%1, %2, %3) "
                        "RETURNING id"
                        ).arg(tubeId)
                        .arg(verId)
                        .arg(index);

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get segment id after insert");
    return -1;
}

bool TubeRepository::saveEdges(int64_t segmentId, const Segment& segment, int verId,
                               const std::map<PointKey, int64_t>& pointIdsMap)
{
    qDebug() << "Saving" << segment.getConnectingEdgeCount() << "edges for segment id:" << segmentId;

    std::map<int, int64_t> sectionIndexToIdCache;

    for (size_t i = 0; i < segment.getConnectingEdgeCount(); ++i) {
        const Edge& edge = segment.getConnectingEdge(static_cast<int>(i + 1));

        qDebug() << "  Processing edge" << edge.getIndex()
                 << "start_section_idx:" << edge.getStartSectionIndex()
                 << "start_point_idx:" << edge.getStartPointIndex()
                 << "end_section_idx:" << edge.getEndSectionIndex()
                 << "end_point_idx:" << edge.getEndPointIndex();

        int64_t startPointId = findPointId(edge.getStartPoint(), pointIdsMap);
        int64_t endPointId = findPointId(edge.getEndPoint(), pointIdsMap);

        if (startPointId == -1) {
            qDebug() << "    Start point not found in map, creating interpolated point";

            int sectionIdx = edge.getStartSectionIndex();
            if (sectionIdx < 1) {
                setLastError(QString("Invalid start section index %1 for edge %2")
                                 .arg(sectionIdx).arg(edge.getIndex()));
                return false;
            }

            int64_t sectionId;
            if (sectionIndexToIdCache.find(sectionIdx) != sectionIndexToIdCache.end()) {
                sectionId = sectionIndexToIdCache[sectionIdx];
            } else {
                QString findSectionQuery = QString(
                                               "SELECT id FROM section WHERE tube_id = "
                                               "(SELECT tube_id FROM segment WHERE id = %1) "
                                               "AND index = %2"
                                               ).arg(segmentId).arg(sectionIdx);

                QSqlQuery sectionResult = db.executeQuery(findSectionQuery);
                if (!sectionResult.next()) {
                    setLastError(QString("Failed to find section %1 for interpolated start point")
                                     .arg(sectionIdx));
                    return false;
                }

                sectionId = sectionResult.value(0).toLongLong();
                sectionIndexToIdCache[sectionIdx] = sectionId;
            }

            startPointId = insertPoint(sectionId, edge.getStartPoint(), verId, -1);

            if (startPointId == -1) {
                setLastError("Failed to insert interpolated start point");
                return false;
            }

            qDebug() << "    Interpolated start point inserted with id:" << startPointId;
        }

        if (endPointId == -1) {
            qDebug() << "    End point not found in map, creating interpolated point";

            int sectionIdx = edge.getEndSectionIndex();
            if (sectionIdx < 1) {
                setLastError(QString("Invalid end section index %1 for edge %2")
                                 .arg(sectionIdx).arg(edge.getIndex()));
                return false;
            }

            int64_t sectionId;
            if (sectionIndexToIdCache.find(sectionIdx) != sectionIndexToIdCache.end()) {
                sectionId = sectionIndexToIdCache[sectionIdx];
            } else {
                QString findSectionQuery = QString(
                                               "SELECT id FROM section WHERE tube_id = "
                                               "(SELECT tube_id FROM segment WHERE id = %1) "
                                               "AND index = %2"
                                               ).arg(segmentId).arg(sectionIdx);

                QSqlQuery sectionResult = db.executeQuery(findSectionQuery);
                if (!sectionResult.next()) {
                    setLastError(QString("Failed to find section %1 for interpolated end point")
                                     .arg(sectionIdx));
                    return false;
                }

                sectionId = sectionResult.value(0).toLongLong();
                sectionIndexToIdCache[sectionIdx] = sectionId;
            }

            endPointId = insertPoint(sectionId, edge.getEndPoint(), verId, -1);

            if (endPointId == -1) {
                setLastError("Failed to insert interpolated end point");
                return false;
            }

            qDebug() << "    Interpolated end point inserted with id:" << endPointId;
        }

        int64_t edgeId = insertEdge(segmentId, edge, verId, edge.getIndex(),
                                    startPointId, endPointId);
        if (edgeId == -1) {
            setLastError(QString("Failed to insert edge %1: %2")
                             .arg(edge.getIndex()).arg(lastError));
            return false;
        }

        qDebug() << "  Edge" << edge.getIndex() << "inserted with id:" << edgeId
                 << "(start_point_id:" << startPointId << ", end_point_id:" << endPointId << ")";
    }

    return true;
}

int64_t TubeRepository::insertEdge(int64_t segmentId, const Edge& edge, int verId,
                                   int index, int64_t startPointId, int64_t endPointId)
{

    QString query = QString(
                        "INSERT INTO edge (segment_id, ver_id, index, start_point_id, end_point_id) "
                        "VALUES (%1, %2, %3, %4, %5) "
                        "RETURNING id"
                        ).arg(segmentId)
                        .arg(verId)
                        .arg(index)
                        .arg(startPointId)
                        .arg(endPointId);

    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        return result.value(0).toLongLong();
    }

    setLastError("Failed to get edge id after insert");
    return -1;
}

int64_t TubeRepository::findPointId(const Point3D& point,
                                    const std::map<PointKey, int64_t>& pointIdsMap) const
{
    PointKey key = makePointKey(point);

    auto it = pointIdsMap.find(key);
    if (it != pointIdsMap.end()) {
        return it->second;
    }


    qDebug() << "  Point not found in map: (" << point.x << "," << point.y << "," << point.z << ")";
    return -1;
}

bool TubeRepository::validateTubeData(const Tube& tube) const
{
    if (tube.getSectionCount() < 2) {
        qDebug() << "Validation failed: tube must have at least 2 sections";
        return false;
    }

    for (size_t i = 0; i < tube.getSectionCount(); ++i) {
        const Section& section = tube.getSection(static_cast<int>(i + 1));

        if (section.getPointCount() < 3) {
            qDebug() << "Validation failed: section" << i + 1 << "has less than 3 points";
            return false;
        }

        if (!section.isValid()) {
            qDebug() << "Validation failed: section" << i + 1 << "is invalid";
            return false;
        }
    }

    if (tube.getSegmentCount() < 1) {
        qDebug() << "Validation failed: tube must have at least 1 segment";
        return false;
    }

    for (size_t i = 0; i < tube.getSegmentCount(); ++i) {
        const Segment& segment = tube.getSegment(static_cast<int>(i + 1));

        if (segment.getConnectingEdgeCount() < 1) {
            qDebug() << "Validation failed: segment" << i + 1 << "has no connecting edges";
            return false;
        }

        if (!segment.isValid()) {
            qDebug() << "Validation failed: segment" << i + 1 << "is invalid";
            return false;
        }
    }

    qDebug() << "Tube data validation passed";
    return true;
}

QString TubeRepository::getLastError() const
{
    return lastError;
}

void TubeRepository::setLastError(const QString& error)
{
    lastError = error;
    qDebug() << "TubeRepository error:" << error;
}

TubeRepository::PointKey TubeRepository::makePointKey(const Point3D& point) const
{

    const float epsilon = 0.001f;
    float roundedX = std::round(point.x / epsilon) * epsilon;
    float roundedY = std::round(point.y / epsilon) * epsilon;
    float roundedZ = std::round(point.z / epsilon) * epsilon;

    return std::make_tuple(roundedX, roundedY, roundedZ);
}

bool TubeRepository::linkTubeVersions(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "TubeRepository::linkTubeVersions - Linking tube versions";
    qDebug() << "  Old tube ID:" << oldTubeId;
    qDebug() << "  New tube ID:" << newTubeId;

    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return false;
    }

    if (!db.beginTransaction()) {
        setLastError("Failed to begin transaction: " + db.getLastError());
        return false;
    }


    QString updateOldQuery = QString(
                                 "UPDATE tube SET future_tube_id = %1 WHERE id = %2"
                                 ).arg(newTubeId).arg(oldTubeId);

    QSqlQuery result1 = db.executeQuery(updateOldQuery);
    if (!result1.isActive()) {
        db.rollbackTransaction();
        setLastError("Failed to update future_tube_id: " + db.getLastError());
        return false;
    }


    QString updateNewQuery = QString(
                                 "UPDATE tube SET past_tube_id = %1 WHERE id = %2"
                                 ).arg(oldTubeId).arg(newTubeId);

    QSqlQuery result2 = db.executeQuery(updateNewQuery);
    if (!result2.isActive()) {
        db.rollbackTransaction();
        setLastError("Failed to update past_tube_id: " + db.getLastError());
        return false;
    }

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        setLastError("Failed to commit transaction: " + db.getLastError());
        return false;
    }

    qDebug() << "Tube versions linked successfully";
    return true;
}



int64_t TubeRepository::getPastTubeId(int64_t tubeId)
{
    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return -1;
    }

    QString query = QString("SELECT past_tube_id FROM tube WHERE id = %1").arg(tubeId);
    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        QVariant pastId = result.value(0);
        if (pastId.isNull()) {
            return -1;
        }
        return pastId.toLongLong();
    }

    setLastError("Failed to get past_tube_id");
    return -1;
}

int64_t TubeRepository::getFutureTubeId(int64_t tubeId)
{
    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return -1;
    }

    QString query = QString("SELECT future_tube_id FROM tube WHERE id = %1").arg(tubeId);
    QSqlQuery result = db.executeQuery(query);

    if (result.next()) {
        QVariant futureId = result.value(0);
        if (futureId.isNull()) {
            return -1;
        }
        return futureId.toLongLong();
    }

    setLastError("Failed to get future_tube_id");
    return -1;
}

bool TubeRepository::loadTubeById(int64_t tubeId, Tube& tube)
{
    qDebug() << "TubeRepository::loadTubeById - Loading tube with id:" << tubeId;

    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return false;
    }

    tube.clear();

    // ---- Проверим, что трубка существует
    {
        QString tubeQuery = QString("SELECT ver_id FROM tube WHERE id = %1").arg(tubeId);
        QSqlQuery tubeResult = db.executeQuery(tubeQuery);
        if (!tubeResult.next()) {
            setLastError(QString("Tube with id %1 not found").arg(tubeId));
            return false;
        }
        int verId = tubeResult.value(0).toInt();
        qDebug() << "Loading tube with ver_id:" << verId;
    }

    // ---- 1) Секциям: читаем сами секции и их контурные точки (index_in_section NOT NULL)
    {
        QString sectionsQuery =
            QString("SELECT id, index, x_cen, y_cen, z_cen "
                    "FROM section WHERE tube_id = %1 ORDER BY index").arg(tubeId);
        QSqlQuery sectionsResult = db.executeQuery(sectionsQuery);

        while (sectionsResult.next()) {
            int64_t sectionId   = sectionsResult.value(0).toLongLong();
            int     sectionIdx  = sectionsResult.value(1).toInt();
            // float x_cen = sectionsResult.value(2).toFloat();
            // float y_cen = sectionsResult.value(3).toFloat();
            // float z_cen = sectionsResult.value(4).toFloat();

            Section section;

            QString pointsQuery =
                QString("SELECT x, y, z, index_in_section "
                        "FROM point "
                        "WHERE section_id = %1 AND index_in_section IS NOT NULL "
                        "ORDER BY index_in_section").arg(sectionId);
            QSqlQuery pointsResult = db.executeQuery(pointsQuery);

            while (pointsResult.next()) {
                Point3D p;
                p.x = pointsResult.value(0).toFloat();
                p.y = pointsResult.value(1).toFloat();
                p.z = pointsResult.value(2).toFloat();
                section.addPoint(p);
            }

            if (tube.addSection(section) == -1) {
                setLastError(QString("Failed to add section %1 to Tube").arg(sectionIdx));
                return false;
            }

            qDebug() << "Loaded section" << sectionIdx
                     << "with" << section.getPointCount() << "points";
        }
    }

    if (tube.getSectionCount() == 0) {
        setLastError("No sections found for the tube");
        return false;
    }

    // ---- 2) Сегменты и рёбра: читаем как сохранены, НИЧЕГО не перестраиваем
    {
        QString segQ =
            QString("SELECT id, index FROM segment "
                    "WHERE tube_id = %1 ORDER BY index").arg(tubeId);
        QSqlQuery segRes = db.executeQuery(segQ);

        while (segRes.next()) {
            int64_t segmentId = segRes.value(0).toLongLong();
            int     segIndex  = segRes.value(1).toInt();

            // Определяем (startSectionIndex, endSectionIndex) по рёбрам сегмента:
            std::set<int> sectionIndicesInSegment;

            // набор секций по стартовым точкам
            {
                QString q =
                    QString("SELECT DISTINCT s.index "
                            "FROM edge e "
                            "JOIN point p ON p.id = e.start_point_id "
                            "JOIN section s ON s.id = p.section_id "
                            "WHERE e.segment_id = %1").arg(segmentId);
                QSqlQuery r = db.executeQuery(q);
                while (r.next()) sectionIndicesInSegment.insert(r.value(0).toInt());
            }
            // набор секций по конечным точкам
            {
                QString q =
                    QString("SELECT DISTINCT s.index "
                            "FROM edge e "
                            "JOIN point p ON p.id = e.end_point_id "
                            "JOIN section s ON s.id = p.section_id "
                            "WHERE e.segment_id = %1").arg(segmentId);
                QSqlQuery r = db.executeQuery(q);
                while (r.next()) sectionIndicesInSegment.insert(r.value(0).toInt());
            }

            if (sectionIndicesInSegment.size() != 2) {
                setLastError(QString("Segment %1 has %2 distinct sections (expected 2)")
                                 .arg(segIndex).arg(sectionIndicesInSegment.size()));
                return false;
            }

            auto it = sectionIndicesInSegment.begin();
            int startSectionIndex = *it; ++it;
            int endSectionIndex   = *it;

            Segment seg(segIndex, startSectionIndex, endSectionIndex);

            // Читаем рёбра по порядку индекса
            QString edgeQ =
                QString("SELECT id, index, start_point_id, end_point_id "
                        "FROM edge WHERE segment_id = %1 ORDER BY index").arg(segmentId);
            QSqlQuery edgeRes = db.executeQuery(edgeQ);

            while (edgeRes.next()) {
                int64_t edgeId = edgeRes.value(0).toLongLong();
                int     eIndex = edgeRes.value(1).toInt();
                int64_t spId   = edgeRes.value(2).toLongLong();
                int64_t epId   = edgeRes.value(3).toLongLong();

                // Стартовая точка ребра
                QString spQ =
                    QString("SELECT p.x, p.y, p.z, p.index_in_section, s.index "
                            "FROM point p JOIN section s ON s.id = p.section_id "
                            "WHERE p.id = %1").arg(spId);
                QSqlQuery sp = db.executeQuery(spQ);
                if (!sp.next()) {
                    setLastError(QString("Start point %1 not found for edge %2").arg(spId).arg(edgeId));
                    return false;
                }
                Point3D spPoint(sp.value(0).toFloat(),
                                sp.value(1).toFloat(),
                                sp.value(2).toFloat());
                bool spFromSection = !sp.value(3).isNull();
                int  spIdxInSec    = spFromSection ? sp.value(3).toInt() : -1;
                int  spSecIndex    = sp.value(4).toInt();

                // Конечная точка ребра
                QString epQ =
                    QString("SELECT p.x, p.y, p.z, p.index_in_section, s.index "
                            "FROM point p JOIN section s ON s.id = p.section_id "
                            "WHERE p.id = %1").arg(epId);
                QSqlQuery ep = db.executeQuery(epQ);
                if (!ep.next()) {
                    setLastError(QString("End point %1 not found for edge %2").arg(epId).arg(edgeId));
                    return false;
                }
                Point3D epPoint(ep.value(0).toFloat(),
                                ep.value(1).toFloat(),
                                ep.value(2).toFloat());
                bool epFromSection = !ep.value(3).isNull();
                int  epIdxInSec    = epFromSection ? ep.value(3).toInt() : -1;
                int  epSecIndex    = ep.value(4).toInt();

                // Собираем Edge по текущему API
                Edge e;
                e.setIndex(eIndex);
                e.setStartPoint(spPoint);
                e.setEndPoint(epPoint);
                e.setSectionIndices(spSecIndex, epSecIndex);     // секции
                e.setPointIndices(spIdxInSec, epIdxInSec);       // индексы точек в секциях (или -1 для интерп.)

                seg.addConnectingEdge(e);
            }

            if (tube.addSegment(seg) == -1) {
                setLastError(QString("Failed to add segment %1 to Tube").arg(segIndex));
                return false;
            }
        }
    }

    // ВАЖНО: не вызывать tube.buildAllSegments() — у нас уже 1:1 топология из БД. :contentReference[oaicite:1]{index=1}

    qDebug() << "Tube loaded successfully with"
             << tube.getSectionCount() << "sections and"
             << tube.getSegmentCount() << "segments";
    return true;
}



bool TubeRepository::deleteFutureVersions(int64_t fromTubeId)
{
    qDebug() << "TubeRepository::deleteFutureVersions - Deleting versions after tube_id:" << fromTubeId;

    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return false;
    }

    if (!db.beginTransaction()) {
        setLastError("Failed to begin transaction: " + db.getLastError());
        return false;
    }


    std::vector<int64_t> tubeIdsToDelete;
    int64_t currentId = getFutureTubeId(fromTubeId);

    while (currentId != -1) {
        tubeIdsToDelete.push_back(currentId);
        int64_t nextId = getFutureTubeId(currentId);
        currentId = nextId;
    }

    if (tubeIdsToDelete.empty()) {
        qDebug() << "No future versions to delete";
        db.commitTransaction();
        return true;
    }

    qDebug() << "Found" << tubeIdsToDelete.size() << "versions to delete";


    for (auto it = tubeIdsToDelete.rbegin(); it != tubeIdsToDelete.rend(); ++it) {
        int64_t tubeId = *it;
        qDebug() << "Deleting tube_id:" << tubeId;


        QString deleteEdges = QString("DELETE FROM edge WHERE segment_id IN "
                                      "(SELECT id FROM segment WHERE tube_id = %1)").arg(tubeId);
        db.executeQuery(deleteEdges);


        QString deleteSegments = QString("DELETE FROM segment WHERE tube_id = %1").arg(tubeId);
        db.executeQuery(deleteSegments);


        QString deletePoints = QString("DELETE FROM point WHERE section_id IN "
                                       "(SELECT id FROM section WHERE tube_id = %1)").arg(tubeId);
        db.executeQuery(deletePoints);


        QString deleteSections = QString("DELETE FROM section WHERE tube_id = %1").arg(tubeId);
        db.executeQuery(deleteSections);


        QString deleteTube = QString("DELETE FROM tube WHERE id = %1").arg(tubeId);
        db.executeQuery(deleteTube);
    }


    QString updateQuery = QString("UPDATE tube SET future_tube_id = NULL WHERE id = %1").arg(fromTubeId);
    db.executeQuery(updateQuery);

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        setLastError("Failed to commit transaction: " + db.getLastError());
        return false;
    }

    qDebug() << "Successfully deleted" << tubeIdsToDelete.size() << "future versions";
    return true;
}

bool TubeRepository::linkVersionEntities(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "TubeRepository::linkVersionEntities - Linking entities between versions";
    qDebug() << "  Old tube ID:" << oldTubeId;
    qDebug() << "  New tube ID:" << newTubeId;

    if (!db.isConnected()) {
        setLastError("Database is not connected");
        return false;
    }

    if (!db.beginTransaction()) {
        setLastError("Failed to begin transaction: " + db.getLastError());
        return false;
    }

    // 1. Связываем сечения (sections)
    if (!linkSections(oldTubeId, newTubeId)) {
        db.rollbackTransaction();
        setLastError("Failed to link sections: " + lastError);
        return false;
    }

    // 2. Связываем сегменты (segments)
    if (!linkSegments(oldTubeId, newTubeId)) {
        db.rollbackTransaction();
        setLastError("Failed to link segments: " + lastError);
        return false;
    }

    // 3. Связываем точки (points) - включая интерполированные
    if (!linkPoints(oldTubeId, newTubeId)) {
        db.rollbackTransaction();
        setLastError("Failed to link points: " + lastError);
        return false;
    }

    // 4. Связываем рёбра (edges)
    if (!linkEdges(oldTubeId, newTubeId)) {
        db.rollbackTransaction();
        setLastError("Failed to link edges: " + lastError);
        return false;
    }

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        setLastError("Failed to commit transaction: " + db.getLastError());
        return false;
    }

    qDebug() << "All entities linked successfully between versions";
    return true;
}

bool TubeRepository::linkSections(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "Linking sections between tubes" << oldTubeId << "and" << newTubeId;

    // Получаем все сечения старой версии
    QString oldQuery = QString(
                           "SELECT id, index FROM section WHERE tube_id = %1 ORDER BY index"
                           ).arg(oldTubeId);

    QSqlQuery oldResult = db.executeQuery(oldQuery);

    std::map<int, int64_t> oldSectionsByIndex;
    while (oldResult.next()) {
        int64_t sectionId = oldResult.value(0).toLongLong();
        int index = oldResult.value(1).toInt();
        oldSectionsByIndex[index] = sectionId;
    }

    // Получаем все сечения новой версии
    QString newQuery = QString(
                           "SELECT id, index FROM section WHERE tube_id = %1 ORDER BY index"
                           ).arg(newTubeId);

    QSqlQuery newResult = db.executeQuery(newQuery);

    std::map<int, int64_t> newSectionsByIndex;
    while (newResult.next()) {
        int64_t sectionId = newResult.value(0).toLongLong();
        int index = newResult.value(1).toInt();
        newSectionsByIndex[index] = sectionId;
    }

    // Проверяем, что количество сечений совпадает
    if (oldSectionsByIndex.size() != newSectionsByIndex.size()) {
        setLastError(QString("Section count mismatch: old=%1, new=%2")
                         .arg(oldSectionsByIndex.size()).arg(newSectionsByIndex.size()));
        return false;
    }

    // Связываем сечения по индексу
    int linkedCount = 0;
    for (const auto& pair : oldSectionsByIndex) {
        int index = pair.first;
        int64_t oldSectionId = pair.second;

        if (newSectionsByIndex.find(index) == newSectionsByIndex.end()) {
            setLastError(QString("New section with index %1 not found").arg(index));
            return false;
        }

        int64_t newSectionId = newSectionsByIndex[index];

        // Обновляем future_section_id у старого сечения
        QString updateOldQuery = QString(
                                     "UPDATE section SET future_section_id = %1 WHERE id = %2"
                                     ).arg(newSectionId).arg(oldSectionId);

        db.executeQuery(updateOldQuery);

        // Обновляем past_section_id у нового сечения
        QString updateNewQuery = QString(
                                     "UPDATE section SET past_section_id = %1 WHERE id = %2"
                                     ).arg(oldSectionId).arg(newSectionId);

        db.executeQuery(updateNewQuery);

        linkedCount++;
        qDebug() << "  Linked section index" << index << ": old_id=" << oldSectionId
                 << "-> new_id=" << newSectionId;
    }

    qDebug() << "Linked" << linkedCount << "sections";
    return true;
}

bool TubeRepository::linkSegments(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "Linking segments between tubes" << oldTubeId << "and" << newTubeId;

    // Получаем все сегменты старой версии
    QString oldQuery = QString(
                           "SELECT id, index FROM segment WHERE tube_id = %1 ORDER BY index"
                           ).arg(oldTubeId);

    QSqlQuery oldResult = db.executeQuery(oldQuery);

    std::map<int, int64_t> oldSegmentsByIndex;
    while (oldResult.next()) {
        int64_t segmentId = oldResult.value(0).toLongLong();
        int index = oldResult.value(1).toInt();
        oldSegmentsByIndex[index] = segmentId;
    }

    // Получаем все сегменты новой версии
    QString newQuery = QString(
                           "SELECT id, index FROM segment WHERE tube_id = %1 ORDER BY index"
                           ).arg(newTubeId);

    QSqlQuery newResult = db.executeQuery(newQuery);

    std::map<int, int64_t> newSegmentsByIndex;
    while (newResult.next()) {
        int64_t segmentId = newResult.value(0).toLongLong();
        int index = newResult.value(1).toInt();
        newSegmentsByIndex[index] = segmentId;
    }

    // Проверяем, что количество сегментов совпадает
    if (oldSegmentsByIndex.size() != newSegmentsByIndex.size()) {
        setLastError(QString("Segment count mismatch: old=%1, new=%2")
                         .arg(oldSegmentsByIndex.size()).arg(newSegmentsByIndex.size()));
        return false;
    }

    // Связываем сегменты по индексу
    int linkedCount = 0;
    for (const auto& pair : oldSegmentsByIndex) {
        int index = pair.first;
        int64_t oldSegmentId = pair.second;

        if (newSegmentsByIndex.find(index) == newSegmentsByIndex.end()) {
            setLastError(QString("New segment with index %1 not found").arg(index));
            return false;
        }

        int64_t newSegmentId = newSegmentsByIndex[index];

        // Обновляем future_segment_id у старого сегмента
        QString updateOldQuery = QString(
                                     "UPDATE segment SET future_segment_id = %1 WHERE id = %2"
                                     ).arg(newSegmentId).arg(oldSegmentId);

        db.executeQuery(updateOldQuery);

        // Обновляем past_segment_id у нового сегмента
        QString updateNewQuery = QString(
                                     "UPDATE segment SET past_segment_id = %1 WHERE id = %2"
                                     ).arg(oldSegmentId).arg(newSegmentId);

        db.executeQuery(updateNewQuery);

        linkedCount++;
        qDebug() << "  Linked segment index" << index << ": old_id=" << oldSegmentId
                 << "-> new_id=" << newSegmentId;
    }

    qDebug() << "Linked" << linkedCount << "segments";
    return true;
}

bool TubeRepository::linkPoints(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "Linking points between tubes" << oldTubeId << "and" << newTubeId;

    // Получаем маппинг старых и новых section_id
    std::map<int64_t, int64_t> oldSectionIdToNewSectionId;

    QString sectionQuery = QString(
                               "SELECT os.id as old_id, ns.id as new_id "
                               "FROM section os "
                               "JOIN section ns ON os.index = ns.index "
                               "WHERE os.tube_id = %1 AND ns.tube_id = %2"
                               ).arg(oldTubeId).arg(newTubeId);

    QSqlQuery sectionResult = db.executeQuery(sectionQuery);
    while (sectionResult.next()) {
        int64_t oldId = sectionResult.value(0).toLongLong();
        int64_t newId = sectionResult.value(1).toLongLong();
        oldSectionIdToNewSectionId[oldId] = newId;
    }

    // Связываем обычные точки (с index_in_section)
    int linkedRegularPoints = 0;
    for (const auto& pair : oldSectionIdToNewSectionId) {
        int64_t oldSectionId = pair.first;
        int64_t newSectionId = pair.second;

        // Получаем точки старого сечения
        QString oldPointsQuery = QString(
                                     "SELECT id, index_in_section FROM point "
                                     "WHERE section_id = %1 AND index_in_section IS NOT NULL "
                                     "ORDER BY index_in_section"
                                     ).arg(oldSectionId);

        QSqlQuery oldPointsResult = db.executeQuery(oldPointsQuery);

        std::map<int, int64_t> oldPointsByIndex;
        while (oldPointsResult.next()) {
            int64_t pointId = oldPointsResult.value(0).toLongLong();
            int index = oldPointsResult.value(1).toInt();
            oldPointsByIndex[index] = pointId;
        }

        // Получаем точки нового сечения
        QString newPointsQuery = QString(
                                     "SELECT id, index_in_section FROM point "
                                     "WHERE section_id = %1 AND index_in_section IS NOT NULL "
                                     "ORDER BY index_in_section"
                                     ).arg(newSectionId);

        QSqlQuery newPointsResult = db.executeQuery(newPointsQuery);

        std::map<int, int64_t> newPointsByIndex;
        while (newPointsResult.next()) {
            int64_t pointId = newPointsResult.value(0).toLongLong();
            int index = newPointsResult.value(1).toInt();
            newPointsByIndex[index] = pointId;
        }

        // Проверяем совпадение количества
        if (oldPointsByIndex.size() != newPointsByIndex.size()) {
            setLastError(QString("Point count mismatch in section: old=%1, new=%2")
                             .arg(oldPointsByIndex.size()).arg(newPointsByIndex.size()));
            return false;
        }

        // Связываем точки
        for (const auto& pointPair : oldPointsByIndex) {
            int index = pointPair.first;
            int64_t oldPointId = pointPair.second;

            if (newPointsByIndex.find(index) == newPointsByIndex.end()) {
                setLastError(QString("New point with index %1 not found in section %2")
                                 .arg(index).arg(newSectionId));
                return false;
            }

            int64_t newPointId = newPointsByIndex[index];

            // Обновляем future_point_id
            QString updateOldQuery = QString(
                                         "UPDATE point SET future_point_id = %1 WHERE id = %2"
                                         ).arg(newPointId).arg(oldPointId);
            db.executeQuery(updateOldQuery);

            // Обновляем past_point_id
            QString updateNewQuery = QString(
                                         "UPDATE point SET past_point_id = %1 WHERE id = %2"
                                         ).arg(oldPointId).arg(newPointId);
            db.executeQuery(updateNewQuery);

            linkedRegularPoints++;
        }
    }

    qDebug() << "Linked" << linkedRegularPoints << "regular points";

    // Связываем интерполированные точки (с index_in_section IS NULL)
    int linkedInterpolatedPoints = linkInterpolatedPoints(oldTubeId, newTubeId);
    if (linkedInterpolatedPoints == -1) {
        return false;
    }

    qDebug() << "Linked" << linkedInterpolatedPoints << "interpolated points";
    qDebug() << "Total points linked:" << (linkedRegularPoints + linkedInterpolatedPoints);

    return true;
}

int TubeRepository::linkInterpolatedPoints(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "Linking interpolated points between tubes" << oldTubeId << "and" << newTubeId;

    // Получаем маппинг segment_id
    std::map<int, std::pair<int64_t, int64_t>> segmentMapping; // index -> (old_id, new_id)

    QString segmentQuery = QString(
                               "SELECT os.index, os.id as old_id, ns.id as new_id "
                               "FROM segment os "
                               "JOIN segment ns ON os.index = ns.index "
                               "WHERE os.tube_id = %1 AND ns.tube_id = %2"
                               ).arg(oldTubeId).arg(newTubeId);

    QSqlQuery segmentResult = db.executeQuery(segmentQuery);
    while (segmentResult.next()) {
        int index = segmentResult.value(0).toInt();
        int64_t oldId = segmentResult.value(1).toLongLong();
        int64_t newId = segmentResult.value(2).toLongLong();
        segmentMapping[index] = std::make_pair(oldId, newId);
    }

    int linkedCount = 0;
    const float EPSILON = 0.001f;

    // Для каждой пары сегментов связываем интерполированные точки
    for (const auto& pair : segmentMapping) {
        int segmentIndex = pair.first;
        int64_t oldSegmentId = pair.second.first;
        int64_t newSegmentId = pair.second.second;

        // Получаем интерполированные точки старого сегмента через рёбра
        QString oldEdgesQuery = QString(
                                    "SELECT DISTINCT p.id, p.x, p.y, p.z, e.index as edge_index, "
                                    "  CASE WHEN e.start_point_id = p.id THEN 'start' ELSE 'end' END as point_role "
                                    "FROM edge e "
                                    "JOIN point p ON (e.start_point_id = p.id OR e.end_point_id = p.id) "
                                    "WHERE e.segment_id = %1 AND p.index_in_section IS NULL "
                                    "ORDER BY e.index, point_role"
                                    ).arg(oldSegmentId);

        QSqlQuery oldEdgesResult = db.executeQuery(oldEdgesQuery);

        std::vector<std::tuple<int64_t, float, float, float, int, QString>> oldInterpolatedPoints;
        // (point_id, x, y, z, edge_index, point_role)

        while (oldEdgesResult.next()) {
            int64_t pointId = oldEdgesResult.value(0).toLongLong();
            float x = oldEdgesResult.value(1).toFloat();
            float y = oldEdgesResult.value(2).toFloat();
            float z = oldEdgesResult.value(3).toFloat();
            int edgeIndex = oldEdgesResult.value(4).toInt();
            QString pointRole = oldEdgesResult.value(5).toString();

            oldInterpolatedPoints.push_back(std::make_tuple(pointId, x, y, z, edgeIndex, pointRole));
        }

        // Получаем интерполированные точки нового сегмента
        QString newEdgesQuery = QString(
                                    "SELECT DISTINCT p.id, p.x, p.y, p.z, e.index as edge_index, "
                                    "  CASE WHEN e.start_point_id = p.id THEN 'start' ELSE 'end' END as point_role "
                                    "FROM edge e "
                                    "JOIN point p ON (e.start_point_id = p.id OR e.end_point_id = p.id) "
                                    "WHERE e.segment_id = %1 AND p.index_in_section IS NULL "
                                    "ORDER BY e.index, point_role"
                                    ).arg(newSegmentId);

        QSqlQuery newEdgesResult = db.executeQuery(newEdgesQuery);

        std::vector<std::tuple<int64_t, float, float, float, int, QString>> newInterpolatedPoints;

        while (newEdgesResult.next()) {
            int64_t pointId = newEdgesResult.value(0).toLongLong();
            float x = newEdgesResult.value(1).toFloat();
            float y = newEdgesResult.value(2).toFloat();
            float z = newEdgesResult.value(3).toFloat();
            int edgeIndex = newEdgesResult.value(4).toInt();
            QString pointRole = newEdgesResult.value(5).toString();

            newInterpolatedPoints.push_back(std::make_tuple(pointId, x, y, z, edgeIndex, pointRole));
        }

        // Связываем точки по их позиции в списке рёбер (edge_index + role)
        std::map<std::pair<int, QString>, int64_t> oldPointsByEdgeAndRole;
        std::map<std::pair<int, QString>, int64_t> newPointsByEdgeAndRole;

        for (const auto& tuple : oldInterpolatedPoints) {
            int64_t pointId = std::get<0>(tuple);
            int edgeIndex = std::get<4>(tuple);
            QString role = std::get<5>(tuple);
            oldPointsByEdgeAndRole[std::make_pair(edgeIndex, role)] = pointId;
        }

        for (const auto& tuple : newInterpolatedPoints) {
            int64_t pointId = std::get<0>(tuple);
            int edgeIndex = std::get<4>(tuple);
            QString role = std::get<5>(tuple);
            newPointsByEdgeAndRole[std::make_pair(edgeIndex, role)] = pointId;
        }

        // Связываем соответствующие точки
        for (const auto& kvp : oldPointsByEdgeAndRole) {
            auto key = kvp.first;
            int64_t oldPointId = kvp.second;

            if (newPointsByEdgeAndRole.find(key) != newPointsByEdgeAndRole.end()) {
                int64_t newPointId = newPointsByEdgeAndRole[key];

                // Обновляем future_point_id
                QString updateOldQuery = QString(
                                             "UPDATE point SET future_point_id = %1 WHERE id = %2"
                                             ).arg(newPointId).arg(oldPointId);
                db.executeQuery(updateOldQuery);

                // Обновляем past_point_id
                QString updateNewQuery = QString(
                                             "UPDATE point SET past_point_id = %1 WHERE id = %2"
                                             ).arg(oldPointId).arg(newPointId);
                db.executeQuery(updateNewQuery);

                linkedCount++;

                qDebug() << "  Linked interpolated point: segment_index=" << segmentIndex
                         << ", edge_index=" << key.first << ", role=" << key.second
                         << ", old_id=" << oldPointId << "-> new_id=" << newPointId;
            }
        }
    }

    return linkedCount;
}

bool TubeRepository::linkEdges(int64_t oldTubeId, int64_t newTubeId)
{
    qDebug() << "Linking edges between tubes" << oldTubeId << "and" << newTubeId;

    // Получаем маппинг segment_id
    std::map<int, std::pair<int64_t, int64_t>> segmentMapping; // index -> (old_id, new_id)

    QString segmentQuery = QString(
                               "SELECT os.index, os.id as old_id, ns.id as new_id "
                               "FROM segment os "
                               "JOIN segment ns ON os.index = ns.index "
                               "WHERE os.tube_id = %1 AND ns.tube_id = %2"
                               ).arg(oldTubeId).arg(newTubeId);

    QSqlQuery segmentResult = db.executeQuery(segmentQuery);
    while (segmentResult.next()) {
        int index = segmentResult.value(0).toInt();
        int64_t oldId = segmentResult.value(1).toLongLong();
        int64_t newId = segmentResult.value(2).toLongLong();
        segmentMapping[index] = std::make_pair(oldId, newId);
    }

    int linkedCount = 0;

    // Для каждой пары сегментов связываем рёбра
    for (const auto& pair : segmentMapping) {
        int segmentIndex = pair.first;
        int64_t oldSegmentId = pair.second.first;
        int64_t newSegmentId = pair.second.second;

        // Получаем рёбра старого сегмента
        QString oldEdgesQuery = QString(
                                    "SELECT id, index FROM edge WHERE segment_id = %1 ORDER BY index"
                                    ).arg(oldSegmentId);

        QSqlQuery oldEdgesResult = db.executeQuery(oldEdgesQuery);

        std::map<int, int64_t> oldEdgesByIndex;
        while (oldEdgesResult.next()) {
            int64_t edgeId = oldEdgesResult.value(0).toLongLong();
            int index = oldEdgesResult.value(1).toInt();
            oldEdgesByIndex[index] = edgeId;
        }

        // Получаем рёбра нового сегмента
        QString newEdgesQuery = QString(
                                    "SELECT id, index FROM edge WHERE segment_id = %1 ORDER BY index"
                                    ).arg(newSegmentId);

        QSqlQuery newEdgesResult = db.executeQuery(newEdgesQuery);

        std::map<int, int64_t> newEdgesByIndex;
        while (newEdgesResult.next()) {
            int64_t edgeId = newEdgesResult.value(0).toLongLong();
            int index = newEdgesResult.value(1).toInt();
            newEdgesByIndex[index] = edgeId;
        }

        // Проверяем совпадение количества
        if (oldEdgesByIndex.size() != newEdgesByIndex.size()) {
            setLastError(QString("Edge count mismatch in segment %1: old=%2, new=%3")
                             .arg(segmentIndex).arg(oldEdgesByIndex.size()).arg(newEdgesByIndex.size()));
            return false;
        }

        // Связываем рёбра
        for (const auto& edgePair : oldEdgesByIndex) {
            int index = edgePair.first;
            int64_t oldEdgeId = edgePair.second;

            if (newEdgesByIndex.find(index) == newEdgesByIndex.end()) {
                setLastError(QString("New edge with index %1 not found in segment %2")
                                 .arg(index).arg(newSegmentId));
                return false;
            }

            int64_t newEdgeId = newEdgesByIndex[index];

            // Обновляем future_edge_id
            QString updateOldQuery = QString(
                                         "UPDATE edge SET future_edge_id = %1 WHERE id = %2"
                                         ).arg(newEdgeId).arg(oldEdgeId);
            db.executeQuery(updateOldQuery);

            // Обновляем past_edge_id
            QString updateNewQuery = QString(
                                         "UPDATE edge SET past_edge_id = %1 WHERE id = %2"
                                         ).arg(oldEdgeId).arg(newEdgeId);
            db.executeQuery(updateNewQuery);

            linkedCount++;
        }
    }

    qDebug() << "Linked" << linkedCount << "edges";
    return true;
}
