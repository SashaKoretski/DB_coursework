#include "segment.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <QDebug>

Segment::Segment()
    : segmentIndex(1), startSectionIndex(-1), endSectionIndex(-1)
{
}

Segment::Segment(int index)
    : segmentIndex(index), startSectionIndex(-1), endSectionIndex(-1)
{
}

Segment::Segment(int index, int startIndex, int endIndex)
    : segmentIndex(index), startSectionIndex(startIndex), endSectionIndex(endIndex)
{
}

Segment::Segment(const Segment& other)
    : segmentIndex(other.segmentIndex), startSectionIndex(other.startSectionIndex),
    endSectionIndex(other.endSectionIndex), connectingEdges(other.connectingEdges)
{
}


Segment& Segment::operator=(const Segment& other)
{
    if (this != &other) {
        segmentIndex = other.segmentIndex;
        startSectionIndex = other.startSectionIndex;
        endSectionIndex = other.endSectionIndex;
        connectingEdges = other.connectingEdges;
    }
    return *this;
}

bool Segment::operator==(const Segment& other) const
{
    return segmentIndex == other.segmentIndex &&
           startSectionIndex == other.startSectionIndex &&
           endSectionIndex == other.endSectionIndex &&
           connectingEdges == other.connectingEdges;
}


int Segment::getSegmentIndex() const
{
    return segmentIndex;
}

void Segment::setSegmentIndex(int index)
{
    if (index >= 1) {
        segmentIndex = index;
    }
}


int Segment::getStartSectionIndex() const
{
    return startSectionIndex;
}

int Segment::getEndSectionIndex() const
{
    return endSectionIndex;
}

void Segment::setStartSectionIndex(int index)
{
    startSectionIndex = index;
}

void Segment::setEndSectionIndex(int index)
{
    endSectionIndex = index;
}

void Segment::setSectionIndices(int startIndex, int endIndex)
{
    startSectionIndex = startIndex;
    endSectionIndex = endIndex;
}


void Segment::addConnectingEdge(const Edge& edge)
{
    connectingEdges.push_back(edge);
    reindexEdges();
}

void Segment::removeConnectingEdge(int index)
{
    if (index >= 1 && index <= static_cast<int>(connectingEdges.size())) {
        connectingEdges.erase(connectingEdges.begin() + (index - 1));
        reindexEdges();
    }
}

Edge& Segment::getConnectingEdge(int index)
{
    return connectingEdges[index - 1];
}

const Edge& Segment::getConnectingEdge(int index) const
{
    return connectingEdges[index - 1];
}

size_t Segment::getConnectingEdgeCount() const
{
    return connectingEdges.size();
}

void Segment::clearConnectingEdges()
{
    connectingEdges.clear();
}

bool Segment::buildNewConnectionMethod(const Section& startSection, const Section& endSection)
{
    qDebug() << "buildNewConnectionMethod: Starting segment construction between sections"
             << startSection.sectionIndex << "and" << endSection.sectionIndex;

    clearConnectingEdges();

    if (startSection.getPointCount() == 0 || endSection.getPointCount() == 0) {
        qDebug() << "buildNewConnectionMethod: ERROR - One or both sections have no points";
        return false;
    }

     
    const Section* firstSection = &startSection;
    const Section* secondSection = &endSection;
    int firstSectionIndex = startSectionIndex;
    int secondSectionIndex = endSectionIndex;

    if (startSection.sectionIndex > endSection.sectionIndex) {
        firstSection = &endSection;
        secondSection = &startSection;
        firstSectionIndex = endSectionIndex;
        secondSectionIndex = startSectionIndex;
        qDebug() << "buildNewConnectionMethod: Swapped section order for processing";
    }

     
    float originalZ1 = firstSection->getCenter().z;
    float originalZ2 = secondSection->getCenter().z;
    qDebug() << "buildNewConnectionMethod: Original Z coordinates -" << originalZ1 << "and" << originalZ2;

     
    Section workSection1 = *firstSection;
    Section workSection2 = *secondSection;

    qDebug() << "buildNewConnectionMethod: Attempting first construction method";
    if (attemptSegmentConstruction(workSection1, workSection2,
                                   firstSectionIndex, secondSectionIndex,
                                   originalZ1, originalZ2)) {
        qDebug() << "buildNewConnectionMethod: SUCCESS - First method worked";
        return true;
    }

    qDebug() << "buildNewConnectionMethod: First method failed, trying second approach";

     
    workSection1 = *firstSection;
    workSection2 = *secondSection;

    if (attemptSegmentConstruction(workSection2, workSection1,
                                   secondSectionIndex, firstSectionIndex,
                                   originalZ2, originalZ1)) {
        qDebug() << "buildNewConnectionMethod: SUCCESS - Second method worked";
        return true;
    }

    qDebug() << "buildNewConnectionMethod: FAILURE - Both construction methods failed";
    return false;
}

