#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grideditor.h"
#include "tube.h"
#include "section.h"
#include "point3d.h"
#include <QGridLayout>
#include "sectionframe.h"
#include "tubeviewer.h"
#include "deformationengine.h"
#include <QVector>
#include <QUndoStack>
#include <QShortcut>
#include <qtextbrowser.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_menuColorTheme_triggered(bool checked);

    void onGridSizeChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_checkButton_clicked();
    void on_clearButton_clicked();
    void on_saveButton_clicked();

    void addSectionFrame(const Section& section);

    void onSectionZChanged(SectionFrame* frame, int newZ);
    void onSectionDuplicated(SectionFrame* frame);
    void onSectionDeleted(SectionFrame* frame);

    void on_tabWidget_currentChanged(int index);

    void on_showTubeButton_clicked();

    void on_backButton_clicked();
    void on_actionExit_triggered();
    void on_actionShowZ_triggered(bool checked);
    void on_actionShowDiameter_triggered(bool checked);

    void on_actionGridSmall_triggered();
    void on_actionGridMedium_triggered();
    void on_actionGridLarge_triggered();

    void on_actionBlackColor_triggered();
    void on_actionBlueColor_triggered();
    void on_actionGreenColor_triggered();
    void on_actionRedColor_triggered();

    void on_actionAlignZ_triggered();
    void on_actionOptimizeSizes_triggered();

    void on_actionUserGuide_triggered();
    void on_actionHotkeys_triggered();
    void on_actionAbout_triggered();
    void populateInstructionTab();

    void on_actionNewProject_triggered();
    void on_actionSaveSections_triggered();
    void on_actionSaveTube_triggered();
    void on_actionSaveTube1_clicked();
    void saveCurrentTube();
    void on_actionLoadSections_triggered();

    void on_surfaceModeCheckBox_toggled(bool checked);

    void on_pointAddCheckBox_toggled(bool checked);
    void on_ZdoubleSpinBox_valueChanged(double value);
    void on_ZhorizontalSlider_valueChanged(int value);
    void on_pointAddBushButton_clicked();

    void on_secPointAddPushButton_clicked();

    void updateDeformationStepState();
    void resetDeformationState();

    void on_doubleSpinBox_valueChanged(double value);
    void on_doubleSpinBox_2_valueChanged(double value);
    void on_doubleSpinBox_3_valueChanged(double value);

    void on_deformationpushButton_clicked();

    void on_RadiusdoubleSpinBox_valueChanged(double value);

    void applyTubeDeformation();

    Section findCrossSectionAtZ(float zCoord, const Tube& tube);
    bool insertCrossSectionIntoTube(Tube& tube, const Section& newSection, float zCoord);
    bool findEdgePlaneIntersection(const Point3D& p1, const Point3D& p2, float zCoord, Point3D& intersection);

    void on_ver_backpushButton_clicked();
    void on_ver_forvardpushButton_clicked();
    void updateVersionNavigationButtons();

private:
    Ui::MainWindow *ui;
    GridEditor *gridEditor;
    Tube tube;
    TubeViewer *tubeViewer;
    bool firstVisitToTab2 = true;
    bool firstVisitToTab4 = true;
    int currentVersionId = 1;
    bool tubeSavedToDatabase;

    void resetCheckButton();
    void resetSaveButton();
    QVector<SectionFrame*> sectionFrames;
    QGridLayout *sectionsGridLayout;
    void updateSectionFramesOrder();
    QShortcut *undoShortcut;
    void fixDuplicateZ();
    void updateShowTubeButtonState();
    QTextBrowser* instructionTextLabel;
    const int GRID_SIZE_SMALL = 3;
    const int GRID_SIZE_MEDIUM = 7;
    const int GRID_SIZE_LARGE = 18;
    void updateSectionsColor(const QColor& color);

    void saveSectionToFile(const SectionFrame* frame, const QString& filepath);

    void setupTubeViewer();

    bool deformationMode = false;
    float maxTubeZ = 0.0f;

    void updateDeformationUIState();
    void updateSliderFromSpinBox();
    void updateSpinBoxFromSlider();
    void updateMaxTubeZ();

    bool isSelectingPointOnCurve = false;
    bool isAddingEndPoint = false;
    Point3D selectedPointOnCurve;
    Point3D selectedEndPoint;
    float selectedCurveZ = 0.0f;

    Point3D selectedCurvePoint;
    bool curvePointSelected = false;
    Point3D endPoint;
    bool endPointSelected = false;

    DeformationEngine deformationEngine;

    void applyCurveDeformation();

    void applyPreviewDeformation();
    void setupRadiusSpinBox();

    std::vector<Point3D> originalCentersCurve;

    bool saveTubeToDatabase(const Tube& tube);

    Tube currentTube;

    int64_t currentTubeId = -1;


protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif
