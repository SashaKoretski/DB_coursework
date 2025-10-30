/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNewProject;
    QAction *actionLoadSections;
    QAction *actionSaveSections;
    QAction *actionSaveTube;
    QAction *actionExit;
    QAction *actionShowZ;
    QAction *actionShowDiameter;
    QAction *menuColorTheme;
    QAction *actionAlignZ;
    QAction *actionOptimizeSizes;
    QAction *actionHotkeys;
    QAction *actionUserGuide;
    QAction *actionAbout;
    QAction *actionGridLarge;
    QAction *actionRedColor;
    QAction *actionGreenColor;
    QAction *actionBlueColor;
    QAction *actionBlackColor;
    QAction *actionGridSmall;
    QAction *actionGridMedium;
    QAction *actionCentering;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_7;
    QWidget *instructionWidget;
    QVBoxLayout *verticalLayout_8;
    QTextBrowser *instructionTextLabel;
    QWidget *tab_section_change;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSlider *horizontalSlider;
    QPushButton *clearButton;
    QPushButton *checkButton;
    QPushButton *saveButton;
    QWidget *gridContainer;
    QWidget *tab_sections;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QSpinBox *spinBox_more_sections;
    QLabel *label_5;
    QPushButton *showTubeButton;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_5;
    QWidget *visualizationWidget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *backButton;
    QPushButton *actionSaveTube1;
    QSpacerItem *horizontalSpacer;
    QPushButton *ver_backpushButton;
    QPushButton *ver_forvardpushButton;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *surfaceModeCheckBox;
    QCheckBox *pointAddCheckBox;
    QLabel *label;
    QDoubleSpinBox *ZdoubleSpinBox;
    QSlider *ZhorizontalSlider;
    QPushButton *pointAddBushButton;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *doubleSpinBox_2;
    QDoubleSpinBox *doubleSpinBox_3;
    QPushButton *secPointAddPushButton;
    QLabel *label_3;
    QDoubleSpinBox *RadiusdoubleSpinBox;
    QPushButton *deformationpushButton;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuGridSize;
    QMenu *menuSectionColor;
    QMenu *menuTools;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(881, 550);
        QIcon icon;
        icon.addFile(QString::fromUtf8("C:/Users/alexandr/Pictures/Screenshots/\320\241\320\275\320\270\320\274\320\276\320\272 \321\215\320\272\321\200\320\260\320\275\320\260 2025-01-26 113257.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        actionNewProject = new QAction(MainWindow);
        actionNewProject->setObjectName("actionNewProject");
        actionNewProject->setCheckable(false);
        actionLoadSections = new QAction(MainWindow);
        actionLoadSections->setObjectName("actionLoadSections");
        actionSaveSections = new QAction(MainWindow);
        actionSaveSections->setObjectName("actionSaveSections");
        actionSaveTube = new QAction(MainWindow);
        actionSaveTube->setObjectName("actionSaveTube");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionShowZ = new QAction(MainWindow);
        actionShowZ->setObjectName("actionShowZ");
        actionShowZ->setCheckable(true);
        actionShowZ->setChecked(true);
        actionShowDiameter = new QAction(MainWindow);
        actionShowDiameter->setObjectName("actionShowDiameter");
        actionShowDiameter->setCheckable(true);
        actionShowDiameter->setChecked(true);
        menuColorTheme = new QAction(MainWindow);
        menuColorTheme->setObjectName("menuColorTheme");
        menuColorTheme->setCheckable(true);
        menuColorTheme->setChecked(true);
        actionAlignZ = new QAction(MainWindow);
        actionAlignZ->setObjectName("actionAlignZ");
        actionOptimizeSizes = new QAction(MainWindow);
        actionOptimizeSizes->setObjectName("actionOptimizeSizes");
        actionHotkeys = new QAction(MainWindow);
        actionHotkeys->setObjectName("actionHotkeys");
        actionUserGuide = new QAction(MainWindow);
        actionUserGuide->setObjectName("actionUserGuide");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionGridLarge = new QAction(MainWindow);
        actionGridLarge->setObjectName("actionGridLarge");
        actionRedColor = new QAction(MainWindow);
        actionRedColor->setObjectName("actionRedColor");
        actionRedColor->setCheckable(true);
        actionGreenColor = new QAction(MainWindow);
        actionGreenColor->setObjectName("actionGreenColor");
        actionGreenColor->setCheckable(true);
        actionBlueColor = new QAction(MainWindow);
        actionBlueColor->setObjectName("actionBlueColor");
        actionBlueColor->setCheckable(true);
        actionBlackColor = new QAction(MainWindow);
        actionBlackColor->setObjectName("actionBlackColor");
        actionBlackColor->setCheckable(true);
        actionBlackColor->setChecked(true);
        actionGridSmall = new QAction(MainWindow);
        actionGridSmall->setObjectName("actionGridSmall");
        actionGridMedium = new QAction(MainWindow);
        actionGridMedium->setObjectName("actionGridMedium");
        actionCentering = new QAction(MainWindow);
        actionCentering->setObjectName("actionCentering");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        verticalLayout_7 = new QVBoxLayout(tab_2);
        verticalLayout_7->setObjectName("verticalLayout_7");
        instructionWidget = new QWidget(tab_2);
        instructionWidget->setObjectName("instructionWidget");
        verticalLayout_8 = new QVBoxLayout(instructionWidget);
        verticalLayout_8->setObjectName("verticalLayout_8");
        instructionTextLabel = new QTextBrowser(instructionWidget);
        instructionTextLabel->setObjectName("instructionTextLabel");
        instructionTextLabel->setOpenExternalLinks(true);

        verticalLayout_8->addWidget(instructionTextLabel);


        verticalLayout_7->addWidget(instructionWidget);

        tabWidget->addTab(tab_2, QString());
        tab_section_change = new QWidget();
        tab_section_change->setObjectName("tab_section_change");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tab_section_change->sizePolicy().hasHeightForWidth());
        tab_section_change->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(tab_section_change);
        verticalLayout_3->setObjectName("verticalLayout_3");
        widget = new QWidget(tab_section_change);
        widget->setObjectName("widget");
        widget->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSlider = new QSlider(widget);
        horizontalSlider->setObjectName("horizontalSlider");
        horizontalSlider->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(horizontalSlider->sizePolicy().hasHeightForWidth());
        horizontalSlider->setSizePolicy(sizePolicy2);
        horizontalSlider->setValue(50);
        horizontalSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout->addWidget(horizontalSlider);

        clearButton = new QPushButton(widget);
        clearButton->setObjectName("clearButton");

        horizontalLayout->addWidget(clearButton);

        checkButton = new QPushButton(widget);
        checkButton->setObjectName("checkButton");

        horizontalLayout->addWidget(checkButton);

        saveButton = new QPushButton(widget);
        saveButton->setObjectName("saveButton");

        horizontalLayout->addWidget(saveButton);


        verticalLayout_3->addWidget(widget);

        gridContainer = new QWidget(tab_section_change);
        gridContainer->setObjectName("gridContainer");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(gridContainer->sizePolicy().hasHeightForWidth());
        gridContainer->setSizePolicy(sizePolicy3);

        verticalLayout_3->addWidget(gridContainer);

        tabWidget->addTab(tab_section_change, QString());
        tab_sections = new QWidget();
        tab_sections->setObjectName("tab_sections");
        verticalLayout = new QVBoxLayout(tab_sections);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(tab_sections);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 839, 376));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_4->setObjectName("verticalLayout_4");
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        spinBox_more_sections = new QSpinBox(tab_sections);
        spinBox_more_sections->setObjectName("spinBox_more_sections");
        spinBox_more_sections->setMinimum(0);
        spinBox_more_sections->setMaximum(15);
        spinBox_more_sections->setSingleStep(1);
        spinBox_more_sections->setValue(0);

        horizontalLayout_6->addWidget(spinBox_more_sections);

        label_5 = new QLabel(tab_sections);
        label_5->setObjectName("label_5");

        horizontalLayout_6->addWidget(label_5);


        verticalLayout->addLayout(horizontalLayout_6);

        showTubeButton = new QPushButton(tab_sections);
        showTubeButton->setObjectName("showTubeButton");

        verticalLayout->addWidget(showTubeButton);

        tabWidget->addTab(tab_sections, QString());
        tab = new QWidget();
        tab->setObjectName("tab");
        horizontalLayout_2 = new QHBoxLayout(tab);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        visualizationWidget = new QWidget(tab);
        visualizationWidget->setObjectName("visualizationWidget");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(visualizationWidget->sizePolicy().hasHeightForWidth());
        visualizationWidget->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(visualizationWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        backButton = new QPushButton(tab);
        backButton->setObjectName("backButton");
        sizePolicy2.setHeightForWidth(backButton->sizePolicy().hasHeightForWidth());
        backButton->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(backButton);

        actionSaveTube1 = new QPushButton(tab);
        actionSaveTube1->setObjectName("actionSaveTube1");

        horizontalLayout_4->addWidget(actionSaveTube1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        ver_backpushButton = new QPushButton(tab);
        ver_backpushButton->setObjectName("ver_backpushButton");

        horizontalLayout_4->addWidget(ver_backpushButton);

        ver_forvardpushButton = new QPushButton(tab);
        ver_forvardpushButton->setObjectName("ver_forvardpushButton");

        horizontalLayout_4->addWidget(ver_forvardpushButton);


        verticalLayout_5->addLayout(horizontalLayout_4);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName("verticalLayout_6");
        surfaceModeCheckBox = new QCheckBox(tab);
        surfaceModeCheckBox->setObjectName("surfaceModeCheckBox");
        surfaceModeCheckBox->setChecked(false);

        verticalLayout_6->addWidget(surfaceModeCheckBox);

        pointAddCheckBox = new QCheckBox(tab);
        pointAddCheckBox->setObjectName("pointAddCheckBox");

        verticalLayout_6->addWidget(pointAddCheckBox);

        label = new QLabel(tab);
        label->setObjectName("label");
        QFont font;
        font.setBold(true);
        label->setFont(font);

        verticalLayout_6->addWidget(label);

        ZdoubleSpinBox = new QDoubleSpinBox(tab);
        ZdoubleSpinBox->setObjectName("ZdoubleSpinBox");

        verticalLayout_6->addWidget(ZdoubleSpinBox);

        ZhorizontalSlider = new QSlider(tab);
        ZhorizontalSlider->setObjectName("ZhorizontalSlider");
        sizePolicy2.setHeightForWidth(ZhorizontalSlider->sizePolicy().hasHeightForWidth());
        ZhorizontalSlider->setSizePolicy(sizePolicy2);
        ZhorizontalSlider->setMaximum(99);
        ZhorizontalSlider->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_6->addWidget(ZhorizontalSlider);

        pointAddBushButton = new QPushButton(tab);
        pointAddBushButton->setObjectName("pointAddBushButton");

        verticalLayout_6->addWidget(pointAddBushButton);

        label_2 = new QLabel(tab);
        label_2->setObjectName("label_2");
        label_2->setFont(font);

        verticalLayout_6->addWidget(label_2);

        doubleSpinBox = new QDoubleSpinBox(tab);
        doubleSpinBox->setObjectName("doubleSpinBox");

        verticalLayout_6->addWidget(doubleSpinBox);

        doubleSpinBox_2 = new QDoubleSpinBox(tab);
        doubleSpinBox_2->setObjectName("doubleSpinBox_2");

        verticalLayout_6->addWidget(doubleSpinBox_2);

        doubleSpinBox_3 = new QDoubleSpinBox(tab);
        doubleSpinBox_3->setObjectName("doubleSpinBox_3");

        verticalLayout_6->addWidget(doubleSpinBox_3);

        secPointAddPushButton = new QPushButton(tab);
        secPointAddPushButton->setObjectName("secPointAddPushButton");

        verticalLayout_6->addWidget(secPointAddPushButton);

        label_3 = new QLabel(tab);
        label_3->setObjectName("label_3");
        label_3->setFont(font);

        verticalLayout_6->addWidget(label_3);

        RadiusdoubleSpinBox = new QDoubleSpinBox(tab);
        RadiusdoubleSpinBox->setObjectName("RadiusdoubleSpinBox");
        RadiusdoubleSpinBox->setMaximum(2.000000000000000);
        RadiusdoubleSpinBox->setSingleStep(0.050000000000000);
        RadiusdoubleSpinBox->setValue(0.500000000000000);

        verticalLayout_6->addWidget(RadiusdoubleSpinBox);

        deformationpushButton = new QPushButton(tab);
        deformationpushButton->setObjectName("deformationpushButton");

        verticalLayout_6->addWidget(deformationpushButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);


        horizontalLayout_2->addLayout(verticalLayout_6);

        tabWidget->addTab(tab, QString());

        verticalLayout_2->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 881, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuGridSize = new QMenu(menuView);
        menuGridSize->setObjectName("menuGridSize");
        menuSectionColor = new QMenu(menuView);
        menuSectionColor->setObjectName("menuSectionColor");
        menuTools = new QMenu(menubar);
        menuTools->setObjectName("menuTools");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNewProject);
        menuFile->addAction(actionLoadSections);
        menuFile->addAction(actionSaveSections);
        menuFile->addAction(actionSaveTube);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuView->addAction(menuGridSize->menuAction());
        menuView->addAction(actionShowZ);
        menuView->addAction(actionShowDiameter);
        menuView->addAction(menuColorTheme);
        menuView->addAction(menuSectionColor->menuAction());
        menuGridSize->addAction(actionGridLarge);
        menuGridSize->addAction(actionGridSmall);
        menuGridSize->addAction(actionGridMedium);
        menuSectionColor->addAction(actionRedColor);
        menuSectionColor->addAction(actionGreenColor);
        menuSectionColor->addAction(actionBlueColor);
        menuSectionColor->addAction(actionBlackColor);
        menuTools->addAction(actionAlignZ);
        menuTools->addAction(actionOptimizeSizes);
        menuHelp->addAction(actionUserGuide);
        menuHelp->addAction(actionHotkeys);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\222\320\270\320\267\321\203\320\260\320\273\320\270\320\267\320\260\321\202\320\276\321\200 \321\202\321\200\321\203\320\261\320\272\320\270", nullptr));
        actionNewProject->setText(QCoreApplication::translate("MainWindow", "&\320\235\320\276\320\262\321\213\320\271 \320\277\321\200\320\276\320\265\320\272\321\202", nullptr));
#if QT_CONFIG(shortcut)
        actionNewProject->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLoadSections->setText(QCoreApplication::translate("MainWindow", "&\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \321\201\320\265\321\207\320\265\320\275\320\270\321\217", nullptr));
#if QT_CONFIG(shortcut)
        actionLoadSections->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveSections->setText(QCoreApplication::translate("MainWindow", "&\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \321\201\320\265\321\207\320\265\320\275\320\270\321\217", nullptr));
#if QT_CONFIG(shortcut)
        actionSaveSections->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveTube->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\265\320\275\320\270\320\265 &\321\202\321\200\321\203\320\261\320\272\320\270", nullptr));
#if QT_CONFIG(shortcut)
        actionSaveTube->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "&\320\222\321\213\321\205\320\276\320\264", nullptr));
        actionShowZ->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\321\213\320\262\320\260\321\202\321\214 \320\272\320\276\320\276\321\200\320\264\320\270\320\275\320\260\321\202\321\213 &Z", nullptr));
        actionShowDiameter->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\321\213\320\262\320\260\321\202\321\214 &\320\264\320\270\320\260\320\274\320\265\321\202\321\200\321\213", nullptr));
        menuColorTheme->setText(QCoreApplication::translate("MainWindow", "&\320\242\320\265\320\274\320\275\320\260\321\217 \321\202\320\265\320\274\320\260", nullptr));
        actionAlignZ->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\321\200\320\260\320\262\320\275\320\270\320\262\320\260\320\275\320\270\320\265 &Z-\320\272\320\276\320\276\321\200\320\264\320\270\320\275\320\260\321\202", nullptr));
