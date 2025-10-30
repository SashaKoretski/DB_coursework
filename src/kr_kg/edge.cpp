#include "edge.h"
#include <cmath>

Edge::Edge()
    : startPoint(0.0f, 0.0f, 0.0f), endPoint(0.0f, 0.0f, 0.0f), edgeIndex(0),
    startSectionIndex(-1), endSectionIndex(-1),
    startPointIndex(-1), endPointIndex(-1)
{
}

Edge::Edge(const Point3D& start, const Point3D& end, int index)
    : startPoint(start), endPoint(end), edgeIndex(index),
    startSectionIndex(-1), endSectionIndex(-1),
    startPointIndex(-1), endPointIndex(-1)
{
}

Edge::Edge(const Point3D& start, const Point3D& end, int index,
           int startSecIdx, int endSecIdx, int startPtIdx, int endPtIdx)
    : startPoint(start), endPoint(end), edgeIndex(index),
    startSectionIndex(startSecIdx), endSectionIndex(endSecIdx),
    startPointIndex(startPtIdx), endPointIndex(endPtIdx)
{
}

Edge::Edge(const Edge& other)
    : startPoint(other.startPoint), endPoint(other.endPoint),
    edgeIndex(other.edgeIndex),
    startSectionIndex(other.startSectionIndex),
    endSectionIndex(other.endSectionIndex),
    startPointIndex(other.startPointIndex),
    endPointIndex(other.endPointIndex)
{
}

Edge& Edge::operator=(const Edge& other)
{
    if (this != &other) {
        startPoint = other.startPoint;
        endPoint = other.endPoint;
        edgeIndex = other.edgeIndex;
        startSectionIndex = other.startSectionIndex;
        endSectionIndex = other.endSectionIndex;
        startPointIndex = other.startPointIndex;
        endPointIndex = other.endPointIndex;
    }
    return *this;
}

bool Edge::operator==(const Edge& other) const
{
    return pointsEqual(startPoint, other.startPoint) &&
           pointsEqual(endPoint, other.endPoint) &&
           edgeIndex == other.edgeIndex;
}

bool Edge::operator!=(const Edge& other) const
{
    return !(*this == other);
}

 
const Point3D& Edge::getStartPoint() const
{
    return startPoint;
}

const Point3D& Edge::getEndPoint() const
{
    return endPoint;
}

void Edge::setStartPoint(const Point3D& point)
{
    startPoint = point;
}

void Edge::setEndPoint(const Point3D& point)
{
    endPoint = point;
}

void Edge::setPoints(const Point3D& start, const Point3D& end)
{
    startPoint = start;
    endPoint = end;
}


int Edge::getIndex() const
{
    return edgeIndex;
}

void Edge::setIndex(int index)
{
    edgeIndex = index;
}


int Edge::getStartSectionIndex() const
{
    return startSectionIndex;
}

int Edge::getEndSectionIndex() const
{
    return endSectionIndex;
}

int Edge::getStartPointIndex() const
{
    return startPointIndex;
}

int Edge::getEndPointIndex() const
{
    return endPointIndex;
}

void Edge::setStartSectionIndex(int index)
{
    startSectionIndex = index;
}

void Edge::setEndSectionIndex(int index)
{
    endSectionIndex = index;
}

void Edge::setStartPointIndex(int index)
{
    startPointIndex = index;
}

void Edge::setEndPointIndex(int index)
{
    endPointIndex = index;
}

void Edge::setSectionIndices(int startSecIdx, int endSecIdx)
{
    startSectionIndex = startSecIdx;
    endSectionIndex = endSecIdx;
}

void Edge::setPointIndices(int startPtIdx, int endPtIdx)
{
    startPointIndex = startPtIdx;
    endPointIndex = endPtIdx;
}

 
float Edge::getLength() const
{
    return Point3D::distance(startPoint, endPoint);
}

float Edge::getLengthSquared() const
{
    return Point3D::distanceSquared(startPoint, endPoint);
}

Point3D Edge::getDirection() const
{
    return (endPoint - startPoint).normalized();
}

Point3D Edge::getMidpoint() const
{
    return (startPoint + endPoint) * 0.5f;
}

 
bool Edge::isValid() const
{
    return !pointsEqual(startPoint, endPoint) && edgeIndex >= 1;
}

bool Edge::hasValidIndex() const
{
    return edgeIndex >= 1;
}

bool Edge::isStartPointFromSection() const
{
    return startSectionIndex >= 1 && startPointIndex >= 1;
}

bool Edge::isEndPointFromSection() const
{
    return endSectionIndex >= 1 && endPointIndex >= 1;
}

bool Edge::hasCommonPoint(const Edge& other) const
{
    return pointsEqual(startPoint, other.startPoint) ||
           pointsEqual(startPoint, other.endPoint) ||
           pointsEqual(endPoint, other.startPoint) ||
           pointsEqual(endPoint, other.endPoint);
}

 
void Edge::swap()
{
    Point3D tempPoint = startPoint;
    startPoint = endPoint;
    endPoint = tempPoint;

    int tempSectionIndex = startSectionIndex;
    startSectionIndex = endSectionIndex;
    endSectionIndex = tempSectionIndex;

    int tempPointIndex = startPointIndex;
    startPointIndex = endPointIndex;
    endPointIndex = tempPointIndex;
}

Edge Edge::reversed() const
{
    return Edge(endPoint, startPoint, edgeIndex,
                endSectionIndex, startSectionIndex,
                endPointIndex, startPointIndex);
}

 
bool Edge::areConnected(const Edge& edge1, const Edge& edge2)
{
    return edge1.hasCommonPoint(edge2);
}

bool Edge::doIntersect(const Edge& edge1, const Edge& edge2)
{
    if (edge1.hasCommonPoint(edge2)) {
        return false;
    }

    const Point3D& p1 = edge1.startPoint;
    const Point3D& p2 = edge1.endPoint;
    const Point3D& p3 = edge2.startPoint;
    const Point3D& p4 = edge2.endPoint;

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


bool Edge::pointsEqual(const Point3D& p1, const Point3D& p2) const
{
    const float epsilon = 0.001f;
    return std::abs(p1.x - p2.x) < epsilon &&
           std::abs(p1.y - p2.y) < epsilon &&
           std::abs(p1.z - p2.z) < epsilon;
}