bool Segment::attemptSegmentConstruction(Section& section1, Section& section2,
                                         int section1Index, int section2Index,
                                         float originalZ1, float originalZ2)
{
    qDebug() << "attemptSegmentConstruction: Starting construction attempt";

     
    qDebug() << "attemptSegmentConstruction: Step 1 - Projecting sections to XY plane";
    projectSectionsToXY(section1, section2);

     
    Section* smallSection = &section1;
    Section* largeSection = &section2;

    qDebug() << "attemptSegmentConstruction: Step 2 - Scaling sections until no intersection";
    if (!scaleSectionsUntilNoIntersection(*smallSection, *largeSection)) {
        qDebug() << "attemptSegmentConstruction: ERROR - Failed to eliminate intersections by scaling";
        return false;
    }
    qDebug() << "attemptSegmentConstruction: Successfully eliminated intersections";

     
    qDebug() << "attemptSegmentConstruction: Step 3 - Building edges using polar method";
    if (!buildEdgesUsingPolarMethod(*smallSection, *largeSection,
                                    section1Index, section2Index,
                                    originalZ1, originalZ2)) {
        qDebug() << "attemptSegmentConstruction: ERROR - Polar method edge building failed";
        return false;
    }
    qDebug() << "attemptSegmentConstruction: Successfully built" << connectingEdges.size() << "edges";

     
    qDebug() << "attemptSegmentConstruction: Step 4 - Checking edge ordering";
    std::vector<int> edgeOrder;

     
    if (buildOrderedEdgeList(*smallSection, section1Index, edgeOrder)) {
        qDebug() << "attemptSegmentConstruction: Edge ordering successful for small section";
    } else {
        qDebug() << "attemptSegmentConstruction: Small section ordering failed, trying large section";
        if (!buildOrderedEdgeList(*largeSection, section2Index, edgeOrder)) {
            qDebug() << "attemptSegmentConstruction: ERROR - Both sections have ordering problems";
            return false;
        }
        qDebug() << "attemptSegmentConstruction: Edge ordering successful for large section";
    }

     
    qDebug() << "attemptSegmentConstruction: Step 5 - Final intersection check";
    if (!checkNoIntersectionsWithSmallSection(*smallSection)) {
        qDebug() << "attemptSegmentConstruction: ERROR - Final intersection check failed";
        return false;
    }

     
    reindexEdges();
    qDebug() << "attemptSegmentConstruction: SUCCESS - Construction completed with" << connectingEdges.size() << "edges";
    return true;
}

bool Segment::scaleSectionsUntilNoIntersection(Section& smallSection, Section& largeSection)
{
    qDebug() << "scaleSectionsUntilNoIntersection: Starting intersection elimination process";

    const int MAX_ITERATIONS = 100;
    const float SCALE_FACTOR = 0.95f;
    const float MIN_SCALE = 0.1f;

    float currentScale = 1.0f;

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        if (!doSectionsIntersect(smallSection, largeSection)) {
            qDebug() << "scaleSectionsUntilNoIntersection: SUCCESS - No intersections after" << iteration << "iterations, scale:" << currentScale;
            return true;
        }

        currentScale *= SCALE_FACTOR;
        if (currentScale < MIN_SCALE) {
            qDebug() << "scaleSectionsUntilNoIntersection: ERROR - Scale too small:" << currentScale;
            return false;
        }

        smallSection.scale(SCALE_FACTOR);

        if (iteration % 20 == 0) {
            qDebug() << "scaleSectionsUntilNoIntersection: Iteration" << iteration << ", current scale:" << currentScale;
        }
    }

    qDebug() << "scaleSectionsUntilNoIntersection: ERROR - Max iterations reached without eliminating intersections";
    return false;
}

