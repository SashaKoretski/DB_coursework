 
#include "sectionframe.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>

SectionFrame::SectionFrame(const Section& sec, QWidget *parent)
    : QFrame(parent), zCoord(0), section(sec)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);

    QVBoxLayout *layout = new QVBoxLayout(this);

    sectionView = new SectionView(this);
    sectionView->setSection(section);

    QHBoxLayout *labelsLayout = new QHBoxLayout();

    zLabel = new QLabel(QString("Z = %1").arg(zCoord), this);
    zLabel->setAlignment(Qt::AlignCenter);

     
    float minX = section.points[0].x;
    float maxX = section.points[0].x;
    for (const auto& point : section.points) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
    }
    float diameter = maxX - minX;

    diameterLabel = new QLabel(QString("Диаметр = %1").arg(diameter), this);
    diameterLabel->setAlignment(Qt::AlignCenter);

    labelsLayout->addWidget(zLabel);
    labelsLayout->addWidget(diameterLabel);

    layout->addWidget(sectionView);
    layout->addLayout(labelsLayout);

    setFixedSize(200, 250);

    zLabel->installEventFilter(this);
    zLabel->setCursor(Qt::PointingHandCursor);

    diameterLabel->installEventFilter(this);
    diameterLabel->setCursor(Qt::PointingHandCursor);
}

void SectionFrame::setZCoordinate(int z)
{
    zCoord = z;
    zLabel->setText(QString("Z = %1").arg(z));
}

void SectionFrame::onZLabelDoubleClicked()
{
    bool ok;
    int newZ = QInputDialog::getInt(this, "Изменить Z",
                                    "Введите новое значение Z:",
                                    zCoord, -10000, 10000, 1, &ok);
    if (ok) {
        emit zCoordinateChanged(this, newZ);
    }
}

void SectionFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QMenu contextMenu(this);
        QAction *duplicateAction = contextMenu.addAction("Дублировать");
        QAction *deleteAction = contextMenu.addAction("Удалить");

        QAction *selectedAction = contextMenu.exec(event->globalPos());

        if (selectedAction == duplicateAction) {
            emit sectionDuplicated(this);
        }
        else if (selectedAction == deleteAction) {
            emit sectionDeleted(this);
        }
    }
}

void SectionFrame::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
         
        float angleDelta = event->angleDelta().y() > 0 ? 5.0f : -5.0f;
        sectionView->rotate(angleDelta);
        event->accept();
    } else {
         
        QFrame::wheelEvent(event);
    }
}

void SectionFrame::updateDiameter(float diameter)
{
    currentDiameter = diameter;   
    diameterLabel->setText(QString("Диаметр = %1").arg(diameter));
}

bool SectionFrame::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == zLabel && event->type() == QEvent::MouseButtonDblClick) {
        onZLabelDoubleClicked();
        return true;
    }
    if (obj == diameterLabel && event->type() == QEvent::MouseButtonDblClick) {
        onDiameterLabelDoubleClicked();
        return true;
    }
    return QFrame::eventFilter(obj, event);
}

void SectionFrame::onDiameterLabelDoubleClicked()
{
    bool ok;
    float newDiameter = QInputDialog::getDouble(this, "Изменить диаметр",
                                                "Введите новый диаметр:",
                                                currentDiameter, 0.1, 10000, 2, &ok);
    if (ok) {
        scaleSectionToNewDiameter(newDiameter);
    }
}

void SectionFrame::scaleSectionToNewDiameter(float newDiameter)
{
    if (currentDiameter <= 0) return;

    float scale = newDiameter / currentDiameter;

     
    float savedAngle = sectionView->getCurrentAngle();
    sectionView->rotate(-savedAngle);   

     
    float centerX = 0, centerY = 0;
    for (const auto& point : section.points) {
        centerX += point.x;
        centerY += point.y;
    }
    centerX /= section.points.size();
    centerY /= section.points.size();

     
    for (auto& point : section.points) {
        point.x = centerX + (point.x - centerX) * scale;
        point.y = centerY + (point.y - centerY) * scale;
    }

     
    currentDiameter = newDiameter;
    sectionView->setSection(section);
    sectionView->rotate(savedAngle);   

    updateDiameter(newDiameter);
}

void SectionFrame::setZVisible(bool visible)
{
    zLabel->setVisible(visible);
}

void SectionFrame::setDiameterVisible(bool visible)
{
    diameterLabel->setVisible(visible);
}

void SectionFrame::rotateSection(float angle)
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

     
    sectionView->setSection(section);
}
