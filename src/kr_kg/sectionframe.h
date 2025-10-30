#ifndef SECTIONFRAME_H
#define SECTIONFRAME_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include "sectionview.h"
#include "section.h"

class SectionFrame : public QFrame
{
    Q_OBJECT

public:
    explicit SectionFrame(const Section& section, QWidget *parent = nullptr);
    void setZCoordinate(int z);
    int getZCoordinate() const { return zCoord; }
    void updateDiameter(float diameter);
    void wheelEvent(QWheelEvent *event) override;

    Section getSection() const {
        Section currentSection = section;
         
        currentSection.rotationAngle = sectionView->getCurrentAngle();
        return currentSection;
    }

    void setZVisible(bool visible);
    void setDiameterVisible(bool visible);
    SectionView *sectionView;
    void scaleSectionToNewDiameter(float newDiameter);

    void rotateSection(float angle);

    void setSection(const Section& newSection) {
        section = newSection;
        sectionView->setSection(section);
         
        updateDiameter(section.getDiameter());
    }


signals:
    void zCoordinateChanged(SectionFrame* frame, int newZ);
    void sectionDuplicated(SectionFrame* frame);
    void sectionDeleted(SectionFrame* frame);

private slots:
    void onZLabelDoubleClicked();
    void onDiameterLabelDoubleClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *zLabel;
    QLabel *diameterLabel;
    int zCoord;
    Section section;   
    float currentDiameter;

     
    void preservePointIndices(const Section& originalSection);
    void updateZCoordinateInPoints();
};

#endif  
