#include "tube.h"
#include <algorithm>
#include <cmath>
#include <array>

Tube::Tube()
{
}

Tube::Tube(const Tube& other)
    : sections(other.sections), segments(other.segments)
{
}


Tube& Tube::operator=(const Tube& other)
{
    if (this != &other) {
        sections = other.sections;
        segments = other.segments;
    }
    return *this;
}


int Tube::addSection(const Section& section)
{
    if (!section.isCenterOfMassInside()) {
        return -1;
    }
    sections.push_back(section);
    int newIndex = static_cast<int>(sections.size());
    sections[newIndex - 1].sectionIndex = newIndex;
    return newIndex;
}

void Tube::removeSection(int index)
{
    if (index >= 1 && index <= static_cast<int>(sections.size())) {
        sections.erase(sections.begin() + (index - 1));

         
        segments.erase(std::remove_if(segments.begin(), segments.end(),
                                      [index](const Segment& segment) {
                                          return segment.getStartSectionIndex() == index ||
                                                 segment.getEndSectionIndex() == index;
                                      }), segments.end());

         
        for (auto& segment : segments) {
            if (segment.getStartSectionIndex() > index) {
                segment.setStartSectionIndex(segment.getStartSectionIndex() - 1);
            }
            if (segment.getEndSectionIndex() > index) {
                segment.setEndSectionIndex(segment.getEndSectionIndex() - 1);
            }
        }

        updateSectionIndices();
        updateSegmentIndices();
    }
}

Section& Tube::getSection(int index)
{
    return sections[index - 1];
}

const Section& Tube::getSection(int index) const
{
    return sections[index - 1];
}

size_t Tube::getSectionCount() const
{
    return sections.size();
}

void Tube::clearSections()
{
    sections.clear();
    segments.clear();
}


int Tube::addSegment(const Segment& segment)
{
    if (validateSegmentConnection(segment)) {
        segments.push_back(segment);
        int newIndex = static_cast<int>(segments.size());
        segments[newIndex - 1].setSegmentIndex(newIndex);
        return newIndex;
    }
    return -1;
}

void Tube::removeSegment(int index)
{
    if (index >= 1 && index <= static_cast<int>(segments.size())) {
        segments.erase(segments.begin() + (index - 1));
        updateSegmentIndices();
    }
}

Segment& Tube::getSegment(int index)
{
    return segments[index - 1];
}

const Segment& Tube::getSegment(int index) const
{
    return segments[index - 1];
}

size_t Tube::getSegmentCount() const
{
    return segments.size();
}

void Tube::clearSegments()
{
    segments.clear();
}


bool Tube::buildAllSegments()
{
    segments.clear();

    if (sections.size() < 2) {
        return false;
    }

    sortSectionsByZ();

    bool allSuccessful = true;

     
    for (size_t i = 0; i < sections.size() - 1; ++i) {
        if (!buildSegment(static_cast<int>(i + 1), static_cast<int>(i + 2))) {
            allSuccessful = false;
        }
    }

    return allSuccessful;
}

bool Tube::buildSegment(int sectionIndex1, int sectionIndex2)
{
    if (sectionIndex1 < 1 || sectionIndex1 > static_cast<int>(sections.size()) ||
        sectionIndex2 < 1 || sectionIndex2 > static_cast<int>(sections.size()) ||
        sectionIndex1 == sectionIndex2) {
        return false;
    }

    return buildNewSegment(sectionIndex1, sectionIndex2);
}

bool Tube::buildNewSegment(int startSectionIndex, int endSectionIndex)
{
    Segment newSegment(static_cast<int>(segments.size() + 1), startSectionIndex, endSectionIndex);

    if (!newSegment.buildNewConnectionMethod(sections[startSectionIndex - 1],
                                             sections[endSectionIndex - 1])) {
        return false;
    }

    segments.push_back(newSegment);
    return true;
}

void Tube::rebuildAllSegments()
{
    segments.clear();
    buildAllSegments();
}


const std::vector<Section>& Tube::getSections() const
{
    return sections;
}

void Tube::clear()
{
    sections.clear();
    segments.clear();
}


float Tube::getTotalLength() const
{
    float totalLength = 0.0f;

    for (const auto& segment : segments) {
        int startIndex = segment.getStartSectionIndex();
        int endIndex = segment.getEndSectionIndex();

        if (startIndex >= 1 && startIndex <= static_cast<int>(sections.size()) &&
            endIndex >= 1 && endIndex <= static_cast<int>(sections.size())) {
            totalLength += segment.getTotalLength(sections[startIndex - 1], sections[endIndex - 1]);
        }
    }

    return totalLength;
}