bool Segment::doSectionsIntersect(const Section& section1, const Section& section2) const
{
    std::vector<Edge> edges1 = section1.getImplicitEdges();
    std::vector<Edge> edges2 = section2.getImplicitEdges();

    for (const auto& edge1 : edges1) {
        for (const auto& edge2 : edges2) {
            if (Edge::doIntersect(edge1, edge2)) {
                return true;
            }
        }
    }
    return false;  
}

bool Segment::buildEdgesUsingPolarMethod(const Section& section1, const Section& section2,
                                         int section1Index, int section2Index,
                                         float originalZ1, float originalZ2)
{
    qDebug() << "buildEdgesUsingPolarMethod: Starting polar coordinate edge building";

     
    std::vector<PolarPoint> polarPoints1 = getPolarCoordinates(section1);
    std::vector<PolarPoint> polarPoints2 = getPolarCoordinates(section2);

    qDebug() << "buildEdgesUsingPolarMethod: Got" << polarPoints1.size() << "polar points from section 1";
    qDebug() << "buildEdgesUsingPolarMethod: Got" << polarPoints2.size() << "polar points from section 2";

     
    std::set<float> uniqueAngles;
    for (const auto& pp : polarPoints1) {
        uniqueAngles.insert(pp.angle);
    }
    for (const auto& pp : polarPoints2) {
        uniqueAngles.insert(pp.angle);
    }

    qDebug() << "buildEdgesUsingPolarMethod: Found" << uniqueAngles.size() << "unique angles";

     
    std::vector<float> sortedAngles(uniqueAngles.begin(), uniqueAngles.end());
    std::sort(sortedAngles.begin(), sortedAngles.end());

    int edgeIndex = 1;
    int totalEdgesCreated = 0;

    for (float angle : sortedAngles) {
        std::vector<Edge> edgesForAngle = createEdgesForAngle(angle, section1, section2,
                                                              polarPoints1, polarPoints2,
                                                              edgeIndex, section1Index, section2Index,
                                                              originalZ1, originalZ2);

        for (const auto& edge : edgesForAngle) {
            if (edge.isValid()) {
                connectingEdges.push_back(edge);
                edgeIndex++;
                totalEdgesCreated++;
            }
        }
    }

    qDebug() << "buildEdgesUsingPolarMethod: Created" << totalEdgesCreated << "edges for" << sortedAngles.size() << "angles";

     
    size_t edgesBeforeDuplicateRemoval = connectingEdges.size();
    removeDuplicateEdges();
    reindexEdges();

    qDebug() << "buildEdgesUsingPolarMethod: Removed" << (edgesBeforeDuplicateRemoval - connectingEdges.size()) << "duplicate edges";
    qDebug() << "buildEdgesUsingPolarMethod: Final edge count:" << connectingEdges.size();

    return !connectingEdges.empty();
}

