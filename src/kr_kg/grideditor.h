#ifndef GRIDEDITOR_H
#define GRIDEDITOR_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include "point3d.h"
#include "section.h"

class GridEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GridEditor(QWidget *parent = nullptr);

     
    void setCellSize(int size);

     
    void clearGrid();

    QPoint getFirstPoint() const { return firstPoint; }
    QPoint getLastPoint() const { return lastPoint; }
    void drawLineBetweenPoints(const QPoint& start, const QPoint& end);   

     
    const std::vector<Point3D>& getPoints() const { return points; }
    std::vector<std::pair<int, int>> getEdges() const;   

     
    Section createSection() const;

    void clearGeometry() {
        points.clear();
    }

    void addClosingEdge() {
         
    }

    bool hasGeometryErrors(QString& errorMsg) const;
    void addPointAndEdge(const QPoint& cell);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int cellSize;   
    QVector<QPoint> filledCells;   
    bool isDrawing;   

    QPoint cellAtPosition(const QPoint &pos) const;   
    void fillCell(const QPoint &cell);   

    bool sectionStarted;   

    QPoint firstPoint;   
    QPoint lastPoint;    

    std::vector<Point3D> points;   

     
    bool doSegmentsIntersect(const Point3D& p1, const Point3D& p2,
                             const Point3D& p3, const Point3D& p4) const;

     
    bool hasIntersectingEdges() const;

     
    bool doesSegmentPassThroughPoint(const Point3D& segStart, const Point3D& segEnd,
                                     const Point3D& point) const;

     
    bool hasDuplicatePoints() const;

signals:
    void gridCleared();
};

#endif  