Point3D Tube::getBoundingBoxMin() const
{
    if (sections.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D minPoint = sections[0].getBoundingBoxMin();
    for (const auto& section : sections) {
        Point3D sectionMin = section.getBoundingBoxMin();
        minPoint.x = std::min(minPoint.x, sectionMin.x);
        minPoint.y = std::min(minPoint.y, sectionMin.y);
        minPoint.z = std::min(minPoint.z, sectionMin.z);
    }

    return minPoint;
}

Point3D Tube::getBoundingBoxMax() const
{
    if (sections.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D maxPoint = sections[0].getBoundingBoxMax();
    for (const auto& section : sections) {
        Point3D sectionMax = section.getBoundingBoxMax();
        maxPoint.x = std::max(maxPoint.x, sectionMax.x);
        maxPoint.y = std::max(maxPoint.y, sectionMax.y);
        maxPoint.z = std::max(maxPoint.z, sectionMax.z);
    }

    return maxPoint;
}

Point3D Tube::getCenterOfMass() const
{
    if (sections.empty()) {
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    Point3D centerOfMass(0.0f, 0.0f, 0.0f);
    int totalPoints = 0;

    for (const auto& section : sections) {
        Point3D sectionCenter = section.getCenter();
        centerOfMass += sectionCenter * static_cast<float>(section.getPointCount());
        totalPoints += static_cast<int>(section.getPointCount());
    }

    if (totalPoints > 0) {
        centerOfMass /= static_cast<float>(totalPoints);
    }

    return centerOfMass;
}


void Tube::translate(const Point3D& offset)
{
    for (auto& section : sections) {
        section.translate(offset);
    }
}

void Tube::scale(float factor)
{
    for (auto& section : sections) {
        section.scale(factor);
    }
}

void Tube::rotateAroundAxis(const Point3D& axis, float angle)
{
    Point3D center = getCenterOfMass();

    for (auto& section : sections) {
        if (std::abs(axis.z - 1.0f) < 0.001f) {
            Point3D negativeCenter = center * -1.0f;
            section.translate(negativeCenter);
            section.rotateAroundCenter(angle);
            section.translate(center);
        }
    }
}


bool Tube::isValid() const
{
    if (sections.size() < 2) {
        return false;
    }

    for (const auto& section : sections) {
        if (!section.isValid()) {
            return false;
        }
    }

    for (const auto& segment : segments) {
        if (!segment.isValid()) {
            return false;
        }
    }

    return hasValidTopology() && validateAllIndices();
}

bool Tube::hasValidTopology() const
{
    if (sections.size() < 2) {
        return false;
    }

     
    return segments.size() >= sections.size() - 1;
}

std::vector<std::pair<int, int>> Tube::getProblematicSegments() const
{
    std::vector<std::pair<int, int>> problematic;

    for (const auto& segment : segments) {
        int startIndex = segment.getStartSectionIndex();
        int endIndex = segment.getEndSectionIndex();

        if (startIndex >= 1 && startIndex <= static_cast<int>(sections.size()) &&
            endIndex >= 1 && endIndex <= static_cast<int>(sections.size())) {

            if (segment.hasIntersectingEdges(sections[startIndex - 1], sections[endIndex - 1])) {
                problematic.emplace_back(startIndex, endIndex);
            }
        }
    }

    return problematic;
}

 
bool Tube::isEmpty() const
{
    return sections.empty();
}

void Tube::sortSectionsByZ()
{
    std::sort(sections.begin(), sections.end(),
              [](const Section& a, const Section& b) {
                  if (a.points.empty() || b.points.empty()) {
                      return false;
                  }
                  return a.points[0].z < b.points[0].z;
              });

    updateSectionIndices();
}

std::vector<int> Tube::getSectionOrder() const
{
    std::vector<std::pair<float, int>> zIndexPairs;

    for (size_t i = 0; i < sections.size(); ++i) {
        if (!sections[i].points.empty()) {
            zIndexPairs.emplace_back(sections[i].points[0].z, static_cast<int>(i + 1));
        }
    }

    std::sort(zIndexPairs.begin(), zIndexPairs.end());

    std::vector<int> order;
    for (const auto& pair : zIndexPairs) {
        order.push_back(pair.second);
    }

    return order;
}


Tube::TubeConstructionResult Tube::buildMesh() const
{
    TubeConstructionResult result;
    result.success = true;

    if (sections.size() < 2) {
        result.success = false;
        return result;
    }

    std::map<std::tuple<float, float, float>, int> interpolatedPointsMap;

    int totalVertices = 0;
    for (const auto& section : sections) {
        result.mesh.sectionStartIndices.push_back(totalVertices);
        result.mesh.pointsPerSection.push_back(static_cast<int>(section.getPointCount()));

        for (const auto& point : section.points) {
            result.mesh.vertices.push_back(point);

            auto key = std::make_tuple(point.x, point.y, point.z);
            interpolatedPointsMap[key] = totalVertices;
            totalVertices++;
        }
    }

    for (size_t s = 0; s < sections.size(); ++s) {
        int startIdx = result.mesh.sectionStartIndices[s];
        int pointCount = result.mesh.pointsPerSection[s];

        for (int i = 0; i < pointCount; ++i) {
            result.mesh.edges.emplace_back(
                startIdx + i,
                startIdx + ((i + 1) % pointCount)
                );
        }
    }

    for (const auto& segment : segments) {
        int startSectionIdx = segment.getStartSectionIndex();
        int endSectionIdx = segment.getEndSectionIndex();

        if (startSectionIdx < 1 || startSectionIdx > static_cast<int>(sections.size()) ||
            endSectionIdx < 1 || endSectionIdx > static_cast<int>(sections.size())) {
            result.problematicSections.emplace_back(startSectionIdx, endSectionIdx);
            continue;
        }

        int startVertexOffset = result.mesh.sectionStartIndices[startSectionIdx - 1];
        int endVertexOffset = result.mesh.sectionStartIndices[endSectionIdx - 1];

        for (size_t i = 0; i < segment.getConnectingEdgeCount(); ++i) {
            const Edge& edge = segment.getConnectingEdge(static_cast<int>(i + 1));

            int startVertexIndex, endVertexIndex;

            if (edge.isStartPointFromSection()) {
                int pointIdx = edge.getStartPointIndex();
                if (pointIdx >= 1 && pointIdx <= result.mesh.pointsPerSection[startSectionIdx - 1]) {
                    startVertexIndex = startVertexOffset + pointIdx - 1;
                } else {
                    result.problematicSections.emplace_back(startSectionIdx, endSectionIdx);
                    continue;
                }
            } else {
                startVertexIndex = addInterpolatedPointToMesh(edge.getStartPoint(),
                                                              result.mesh, interpolatedPointsMap);
            }

            if (edge.isEndPointFromSection()) {
                int pointIdx = edge.getEndPointIndex();
                if (pointIdx >= 1 && pointIdx <= result.mesh.pointsPerSection[endSectionIdx - 1]) {
                    endVertexIndex = endVertexOffset + pointIdx - 1;
                } else {
                    result.problematicSections.emplace_back(startSectionIdx, endSectionIdx);
                    continue;
                }
            } else {
                endVertexIndex = addInterpolatedPointToMesh(edge.getEndPoint(),
                                                            result.mesh, interpolatedPointsMap);
            }

            result.mesh.edges.emplace_back(startVertexIndex, endVertexIndex);
        }
    }

    generateFaces(result.mesh);

    result.success = !result.mesh.vertices.empty() && !result.mesh.edges.empty();

    return result;
}


int Tube::findSectionByIndex(int sectionIndex) const
{
    for (size_t i = 0; i < sections.size(); ++i) {
        if (sections[i].sectionIndex == sectionIndex) {
            return static_cast<int>(i + 1);
        }
    }
    return -1;
}

int Tube::findSegmentBetweenSections(int section1Index, int section2Index) const
{
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        if ((segment.getStartSectionIndex() == section1Index && segment.getEndSectionIndex() == section2Index) ||
            (segment.getStartSectionIndex() == section2Index && segment.getEndSectionIndex() == section1Index)) {
            return static_cast<int>(i + 1);
        }
    }
    return -1;
}

std::vector<int> Tube::getAdjacentSections(int sectionIndex) const
{
    std::vector<int> adjacent;

    for (const auto& segment : segments) {
        if (segment.getStartSectionIndex() == sectionIndex) {
            adjacent.push_back(segment.getEndSectionIndex());
        } else if (segment.getEndSectionIndex() == sectionIndex) {
            adjacent.push_back(segment.getStartSectionIndex());
        }
    }

    return adjacent;
}


void Tube::updateSectionIndices()
{
    for (size_t i = 0; i < sections.size(); ++i) {
        sections[i].sectionIndex = static_cast<int>(i + 1);
    }
}

void Tube::updateSegmentIndices()
{
    for (size_t i = 0; i < segments.size(); ++i) {
        segments[i].setSegmentIndex(static_cast<int>(i + 1));
    }
}

bool Tube::validateAllIndices() const
{
     
    for (size_t i = 0; i < sections.size(); ++i) {
        if (sections[i].sectionIndex != static_cast<int>(i + 1)) {
            return false;
        }
        if (!sections[i].validateIndices()) {
            return false;
        }
    }

     
    for (size_t i = 0; i < segments.size(); ++i) {
        if (segments[i].getSegmentIndex() != static_cast<int>(i + 1)) {
            return false;
        }
        if (!segments[i].validateIndices()) {
            return false;
        }
    }

    return true;
}


bool Tube::validateSegmentConnection(const Segment& segment) const
{
    int startIndex = segment.getStartSectionIndex();
    int endIndex = segment.getEndSectionIndex();

    return startIndex >= 1 && startIndex <= static_cast<int>(sections.size()) &&
           endIndex >= 1 && endIndex <= static_cast<int>(sections.size()) &&
           startIndex != endIndex;
}

int Tube::addInterpolatedPointToMesh(const Point3D& point, TubeMesh& mesh,
                                     std::map<std::tuple<float, float, float>, int>& pointsMap) const
{
    const float EPSILON = 0.001f;

     
    float roundedX = std::round(point.x / EPSILON) * EPSILON;
    float roundedY = std::round(point.y / EPSILON) * EPSILON;
    float roundedZ = std::round(point.z / EPSILON) * EPSILON;

    auto key = std::make_tuple(roundedX, roundedY, roundedZ);

     
    auto it = pointsMap.find(key);
    if (it != pointsMap.end()) {
        return it->second;   
    }

     
    int newIndex = static_cast<int>(mesh.vertices.size());
    mesh.vertices.push_back(point);
    pointsMap[key] = newIndex;

    return newIndex;
}

void Tube::generateFaces(TubeMesh& mesh) const
{
     
    for (const auto& segment : segments) {
        if (segment.getConnectingEdgeCount() < 3) {
            continue;  
        }

         
        std::vector<std::pair<Point3D, int>> pointToVertexIndex;

         
        auto findPointIndex = [&pointToVertexIndex](const Point3D& point) -> int {
            const float EPSILON = 0.001f;
            for (const auto& pair : pointToVertexIndex) {
                if (Point3D::distance(pair.first, point) < EPSILON) {
                    return pair.second;
                }
            }
            return -1;
        };

         
        for (size_t i = 0; i < segment.getConnectingEdgeCount(); ++i) {
            const Edge& edge = segment.getConnectingEdge(static_cast<int>(i + 1));

             
            int startIndex = findPointIndex(edge.getStartPoint());
            if (startIndex == -1) {
                startIndex = static_cast<int>(mesh.vertices.size());
                pointToVertexIndex.push_back({edge.getStartPoint(), startIndex});
                mesh.vertices.push_back(edge.getStartPoint());
            }

             
            int endIndex = findPointIndex(edge.getEndPoint());
            if (endIndex == -1) {
                endIndex = static_cast<int>(mesh.vertices.size());
                pointToVertexIndex.push_back({edge.getEndPoint(), endIndex});
                mesh.vertices.push_back(edge.getEndPoint());
            }
        }

         
        for (size_t i = 0; i < segment.getConnectingEdgeCount(); ++i) {
            size_t nextI = (i + 1) % segment.getConnectingEdgeCount();

            const Edge& edge1 = segment.getConnectingEdge(static_cast<int>(i + 1));
            const Edge& edge2 = segment.getConnectingEdge(static_cast<int>(nextI + 1));

             
            int v1 = findPointIndex(edge1.getStartPoint());   
            int v2 = findPointIndex(edge1.getEndPoint());     
            int v3 = findPointIndex(edge2.getStartPoint());   
            int v4 = findPointIndex(edge2.getEndPoint());     

             
            if (v1 == -1 || v2 == -1 || v3 == -1 || v4 == -1) {
                continue;  
            }

             
             
            std::array<int, 3> face1 = {v1, v3, v2};

             
            std::array<int, 3> face2 = {v2, v3, v4};

            mesh.faces.push_back(face1);
            mesh.faces.push_back(face2);
        }
    }

     
}

 
void Tube::addSectionEndCapFaces(TubeMesh& mesh, int sectionIndex, bool isStartCap) const
{
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(sections.size())) {
        return;
    }

    const Section& section = sections[sectionIndex];
    if (section.getPointCount() < 3) {
        return;
    }

     
    std::vector<int> sectionVertexIndices;
    for (size_t i = 0; i < section.getPointCount(); ++i) {
        const Point3D& point = section.getPoint(static_cast<int>(i + 1));

         
        int vertexIndex = -1;
        for (size_t j = 0; j < mesh.vertices.size(); ++j) {
            if (Point3D::distance(mesh.vertices[j], point) < 0.001f) {
                vertexIndex = static_cast<int>(j);
                break;
            }
        }

         
        if (vertexIndex == -1) {
            vertexIndex = static_cast<int>(mesh.vertices.size());
            mesh.vertices.push_back(point);
        }

        sectionVertexIndices.push_back(vertexIndex);
    }

     
    for (size_t i = 1; i < sectionVertexIndices.size() - 1; ++i) {
        std::array<int, 3> face;

        if (isStartCap) {
             
            face = {sectionVertexIndices[0],
                    sectionVertexIndices[i + 1],
                    sectionVertexIndices[i]};
        } else {
             
            face = {sectionVertexIndices[0],
                    sectionVertexIndices[i],
                    sectionVertexIndices[i + 1]};
        }

        mesh.faces.push_back(face);
    }
}

void Tube::addSectionFaces(TubeMesh& mesh, int sectionIndex, bool inward) const
{
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(sections.size())) {
        return;
    }

    int vertexOffset = mesh.sectionStartIndices[sectionIndex];
    int pointCount = mesh.pointsPerSection[sectionIndex];

    if (pointCount < 3) {
        return;
    }

    for (int i = 1; i < pointCount - 1; ++i) {
        std::array<int, 3> face;

        if (inward) {
            face = {vertexOffset, vertexOffset + i, vertexOffset + i + 1};
        } else {
            face = {vertexOffset, vertexOffset + i + 1, vertexOffset + i};
        }

        mesh.faces.push_back(face);
    }
}