std::vector<Edge> Segment::createEdgesForAngle(float angle, const Section& section1, const Section& section2,
                                               const std::vector<PolarPoint>& polar1,
                                               const std::vector<PolarPoint>& polar2,
                                               int& edgeIndex, int section1Index, int section2Index,
                                               float originalZ1, float originalZ2)
{
    std::vector<Edge> result;
    const float ANGLE_EPSILON = 0.001f;

     
    std::vector<const PolarPoint*> points1;
    for (const auto& pp : polar1) {
        if (std::abs(pp.angle - angle) < ANGLE_EPSILON) {
            points1.push_back(&pp);
        }
    }

     
    std::vector<const PolarPoint*> points2;
    for (const auto& pp : polar2) {
        if (std::abs(pp.angle - angle) < ANGLE_EPSILON) {
            points2.push_back(&pp);
        }
    }

     
    if (!points1.empty()) {
        for (const auto* point1 : points1) {
            if (!points2.empty()) {
                 
                for (const auto* point2 : points2) {
                    Point3D startPoint = section1.getPoint(point1->originalIndex);
                    Point3D endPoint = section2.getPoint(point2->originalIndex);

                    startPoint.z = originalZ1;
                    endPoint.z = originalZ2;

                    Edge edge(startPoint, endPoint, edgeIndex,
                              section1Index, section2Index,
                              point1->originalIndex, point2->originalIndex);
                    result.push_back(edge);
                    edgeIndex++;
                }
            } else {
                 
                Point3D startPoint = section1.getPoint(point1->originalIndex);
                Point3D endPoint = findIntersectionPointWithSection(angle, section2);

                startPoint.z = originalZ1;
                endPoint.z = originalZ2;

                Edge edge(startPoint, endPoint, edgeIndex,
                          section1Index, section2Index,
                          point1->originalIndex, -1);
                result.push_back(edge);
                edgeIndex++;
            }
        }
    } else if (!points2.empty()) {
         
        for (const auto* point2 : points2) {
            Point3D startPoint = findIntersectionPointWithSection(angle, section1);
            Point3D endPoint = section2.getPoint(point2->originalIndex);

            startPoint.z = originalZ1;
            endPoint.z = originalZ2;

            Edge edge(startPoint, endPoint, edgeIndex,
                      section1Index, section2Index,
                      -1, point2->originalIndex);
            result.push_back(edge);
            edgeIndex++;
        }
    }

    return result;
}

bool Segment::buildOrderedEdgeList(const Section& section, int sectionIndex, std::vector<int>& edgeOrder)
{
    qDebug() << "buildOrderedEdgeList: Checking edge ordering for section" << sectionIndex;

    edgeOrder.clear();
    int problematicPoints = 0;

     
    for (size_t i = 0; i < section.getPointCount(); ++i) {
        int pointIndex = static_cast<int>(i + 1);
        std::vector<int> edgesForPoint;

         
        for (size_t j = 0; j < connectingEdges.size(); ++j) {
            const Edge& edge = connectingEdges[j];

            bool startsFromThisPoint = (edge.getStartSectionIndex() == sectionIndex &&
                                        edge.getStartPointIndex() == pointIndex);
            bool endsAtThisPoint = (edge.getEndSectionIndex() == sectionIndex &&
                                    edge.getEndPointIndex() == pointIndex);

            if (startsFromThisPoint || endsAtThisPoint) {
                edgesForPoint.push_back(static_cast<int>(j + 1));
            }
        }

         
        if (edgesForPoint.size() > 1) {
            qDebug() << "buildOrderedEdgeList: ERROR - Point" << pointIndex << "has" << edgesForPoint.size() << "edges connected";
            problematicPoints++;
        }

         
        if (edgesForPoint.size() == 1) {
            edgeOrder.push_back(edgesForPoint[0]);
        }
    }

    if (problematicPoints > 0) {
        qDebug() << "buildOrderedEdgeList: FAILURE - Found" << problematicPoints << "problematic points";
        return false;
    }

    qDebug() << "buildOrderedEdgeList: SUCCESS - All points have valid edge connections";
    return true;
}

bool Segment::checkNoIntersectionsWithSmallSection(const Section& smallSection)
{
    qDebug() << "checkNoIntersectionsWithSmallSection: Performing final intersection check";

     
    std::vector<Edge> sectionEdges = smallSection.getImplicitEdges();
    int invalidIntersections = 0;

    for (const auto& segmentEdge : connectingEdges) {
         
        Point3D segStart2D = segmentEdge.getStartPoint().toXY();
        Point3D segEnd2D = segmentEdge.getEndPoint().toXY();

        for (const auto& sectionEdge : sectionEdges) {
            Point3D secStart2D = sectionEdge.getStartPoint().toXY();
            Point3D secEnd2D = sectionEdge.getEndPoint().toXY();

             
            if (doSegments2DIntersect(segStart2D, segEnd2D, secStart2D, secEnd2D)) {
                 
                Point3D intersection = findSegmentIntersection(segStart2D, segEnd2D, secStart2D, secEnd2D);

                const float VERTEX_EPSILON = 0.001f;
                bool intersectionAtSegmentVertex =
                    (Point3D::distance(intersection, segStart2D) < VERTEX_EPSILON) ||
                    (Point3D::distance(intersection, segEnd2D) < VERTEX_EPSILON);

                if (!intersectionAtSegmentVertex) {
                    invalidIntersections++;
                }
            }
        }
    }

    if (invalidIntersections > 0) {
        qDebug() << "checkNoIntersectionsWithSmallSection: ERROR - Found" << invalidIntersections << "invalid intersections";
        return false;
    }

    qDebug() << "checkNoIntersectionsWithSmallSection: SUCCESS - No invalid intersections found";
    return true;
}

