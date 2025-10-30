/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "on_menuColorTheme_triggered",
    "",
    "checked",
    "onGridSizeChanged",
    "value",
    "on_horizontalSlider_valueChanged",
    "on_checkButton_clicked",
    "on_clearButton_clicked",
    "on_saveButton_clicked",
    "addSectionFrame",
    "Section",
    "section",
    "onSectionZChanged",
    "SectionFrame*",
    "frame",
    "newZ",
    "onSectionDuplicated",
    "onSectionDeleted",
    "on_tabWidget_currentChanged",
    "index",
    "on_showTubeButton_clicked",
    "on_backButton_clicked",
    "on_actionExit_triggered",
    "on_actionShowZ_triggered",
    "on_actionShowDiameter_triggered",
    "on_actionGridSmall_triggered",
    "on_actionGridMedium_triggered",
    "on_actionGridLarge_triggered",
    "on_actionBlackColor_triggered",
    "on_actionBlueColor_triggered",
    "on_actionGreenColor_triggered",
    "on_actionRedColor_triggered",
    "on_actionAlignZ_triggered",
    "on_actionOptimizeSizes_triggered",
    "on_actionUserGuide_triggered",
    "on_actionHotkeys_triggered",
    "on_actionAbout_triggered",
    "populateInstructionTab",
    "on_actionNewProject_triggered",
    "on_actionSaveSections_triggered",
    "on_actionSaveTube_triggered",
    "on_actionSaveTube1_clicked",
    "saveCurrentTube",
    "on_actionLoadSections_triggered",
    "on_surfaceModeCheckBox_toggled",
    "on_pointAddCheckBox_toggled",
    "on_ZdoubleSpinBox_valueChanged",
    "on_ZhorizontalSlider_valueChanged",
    "on_pointAddBushButton_clicked",
    "on_secPointAddPushButton_clicked",
    "updateDeformationStepState",
    "resetDeformationState",
    "on_doubleSpinBox_valueChanged",
    "on_doubleSpinBox_2_valueChanged",
    "on_doubleSpinBox_3_valueChanged",
    "on_deformationpushButton_clicked",
    "on_RadiusdoubleSpinBox_valueChanged",
    "applyTubeDeformation",
    "findCrossSectionAtZ",
    "zCoord",
    "Tube",
    "tube",
    "insertCrossSectionIntoTube",
    "Tube&",
    "newSection",
    "findEdgePlaneIntersection",
    "Point3D",
    "p1",
    "p2",
    "Point3D&",
    "intersection",
    "on_ver_backpushButton_clicked",
    "on_ver_forvardpushButton_clicked",
    "updateVersionNavigationButtons"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      55,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  344,    2, 0x08,    1 /* Private */,
       4,    1,  347,    2, 0x08,    3 /* Private */,
       6,    1,  350,    2, 0x08,    5 /* Private */,
       7,    0,  353,    2, 0x08,    7 /* Private */,
       8,    0,  354,    2, 0x08,    8 /* Private */,
       9,    0,  355,    2, 0x08,    9 /* Private */,
      10,    1,  356,    2, 0x08,   10 /* Private */,
      13,    2,  359,    2, 0x08,   12 /* Private */,
      17,    1,  364,    2, 0x08,   15 /* Private */,
      18,    1,  367,    2, 0x08,   17 /* Private */,
      19,    1,  370,    2, 0x08,   19 /* Private */,
      21,    0,  373,    2, 0x08,   21 /* Private */,
      22,    0,  374,    2, 0x08,   22 /* Private */,
      23,    0,  375,    2, 0x08,   23 /* Private */,
      24,    1,  376,    2, 0x08,   24 /* Private */,
      25,    1,  379,    2, 0x08,   26 /* Private */,
      26,    0,  382,    2, 0x08,   28 /* Private */,
      27,    0,  383,    2, 0x08,   29 /* Private */,
      28,    0,  384,    2, 0x08,   30 /* Private */,
      29,    0,  385,    2, 0x08,   31 /* Private */,
      30,    0,  386,    2, 0x08,   32 /* Private */,
      31,    0,  387,    2, 0x08,   33 /* Private */,
      32,    0,  388,    2, 0x08,   34 /* Private */,
      33,    0,  389,    2, 0x08,   35 /* Private */,
      34,    0,  390,    2, 0x08,   36 /* Private */,
      35,    0,  391,    2, 0x08,   37 /* Private */,
      36,    0,  392,    2, 0x08,   38 /* Private */,
      37,    0,  393,    2, 0x08,   39 /* Private */,
      38,    0,  394,    2, 0x08,   40 /* Private */,
      39,    0,  395,    2, 0x08,   41 /* Private */,
      40,    0,  396,    2, 0x08,   42 /* Private */,
      41,    0,  397,    2, 0x08,   43 /* Private */,
      42,    0,  398,    2, 0x08,   44 /* Private */,
      43,    0,  399,    2, 0x08,   45 /* Private */,
      44,    0,  400,    2, 0x08,   46 /* Private */,
      45,    1,  401,    2, 0x08,   47 /* Private */,
      46,    1,  404,    2, 0x08,   49 /* Private */,
      47,    1,  407,    2, 0x08,   51 /* Private */,
      48,    1,  410,    2, 0x08,   53 /* Private */,
      49,    0,  413,    2, 0x08,   55 /* Private */,
      50,    0,  414,    2, 0x08,   56 /* Private */,
      51,    0,  415,    2, 0x08,   57 /* Private */,
      52,    0,  416,    2, 0x08,   58 /* Private */,
      53,    1,  417,    2, 0x08,   59 /* Private */,
      54,    1,  420,    2, 0x08,   61 /* Private */,
      55,    1,  423,    2, 0x08,   63 /* Private */,
      56,    0,  426,    2, 0x08,   65 /* Private */,
      57,    1,  427,    2, 0x08,   66 /* Private */,
      58,    0,  430,    2, 0x08,   68 /* Private */,
      59,    2,  431,    2, 0x08,   69 /* Private */,
      63,    3,  436,    2, 0x08,   72 /* Private */,
      66,    4,  443,    2, 0x08,   76 /* Private */,
      72,    0,  452,    2, 0x08,   81 /* Private */,
      73,    0,  453,    2, 0x08,   82 /* Private */,
      74,    0,  454,    2, 0x08,   83 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Int,   15,   16,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    0x80000000 | 11, QMetaType::Float, 0x80000000 | 61,   60,   62,
    QMetaType::Bool, 0x80000000 | 64, 0x80000000 | 11, QMetaType::Float,   62,   65,   60,
    QMetaType::Bool, 0x80000000 | 67, 0x80000000 | 67, QMetaType::Float, 0x80000000 | 70,   68,   69,   60,   71,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_menuColorTheme_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onGridSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_horizontalSlider_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_clearButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_saveButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addSectionFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Section &, std::false_type>,
        // method 'onSectionZChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SectionFrame *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSectionDuplicated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SectionFrame *, std::false_type>,
        // method 'onSectionDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SectionFrame *, std::false_type>,
        // method 'on_tabWidget_currentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_showTubeButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_backButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionExit_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionShowZ_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionShowDiameter_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionGridSmall_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionGridMedium_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionGridLarge_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionBlackColor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionBlueColor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionGreenColor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionRedColor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionAlignZ_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionOptimizeSizes_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUserGuide_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionHotkeys_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionAbout_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'populateInstructionTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionNewProject_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSaveSections_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSaveTube_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSaveTube1_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveCurrentTube'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLoadSections_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_surfaceModeCheckBox_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_pointAddCheckBox_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_ZdoubleSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_ZhorizontalSlider_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_pointAddBushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_secPointAddPushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateDeformationStepState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetDeformationState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_doubleSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_doubleSpinBox_2_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_doubleSpinBox_3_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_deformationpushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_RadiusdoubleSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'applyTubeDeformation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'findCrossSectionAtZ'
        QtPrivate::TypeAndForceComplete<Section, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Tube &, std::false_type>,
        // method 'insertCrossSectionIntoTube'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<Tube &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Section &, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'findEdgePlaneIntersection'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Point3D &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Point3D &, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<Point3D &, std::false_type>,
        // method 'on_ver_backpushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ver_forvardpushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateVersionNavigationButtons'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_menuColorTheme_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->onGridSizeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->on_horizontalSlider_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->on_checkButton_clicked(); break;
        case 4: _t->on_clearButton_clicked(); break;
        case 5: _t->on_saveButton_clicked(); break;
        case 6: _t->addSectionFrame((*reinterpret_cast< std::add_pointer_t<Section>>(_a[1]))); break;
        case 7: _t->onSectionZChanged((*reinterpret_cast< std::add_pointer_t<SectionFrame*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->onSectionDuplicated((*reinterpret_cast< std::add_pointer_t<SectionFrame*>>(_a[1]))); break;
        case 9: _t->onSectionDeleted((*reinterpret_cast< std::add_pointer_t<SectionFrame*>>(_a[1]))); break;
        case 10: _t->on_tabWidget_currentChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->on_showTubeButton_clicked(); break;
        case 12: _t->on_backButton_clicked(); break;
        case 13: _t->on_actionExit_triggered(); break;
        case 14: _t->on_actionShowZ_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->on_actionShowDiameter_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->on_actionGridSmall_triggered(); break;
        case 17: _t->on_actionGridMedium_triggered(); break;
        case 18: _t->on_actionGridLarge_triggered(); break;
        case 19: _t->on_actionBlackColor_triggered(); break;
        case 20: _t->on_actionBlueColor_triggered(); break;
        case 21: _t->on_actionGreenColor_triggered(); break;
        case 22: _t->on_actionRedColor_triggered(); break;
        case 23: _t->on_actionAlignZ_triggered(); break;
        case 24: _t->on_actionOptimizeSizes_triggered(); break;
        case 25: _t->on_actionUserGuide_triggered(); break;
        case 26: _t->on_actionHotkeys_triggered(); break;
        case 27: _t->on_actionAbout_triggered(); break;
        case 28: _t->populateInstructionTab(); break;
        case 29: _t->on_actionNewProject_triggered(); break;
        case 30: _t->on_actionSaveSections_triggered(); break;
        case 31: _t->on_actionSaveTube_triggered(); break;
        case 32: _t->on_actionSaveTube1_clicked(); break;
        case 33: _t->saveCurrentTube(); break;
        case 34: _t->on_actionLoadSections_triggered(); break;
        case 35: _t->on_surfaceModeCheckBox_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 36: _t->on_pointAddCheckBox_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 37: _t->on_ZdoubleSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 38: _t->on_ZhorizontalSlider_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 39: _t->on_pointAddBushButton_clicked(); break;
        case 40: _t->on_secPointAddPushButton_clicked(); break;
        case 41: _t->updateDeformationStepState(); break;
        case 42: _t->resetDeformationState(); break;
        case 43: _t->on_doubleSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 44: _t->on_doubleSpinBox_2_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 45: _t->on_doubleSpinBox_3_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 46: _t->on_deformationpushButton_clicked(); break;
        case 47: _t->on_RadiusdoubleSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 48: _t->applyTubeDeformation(); break;
        case 49: { Section _r = _t->findCrossSectionAtZ((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Tube>>(_a[2])));
            if (_a[0]) *reinterpret_cast< Section*>(_a[0]) = std::move(_r); }  break;
        case 50: { bool _r = _t->insertCrossSectionIntoTube((*reinterpret_cast< std::add_pointer_t<Tube&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Section>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 51: { bool _r = _t->findEdgePlaneIntersection((*reinterpret_cast< std::add_pointer_t<Point3D>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Point3D>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<Point3D&>>(_a[4])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 52: _t->on_ver_backpushButton_clicked(); break;
        case 53: _t->on_ver_forvardpushButton_clicked(); break;
        case 54: _t->updateVersionNavigationButtons(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< SectionFrame* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< SectionFrame* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< SectionFrame* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 55)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 55;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 55)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 55;
    }
    return _id;
}
QT_WARNING_POP
