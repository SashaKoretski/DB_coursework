#include "grideditor.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

GridEditor::GridEditor(QWidget *parent)
    : QWidget(parent)
    , cellSize(10)   
    , isDrawing(false)
    , sectionStarted(false)
{
     
    setMouseTracking(true);
     
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void GridEditor::setCellSize(int size)
{
    cellSize = size;
    filledCells.clear();
    sectionStarted = false;
    clearGeometry();
    update();
}

void GridEditor::clearGrid()
{
    filledCells.clear();
    sectionStarted = false;
    clearGeometry();
    update();
    emit gridCleared();
}

void GridEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::lightGray, 1));

     
    for (int x = 0; x <= width(); x += cellSize) {
        painter.drawLine(x, 0, x, height());
    }

     
    for (int y = 0; y <= height(); y += cellSize) {
        painter.drawLine(0, y, width(), y);
    }

    painter.setBrush(Qt::black);
    for (const QPoint &cell : filledCells) {
        painter.fillRect(cell.x() * cellSize,
                         cell.y() * cellSize,
                         cellSize,
                         cellSize,
                         Qt::black);
    }
}

void GridEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!sectionStarted) {
            sectionStarted = true;
            isDrawing = true;
            QPoint cell = cellAtPosition(event->pos());
            firstPoint = cell;
            fillCell(cell);
        } else {
            QMessageBox::warning(this, "Предупреждение",
                                 "Нельзя рисовать второе сечение!");
        }
    }
}

void GridEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawing) {
        QPoint cell = cellAtPosition(event->pos());
        fillCell(cell);
    }
}

void GridEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDrawing = false;
        lastPoint = cellAtPosition(event->pos());
    }
}

QPoint GridEditor::cellAtPosition(const QPoint &pos) const
{
    return QPoint(pos.x() / cellSize,
                  pos.y() / cellSize);
}

void GridEditor::fillCell(const QPoint& cell)
{
    if (cell.x() >= 0 && cell.y() >= 0 &&
        cell.x() < width() / cellSize &&
        cell.y() < height() / cellSize) {

        if (!filledCells.contains(cell)) {
            filledCells.append(cell);
            addPointAndEdge(cell);
            update();
        }
    }
}

void GridEditor::drawLineBetweenPoints(const QPoint& start, const QPoint& end)
{
    int x1 = start.x();
    int y1 = start.y();
    int x2 = end.x();
    int y2 = end.y();

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int x = x1;
    int y = y1;

    while (true) {
        QPoint cell(x, y);
        if (!filledCells.contains(cell)) {
            fillCell(cell);
        }

        if (x == x2 && y == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void GridEditor::addPointAndEdge(const QPoint& cell)
{
    float x = cell.x() * cellSize + cellSize/2.0f;
    float y = cell.y() * cellSize + cellSize/2.0f;

    int pointIndex = static_cast<int>(points.size()) + 1;
    Point3D newPoint(x, y, 0.0f, pointIndex);

    points.push_back(newPoint);
}

std::vector<std::pair<int, int>> GridEditor::getEdges() const
{
    std::vector<std::pair<int, int>> edges;

    if (points.size() < 2) {
        return edges;
    }

    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextIndex = (i + 1) % points.size();

        edges.push_back({points[i].getIndex(), points[nextIndex].getIndex()});
    }

    return edges;
}


Section GridEditor::createSection() const
{
    Section newSection;

    for (const Point3D& point : points) {
        newSection.addPoint(point);
    }

    return newSection;
}

static float crossProduct(const Point3D& p1, const Point3D& p2, const Point3D& p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool GridEditor::doSegmentsIntersect(const Point3D& p1, const Point3D& p2,
                                     const Point3D& p3, const Point3D& p4) const {
    float d1 = crossProduct(p3, p4, p1);
    float d2 = crossProduct(p3, p4, p2);
    float d3 = crossProduct(p1, p2, p3);
    float d4 = crossProduct(p1, p2, p4);

    if (p1.x == p3.x && p1.y == p3.y) return false;
    if (p1.x == p4.x && p1.y == p4.y) return false;
    if (p2.x == p3.x && p2.y == p3.y) return false;
    if (p2.x == p4.x && p2.y == p4.y) return false;

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }

    return false;
}

bool GridEditor::hasIntersectingEdges() const
{
    if (points.size() < 4) {
        return false;
    }

    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextI = (i + 1) % points.size();
        const Point3D& p1 = points[i];
        const Point3D& p2 = points[nextI];

        for (size_t j = i + 2; j < points.size(); ++j) {
            if (j == (i + points.size() - 1) % points.size()) {
                continue;
            }

            size_t nextJ = (j + 1) % points.size();
            const Point3D& p3 = points[j];
            const Point3D& p4 = points[nextJ];

            if (doSegmentsIntersect(p1, p2, p3, p4)) {
                return true;
            }
        }
    }

    return false;
}

bool GridEditor::doesSegmentPassThroughPoint(const Point3D& segStart, const Point3D& segEnd,
                                             const Point3D& point) const
{
    if ((point.x == segStart.x && point.y == segStart.y) ||
        (point.x == segEnd.x && point.y == segEnd.y)) {
        return false;
    }

    float crossProd = crossProduct(segStart, segEnd, point);
    if (abs(crossProd) > 0.001f)
        return false;

    float dotProduct = (point.x - segStart.x) * (segEnd.x - segStart.x) +
                       (point.y - segStart.y) * (segEnd.y - segStart.y);
    float squaredLength = (segEnd.x - segStart.x) * (segEnd.x - segStart.x) +
                          (segEnd.y - segStart.y) * (segEnd.y - segStart.y);

    return dotProduct >= 0 && dotProduct <= squaredLength;
}

bool GridEditor::hasDuplicatePoints() const
{
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            if (abs(points[i].x - points[j].x) < 0.001f &&
                abs(points[i].y - points[j].y) < 0.001f) {
                return true;
            }
        }
    }
    return false;
}

bool GridEditor::hasGeometryErrors(QString& errorMsg) const
{
    if (hasDuplicatePoints()) {
        errorMsg = "В сечении есть повторяющиеся точки!";
        return true;
    }

    if (points.size() < 3) {
        errorMsg = "Недостаточно точек для создания сечения!";
        return true;
    }

    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextIndex = (i + 1) % points.size();
        const Point3D& p1 = points[i];
        const Point3D& p2 = points[nextIndex];

        for (size_t j = 0; j < points.size(); ++j) {
            if (j != i && j != nextIndex) {
                if (doesSegmentPassThroughPoint(p1, p2, points[j])) {
                    errorMsg = "Ребро проходит через точку!";
                    return true;
                }
            }
        }
    }

    for (size_t i = 0; i < points.size(); ++i) {
        size_t nextI = (i + 1) % points.size();
        const Point3D& p1 = points[i];
        const Point3D& p2 = points[nextI];

        for (size_t j = i + 1; j < points.size(); ++j) {
            size_t nextJ = (j + 1) % points.size();

            if (nextI == j || i == nextJ) {
                continue;
            }

            const Point3D& p3 = points[j];
            const Point3D& p4 = points[nextJ];

            if (doSegmentsIntersect(p1, p2, p3, p4)) {
                errorMsg = "Обнаружено пересечение рёбер!";
                return true;
            }
        }
    }

    return false;
}