bool Segment::doSegments2DIntersect(const Point3D& p1, const Point3D& p2,
                                    const Point3D& p3, const Point3D& p4) const
{
    auto crossProduct = [](const Point3D& p1, const Point3D& p2, const Point3D& p3) -> float {
        return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    };

    float d1 = crossProduct(p3, p4, p1);
    float d2 = crossProduct(p3, p4, p2);
    float d3 = crossProduct(p1, p2, p3);
    float d4 = crossProduct(p1, p2, p4);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }

    return false;
}

Point3D Segment::findSegmentIntersection(const Point3D& p1, const Point3D& p2,
                                         const Point3D& p3, const Point3D& p4) const
{
    float denom = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

    if (std::abs(denom) < 0.001f) {
        return Point3D(0.0f, 0.0f, 0.0f);  
    }

    float t = ((p1.x - p3.x) * (p3.y - p4.y) - (p1.y - p3.y) * (p3.x - p4.x)) / denom;

    return Point3D(p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y), 0.0f);
}

void Segment::projectSectionsToXY(Section& section1, Section& section2)
{
    qDebug() << "projectSectionsToXY: Projecting sections to XY plane and centering";

     
    for (auto& point : section1.points) {
        point.z = 0.0f;
    }
    for (auto& point : section2.points) {
        point.z = 0.0f;
    }

     
    Point3D center1 = section1.getCenter();
    Point3D center2 = section2.getCenter();
    Point3D commonCenter = (center1 + center2) * 0.5f;

    section1.centerAt(commonCenter);
    section2.centerAt(commonCenter);

    qDebug() << "projectSectionsToXY: Sections centered at (" << commonCenter.x << "," << commonCenter.y << ")";
}

std::vector<Segment::PolarPoint> Segment::getPolarCoordinates(const Section& section)
{
    std::vector<PolarPoint> polarPoints;
    Point3D center = section.getCenter();

    for (size_t i = 0; i < section.getPointCount(); ++i) {
        const Point3D& point = section.getPoint(static_cast<int>(i + 1));

        PolarPoint pp;
        pp.originalIndex = static_cast<int>(i + 1);

        float dx = point.x - center.x;
        float dy = point.y - center.y;

        pp.radius = std::sqrt(dx * dx + dy * dy);
        pp.angle = std::atan2(dy, dx);

         
        if (pp.angle < 0) {
            pp.angle += 2.0f * M_PI;
        }

        polarPoints.push_back(pp);
    }

    return polarPoints;
}

