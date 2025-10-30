#ifndef EDGE_H
#define EDGE_H

#include "point3d.h"

class Edge
{
public:
    Point3D startPoint;
    Point3D endPoint;
    int edgeIndex;


    int startSectionIndex;   
    int endSectionIndex;     
    int startPointIndex;     
    int endPointIndex;       


    Edge();
    Edge(const Point3D& start, const Point3D& end, int index = 0);
    Edge(const Point3D& start, const Point3D& end, int index,
         int startSecIdx, int endSecIdx, int startPtIdx, int endPtIdx);
    Edge(const Edge& other);


    Edge& operator=(const Edge& other);
    bool operator==(const Edge& other) const;
    bool operator!=(const Edge& other) const;


    const Point3D& getStartPoint() const;
    const Point3D& getEndPoint() const;
    void setStartPoint(const Point3D& point);
    void setEndPoint(const Point3D& point);
    void setPoints(const Point3D& start, const Point3D& end);


    int getIndex() const;
    void setIndex(int index);


    int getStartSectionIndex() const;
    int getEndSectionIndex() const;
    int getStartPointIndex() const;
    int getEndPointIndex() const;

    void setStartSectionIndex(int index);
    void setEndSectionIndex(int index);
    void setStartPointIndex(int index);
    void setEndPointIndex(int index);

    void setSectionIndices(int startSecIdx, int endSecIdx);
    void setPointIndices(int startPtIdx, int endPtIdx);


    float getLength() const;
    float getLengthSquared() const;
    Point3D getDirection() const;
    Point3D getMidpoint() const;


    bool isValid() const;
    bool hasValidIndex() const;
    bool isStartPointFromSection() const;   
    bool isEndPointFromSection() const;     
    bool hasCommonPoint(const Edge& other) const;

     
    void swap();   
    Edge reversed() const;   

     
    static bool areConnected(const Edge& edge1, const Edge& edge2);
    static bool doIntersect(const Edge& edge1, const Edge& edge2);

    ~Edge() = default;

private:
    bool pointsEqual(const Point3D& p1, const Point3D& p2) const;
};

#endif  
