 
#include "sectionview.h"
#include "sectionframe.h"
#include <QPainter>
#include <cmath>
using std::cos;
using std::sin;

SectionView::SectionView(QWidget *parent)
    : QWidget(parent)
    , currentAngle(0.0f)
    , sectionColor(Qt::black)
{
    setMinimumSize(150, 150);
}

void SectionView::setSection(const Section& newSection)
{
    section = newSection;
    currentAngle = section.rotationAngle;   
    update();
}

void SectionView::paintEvent(QPaintEvent *)
{
    if (section.points.empty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

     
    float centerX = 0, centerY = 0;
    for (const auto& point : section.points) {
        centerX += point.x;
        centerY += point.y;
    }
    centerX /= section.points.size();
    centerY /= section.points.size();

     
    QVector<QPointF> rotatedPoints;
    const float PI = 3.14159265358979323846f;
    float angleRad = currentAngle * PI / 180.0f;   
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& point : section.points) {
         
        float dx = point.x - centerX;
        float dy = point.y - centerY;

         
        float rotX = dx * cosA - dy * sinA;
        float rotY = dx * sinA + dy * cosA;

         
        rotX += centerX;
        rotY += centerY;

        minX = std::min(minX, rotX);
        maxX = std::max(maxX, rotX);
        minY = std::min(minY, rotY);
        maxY = std::max(maxY, rotY);

        rotatedPoints.append(QPointF(rotX, rotY));
    }

     
    float width = maxX - minX;
    float height = maxY - minY;
    float scale = std::min((this->width() - 20) / width,
                           (this->height() - 20) / height);

    float offsetX = (this->width() - width * scale) / 2;
    float offsetY = (this->height() - height * scale) / 2;

    QVector<QPointF> transformedPoints;
    for (const auto& point : rotatedPoints) {
        float x = (point.x() - minX) * scale + offsetX;
        float y = (point.y() - minY) * scale + offsetY;
        transformedPoints.append(QPointF(x, y));
    }

     
    painter.setPen(QPen(Qt::black, 2));
    for (int i = 0; i < transformedPoints.size(); ++i) {
        int j = (i + 1) % transformedPoints.size();
        painter.drawLine(transformedPoints[i], transformedPoints[j]);
    }

     
    painter.setBrush(Qt::black);
    for (const auto& point : transformedPoints) {
        painter.drawEllipse(point, 2, 2);
    }

     
    if (auto parent = qobject_cast<SectionFrame*>(parentWidget())) {
        parent->updateDiameter(maxX - minX);
    }

    painter.setPen(QPen(sectionColor, 2));
    for (int i = 0; i < transformedPoints.size(); ++i) {
        int j = (i + 1) % transformedPoints.size();
        painter.drawLine(transformedPoints[i], transformedPoints[j]);
    }

    painter.setBrush(sectionColor);
    for (const auto& point : transformedPoints) {
        painter.drawEllipse(point, 2, 2);
    }
}

void SectionView::setColor(const QColor& newColor)
{
    sectionColor = newColor;
    update();   
}

void SectionView::rotate(float angle)
{
    currentAngle += angle;
    if (currentAngle >= 360.0f) currentAngle -= 360.0f;
    if (currentAngle < 0.0f) currentAngle += 360.0f;

     
    if (auto parent = qobject_cast<SectionFrame*>(parentWidget())) {
        parent->rotateSection(angle);
    }
    update();
}

void SectionView::rotatePoints(float angle)
{
    if (section.points.empty()) return;

     
    float centerX = 0, centerY = 0;
    for (const auto& point : section.points) {
        centerX += point.x;
        centerY += point.y;
    }
    centerX /= section.points.size();
    centerY /= section.points.size();

     
    float angleRad = angle * M_PI / 180.0f;
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

     
    for (auto& point : section.points) {
         
        float dx = point.x - centerX;
        float dy = point.y - centerY;

         
        float newX = dx * cosA - dy * sinA;
        float newY = dx * sinA + dy * cosA;

         
        point.x = centerX + newX;
        point.y = centerY + newY;
    }
}