std::vector<Point3D> Tube::getCentersCurve() const
{
    std::vector<Point3D> centersCurve;
    centersCurve.reserve(sections.size());

    for (const auto& section : sections) {
        centersCurve.push_back(section.getCenter());
    }

    return centersCurve;
}

void Tube::updateSegmentGeometry()
{

    if (segments.empty()) {
        return;
    }

    int edgesUpdated = 0;
    int edgesSkipped = 0;

     
    for (size_t segIdx = 0; segIdx < segments.size(); ++segIdx) {
        Segment& segment = segments[segIdx];
        int startSecIdx = segment.getStartSectionIndex();
        int endSecIdx = segment.getEndSectionIndex();

         
        if (startSecIdx < 1 || startSecIdx > static_cast<int>(sections.size()) ||
            endSecIdx < 1 || endSecIdx > static_cast<int>(sections.size())) {
            continue;
        }

        const Section& startSection = sections[startSecIdx - 1];
        const Section& endSection = sections[endSecIdx - 1];

         
        for (size_t edgeIdx = 0; edgeIdx < segment.getConnectingEdgeCount(); ++edgeIdx) {
            Edge& edge = segment.getConnectingEdge(static_cast<int>(edgeIdx + 1));

            bool updated = false;

             
            if (edge.isStartPointFromSection()) {
                int startPtIdx = edge.getStartPointIndex();

                if (startPtIdx >= 1 && startPtIdx <= static_cast<int>(startSection.getPointCount())) {
                    const Point3D& newStartPoint = startSection.getPoint(startPtIdx);
                    edge.setStartPoint(newStartPoint);
                    updated = true;
                }
            }

             
            if (edge.isEndPointFromSection()) {
                int endPtIdx = edge.getEndPointIndex();

                if (endPtIdx >= 1 && endPtIdx <= static_cast<int>(endSection.getPointCount())) {
                    const Point3D& newEndPoint = endSection.getPoint(endPtIdx);
                    edge.setEndPoint(newEndPoint);
                    updated = true;
                }
            }

            if (updated) {
                edgesUpdated++;
            } else {
                edgesSkipped++;
            }
        }
    }
}