Edge Segment::createEdgeForAngle(float angle, const Section& section1, const Section& section2,
                                 const std::vector<PolarPoint>& polar1,
                                 const std::vector<PolarPoint>& polar2,
                                 int edgeIndex, int section1Index, int section2Index,
                                 float originalZ1, float originalZ2)
{
    const float ANGLE_EPSILON = 0.001f;

     
    const PolarPoint* point1 = nullptr;
    for (const auto& pp : polar1) {
        if (std::abs(pp.angle - angle) < ANGLE_EPSILON) {
            point1 = &pp;
            break;
        }
    }

     
    const PolarPoint* point2 = nullptr;
    for (const auto& pp : polar2) {
        if (std::abs(pp.angle - angle) < ANGLE_EPSILON) {
            point2 = &pp;
            break;
        }
    }

    Point3D startPoint, endPoint;
    int startSectionIdx = -1, endSectionIdx = -1;
    int startPointIdx = -1, endPointIdx = -1;

     
    if (point1 && point2) {
        startPoint = section1.getPoint(point1->originalIndex);
        endPoint = section2.getPoint(point2->originalIndex);

         
        startPoint.z = originalZ1;
        endPoint.z = originalZ2;

        startSectionIdx = section1Index;
        endSectionIdx = section2Index;
        startPointIdx = point1->originalIndex;
        endPointIdx = point2->originalIndex;
    }
     
    else if (point1) {
        startPoint = section1.getPoint(point1->originalIndex);
        endPoint = findIntersectionPointWithSection(angle, section2);

         
        startPoint.z = originalZ1;
        endPoint.z = originalZ2;

        startSectionIdx = section1Index;
        endSectionIdx = section2Index;
        startPointIdx = point1->originalIndex;
        endPointIdx = -1;  
    }
     
    else if (point2) {
        startPoint = findIntersectionPointWithSection(angle, section1);
        endPoint = section2.getPoint(point2->originalIndex);

         
        startPoint.z = originalZ1;
        endPoint.z = originalZ2;

        startSectionIdx = section1Index;
        endSectionIdx = section2Index;
        startPointIdx = -1;  
        endPointIdx = point2->originalIndex;
    }
     
    else {
        return Edge();  
    }

     
    Edge edge(startPoint, endPoint, edgeIndex, startSectionIdx, endSectionIdx,
              startPointIdx, endPointIdx);

    return edge;
}

Point3D Segment::findIntersectionPointWithSection(float angle, const Section& section)
{
    Point3D center = section.getCenter();
    Point3D rayDirection(std::cos(angle), std::sin(angle), 0.0f);

     
    for (size_t i = 0; i < section.getPointCount(); ++i) {
        size_t nextIndex = (i + 1) % section.getPointCount();

        const Point3D& p1 = section.getPoint(static_cast<int>(i + 1));
        const Point3D& p2 = section.getPoint(static_cast<int>(nextIndex + 1));

        Point3D intersection;
        if (rayIntersectsSegment(center, rayDirection, p1, p2, intersection)) {
            return intersection;
        }
    }

     
    float avgRadius = 0.0f;
    for (size_t i = 0; i < section.getPointCount(); ++i) {
        const Point3D& point = section.getPoint(static_cast<int>(i + 1));
        avgRadius += Point3D::distance(center, point);
    }
    avgRadius /= static_cast<float>(section.getPointCount());

    return center + rayDirection * avgRadius;
}

bool Segment::rayIntersectsSegment(const Point3D& rayStart, const Point3D& rayDir,
                                   const Point3D& segStart, const Point3D& segEnd,
                                   Point3D& intersection)
{
    Point3D segDir = segEnd - segStart;
    Point3D startDiff = segStart - rayStart;

    float cross = rayDir.x * segDir.y - rayDir.y * segDir.x;

    if (std::abs(cross) < 0.001f) {
        return false;  
    }

    float t = (startDiff.x * segDir.y - startDiff.y * segDir.x) / cross;
    float u = (startDiff.x * rayDir.y - startDiff.y * rayDir.x) / cross;

    if (t >= 0 && u >= 0 && u <= 1) {
        intersection = rayStart + rayDir * t;
        return true;
    }

    return false;
}

