#ifndef SEGMENT_H
#define SEGMENT_H

#include "section.h"
#include "edge.h"
#include <vector>
#include <set>
#include <cmath>

class Segment
{
public:
    int segmentIndex;    
    int startSectionIndex;   
    int endSectionIndex;     
    std::vector<Edge> connectingEdges;   

    Segment();
    Segment(int index);
    Segment(int index, int startIndex, int endIndex);
    Segment(const Segment& other);

    Segment& operator=(const Segment& other);
    bool operator==(const Segment& other) const;

    int getSegmentIndex() const;
    void setSegmentIndex(int index);

    int getStartSectionIndex() const;
    int getEndSectionIndex() const;
    void setStartSectionIndex(int index);
    void setEndSectionIndex(int index);
    void setSectionIndices(int startIndex, int endIndex);

    void addConnectingEdge(const Edge& edge);
    void removeConnectingEdge(int index);
    Edge& getConnectingEdge(int index);
    const Edge& getConnectingEdge(int index) const;
    size_t getConnectingEdgeCount() const;
    void clearConnectingEdges();

    bool buildNewConnectionMethod(const Section& startSection, const Section& endSection);

    float getTotalLength(const Section& startSection, const Section& endSection) const;
    Point3D getDirection(const Section& startSection, const Section& endSection) const;

    bool isValid() const;
    bool hasValidConnections(const Section& startSection, const Section& endSection) const;
    bool hasIntersectingEdges(const Section& startSection, const Section& endSection) const;
    bool validateIndices() const;

    void reverseDirection();
    bool isEmpty() const;

    void reindexEdges();
    void updateEdgeIndices();

    static bool canConnect(const Section& section1, const Section& section2);

    ~Segment() = default;

private:
     
    struct PolarPoint {
        int originalIndex;   
        float angle;         
        float radius;        
    };

     
    void projectSectionsToXY(Section& section1, Section& section2);
    std::vector<PolarPoint> getPolarCoordinates(const Section& section);
    Edge createEdgeForAngle(float angle, const Section& section1, const Section& section2,
                            const std::vector<PolarPoint>& polar1,
                            const std::vector<PolarPoint>& polar2,
                            int edgeIndex, int section1Index, int section2Index,
                            float originalZ1, float originalZ2);


    bool attemptSegmentConstruction(Section& section1, Section& section2,
                                    int section1Index, int section2Index,
                                    float originalZ1, float originalZ2);

    bool scaleSectionsUntilNoIntersection(Section& smallSection, Section& largeSection);
    bool doSectionsIntersect(const Section& section1, const Section& section2) const;

    bool buildEdgesUsingPolarMethod(const Section& section1, const Section& section2,
                                    int section1Index, int section2Index,
                                    float originalZ1, float originalZ2);

    std::vector<Edge> createEdgesForAngle(float angle, const Section& section1, const Section& section2,
                                          const std::vector<PolarPoint>& polar1,
                                          const std::vector<PolarPoint>& polar2,
                                          int& edgeIndex, int section1Index, int section2Index,
                                          float originalZ1, float originalZ2);

    bool buildOrderedEdgeList(const Section& section, int sectionIndex, std::vector<int>& edgeOrder);

    bool checkNoIntersectionsWithSmallSection(const Section& smallSection);

    bool doSegments2DIntersect(const Point3D& p1, const Point3D& p2,
                               const Point3D& p3, const Point3D& p4) const;
    Point3D findSegmentIntersection(const Point3D& p1, const Point3D& p2,
                                    const Point3D& p3, const Point3D& p4) const;

    Point3D findIntersectionPointWithSection(float angle, const Section& section);
    bool rayIntersectsSegment(const Point3D& rayStart, const Point3D& rayDir,
                              const Point3D& segStart, const Point3D& segEnd,
                              Point3D& intersection);

    void removeDuplicateEdges();

     
    bool segmentsIntersect(const Point3D& p1, const Point3D& p2,
                           const Point3D& p3, const Point3D& p4) const;

    void applyNewIndexingScheme(const Section& startSection, const Section& endSection);
};

#endif  