#if QT_CONFIG(shortcut)
        actionAlignZ->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOptimizeSizes->setText(QCoreApplication::translate("MainWindow", "\320\236\320\277\321\202\320\270\320\274\320\270\320\267\320\260\321\206\320\270\321\217 &\321\200\320\260\320\267\320\274\320\265\321\200\320\276\320\262", nullptr));
#if QT_CONFIG(shortcut)
        actionOptimizeSizes->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+D", nullptr));
#endif // QT_CONFIG(shortcut)
        actionHotkeys->setText(QCoreApplication::translate("MainWindow", "&\320\223\320\276\321\200\321\217\321\207\320\270\320\265 \320\272\320\273\320\260\320\262\320\270\321\210\320\270", nullptr));
        actionUserGuide->setText(QCoreApplication::translate("MainWindow", "&\320\240\321\203\320\272\320\276\320\262\320\276\320\264\321\201\321\202\320\262\320\276 \320\277\320\276\320\273\321\214\320\267\320\276\320\262\320\260\321\202\320\265\320\273\321\217", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "&\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", nullptr));
        actionGridLarge->setText(QCoreApplication::translate("MainWindow", "&\320\221\320\276\320\273\321\214\321\210\320\276\320\271", nullptr));
        actionRedColor->setText(QCoreApplication::translate("MainWindow", "&\320\232\321\200\320\260\321\201\320\275\321\213\320\271", nullptr));
        actionGreenColor->setText(QCoreApplication::translate("MainWindow", "&\320\227\320\265\320\273\320\265\320\275\321\213\320\271", nullptr));
        actionBlueColor->setText(QCoreApplication::translate("MainWindow", "&\320\241\320\270\320\275\320\270\320\271", nullptr));
        actionBlackColor->setText(QCoreApplication::translate("MainWindow", "&\320\247\320\265\321\200\320\275\321\213\320\271", nullptr));
        actionGridSmall->setText(QCoreApplication::translate("MainWindow", "&\320\234\320\260\320\273\320\265\320\275\321\214\320\272\320\270\320\271", nullptr));
        actionGridMedium->setText(QCoreApplication::translate("MainWindow", "&\320\241\321\200\320\265\320\264\320\275\320\270\320\271", nullptr));
        actionCentering->setText(QCoreApplication::translate("MainWindow", "&\320\246\320\265\320\275\321\202\321\200\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265 ", nullptr));
