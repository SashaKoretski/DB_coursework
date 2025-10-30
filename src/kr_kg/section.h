#ifndef SECTION_H
#define SECTION_H

#include "point3d.h"
#include "edge.h"
#include <string>
#include <vector>

class Section
{
public:
    std::vector<Point3D> points;     
    int sectionIndex;    
    float rotationAngle;     

    Section();
    Section(int index);
    Section(const Section& other);

    Section& operator=(const Section& other);
    bool operator==(const Section& other) const;


    int addPoint(const Point3D& point);
    void removePoint(int index);
    Point3D& getPoint(int index);
    const Point3D& getPoint(int index) const;
    size_t getPointCount() const;
    void clearPoints();


    std::vector<Edge> getImplicitEdges() const;


    Point3D getCenter() const;
    float getDiameter() const;   
    float getPerimeter() const;  
    Point3D getBoundingBoxMin() const;
    Point3D getBoundingBoxMax() const;


    bool hasGeometryErrors(std::string& errorMsg) const;
    bool hasDuplicatePoints() const;
    bool hasIntersectingEdges() const;   


    void translate(const Point3D& offset);
    void scale(float factor);
    void scaleToNewDiameter(float newDiameter);
    void rotateAroundCenter(float angle);    
    void centerAt(const Point3D& newCenter);


    bool isValid() const;
    bool isClosed() const;   
    void makeClockwise();    
    void makeCounterClockwise();     


    void reindexPoints();    
    bool validateIndices() const;    


    std::vector<int> getOriginalIndices() const;
    void setOriginalIndices(const std::vector<int>& indices);

    bool isPointInside(const Point3D& point) const;
    bool isCenterOfMassInside() const;

    ~Section() = default;

private:
    std::vector<int> originalIndices;

    bool doesSegmentPassThroughPoint(const Point3D& segStart, const Point3D& segEnd,
                                     const Point3D& point) const;
    bool doSegmentsIntersect(const Point3D& p1, const Point3D& p2,
                             const Point3D& p3, const Point3D& p4) const;
    float crossProduct2D(const Point3D& p1, const Point3D& p2, const Point3D& p3) const;
};

#endif  