void Segment::removeDuplicateEdges()
{
    const float POINT_EPSILON = 0.001f;

    for (auto it = connectingEdges.begin(); it != connectingEdges.end(); ) {
        bool isDuplicate = false;

        for (auto other = connectingEdges.begin(); other != it; ++other) {
             
            bool sameDirection = (Point3D::distance(it->getStartPoint(), other->getStartPoint()) < POINT_EPSILON &&
                                  Point3D::distance(it->getEndPoint(), other->getEndPoint()) < POINT_EPSILON);

            bool reverseDirection = (Point3D::distance(it->getStartPoint(), other->getEndPoint()) < POINT_EPSILON &&
                                     Point3D::distance(it->getEndPoint(), other->getStartPoint()) < POINT_EPSILON);

            if (sameDirection || reverseDirection) {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate) {
            it = connectingEdges.erase(it);
        } else {
            ++it;
        }
    }
}


float Segment::getTotalLength(const Section& startSection, const Section& endSection) const
{
    float totalLength = 0.0f;

    for (const auto& edge : connectingEdges) {
        if (edge.getStartPointIndex() >= 1 && edge.getStartPointIndex() <= static_cast<int>(startSection.getPointCount()) &&
            edge.getEndPointIndex() >= 1 && edge.getEndPointIndex() <= static_cast<int>(endSection.getPointCount())) {

            const Point3D& startPoint = startSection.getPoint(edge.getStartPointIndex());
            const Point3D& endPoint = endSection.getPoint(edge.getEndPointIndex());
            totalLength += Point3D::distance(startPoint, endPoint);
        }
    }

    return totalLength;
}

Point3D Segment::getDirection(const Section& startSection, const Section& endSection) const
{
    Point3D startCenter = startSection.getCenter();
    Point3D endCenter = endSection.getCenter();
    return (endCenter - startCenter).normalized();
}


bool Segment::isValid() const
{
    return segmentIndex >= 1 &&
           startSectionIndex >= 1 &&
           endSectionIndex >= 1 &&
           startSectionIndex != endSectionIndex &&
           !connectingEdges.empty() &&
           validateIndices();
}

bool Segment::hasValidConnections(const Section& startSection, const Section& endSection) const
{
    for (const auto& edge : connectingEdges) {
        if (edge.getStartPointIndex() < 1 || edge.getStartPointIndex() > static_cast<int>(startSection.getPointCount()) ||
            edge.getEndPointIndex() < 1 || edge.getEndPointIndex() > static_cast<int>(endSection.getPointCount())) {
            return false;
        }
    }
    return true;
}

bool Segment::hasIntersectingEdges(const Section& startSection, const Section& endSection) const
{
    if (!hasValidConnections(startSection, endSection)) {
        return false;
    }

    for (size_t i = 0; i < connectingEdges.size(); ++i) {
        for (size_t j = i + 1; j < connectingEdges.size(); ++j) {
            const Point3D& p1 = startSection.getPoint(connectingEdges[i].getStartPointIndex());
            const Point3D& p2 = endSection.getPoint(connectingEdges[i].getEndPointIndex());
            const Point3D& p3 = startSection.getPoint(connectingEdges[j].getStartPointIndex());
            const Point3D& p4 = endSection.getPoint(connectingEdges[j].getEndPointIndex());

            if (segmentsIntersect(p1, p2, p3, p4)) {
                return true;
            }
        }
    }
    return false;
}

bool Segment::validateIndices() const
{
    if (segmentIndex < 1) {
        return false;
    }

    for (size_t i = 0; i < connectingEdges.size(); ++i) {
        if (connectingEdges[i].getIndex() != static_cast<int>(i + 1)) {
            return false;
        }
        if (!connectingEdges[i].isValid()) {
            return false;
        }
    }

    return true;
}


void Segment::reverseDirection()
{
    std::swap(startSectionIndex, endSectionIndex);
    for (auto& edge : connectingEdges) {
        edge.swap();
    }
}

bool Segment::isEmpty() const
{
    return connectingEdges.empty();
}


void Segment::reindexEdges()
{
    for (size_t i = 0; i < connectingEdges.size(); ++i) {
        connectingEdges[i].setIndex(static_cast<int>(i + 1));
    }
}

void Segment::updateEdgeIndices()
{
    reindexEdges();
}

bool Segment::canConnect(const Section& section1, const Section& section2)
{
    return section1.isValid() && section2.isValid() &&
           section1.getPointCount() > 0 && section2.getPointCount() > 0;
}


bool Segment::segmentsIntersect(const Point3D& p1, const Point3D& p2,
                                const Point3D& p3, const Point3D& p4) const
{
    auto crossProduct = [](const Point3D& p1, const Point3D& p2, const Point3D& p3) -> float {
        return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    };

    float d1 = crossProduct(p3, p4, p1);
    float d2 = crossProduct(p3, p4, p2);
    float d3 = crossProduct(p1, p2, p3);
    float d4 = crossProduct(p1, p2, p4);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }

    return false;
}
