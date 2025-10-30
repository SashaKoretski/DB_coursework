#ifndef TUBE_H
#define TUBE_H

#include "section.h"
#include "segment.h"
#include <vector>
#include <array>
#include <utility>
#include <map>

class Tube
{
public:
    std::vector<Section> sections;   
    std::vector<Segment> segments;   

    Tube();
    Tube(const Tube& other);


    Tube& operator=(const Tube& other);


    int addSection(const Section& section);
    void removeSection(int index);
    Section& getSection(int index);
    const Section& getSection(int index) const;
    size_t getSectionCount() const;
    void clearSections();


    int addSegment(const Segment& segment);
    void removeSegment(int index);
    Segment& getSegment(int index);
    const Segment& getSegment(int index) const;
    size_t getSegmentCount() const;
    void clearSegments();


    bool buildAllSegments();     
    bool buildSegment(int sectionIndex1, int sectionIndex2);     
    void rebuildAllSegments();   


    const std::vector<Section>& getSections() const;
    void clear();


    float getTotalLength() const;
    Point3D getBoundingBoxMin() const;
    Point3D getBoundingBoxMax() const;
    Point3D getCenterOfMass() const;


    void translate(const Point3D& offset);
    void scale(float factor);
    void rotateAroundAxis(const Point3D& axis, float angle);


    bool isValid() const;
    bool hasValidTopology() const;   
    std::vector<std::pair<int, int>> getProblematicSegments() const;


    bool isEmpty() const;
    void sortSectionsByZ();
    std::vector<int> getSectionOrder() const;    


    struct TubeMesh {
        std::vector<Point3D> vertices;
        std::vector<std::pair<int, int>> edges;
        std::vector<std::array<int, 3>> faces;
        std::vector<int> sectionStartIndices;
        std::vector<int> pointsPerSection;

        void clear() {
            vertices.clear();
            edges.clear();
            faces.clear();
            sectionStartIndices.clear();
            pointsPerSection.clear();
        }
    };

    struct TubeConstructionResult {
        TubeMesh mesh;
        std::vector<std::pair<int, int>> problematicSections;
        bool success;
    };

    TubeConstructionResult buildMesh() const;


    int findSectionByIndex(int sectionIndex) const;
    int findSegmentBetweenSections(int section1Index, int section2Index) const;
    std::vector<int> getAdjacentSections(int sectionIndex) const;


    void updateSectionIndices();     
    void updateSegmentIndices();     
    bool validateAllIndices() const;     

    ~Tube() = default;

    std::vector<Point3D> getCentersCurve() const;

    void updateSegmentGeometry();

private:
    bool validateSegmentConnection(const Segment& segment) const;
    void generateFaces(TubeMesh& mesh) const;
    void addSectionEndCapFaces(TubeMesh& mesh, int sectionIndex, bool isStartCap) const;

    bool buildNewSegment(int startSectionIndex, int endSectionIndex);

    int addInterpolatedPointToMesh(const Point3D& point, TubeMesh& mesh,
                                   std::map<std::tuple<float, float, float>, int>& pointsMap) const;

    void addSectionFaces(TubeMesh& mesh, int sectionIndex, bool inward) const;
};

#endif  
