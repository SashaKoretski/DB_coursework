#ifndef SECTIONVIEW_H
#define SECTIONVIEW_H

#include <QWidget>
#include <QPainter>
#include "section.h"

class SectionView : public QWidget
{
    Q_OBJECT

public:
    explicit SectionView(QWidget *parent = nullptr);
    void setSection(const Section& section);
    void rotate(float angle);
    float getCurrentAngle() const { return currentAngle; }
    void setColor(const QColor& newColor);

    QColor getCurrentColor() const {
        return sectionColor;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Section section;
    float currentAngle;
    QColor sectionColor;
    void rotatePoints(float angle);
};

#endif  