#if QT_CONFIG(shortcut)
        actionCentering->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\320\230\320\275\321\201\321\202\321\200\321\203\320\272\321\206\320\270\321\217", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \321\201\320\265\321\202\320\272\321\203", nullptr));
        checkButton->setText(QCoreApplication::translate("MainWindow", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214 \321\201\320\265\321\207\320\265\320\275\320\270\320\265", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \321\201\320\265\321\207\320\265\320\275\320\270\320\265", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_section_change), QCoreApplication::translate("MainWindow", "\320\240\320\265\320\264\320\260\320\272\321\202\320\276\321\200 \321\201\320\265\321\207\320\265\320\275\320\270\320\271", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\342\200\224 \320\272\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \320\264\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\321\213\321\205 \321\201\320\265\321\207\320\265\320\275\320\270\320\271 \320\274\320\265\320\266\320\264\321\203 \320\272\320\260\320\266\320\264\320\276\320\271 \320\277\320\260\321\200\320\276\320\271 \321\201\320\265\321\207\320\265\320\275\320\270\320\271", nullptr));
        showTubeButton->setText(QCoreApplication::translate("MainWindow", "\320\222\320\270\320\267\321\203\320\260\320\273\320\270\320\267\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \320\261\320\260\320\267\320\276\320\262\321\203\321\216 \321\202\321\200\321\203\320\261\320\272\321\203", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_sections), QCoreApplication::translate("MainWindow", "\320\241\320\277\320\270\321\201\320\276\320\272 \321\201\320\265\321\207\320\265\320\275\320\270\320\271", nullptr));
        backButton->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\267\320\260\320\264", nullptr));
        actionSaveTube1->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        ver_backpushButton->setText(QCoreApplication::translate("MainWindow", "\320\275\320\260\320\267\320\260\320\264", nullptr));
        ver_forvardpushButton->setText(QCoreApplication::translate("MainWindow", "\320\262\320\277\320\265\321\200\320\265\320\264", nullptr));
        surfaceModeCheckBox->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\262\320\265\321\200\321\205\320\275\320\276\321\201\321\202\320\275\320\260\321\217 \320\274\320\276\320\264\320\265\320\273\321\214", nullptr));
        pointAddCheckBox->setText(QCoreApplication::translate("MainWindow", "\320\240\320\265\320\266\320\270\320\274 \320\264\320\265\321\204\320\276\321\200\320\274\320\260\321\206\320\270\320\270", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\276\321\200\320\264\320\270\320\275\320\260\321\202\320\260 Z \321\202\320\276\321\207\320\272\320\270 \320\275\320\260 \320\272\321\200\320\270\320\262\320\276\320\271", nullptr));
        pointAddBushButton->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \321\202\320\276\321\207\320\272\321\203 \320\275\320\260 \320\272\321\200\320\270\320\262\320\276\320\271", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\276\321\200\320\264\320\270\320\275\320\260\321\202\321\213 \320\272\320\276\320\275\320\265\321\207\320\275\320\276\320\271 \321\202\320\276\321\207\320\272\320\270", nullptr));
        secPointAddPushButton->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\272\320\276\320\275\320\265\321\207\320\275\321\203\321\216 \321\202\320\276\321\207\320\272\321\203", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\240\320\260\320\264\320\270\321\203\321\201 \320\262\320\273\320\270\321\217\320\275\320\270\321\217", nullptr));
        deformationpushButton->setText(QCoreApplication::translate("MainWindow", "\320\240\320\265\320\260\320\273\320\270\320\267\320\276\320\262\320\260\321\202\321\214 \320\264\320\265\321\204\320\276\321\200\320\274\320\260\321\206\320\270\321\216", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "\320\222\320\270\320\267\321\203\320\260\320\273\320\270\320\267\320\260\321\206\320\270\321\217 \321\202\321\200\321\203\320\261\320\272\320\270", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&\320\244\320\260\320\271\320\273", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "&\320\222\320\270\320\264", nullptr));
        menuGridSize->setTitle(QCoreApplication::translate("MainWindow", "&\320\240\320\260\320\267\320\274\320\265\321\200 \321\201\320\265\321\202\320\272\320\270", nullptr));
        menuSectionColor->setTitle(QCoreApplication::translate("MainWindow", "&\320\246\320\262\320\265\321\202 \321\201\320\265\321\207\320\265\320\275\320\270\320\271", nullptr));
        menuTools->setTitle(QCoreApplication::translate("MainWindow", "&\320\230\320\275\321\201\321\202\321\200\321\203\320\274\320\265\320\275\321\202\321\213", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "&\320\237\320\276\320\274\320\276\321\211\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
