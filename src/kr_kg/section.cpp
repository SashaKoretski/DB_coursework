#include "section.h"
#include <cmath>
#include <algorithm>
#include <numeric>

Section::Section()
    : sectionIndex(1), rotationAngle(0.0f)
{
}

Section::Section(int index)
    : sectionIndex(index), rotationAngle(0.0f)
{
}

Section::Section(const Section& other)
    : points(other.points), sectionIndex(other.sectionIndex),
    rotationAngle(other.rotationAngle), originalIndices(other.originalIndices)
{
    reindexPoints();
}

Section& Section::operator=(const Section& other)
{
    if (this != &other) {
        points = other.points;
        sectionIndex = other.sectionIndex;
        rotationAngle = other.rotationAngle;
        originalIndices = other.originalIndices;

        reindexPoints();
    }
    return *this;
}

bool Section::operator==(const Section& other) const
{
    return sectionIndex == other.sectionIndex &&
           points.size() == other.points.size() &&
           std::equal(points.begin(), points.end(), other.points.begin());
}

int Section::addPoint(const Point3D& point)
{
    Point3D newPoint = point;

    newPoint.setIndex(static_cast<int>(points.size()) + 1);

    points.push_back(newPoint);

    return newPoint.getIndex();
}

void Section::removePoint(int index)
{
    if (index < 1 || index > static_cast<int>(points.size())) {
        return;
    }

    points.erase(points.begin() + (index - 1));

    reindexPoints();
}

Point3D& Section::getPoint(int index)
{
    return points[index - 1];
}

const Point3D& Section::getPoint(int index) const
{
    return points[index - 1];
}

size_t Section::getPointCount() const
{
    return points.size();
}

void Section::clearPoints()
{
    points.clear();
    originalIndices.clear();
}

std::vector<Edge> Section::getImplicitEdges() const
{
    std::vector<Edge> edges;

    if (points.size() < 2) {
        return edges;
    }

     
    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextIndex = (i + 1) % points.size();

        Edge edge(static_cast<int>(i + 1), static_cast<int>(nextIndex + 1));
        edge.setIndex(static_cast<int>(i + 1));

        edges.push_back(edge);
    }

    return edges;
}

Point3D Section::getCenter() const
{
    if (points.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D center(0.0f, 0.0f, 0.0f);
    for (const auto& point : points) {
        center += point;
    }
    return center / static_cast<float>(points.size());
}

float Section::getDiameter() const
{
    if (points.size() < 2) {
        return 0.0f;
    }

    float maxDistance = 0.0f;
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            float distance = Point3D::distance(points[i], points[j]);
            maxDistance = std::max(maxDistance, distance);
        }
    }
    return maxDistance;
}

float Section::getPerimeter() const
{
    if (points.size() < 2) {
        return 0.0f;
    }

    float perimeter = 0.0f;

    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextIndex = (i + 1) % points.size();
        perimeter += Point3D::distance(points[i], points[nextIndex]);
    }

    return perimeter;
}

Point3D Section::getBoundingBoxMin() const
{
    if (points.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D minPoint = points[0];
    for (const auto& point : points) {
        minPoint.x = std::min(minPoint.x, point.x);
        minPoint.y = std::min(minPoint.y, point.y);
        minPoint.z = std::min(minPoint.z, point.z);
    }
    return minPoint;
}

Point3D Section::getBoundingBoxMax() const
{
    if (points.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D maxPoint = points[0];
    for (const auto& point : points) {
        maxPoint.x = std::max(maxPoint.x, point.x);
        maxPoint.y = std::max(maxPoint.y, point.y);
        maxPoint.z = std::max(maxPoint.z, point.z);
    }
    return maxPoint;
}

bool Section::hasGeometryErrors(std::string& errorMsg) const
{
    if (hasDuplicatePoints()) {
        errorMsg = "В сечении есть повторяющиеся точки!";
        return true;
    }

    if (hasIntersectingEdges()) {
        errorMsg = "Обнаружено пересечение рёбер!";
        return true;
    }

     
    std::vector<Edge> implicitEdges = getImplicitEdges();
    for (const auto& edge : implicitEdges) {
        const Point3D& p1 = points[edge.getStartPointIndex() - 1];
        const Point3D& p2 = points[edge.getEndPointIndex() - 1];

        for (size_t j = 0; j < points.size(); ++j) {
            int pointIndex = static_cast<int>(j + 1);
            if (pointIndex != edge.getStartPointIndex() && pointIndex != edge.getEndPointIndex()) {
                if (doesSegmentPassThroughPoint(p1, p2, points[j])) {
                    errorMsg = "Ребро проходит через точку!";
                    return true;
                }
            }
        }
    }

    return false;
}

bool Section::hasDuplicatePoints() const
{
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            if (points[i] == points[j]) {
                return true;
            }
        }
    }
    return false;
}

bool Section::hasIntersectingEdges() const
{
    std::vector<Edge> implicitEdges = getImplicitEdges();

    for (size_t i = 0; i < implicitEdges.size(); ++i) {
        for (size_t j = i + 1; j < implicitEdges.size(); ++j) {
            if (Edge::doIntersect(implicitEdges[i], implicitEdges[j])) {
                return true;
            }
        }
    }
    return false;
}

