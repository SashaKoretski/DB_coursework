#ifndef TUBEREPOSITORY_H
#define TUBEREPOSITORY_H

#include "databasemanager.h"
#include "tube.h"
#include "section.h"
#include "segment.h"
#include "edge.h"
#include "point3d.h"
#include <map>
#include <QString>
#include <QDebug>

class TubeRepository
{
public:
    TubeRepository();
    ~TubeRepository();

    int64_t saveTube(const Tube& tube, int verId, int64_t previousTubeId = -1);

    QString getLastError() const;

    bool linkTubeVersions(int64_t oldTubeId, int64_t newTubeId);

    int64_t getPastTubeId(int64_t tubeId);
    int64_t getFutureTubeId(int64_t tubeId);
    bool loadTubeById(int64_t tubeId, Tube& tube);
    bool deleteFutureVersions(int64_t fromTubeId);

    bool linkVersionEntities(int64_t oldTubeId, int64_t newTubeId);

private:
    DatabaseManager& db;
    QString lastError;

    using PointKey = std::tuple<float, float, float>;

    int64_t insertTubeRecord(int verId);
    bool saveSections(int64_t tubeId, const Tube& tube, int verId,
                                      std::map<PointKey, int64_t>& pointIdsMap);
    bool saveSegments(int64_t tubeId, const Tube& tube, int verId,
                                      const std::map<PointKey, int64_t>& pointIdsMap);

    int64_t insertSection(int64_t tubeId, const Section& section, int verId, int index);
    bool savePoints(int64_t sectionId, const Section& section, int verId,
                                    std::map<PointKey, int64_t>& pointIdsMap);
    int64_t insertPoint(int64_t sectionId, const Point3D& point, int verId,
                        int indexInSection);

    int64_t insertSegment(int64_t tubeId, const Segment& segment, int verId, int index);
    bool saveEdges(int64_t segmentId, const Segment& segment, int verId,
                                   const std::map<PointKey, int64_t>& pointIdsMap);
    int64_t insertEdge(int64_t segmentId, const Edge& edge, int verId, int index,
                       int64_t startPointId, int64_t endPointId);

    int64_t findPointId(const Point3D& point,
                                        const std::map<PointKey, int64_t>& pointIdsMap) const;
    bool validateTubeData(const Tube& tube) const;
    void setLastError(const QString& error);

    PointKey makePointKey(const Point3D& point) const;

    bool loadOldSectionIds(int64_t tubeId, std::map<int, int64_t>& sectionIndexToId);

    // Сохранение с версионностью
    bool saveSectionsWithVersioning(
        int64_t tubeId,
        const Tube& tube,
        int verId,
        int64_t previousTubeId,
        std::map<PointKey, int64_t>& pointIdsMap,
        std::map<int, int64_t>& sectionIndexToNewId,
        std::map<int, int64_t>& sectionIndexToOldId,
        std::map<int64_t, int64_t>& oldPointIdToNewPointId);

    bool savePointsWithVersioning(
        int64_t newSectionId,
        int64_t oldSectionId,
        const Section& section,
        int verId,
        std::map<PointKey, int64_t>& pointIdsMap,
        std::map<int64_t, int64_t>& oldPointIdToNewPointId);

    bool saveSegmentsWithVersioning(
        int64_t tubeId,
        const Tube& tube,
        int verId,
        int64_t previousTubeId,
        const std::map<PointKey, int64_t>& pointIdsMap,
        const std::map<int64_t, int64_t>& oldPointIdToNewPointId);

    bool saveEdgesWithVersioning(
        int64_t newSegmentId,
        int64_t oldSegmentId,
        const Segment& segment,
        int verId,
        const std::map<PointKey, int64_t>& pointIdsMap,
        const std::map<int64_t, int64_t>& oldPointIdToNewPointId);

    // Вставка с указанием past_id
    int64_t insertSectionWithPast(
        int64_t tubeId,
        const Section& section,
        int verId,
        int index,
        int64_t pastSectionId);

    int64_t insertPointWithPast(
        int64_t sectionId,
        const Point3D& point,
        int verId,
        int indexInSection,
        int64_t pastPointId);

    int64_t insertSegmentWithPast(
        int64_t tubeId,
        const Segment& segment,
        int verId,
        int index,
        int64_t pastSegmentId);

    int64_t insertEdgeWithPast(
        int64_t segmentId,
        const Edge& edge,
        int verId,
        int index,
        int64_t startPointId,
        int64_t endPointId,
        int64_t pastEdgeId);

    bool linkSections(int64_t oldTubeId, int64_t newTubeId);
    bool linkSegments(int64_t oldTubeId, int64_t newTubeId);
    bool linkPoints(int64_t oldTubeId, int64_t newTubeId);
    int linkInterpolatedPoints(int64_t oldTubeId, int64_t newTubeId);
    bool linkEdges(int64_t oldTubeId, int64_t newTubeId);
};

#endif