void Section::translate(const Point3D& offset)
{
    for (auto& point : points) {
        point += offset;
    }
}

void Section::scale(float factor)
{
    Point3D center = getCenter();
    for (auto& point : points) {
        point = center + (point - center) * factor;
    }
}

void Section::scaleToNewDiameter(float newDiameter)
{
    float currentDiameter = getDiameter();
    if (currentDiameter > 0.001f) {
        scale(newDiameter / currentDiameter);
    }
}

void Section::rotateAroundCenter(float angle)
{
    Point3D center = getCenter();
    float angleRad = angle * M_PI / 180.0f;
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);

    for (auto& point : points) {
        float dx = point.x - center.x;
        float dy = point.y - center.y;

        point.x = center.x + dx * cosA - dy * sinA;
        point.y = center.y + dx * sinA + dy * cosA;
    }

    rotationAngle += angle;
    if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
    if (rotationAngle < 0.0f) rotationAngle += 360.0f;
}

void Section::centerAt(const Point3D& newCenter)
{
    Point3D currentCenter = getCenter();
    translate(newCenter - currentCenter);
}

bool Section::isValid() const
{
    return !points.empty() && points.size() >= 3 && validateIndices();
}

bool Section::isClosed() const
{
    if (points.size() < 3) {
        return false;
    }

    return true;
}

void Section::makeClockwise()
{
    if (points.size() < 3) {
        return;
    }

     
    float area = 0.0f;
    for (size_t i = 0; i < points.size(); ++i) {
        size_t j = (i + 1) % points.size();
        area += (points[j].x - points[i].x) * (points[j].y + points[i].y);
    }

     
    if (area < 0) {
        std::reverse(points.begin(), points.end());
        reindexPoints();
    }
}

void Section::makeCounterClockwise()
{
    if (points.size() < 3) {
        return;
    }

     
    float area = 0.0f;
    for (size_t i = 0; i < points.size(); ++i) {
        size_t j = (i + 1) % points.size();
        area += (points[j].x - points[i].x) * (points[j].y + points[i].y);
    }

     
    if (area > 0) {
        std::reverse(points.begin(), points.end());
        reindexPoints();
    }
}

void Section::reindexPoints()
{
    for (size_t i = 0; i < points.size(); ++i) {
        points[i].setIndex(static_cast<int>(i + 1));
    }
}

bool Section::validateIndices() const
{
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i].getIndex() != static_cast<int>(i + 1)) {
            return false;
        }
    }
    return true;
}

std::vector<int> Section::getOriginalIndices() const
{
    if (originalIndices.empty()) {
        std::vector<int> indices(points.size());
        std::iota(indices.begin(), indices.end(), 1);
        return indices;
    }
    return originalIndices;
}

void Section::setOriginalIndices(const std::vector<int>& indices)
{
    originalIndices = indices;
}

bool Section::doesSegmentPassThroughPoint(const Point3D& segStart, const Point3D& segEnd,
                                          const Point3D& point) const
{
    if (point == segStart || point == segEnd) {
        return false;
    }

    float crossProd = crossProduct2D(segStart, segEnd, point);
    if (std::abs(crossProd) > 0.001f) {
        return false;
    }

    float dotProduct = (point.x - segStart.x) * (segEnd.x - segStart.x) +
                       (point.y - segStart.y) * (segEnd.y - segStart.y);
    float squaredLength = (segEnd.x - segStart.x) * (segEnd.x - segStart.x) +
                          (segEnd.y - segStart.y) * (segEnd.y - segStart.y);

    return dotProduct >= 0 && dotProduct <= squaredLength;
}

bool Section::doSegmentsIntersect(const Point3D& p1, const Point3D& p2,
                                  const Point3D& p3, const Point3D& p4) const
{
    float d1 = crossProduct2D(p3, p4, p1);
    float d2 = crossProduct2D(p3, p4, p2);
    float d3 = crossProduct2D(p1, p2, p3);
    float d4 = crossProduct2D(p1, p2, p4);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }

    return false;
}

float Section::crossProduct2D(const Point3D& p1, const Point3D& p2, const Point3D& p3) const
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool Section::isPointInside(const Point3D& point) const
{
    if (points.size() < 3) return false;

     
    int intersections = 0;
    Point3D rayPoint = point.toXY();  

    for (size_t i = 0; i < points.size(); ++i) {
        size_t j = (i + 1) % points.size();
        Point3D p1 = points[i].toXY();
        Point3D p2 = points[j].toXY();

        if (((p1.y > rayPoint.y) != (p2.y > rayPoint.y)) &&
            (rayPoint.x < (p2.x - p1.x) * (rayPoint.y - p1.y) / (p2.y - p1.y) + p1.x)) {
            intersections++;
        }
    }
    return (intersections % 2) == 1;
}

bool Section::isCenterOfMassInside() const
{
    Point3D center = getCenter();
    return isPointInside(center);
}
