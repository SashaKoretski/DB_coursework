#include "mainwindow.h"
#include "databasemanager.h"
#include "tuberepository.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QCloseEvent>
#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    on_menuColorTheme_triggered(true);

    gridEditor = new GridEditor(this);
    if (ui->gridContainer) {
        if (ui->gridContainer->layout()) {
            ui->gridContainer->layout()->addWidget(gridEditor);
        } else {
            QVBoxLayout *layout = new QVBoxLayout(ui->gridContainer);
            layout->addWidget(gridEditor);
        }
    }

    QWidget* container = ui->visualizationWidget;
    if (container) {
        tubeViewer = new TubeViewer(container);
        if (!container->layout()) {
            QVBoxLayout* layout = new QVBoxLayout(container);
            layout->setContentsMargins(0, 0, 0, 0);
        }
        container->layout()->addWidget(tubeViewer);
        qDebug() << "TubeViewer created and added to layout";
    }

    if (ui->scrollAreaWidgetContents->layout()) {
        delete ui->scrollAreaWidgetContents->layout();
    }
    sectionsGridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    sectionsGridLayout->setSpacing(10);

    connect(ui->horizontalSlider, &QSlider::valueChanged,
            this, &MainWindow::onGridSizeChanged);
    connect(gridEditor, &GridEditor::gridCleared,
            this, &MainWindow::resetSaveButton);

    resetSaveButton();
    ui->tabWidget->setTabEnabled(3, false);
    updateShowTubeButtonState();

    instructionTextLabel = ui->instructionTextLabel;
    populateInstructionTab();
    updateDeformationUIState();

    currentVersionId = 1;
    currentTubeId = -1;
    tubeSavedToDatabase = false;

    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.initialize("localhost", 5432, "tube_deformation", "postgres", "12345")) {
        qWarning() << "Failed to initialize database connection:" << db.getLastError();
    }

    connect(ui->deformationpushButton, &QPushButton::clicked,
            this, &MainWindow::applyTubeDeformation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setText("Предупреждение");
    msgBox.setInformativeText("Все несохраненные данные будут удалены.\nВы уверены, что хотите выйти?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");

    if (msgBox.exec() == QMessageBox::Yes) {
        event->accept();

        DatabaseManager& db = DatabaseManager::getInstance();
        if (db.isConnected()) {
            qDebug() << "Clearing database tables...";
            if (db.clearAllTables()) {
                qDebug() << "Database cleaned successfully";
            } else {
                qWarning() << "Failed to clean database:" << db.getLastError();
            }
        }
    } else {
        event->ignore();
    }
}

void MainWindow::on_menuColorTheme_triggered(bool checked)
{
    QString style;
    if (checked) {
        style = R"(
            QMainWindow {
                background-color: #2b2b2b;
            }
            QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QPushButton {
                background-color: #3b3b3b;
                border: 1px solid #505050;
                padding: 5px;
                border-radius: 3px;
            }
            QPushButton:hover {
                background-color: #454545;
            }
            QMenuBar {
                background-color: #3b3b3b;
            }
            QMenuBar::item:selected {
                background-color: #454545;
            }
            QMenu {
                background-color: #2b2b2b;
                border: 1px solid #505050;
            }
            QMenu::item:selected {
                background-color: #454545;
            }
            QTabWidget::pane {
                border: 1px solid #505050;
            }
            QTabBar::tab {
                background-color: #3b3b3b;
                border: 1px solid #505050;
                padding: 5px;
            }
            QTabBar::tab:selected {
                background-color: #2b2b2b;
            }
            QTabBar::tab:disabled {
                background-color: #252525;
                color: #666666;
                border: 1px solid #353535;
            }
            QScrollArea {
                border: 1px solid #505050;
            }
            QPushButton:disabled {
                background-color: #2b2b2b;
                color: #666666;
                border: 1px solid #404040;
            }
        )";
    } else {
        style = R"(
            QMainWindow {
                background-color: #ffffff;
            }
            QWidget {
                background-color: #ffffff;
                color: #000000;
            }
            QPushButton {
                background-color: #f0f0f0;
                border: 1px solid #c0c0c0;
                padding: 5px;
                border-radius: 3px;
            }
            QPushButton:hover {
                background-color: #e0e0e0;
            }
            QMenuBar {
                background-color: #f0f0f0;
            }
            QMenuBar::item:selected {
                background-color: #e0e0e0;
            }
            QMenu {
                background-color: #ffffff;
                border: 1px solid #c0c0c0;
            }
            QMenu::item:selected {
                background-color: #e0e0e0;
            }
            QTabWidget::pane {
                border: 1px solid #c0c0c0;
            }
            QTabBar::tab {
                background-color: #f0f0f0;
                border: 1px solid #c0c0c0;
                padding: 5px;
            }
            QTabBar::tab:selected {
                background-color: #ffffff;
            }
            QTabBar::tab:disabled {
                background-color: #f5f5f5;
                color: #b0b0b0;
                border: 1px solid #d5d5d5;
            }
            QScrollArea {
                border: 1px solid #c0c0c0;
            }
            QPushButton:disabled {
                background-color: #e0e0e0;
                color: #a0a0a0;
                border: 1px solid #d0d0d0;
            }
        )";
    }
    qApp->setStyleSheet(style);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    int cellSize = 10 + value * 40 / 100;
    gridEditor->setCellSize(cellSize);
}

void MainWindow::resetCheckButton()
{
    ui->checkButton->setStyleSheet("");
    ui->checkButton->setText("Проверить сечение");
    ui->checkButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
}

void MainWindow::onGridSizeChanged(int value)
{
    int cellSize = 3 + value * 15 / 100;
    gridEditor->setCellSize(cellSize);
    resetCheckButton();
}

void MainWindow::on_checkButton_clicked()
{
    if (gridEditor->getPoints().empty()) {
        QMessageBox::warning(this, "Ошибка", "Сечение не нарисовано!");
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Как замкнуть сечение?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "Уже нарисованы все точки");
    msgBox.setButtonText(QMessageBox::No, "Закончить автоматически");

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        gridEditor->addClosingEdge();
    } else {
        gridEditor->drawLineBetweenPoints(
            gridEditor->getLastPoint(),
            gridEditor->getFirstPoint()
            );
    }

    QString errorMsg;
    if (gridEditor->hasGeometryErrors(errorMsg)) {
        QMessageBox::critical(this, "Ошибка",
                              errorMsg + "\nСечение не может быть сохранено.");
        return;
    }

    Section newSection(0);
    newSection.points = gridEditor->getPoints();
    tube.addSection(newSection);

    ui->checkButton->setStyleSheet("background-color: #90EE90;");
    ui->checkButton->setText("Проверка пройдена");
    ui->checkButton->setEnabled(false);
    ui->saveButton->setEnabled(true);
}

void MainWindow::on_clearButton_clicked()
{
    gridEditor->clearGrid();
    resetCheckButton();
}

void MainWindow::resetSaveButton()
{
    ui->saveButton->setEnabled(false);
}

void MainWindow::on_saveButton_clicked()
{
    qDebug() << "Save button clicked";


    auto points = gridEditor->getPoints();
    qDebug() << "Number of points:" << points.size();

    Section newSection(tube.getSections().size());
    newSection.points = points;


    qDebug() << "Section created with" << newSection.points.size() << "points";

    tube.addSection(newSection);
    qDebug() << "Section added to tube";

    addSectionFrame(newSection);
    qDebug() << "Frame should be added";


    gridEditor->clearGrid();
    resetCheckButton();
    resetSaveButton();
}

void MainWindow::addSectionFrame(const Section& section)
{
    qDebug() << "Starting addSectionFrame";

    if (!sectionsGridLayout) {
        qDebug() << "Error: sectionsGridLayout is null!";
        return;
    }


    int maxZ = -100;
    for (const auto& frame : sectionFrames) {
        maxZ = std::max(maxZ, frame->getZCoordinate());
    }

    SectionFrame *frame = new SectionFrame(section, ui->scrollAreaWidgetContents);
    frame->setZCoordinate(maxZ + 100);
    qDebug() << "Frame created with Z =" << maxZ + 100;

    connect(frame, &SectionFrame::zCoordinateChanged,
            this, &MainWindow::onSectionZChanged);
    connect(frame, &SectionFrame::sectionDuplicated,
            this, &MainWindow::onSectionDuplicated);
    connect(frame, &SectionFrame::sectionDeleted,
            this, &MainWindow::onSectionDeleted);

    int row = sectionFrames.size() / 3;
    int col = sectionFrames.size() % 3;
    qDebug() << "Adding frame to grid at position" << row << col;


    if (ui->actionBlackColor->isChecked())
        frame->sectionView->setColor(Qt::black);
    else if (ui->actionBlueColor->isChecked())
        frame->sectionView->setColor(Qt::blue);
    else if (ui->actionGreenColor->isChecked())
        frame->sectionView->setColor(Qt::green);
    else if (ui->actionRedColor->isChecked())
        frame->sectionView->setColor(Qt::red);

    frame->setZVisible(ui->actionShowZ->isChecked());
    frame->setDiameterVisible(ui->actionShowDiameter->isChecked());

    sectionsGridLayout->addWidget(frame, row, col);
    sectionFrames.append(frame);
    updateShowTubeButtonState();
}

void MainWindow::onSectionZChanged(SectionFrame* frame, int newZ)
{

    for (SectionFrame* existingFrame : sectionFrames) {
        if (existingFrame != frame && existingFrame->getZCoordinate() == newZ) {
            QMessageBox::warning(this, "Ошибка",
                                 "Сечение с такой Z-координатой уже существует!");
            return;
        }
    }


    frame->setZCoordinate(newZ);
    updateSectionFramesOrder();
}

void MainWindow::updateSectionFramesOrder()
{

    std::sort(sectionFrames.begin(), sectionFrames.end(),
              [](SectionFrame* a, SectionFrame* b) {
                  return a->getZCoordinate() < b->getZCoordinate();
              });


    for (int i = 0; i < sectionFrames.size(); ++i) {
        int row = i / 3;
        int col = i % 3;


        sectionsGridLayout->removeWidget(sectionFrames[i]);
        sectionsGridLayout->addWidget(sectionFrames[i], row, col);
    }
}

void MainWindow::onSectionDuplicated(SectionFrame* frame)
{
    int currentIndex = sectionFrames.indexOf(frame);


    Section newSection = frame->getSection();


    for (int i = sectionFrames.size() - 1; i > currentIndex; --i) {
        sectionFrames[i]->setZCoordinate(sectionFrames[i]->getZCoordinate() + 100);
    }


    SectionFrame *newFrame = new SectionFrame(newSection, ui->scrollAreaWidgetContents);
    newFrame->setZCoordinate(frame->getZCoordinate() + 100);


    if (ui->actionBlackColor->isChecked())
        newFrame->sectionView->setColor(Qt::black);
    else if (ui->actionBlueColor->isChecked())
        newFrame->sectionView->setColor(Qt::blue);
    else if (ui->actionGreenColor->isChecked())
        newFrame->sectionView->setColor(Qt::green);
    else if (ui->actionRedColor->isChecked())
        newFrame->sectionView->setColor(Qt::red);


    connect(newFrame, &SectionFrame::zCoordinateChanged,
            this, &MainWindow::onSectionZChanged);
    connect(newFrame, &SectionFrame::sectionDuplicated,
            this, &MainWindow::onSectionDuplicated);
    connect(newFrame, &SectionFrame::sectionDeleted,
            this, &MainWindow::onSectionDeleted);


    sectionFrames.insert(currentIndex + 1, newFrame);

    updateSectionFramesOrder();
}

void MainWindow::onSectionDeleted(SectionFrame* frame)
{

    int currentIndex = sectionFrames.indexOf(frame);


    sectionsGridLayout->removeWidget(frame);
    sectionFrames.removeAt(currentIndex);

    frame->deleteLater();
    updateSectionFramesOrder();
}

void MainWindow::fixDuplicateZ()
{
    bool hasChanges;
    do {
        hasChanges = false;
        for (int i = 0; i < sectionFrames.size(); ++i) {
            for (int j = i + 1; j < sectionFrames.size(); ++j) {
                if (sectionFrames[i]->getZCoordinate() == sectionFrames[j]->getZCoordinate()) {

                    sectionFrames[j]->setZCoordinate(sectionFrames[j]->getZCoordinate() + 100);
                    hasChanges = true;
                }
            }
        }
    } while (hasChanges);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
    if (index == 1 && firstVisitToTab2) {
        QMessageBox::information(this, "Правила построения сечения",
                                 "1. Сечение должно быть замкнутым\n"
                                 "2. Рёбра сечения не должны пересекаться\n"
                                 "3. Не должно быть повторяющихся точек\n"
                                 "4. Рёбра не должны проходить через точки\n"
                                 "5. Сечение нужно рисовать, удерживая левую кнопку мыши, не отпуская её до завершения\n"
                                 "6. При нажатии кнопки 'Проверить сечение' нужно будет сделать выбор:\n"
                                 "• если выбрать \"все точки нарисованы\", то первая и последняя точки будут соединены ребром;\n"
                                 "• если выбрать \"завершить автоматически\", первая и последняя точки соединятся прямой линией, добавив в сечение все точки, пересеченные этой линией.");
        firstVisitToTab2 = false;
    }
    else if (index == 3 && firstVisitToTab4) {
        QMessageBox::information(this, "Управление визуализацией",
                                 "Управление камерой:\n"
                                 "• Поворот: зажмите ЛКМ и двигайте мышь\n"
                                 "• Масштаб: прокрутка колесика мыши\n"
                                 "• Движение: WASD + Пробел(вверх)/Ctrl(вниз)\n\n"
                                 "Режимы отображения:\n"
                                 "• Каркасный/поверхностный - переключатель справа");
        firstVisitToTab4 = false;
    }
}

void MainWindow::on_showTubeButton_clicked()
{
    qDebug() << "Starting visualization...";

    QElapsedTimer timer;
    timer.start();

    float centerX = 0;
    float centerY = 0;


    for (auto frame : sectionFrames) {
        Section section = frame->getSection();


        float currentCenterX = 0, currentCenterY = 0;
        for (const auto& point : section.points) {
            currentCenterX += point.x;
            currentCenterY += point.y;
        }
        currentCenterX /= section.points.size();
        currentCenterY /= section.points.size();


        float dx = centerX - currentCenterX;
        float dy = centerY - currentCenterY;

        for (auto& point : section.points) {
            point.x += dx;
            point.y += dy;
        }


        frame->setSection(section);
    }


    std::vector<Section> sections;
    for (const auto& frame : sectionFrames) {
        Section section = frame->getSection();

        for (auto& point : section.points) {
            point.z = frame->getZCoordinate();
        }
        qDebug() << "Processing section with" << section.points.size() << "points at Z =" << frame->getZCoordinate();
        sections.push_back(section);
    }


    if (sectionFrames.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Необходимо минимум два сечения!");
        return;
    }


    std::sort(sections.begin(), sections.end(),
              [](const Section& a, const Section& b) {
                  return a.points[0].z < b.points[0].z;
              });


    tube = Tube();
    for (const auto& section : sections) {
        tube.addSection(section);
    }


    if (!tube.buildAllSegments()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось построить сегменты трубки!");
        return;
    }


    int numIntermediateSections = ui->spinBox_more_sections->value();
    qDebug() << "Number of intermediate sections requested:" << numIntermediateSections;

    if (numIntermediateSections > 0) {
        qDebug() << "Starting to add intermediate sections by dividing segment edges";


        std::vector<Section> allSections;


        for (size_t segIdx = 0; segIdx < tube.getSegmentCount(); ++segIdx) {

            if (segIdx == 0) {
                allSections.push_back(tube.getSection(static_cast<int>(segIdx + 1)));
            }

            const Segment& segment = tube.getSegment(static_cast<int>(segIdx + 1));
            size_t numEdges = segment.getConnectingEdgeCount();

            qDebug() << "Processing segment" << segIdx + 1 << "with" << numEdges << "edges";

            if (numEdges == 0) {
                qDebug() << "WARNING: Segment" << segIdx + 1 << "has no edges, skipping";
                continue;
            }


            for (int intermediateIdx = 1; intermediateIdx <= numIntermediateSections; ++intermediateIdx) {

                float t = static_cast<float>(intermediateIdx) / (numIntermediateSections + 1);

                Section intermediateSection(0);
                intermediateSection.points.reserve(numEdges);


                for (size_t edgeIdx = 0; edgeIdx < numEdges; ++edgeIdx) {
                    const Edge& edge = segment.getConnectingEdge(static_cast<int>(edgeIdx + 1));


                    Point3D startPoint = edge.getStartPoint();
                    Point3D endPoint = edge.getEndPoint();


                    Point3D interpolatedPoint;
                    interpolatedPoint.x = startPoint.x + t * (endPoint.x - startPoint.x);
                    interpolatedPoint.y = startPoint.y + t * (endPoint.y - startPoint.y);
                    interpolatedPoint.z = startPoint.z + t * (endPoint.z - startPoint.z);


                    intermediateSection.points.push_back(interpolatedPoint);
                }


                allSections.push_back(intermediateSection);

                qDebug() << "Created intermediate section" << intermediateIdx << "with"
                         << intermediateSection.points.size() << "points at t =" << t;
            }


            allSections.push_back(tube.getSection(static_cast<int>(segIdx + 2)));
        }

        qDebug() << "Total sections after adding intermediate:" << allSections.size();
        qDebug() << "Original sections:" << tube.getSectionCount();
        qDebug() << "Added intermediate sections:" << (allSections.size() - tube.getSectionCount());


        Tube newTube;
        for (const auto& section : allSections) {
            newTube.addSection(section);
        }


        if (!newTube.buildAllSegments()) {
            QMessageBox::warning(this, "Ошибка",
                                 "Не удалось построить сегменты для трубки с промежуточными сечениями!");
            return;
        }


        tube = newTube;

        QMessageBox::information(this, "Информация",
                                 QString("Добавлено %1 промежуточных сечений между каждой парой сечений.\n"
                                         "Общее количество сечений: %2")
                                     .arg(numIntermediateSections)
                                     .arg(tube.getSectionCount()));
    }



    Tube::TubeConstructionResult tubeResult = tube.buildMesh();
    if (!tubeResult.success) {
        QString message = "Невозможно построить некоторые участки трубки.\n"
                          "Рекомендуется добавить промежуточные сечения между:\n\n";
        for (const auto& pair : tubeResult.problematicSections) {
            message += QString("Сечениями %1 и %2\n")
                           .arg(pair.first)
                           .arg(pair.second);
        }
        QMessageBox::warning(this, "Предупреждение", message);
        return;
    }

    qDebug() << "Time to construct tube geometry: " << timer.nsecsElapsed() / 1000000.0 << "mc";
    timer.restart();


    if (tubeViewer) {
        if (!sectionFrames.empty()) {
            QColor color = sectionFrames[0]->sectionView->getCurrentColor();
            tubeViewer->setColor(color);
        }
        tubeViewer->setTubeMesh(tubeResult.mesh);
    }


    std::vector<Point3D> centersCurve = tube.getCentersCurve();
    tubeViewer->setCentersCurve(centersCurve);


    ui->tabWidget->setTabEnabled(3, true);
    ui->tabWidget->setCurrentIndex(3);

    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(1, false);

    if (deformationMode) {
        updateMaxTubeZ();
    }

    if (tubeResult.success) {
        bool saved = saveTubeToDatabase(tube);
        if (saved) {
            QMessageBox::information(this, "Успех",
                                     "Трубка визуализирована и сохранена в БД!");
            tubeSavedToDatabase = true;
        } else {
            QMessageBox::warning(this, "Ошибка",
                                 "Трубка визуализирована, но не удалось сохранить в БД:\n" +
                                     DatabaseManager::getInstance().getLastError());
        }
    }

    qDebug() << "Time to case visualisation: " << timer.nsecsElapsed() / 1000000.0 << "mc";
    qDebug() << "Visualization completed";
}

void MainWindow::on_backButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Предупреждение");
    msgBox.setInformativeText("При возврате назад все изменения будут удалены. Вы уверены?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {

        ui->tabWidget->setTabEnabled(2, true);
        ui->tabWidget->setTabEnabled(1, true);


        ui->tabWidget->setCurrentIndex(2);


        ui->tabWidget->setTabEnabled(3, false);
    }
}

void MainWindow::updateShowTubeButtonState()
{
    ui->showTubeButton->setEnabled(!sectionFrames.isEmpty());
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionShowZ_triggered(bool checked)
{
    for (SectionFrame* frame : sectionFrames) {
        frame->setZVisible(checked);
    }
}

void MainWindow::on_actionShowDiameter_triggered(bool checked)
{
    for (SectionFrame* frame : sectionFrames) {
        frame->setDiameterVisible(checked);
    }
}

void MainWindow::on_actionGridSmall_triggered()
{
    ui->horizontalSlider->setValue(0);
    gridEditor->setCellSize(GRID_SIZE_SMALL);
}

void MainWindow::on_actionGridMedium_triggered()
{
    ui->horizontalSlider->setValue(50);
    gridEditor->setCellSize(GRID_SIZE_MEDIUM);
}

void MainWindow::on_actionGridLarge_triggered()
{
    ui->horizontalSlider->setValue(100);
    gridEditor->setCellSize(GRID_SIZE_LARGE);
}

void MainWindow::updateSectionsColor(const QColor& color)
{
    for (SectionFrame* frame : sectionFrames) {
        frame->sectionView->setColor(color);
    }
}

void MainWindow::on_actionBlackColor_triggered()
{
    ui->actionBlueColor->setChecked(false);
    ui->actionGreenColor->setChecked(false);
    ui->actionRedColor->setChecked(false);
    ui->actionBlackColor->setChecked(true);
    updateSectionsColor(Qt::black);
}

void MainWindow::on_actionBlueColor_triggered()
{
    ui->actionBlackColor->setChecked(false);
    ui->actionGreenColor->setChecked(false);
    ui->actionRedColor->setChecked(false);
    ui->actionBlueColor->setChecked(true);
    updateSectionsColor(Qt::blue);
}

void MainWindow::on_actionGreenColor_triggered()
{
    ui->actionBlackColor->setChecked(false);
    ui->actionBlueColor->setChecked(false);
    ui->actionRedColor->setChecked(false);
    ui->actionGreenColor->setChecked(true);
    updateSectionsColor(Qt::green);
}

void MainWindow::on_actionRedColor_triggered()
{
    ui->actionBlackColor->setChecked(false);
    ui->actionBlueColor->setChecked(false);
    ui->actionGreenColor->setChecked(false);
    ui->actionRedColor->setChecked(true);
    updateSectionsColor(Qt::red);
}

void MainWindow::on_actionAlignZ_triggered()
{
    if (sectionFrames.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет сечений для выравнивания!");
        return;
    }

    bool ok;
    int distance = QInputDialog::getInt(this, "Выравнивание Z-координат",
                                        "Введите расстояние между сечениями:",
                                        100,
                                        1,
                                        10000,
                                        1,
                                        &ok);

    if (!ok) return;

    QMessageBox confirmBox;
    confirmBox.setText("Подтверждение");
    confirmBox.setInformativeText("Вы уверены, что хотите изменить Z-координаты всех сечений?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setButtonText(QMessageBox::Yes, "Да");
    confirmBox.setButtonText(QMessageBox::No, "Нет");

    if (confirmBox.exec() == QMessageBox::Yes) {

        QVector<SectionFrame*> orderedFrames = sectionFrames;


        for (int i = 0; i < orderedFrames.size(); ++i) {
            orderedFrames[i]->setZCoordinate(i * distance);
        }


        updateSectionFramesOrder();
    }
}

void MainWindow::on_actionOptimizeSizes_triggered()
{
    if (sectionFrames.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет сечений для изменения!");
        return;
    }

    bool ok;
    float diameter = QInputDialog::getDouble(this, "Оптимизация размеров",
                                             "Введите новый диаметр для всех сечений:",
                                             100.0,
                                             0.1,
                                             10000.0,
                                             2,
                                             &ok);

    if (!ok) return;

    QMessageBox confirmBox;
    confirmBox.setText("Подтверждение");
    confirmBox.setInformativeText("Вы уверены, что хотите изменить диаметр всех сечений?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setButtonText(QMessageBox::Yes, "Да");
    confirmBox.setButtonText(QMessageBox::No, "Нет");

    if (confirmBox.exec() == QMessageBox::Yes) {

        for (auto frame : sectionFrames) {
            frame->scaleSectionToNewDiameter(diameter);
        }
    }
}

void MainWindow::on_actionUserGuide_triggered()
{

    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_actionHotkeys_triggered()
{
    QString hotkeysText =
        "Доступные горячие клавиши:\n\n"
        "Ctrl+N - Новый проект\n"
        "Ctrl+O - Загрузить сечения\n"
        "Ctrl+S - Сохранить сечения\n"
        "Ctrl+Shift+S - Сохранение трубки\n"
        "Alt+F4 - Выход\n\n"
        "Ctrl+Y - Выравнивание Z-координат\n"
        "Ctrl+D - Оптимизация размеров\n"
        "Ctrl+W - Центрирование";

    QMessageBox::information(this, "Горячие клавиши", hotkeysText);
}

void MainWindow::on_actionAbout_triggered()
{
    QString aboutText =
        "Курсовая работа по компьютерной графике\n"
        "Приложение для создания и визуализации трубок\n\n"
        "Разработчик: Корецкий Александр\n"
        "Группа: ИУ7-55Б";

    QMessageBox::about(this, "О программе", aboutText);
}

void MainWindow::populateInstructionTab() {
    QString instructionText =
        "<h2>Инструкция по использованию программы</h2>"

        "<h3>1. Начало работы</h3>"
        "<h4>1.1. Запуск нового проекта</h4>"
        "<ul>"
        "<li>Запустите программу</li>"
        "<li>Выберите \"Файл\" → \"Новый проект\" или используйте горячую клавишу Ctrl+N</li>"
        "<li>Программа очистит все текущие данные и подготовит чистый проект</li>"
        "</ul>"

        "<h4>1.2. Интерфейс программы</h4>"
        "<p>Программа состоит из четырех основных вкладок:</p>"
        "<ul>"
        "<li>Инструкция</li>"
        "<li>Редактор сечений</li>"
        "<li>Список сечений</li>"
        "<li>Визуализация</li>"
        "</ul>"

        "<h3>2. Создание сечений</h3>"
        "<h4>2.1. Настройка сетки</h4>"
        "<ul>"
        "<li>Используйте горизонтальный слайдер для изменения размера ячеек сетки</li>"
        "<li>Доступны три предустановленных размера (через меню):"
        "<ul>"
        "<li>Маленькая сетка</li>"
        "<li>Средняя сетка</li>"
        "<li>Большая сетка</li>"
        "</ul>"
        "</li>"
        "</ul>"

        "<h4>2.2. Рисование сечения</h4>"
        "<ol>"
        "<li>Нажмите левую кнопку мыши в начальной точке сечения</li>"
        "<li>Удерживая кнопку, перемещайте курсор для рисования контура</li>"
        "<li>Отпустите кнопку мыши, когда закончите рисование</li>"
        "<li>При проверке сечения появится диалоговое окно с двумя вариантами замыкания контура:"
        "<ul>"
        "<li>\"Уже нарисованы все точки\" - просто соединит первую и последнюю точки прямой линией</li>"
        "<li>\"Закончить автоматически\" - добавит промежуточные точки между первой и последней точкой по алгоритму Брезенхема, обеспечивая плавное соединение</li>"
        "</ul>"
        "</li>"
        "</ol>"

        "<h4>2.3. Проверка и сохранение сечения</h4>"
        "<ol>"
        "<li>Нажмите кнопку \"Проверить сечение\"</li>"
        "<li>Программа проверит:"
        "<ul>"
        "<li>Отсутствие пересекающихся рёбер</li>"
        "<li>Отсутствие дублирующихся точек</li>"
        "</ul>"
        "</li>"
        "<li>При успешной проверке:"
        "<ul>"
        "<li>Кнопка станет зеленой</li>"
        "<li>Станет доступна кнопка \"Сохранить сечение\"</li>"
        "</ul>"
        "</li>"
        "<li>Нажмите \"Сохранить сечение\" для добавления сечения в проект</li>"
        "</ol>"

        "<h4>2.4. Очистка сетки</h4>"
        "<ul>"
        "<li>Нажмите кнопку \"Очистить сетку\" для удаления текущего рисунка</li>"
        "<li>Также сетка очищается автоматически после сохранения сечения</li>"
        "</ul>"

        "<h3>3. Работа со списком сечений</h3>"
        "<h4>3.1. Управление сечениями</h4>"
        "<p>Каждое сечение отображается как миниатюра с информацией:</p>"
        "<ul>"
        "<li>Z-координата</li>"
        "<li>Диаметр сечения</li>"
        "</ul>"

        "<h4>3.2. Операции с сечениями</h4>"
        "<p>Щелкните правой кнопкой мыши на сечении для доступа к контекстному меню:</p>"
        "<ul>"
        "<li>Дублировать сечение</li>"
        "<li>Удалить сечение</li>"
        "<li>Изменить Z-координату (доступно по двойному щелчку на значении Z)</li>"
        "<li>Изменить диаметр (доступно по двойному щелчку на значении диаметра)</li>"
        "</ul>"

        "<h4>3.3. Настройка отображения сечений</h4>"
        "<p>Через меню доступны опции:</p>"
        "<ul>"
        "<li>Показать/скрыть Z-координаты</li>"
        "<li>Показать/скрыть диаметры</li>"
        "<li>Выбор цвета отображения (черный, синий, зеленый, красный)</li>"
        "</ul>"

        "<h4>3.4. Поворот сечений</h4>"
        "<ul>"
        "<li>Удерживайте Shift и используйте колесико мыши для поворота сечения</li>"
        "</ul>"

        "<h3>4. Визуализация трубки</h3>"
        "<h4>4.1. Создание 3D модели</h4>"
        "<ol>"
        "<li>Убедитесь, что создано минимум два сечения</li>"
        "<li>Нажмите кнопку \"Визуализировать трубку\"</li>"
        "</ol>"

        "<h4>4.2. Управление просмотром</h4>"
        "<ul>"
        "<li>Вращение камеры:"
        "<ul>"
        "<li>Удерживайте левую кнопку мыши и перемещайте курсор для поворота камеры</li>"
        "<li>Перемещение мыши влево/вправо поворачивает камеру по горизонтали</li>"
        "<li>Перемещение мыши вверх/вниз поворачивает камеру по вертикали</li>"
        "</ul>"
        "</li>"
        "<li>Перемещение камеры:"
        "<ul>"
        "<li>W - движение вперед в направлении взгляда</li>"
        "<li>S - движение назад</li>"
        "<li>A - движение влево</li>"
        "<li>D - движение вправо</li>"
        "<li>Пробел - подъем вверх</li>"
        "<li>Control - спуск вниз</li>"
        "</ul>"
        "</li>"
        "<li>Масштабирование: прокрутка колесика мыши приближает или отдаляет модель</li>"
        "<li>Переключение режимов отображения: установите/снимите флажок \"Поверхностный режим\" для переключения между каркасным и поверхностным отображением модели</li>"
        "</ul>"

        "<h4>4.3. Возврат к редактированию</h4>"
        "<ul>"
        "<li>Нажмите кнопку \"Назад\" для возврата к списку сечений</li>"
        "<li>Программа запросит подтверждение, так как все изменения в визуализации будут потеряны</li>"
        "</ul>"

        "<h3>5. Дополнительные инструменты</h3>"
        "<h4>5.1. Выравнивание Z-координат (Ctrl+Y)</h4>"
        "<ul>"
        "<li>Автоматическое распределение сечений по Z-координате</li>"
        "<li>Позволяет задать равномерное расстояние между сечениями</li>"
        "</ul>"

        "<h4>5.2. Оптимизация размеров (Ctrl+D)</h4>"
        "<ul>"
        "<li>Установка одинакового диаметра для всех сечений</li>"
        "<li>Помогает создать более равномерную трубку</li>"
        "</ul>"

        "<h4>5.3. Центрирование (Ctrl+W)</h4>"
        "<ul>"
        "<li>Выравнивание всех сечений относительно общего центра</li>"
        "<li>Улучшает качество итоговой визуализации</li>"
        "</ul>"

        "<h3>6. Горячие клавиши</h3>"
        "<ul>"
        "<li>Ctrl+N - Новый проект</li>"
        "<li>Ctrl+O - Загрузить сечения</li>"
        "<li>Ctrl+S - Сохранить сечения</li>"
        "<li>Ctrl+Shift+S - Сохранение трубки</li>"
        "<li>Ctrl+Y - Выравнивание Z-координат</li>"
        "<li>Ctrl+D - Оптимизация размеров</li>"
        "<li>Ctrl+W - Центрирование</li>"
        "<li>Alt+F4 - Выход</li>"
        "</ul>"

        "<h3>7. Сохранение и загрузка</h3>"
        "<h4>7.1. Сохранение сечений</h4>"
        "<ol>"
        "<li>Выберите \"Файл\" → \"Сохранить сечения\" (Ctrl+S)</li>"
        "<li>Выберите директорию для сохранения</li>"
        "<li>Введите название проекта</li>"
        "<li>Программа создаст папку с файлами сечений</li>"
        "</ol>"

        "<h4>7.2. Сохранение трубки</h4>"
        "<ol>"
        "<li>Выберите \"Файл\" → \"Сохранение трубки\" (Ctrl+Shift+S)</li>"
        "<li>Выберите директорию для сохранения</li>"
        "<li>Введите название проекта</li>"
        "<li>Программа сохранит:"
        "<ul>"
        "<li>Каркасную модель (tube_wireframe.txt)</li>"
        "<li>Поверхностную модель (tube_solid.txt)</li>"
        "</ul>"
        "</li>"
        "</ol>"

        "<h4>7.3. Загрузка сечений</h4>"
        "<ol>"
        "<li>Выберите \"Файл\" → \"Загрузить сечения\" (Ctrl+O)</li>"
        "<li>Выберите файлы сечений</li>"
        "<li>Укажите расстояние до существующих сечений (если они есть)</li>"
        "<li>Сечения будут добавлены в проект</li>"
        "</ol>";

    instructionTextLabel->setHtml(instructionText);
}

void MainWindow::on_actionNewProject_triggered()
{
    if (sectionFrames.isEmpty()) {
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Предупреждение");
    msgBox.setInformativeText("Все несохраненные данные будут удалены.\nВы уверены, что хотите начать новый проект?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");

    if (msgBox.exec() == QMessageBox::Yes) {

        for (auto frame : sectionFrames) {
            sectionsGridLayout->removeWidget(frame);
            delete frame;
        }
        sectionFrames.clear();


        gridEditor->clearGrid();


        ui->tabWidget->setTabEnabled(3, false);


        ui->tabWidget->setCurrentIndex(1);


        updateShowTubeButtonState();
    }
}

void MainWindow::on_actionSaveSections_triggered()
{
    if (sectionFrames.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет сечений для сохранения!");
        return;
    }


    QString baseDir = QFileDialog::getExistingDirectory(this, "Выберите папку для сохранения");
    if (baseDir.isEmpty()) return;


    bool ok;
    QString projectName = QInputDialog::getText(this, "Сохранение сечений",
                                                "Введите название проекта:",
                                                QLineEdit::Normal,
                                                "Project", &ok);
    if (!ok || projectName.isEmpty()) return;


    QDir dir(baseDir);
    if (!dir.mkdir(projectName)) {
        if (!dir.exists(projectName)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось создать директорию проекта!");
            return;
        }
    }
    dir.cd(projectName);


    for (int i = 0; i < sectionFrames.size(); ++i) {
        QString filename = QString("section_%1.txt").arg(i + 1, 3, 10, QChar('0'));
        saveSectionToFile(sectionFrames[i], dir.filePath(filename));
    }

    QMessageBox::information(this, "Успех",
                             QString("Сечения успешно сохранены в папку:\n%1").arg(dir.absolutePath()));
}

void MainWindow::on_actionSaveTube_triggered()
{
    saveCurrentTube();
}

void MainWindow::on_actionSaveTube1_clicked()
{
    saveCurrentTube();
}

void MainWindow::saveCurrentTube()
{
    if (tubeViewer->getTubeMesh().vertices.empty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для сохранения трубки!");
        return;
    }


    QString baseDir = QFileDialog::getExistingDirectory(this, "Выберите папку для сохранения");
    if (baseDir.isEmpty()) return;


    bool ok;
    QString projectName = QInputDialog::getText(this, "Сохранение трубки",
                                                "Введите название проекта:",
                                                QLineEdit::Normal,
                                                "Project", &ok);
    if (!ok || projectName.isEmpty()) return;


    QDir dir(baseDir);
    if (!dir.mkdir(projectName)) {
        if (!dir.exists(projectName)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось создать директорию проекта!");
            return;
        }
    }
    dir.cd(projectName);







    QMessageBox::information(this, "Успех",
                             QString("Трубка успешно сохранена в папку:\n%1").arg(dir.absolutePath()));
}

void MainWindow::saveSectionToFile(const SectionFrame* frame, const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    const auto& points = frame->getSection().points;


    out << points.size() << "\n";

    for (const auto& point : points) {
        out << point.x << " " << point.y << " " << frame->getZCoordinate() << "\n";
    }


    out << points.size() << "\n";


    for (size_t i = 0; i < points.size(); ++i) {
        out << i << " " << (i + 1) % points.size() << "\n";
    }

    file.close();
}

void MainWindow::on_actionLoadSections_triggered()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this, "Выберите файлы сечений", QString(),
        "Text files (*.txt);;All files (*.*)"
        );
    if (filePaths.isEmpty()) return;

    float maxZ = 0;
    for (const auto& frame : sectionFrames) {
        maxZ = std::max(maxZ, static_cast<float>(frame->getZCoordinate()));
    }

    bool ok;
    int distance = QInputDialog::getInt(this, "Загрузка сечений",
                                        "Введите расстояние до новых сечений:",
                                        100, 1, 10000, 1, &ok);
    if (!ok) return;

    float zOffset = maxZ + distance;
    QSet<float> usedZ;
    for (const auto& frame : sectionFrames) {
        usedZ.insert(frame->getZCoordinate());
    }

    for (const QString& filePath : filePaths) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Не удалось открыть файл: %1").arg(filePath));
            continue;
        }

        QTextStream in(&file);


        int pointCount;
        in >> pointCount;


        std::vector<Point3D> points;
        float firstZ = 0;
        bool firstPoint = true;

        for (int i = 0; i < pointCount; i++) {
            float x, y, z;
            if (in.atEnd()) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Неверное устройство файла: %1\nНе хватает точек").arg(filePath));
                file.close();
                return;
            }
            in >> x >> y >> z;

            if (firstPoint) {
                firstZ = z;
                firstPoint = false;
            } else if (abs(z - firstZ) > 0.001f) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Z-координаты точек в сечении различаются: %1").arg(filePath));
                file.close();
                return;
            }

            points.push_back(Point3D(x, y, 0));
        }


        int edgeCount;
        if (in.atEnd()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверное устройство файла: %1\nОтсутствует количество ребер").arg(filePath));
            file.close();
            return;
        }
        in >> edgeCount;


        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < edgeCount; i++) {
            if (in.atEnd()) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Неверное устройство файла: %1\nНе хватает ребер").arg(filePath));
                file.close();
                return;
            }

            int v1, v2;
            in >> v1 >> v2;

            if (v1 < 0 || v1 >= pointCount || v2 < 0 || v2 >= pointCount) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Неверное устройство файла: %1\nНекорректные индексы ребер").arg(filePath));
                file.close();
                return;
            }

            edges.push_back({v1, v2});
        }


        QString remaining;
        in >> remaining;
        if (!in.atEnd()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Неверное устройство файла: %1\nЛишние данные в конце файла").arg(filePath));
            file.close();
            return;
        }

        file.close();


        Section newSection(sectionFrames.size());
        newSection.points = points;


        QString errorMsg;
        GridEditor editor;
        for (const auto& edge : edges) {
            editor.addPointAndEdge(QPoint(
                newSection.points[edge.first].x,
                newSection.points[edge.first].y
                ));
        }
        if (editor.hasGeometryErrors(errorMsg)) {
            QMessageBox::warning(this, "Ошибка геометрии",
                                 QString("Файл %1: %2").arg(filePath).arg(errorMsg));
            continue;
        }

        float newZ = zOffset + std::abs(firstZ);
        if (usedZ.contains(newZ)) {
            QMessageBox::warning(this, "Предупреждение",
                                 "Обнаружены сечения с одинаковыми Z-координатами.\n"
                                 "Рекомендуется использовать инструмент выравнивания Z-координат (Ctrl+Y)");
        }

        SectionFrame *frame = new SectionFrame(newSection, ui->scrollAreaWidgetContents);
        frame->setZCoordinate(newZ);
        usedZ.insert(newZ);

        connect(frame, &SectionFrame::zCoordinateChanged, this, &MainWindow::onSectionZChanged);
        connect(frame, &SectionFrame::sectionDuplicated, this, &MainWindow::onSectionDuplicated);
        connect(frame, &SectionFrame::sectionDeleted, this, &MainWindow::onSectionDeleted);

        frame->setZVisible(ui->actionShowZ->isChecked());
        frame->setDiameterVisible(ui->actionShowDiameter->isChecked());

        if (ui->actionBlackColor->isChecked())
            frame->sectionView->setColor(Qt::black);
        else if (ui->actionBlueColor->isChecked())
            frame->sectionView->setColor(Qt::blue);
        else if (ui->actionGreenColor->isChecked())
            frame->sectionView->setColor(Qt::green);
        else if (ui->actionRedColor->isChecked())
            frame->sectionView->setColor(Qt::red);

        int row = sectionFrames.size() / 3;
        int col = sectionFrames.size() % 3;
        sectionsGridLayout->addWidget(frame, row, col);
        sectionFrames.append(frame);
    }

    updateSectionFramesOrder();
    updateShowTubeButtonState();
}

void MainWindow::setupTubeViewer()
{
    QWidget* container = ui->visualizationWidget;
    if (container) {

        tubeViewer = new TubeViewer(container);

        if (!container->layout()) {
            QVBoxLayout* layout = new QVBoxLayout(container);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
        }

        container->layout()->addWidget(tubeViewer);

        tubeViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        qDebug() << "TubeViewer setup completed successfully";
    } else {
        qDebug() << "ERROR: Visualization container not found!";
    }
}

void MainWindow::on_surfaceModeCheckBox_toggled(bool checked)
{
    if (tubeViewer) {
        tubeViewer->setSurfaceMode(checked);
    }
}

void MainWindow::updateMaxTubeZ()
{
    maxTubeZ = 0.0f;

    if (!sectionFrames.isEmpty()) {

        for (const auto* frame : sectionFrames) {
            float z = static_cast<float>(frame->getZCoordinate());
            if (z > maxTubeZ) {
                maxTubeZ = z;
            }
        }
    }


    ui->ZdoubleSpinBox->setMinimum(0.0);
    ui->ZdoubleSpinBox->setMaximum(static_cast<double>(maxTubeZ));
    ui->ZdoubleSpinBox->setValue(0.0);

    ui->ZhorizontalSlider->setMinimum(0);
    ui->ZhorizontalSlider->setMaximum(100);
    ui->ZhorizontalSlider->setValue(0);


    if (deformationMode && tubeViewer && tubeViewer->isShowingCentersCurve()) {
        Point3D pointOnCurve = tubeViewer->getDeformationPointOnCurve(0.0f);
        tubeViewer->setDeformationPoint(pointOnCurve);
        tubeViewer->setShowDeformationPoint(true);

        tubeViewer->setDeformationPlaneZ(0.0f);
        tubeViewer->setShowDeformationPlane(true);
    }
}

void MainWindow::on_ZdoubleSpinBox_valueChanged(double value)
{
    if (!deformationMode) return;


    ui->ZhorizontalSlider->blockSignals(true);

    if (maxTubeZ > 0.0f) {

        int sliderValue = static_cast<int>((value / maxTubeZ) * 100.0);
        sliderValue = std::clamp(sliderValue, 0, 100);
        ui->ZhorizontalSlider->setValue(sliderValue);
    }

    ui->ZhorizontalSlider->blockSignals(false);


    if (tubeViewer && tubeViewer->isShowingCentersCurve()) {
        Point3D newPoint = tubeViewer->getDeformationPointOnCurve(static_cast<float>(value));
        tubeViewer->setDeformationPoint(newPoint);
        tubeViewer->setDeformationPlaneZ(static_cast<float>(value));
    }
}

void MainWindow::on_ZhorizontalSlider_valueChanged(int value)
{
    if (!deformationMode) return;


    ui->ZdoubleSpinBox->blockSignals(true);

    double zValue = 0.0;
    if (maxTubeZ > 0.0f) {

        zValue = (static_cast<double>(value) / 100.0) * maxTubeZ;
        ui->ZdoubleSpinBox->setValue(zValue);
    }

    ui->ZdoubleSpinBox->blockSignals(false);


    if (tubeViewer && tubeViewer->isShowingCentersCurve()) {
        Point3D newPoint = tubeViewer->getDeformationPointOnCurve(static_cast<float>(zValue));
        tubeViewer->setDeformationPoint(newPoint);
        tubeViewer->setDeformationPlaneZ(static_cast<float>(zValue));
    }
}

void MainWindow::updateDeformationUIState()
{

    if (!deformationMode) {
        ui->pointAddBushButton->setEnabled(false);
        ui->ZdoubleSpinBox->setEnabled(false);
        ui->ZhorizontalSlider->setEnabled(false);
        ui->secPointAddPushButton->setEnabled(false);
        ui->doubleSpinBox->setEnabled(false);
        ui->doubleSpinBox_2->setEnabled(false);
        ui->doubleSpinBox_3->setEnabled(false);
        ui->RadiusdoubleSpinBox->setEnabled(false);
        ui->deformationpushButton->setEnabled(false);
        return;
    }


    bool enableCurveSelection = !curvePointSelected || endPointSelected;
    ui->pointAddBushButton->setEnabled(enableCurveSelection);
    ui->ZdoubleSpinBox->setEnabled(enableCurveSelection);
    ui->ZhorizontalSlider->setEnabled(enableCurveSelection);


    ui->secPointAddPushButton->setEnabled(false);
    ui->doubleSpinBox->setEnabled(false);
    ui->doubleSpinBox_2->setEnabled(false);
    ui->doubleSpinBox_3->setEnabled(false);
    ui->RadiusdoubleSpinBox->setEnabled(false);
    ui->deformationpushButton->setEnabled(false);


    if (curvePointSelected && !endPointSelected) {
        ui->secPointAddPushButton->setEnabled(true);
        ui->doubleSpinBox->setEnabled(true);
        ui->doubleSpinBox_2->setEnabled(true);

    }


    if (curvePointSelected && endPointSelected) {
        ui->RadiusdoubleSpinBox->setEnabled(true);
        ui->deformationpushButton->setEnabled(true);


        ui->secPointAddPushButton->setEnabled(false);
        ui->doubleSpinBox->setEnabled(false);
        ui->doubleSpinBox_2->setEnabled(false);
    }
}


void MainWindow::updateDeformationStepState()
{
    updateDeformationUIState();


    if (curvePointSelected && !endPointSelected) {
        ui->pointAddBushButton->setText("Точка на кривой выбрана ✓");
    } else if (endPointSelected) {
        ui->pointAddBushButton->setText("Выбрать новую точку на кривой");
    } else {
        ui->pointAddBushButton->setText("Выбрать точку на кривой");
    }

    if (endPointSelected) {
        ui->secPointAddPushButton->setText("Конечная точка добавлена ✓");
    } else if (curvePointSelected) {
        ui->secPointAddPushButton->setText("Добавить конечную точку");
    } else {
        ui->secPointAddPushButton->setText("Добавить конечную точку");
    }
}


void MainWindow::on_doubleSpinBox_valueChanged(double value)
{

    if (curvePointSelected && !endPointSelected && tubeViewer) {

        float x = static_cast<float>(value);
        float y = static_cast<float>(ui->doubleSpinBox_2->value());
        float z = selectedCurvePoint.z;

        Point3D previewPoint(x, y, z);
        tubeViewer->setEndPoint(previewPoint);
        tubeViewer->setShowEndPoint(true);
    }
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double value)
{

    if (curvePointSelected && !endPointSelected && tubeViewer) {

        float x = static_cast<float>(ui->doubleSpinBox->value());
        float y = static_cast<float>(value);
        float z = selectedCurvePoint.z;

        Point3D previewPoint(x, y, z);
        tubeViewer->setEndPoint(previewPoint);
        tubeViewer->setShowEndPoint(true);
    }
}

void MainWindow::on_doubleSpinBox_3_valueChanged(double value)
{

}

void MainWindow::on_pointAddBushButton_clicked()
{
    if (!deformationMode) {
        QMessageBox::warning(this, "Предупреждение", "Сначала включите режим деформации!");
        return;
    }


    if (curvePointSelected && endPointSelected) {
        resetDeformationState();
    }

    if (curvePointSelected && !endPointSelected) {
        QMessageBox::information(this, "Информация", "Сначала добавьте конечную точку!");
        return;
    }

    if (currentTubeId != -1) {
        TubeRepository repository;
        int64_t futureId = repository.getFutureTubeId(currentTubeId);

        if (futureId != -1) {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      "Подтверждение",
                                                                      "Вы находитесь не на последней версии трубки.\n"
                                                                      "При добавлении новой точки все более поздние версии будут удалены.\n"
                                                                      "Продолжить?",
                                                                      QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            }

            if (!repository.deleteFutureVersions(currentTubeId)) {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось удалить будущие версии: " + repository.getLastError());
                return;
            }

            qDebug() << "Future versions deleted successfully";
            updateVersionNavigationButtons();
        }
    }

    double zCoord = ui->ZdoubleSpinBox->value();
    selectedCurvePoint = tubeViewer->getDeformationPointOnCurve(static_cast<float>(zCoord));
    curvePointSelected = true;


    Section newSection = findCrossSectionAtZ(static_cast<float>(zCoord), currentTube);

    if (newSection.getPointCount() >= 3) {

        bool success = insertCrossSectionIntoTube(currentTube, newSection, static_cast<float>(zCoord));

        if (success) {
            qDebug() << "New cross-section successfully added at Z =" << zCoord;


            Tube::TubeConstructionResult result = currentTube.buildMesh();
            if (result.mesh.vertices.empty()) {
                QMessageBox::warning(this, "Предупреждение",
                                     "Не удалось построить сетку после добавления сечения");
            } else {
                tubeViewer->setTubeMesh(result.mesh);


                std::vector<Point3D> centersCurve;
                for (size_t i = 0; i < currentTube.getSectionCount(); ++i) {
                    centersCurve.push_back(currentTube.getSection(static_cast<int>(i + 1)).getCenter());
                }
                tubeViewer->setCentersCurve(centersCurve);
                originalCentersCurve = centersCurve;
            }
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить новое сечение");
        }
    } else {
        qDebug() << "WARNING: Generated section has insufficient points";
    }


    ui->doubleSpinBox_3->setValue(static_cast<double>(selectedCurvePoint.z));


    ui->doubleSpinBox->setMinimum(-10000.0);
    ui->doubleSpinBox->setMaximum(10000.0);
    ui->doubleSpinBox->setValue(0.0);
    ui->doubleSpinBox->setDecimals(2);
    ui->doubleSpinBox->setSingleStep(1.0);

    ui->doubleSpinBox_2->setMinimum(-10000.0);
    ui->doubleSpinBox_2->setMaximum(10000.0);
    ui->doubleSpinBox_2->setValue(0.0);
    ui->doubleSpinBox_2->setDecimals(2);
    ui->doubleSpinBox_2->setSingleStep(1.0);


    updateDeformationStepState();

    QMessageBox::information(this, "Информация",
                             QString("Точка на кривой выбрана!\nКоординаты: (%1, %2, %3)\nНовое сечение добавлено с %4 точками.\nТеперь можно добавить конечную точку.")
                                 .arg(selectedCurvePoint.x)
                                 .arg(selectedCurvePoint.y)
                                 .arg(selectedCurvePoint.z)
                                 .arg(newSection.getPointCount()));
}


void MainWindow::resetDeformationState()
{
    curvePointSelected = false;
    endPointSelected = false;
    selectedCurvePoint = Point3D(0, 0, 0);
    endPoint = Point3D(0, 0, 0);


    if (tubeViewer) {
        tubeViewer->setShowEndPoint(false);
    }


    ui->doubleSpinBox->setValue(0.0);
    ui->doubleSpinBox_2->setValue(0.0);
    ui->doubleSpinBox_3->setValue(0.0);


    ui->doubleSpinBox->setMinimum(0.0);
    ui->doubleSpinBox->setMaximum(99.99);
    ui->doubleSpinBox_2->setMinimum(0.0);
    ui->doubleSpinBox_2->setMaximum(99.99);


    ui->RadiusdoubleSpinBox->setValue(0.5);

    updateDeformationStepState();
}

void MainWindow::on_pointAddCheckBox_toggled(bool checked)
{
    deformationMode = checked;

    if (checked) {
        updateMaxTubeZ();
        resetDeformationState();


        ui->doubleSpinBox->setMinimum(0.0);
        ui->doubleSpinBox->setMaximum(99.99);
        ui->doubleSpinBox->setValue(0.0);
        ui->doubleSpinBox->setDecimals(2);
        ui->doubleSpinBox->setSingleStep(1.0);

        ui->doubleSpinBox_2->setMinimum(0.0);
        ui->doubleSpinBox_2->setMaximum(99.99);
        ui->doubleSpinBox_2->setValue(0.0);
        ui->doubleSpinBox_2->setDecimals(2);
        ui->doubleSpinBox_2->setSingleStep(1.0);

        ui->doubleSpinBox_3->setMinimum(0.0);
        ui->doubleSpinBox_3->setMaximum(99999.0);
        ui->doubleSpinBox_3->setValue(0.0);
        ui->doubleSpinBox_3->setDecimals(2);
        ui->doubleSpinBox_3->setSingleStep(1.0);

        if (tubeViewer && tubeViewer->isShowingCentersCurve()) {
            double zCoord = ui->ZdoubleSpinBox->value();
            Point3D pointOnCurve = tubeViewer->getDeformationPointOnCurve(static_cast<float>(zCoord));
            tubeViewer->setDeformationPoint(pointOnCurve);
            tubeViewer->setShowDeformationPoint(true);

            tubeViewer->setDeformationPlaneZ(static_cast<float>(zCoord));
            tubeViewer->setShowDeformationPlane(true);
        }
    } else {
        resetDeformationState();

        if (tubeViewer) {
            tubeViewer->setShowDeformationPoint(false);
            tubeViewer->setShowDeformationPlane(false);
        }
    }

    updateDeformationUIState();
}

void MainWindow::applyCurveDeformation()
{
    qDebug() << "Starting applyCurveDeformation";

    if (!tubeViewer || !curvePointSelected || !endPointSelected) {
        qDebug() << "Early return: tubeViewer=" << (tubeViewer != nullptr)
        << "curvePointSelected=" << curvePointSelected
        << "endPointSelected=" << endPointSelected;
        return;
    }

    qDebug() << "Selected curve point:" << selectedCurvePoint.x << selectedCurvePoint.y << selectedCurvePoint.z;
    qDebug() << "End point:" << endPoint.x << endPoint.y << endPoint.z;


    std::vector<Point3D> currentCurve;

    try {
        currentCurve = tubeViewer->getCentersCurve();
        originalCentersCurve = currentCurve;
        qDebug() << "Current curve size:" << currentCurve.size();


        if (currentCurve.empty() && !sectionFrames.isEmpty()) {

            std::vector<Section> sections;
            for (const auto& frame : sectionFrames) {
                sections.push_back(frame->getSection());
            }


            std::sort(sections.begin(), sections.end(),
                      [](const Section& a, const Section& b) {
                          return a.points[0].z < b.points[0].z;
                      });


            for (const auto& section : sections) {
                currentCurve.push_back(section.getCenter());
            }

            qDebug() << "Curve created from sections, size:" << currentCurve.size();


            tubeViewer->setCentersCurve(currentCurve);
        }


        for (size_t i = 0; i < currentCurve.size() && i < 5; ++i) {
            qDebug() << "Curve point" << i << ":" << currentCurve[i].x << currentCurve[i].y << currentCurve[i].z;
        }
    }
    catch (const std::exception& e) {
        qDebug() << "Exception getting curve:" << e.what();
        QMessageBox::critical(this, "Ошибка", QString("Ошибка получения кривой: %1").arg(e.what()));
        return;
    }

    if (currentCurve.empty()) {
        qDebug() << "Current curve is still empty!";
        QMessageBox::warning(this, "Ошибка", "Кривая центров не найдена! Убедитесь, что создана трубка с сечениями.");
        return;
    }


    Point3D displacement(
        endPoint.x - selectedCurvePoint.x,
        endPoint.y - selectedCurvePoint.y,
        endPoint.z - selectedCurvePoint.z
        );

    qDebug() << "Displacement vector:" << displacement.x << displacement.y << displacement.z;

    try {
        float maxDistance = currentCurve[0].z;
        for (const Point3D& point : currentCurve) {
            maxDistance = std::max(maxDistance, point.z);
        }

        double radiusCoefficient = ui->RadiusdoubleSpinBox->value();
        float globalRadius = maxDistance * static_cast<float>(radiusCoefficient);


        DeformationPoint defPoint(
            selectedCurvePoint,
            displacement,
            globalRadius,
            DeformationPoint::GAUSSIAN
            );

        qDebug() << "DeformationPoint created successfully";


        defPoint.setStrength(1.0f);
        qDebug() << "Strength set to 1.0f";


        deformationEngine.clearDeformationPoints();
        qDebug() << "Cleared previous deformation points";

        deformationEngine.addDeformationPoint(defPoint);
        qDebug() << "Added new deformation point";


        qDebug() << "About to apply deformation to curve...";
        std::vector<Point3D> deformedCurve = deformationEngine.applyDeformationToCurve(currentCurve);

        bool curveChanged = false;
        for (size_t i = 0; i < std::min(currentCurve.size(), deformedCurve.size()); ++i) {
            float dx = std::abs(currentCurve[i].x - deformedCurve[i].x);
            float dy = std::abs(currentCurve[i].y - deformedCurve[i].y);
            float dz = std::abs(currentCurve[i].z - deformedCurve[i].z);

            if (dx > 0.001f || dy > 0.001f || dz > 0.001f) {
                curveChanged = true;
                qDebug() << "Point" << i << "changed by:" << dx << dy << dz;
                break;
            }
        }

        if (!curveChanged) {
            qDebug() << "WARNING: Curve did not change after deformation!";
            qDebug() << "Source point:" << selectedCurvePoint.x << selectedCurvePoint.y << selectedCurvePoint.z;
            qDebug() << "Target point:" << endPoint.x << endPoint.y << endPoint.z;
            qDebug() << "Displacement magnitude:" << std::sqrt(displacement.x*displacement.x + displacement.y*displacement.y + displacement.z*displacement.z);
        }

        qDebug() << "Deformation applied, result size:" << deformedCurve.size();


        tubeViewer->updateCentersCurve(deformedCurve);
        qDebug() << "Visualization updated";

        QMessageBox::information(this, "Деформация применена",
                                 QString("Кривая центров успешно деформирована!\nСмещение: (%1, %2, %3)")
                                     .arg(displacement.x)
                                     .arg(displacement.y)
                                     .arg(displacement.z));
    }
    catch (const std::exception& e) {
        qDebug() << "Exception during deformation:" << e.what();
        QMessageBox::critical(this, "Ошибка", QString("Ошибка при деформации: %1").arg(e.what()));
    }

    qDebug() << "Finished applyCurveDeformation";
}


void MainWindow::on_secPointAddPushButton_clicked()
{
    if (!deformationMode || !curvePointSelected) {
        QMessageBox::warning(this, "Предупреждение", "Сначала выберите точку на кривой!");
        return;
    }

    if (endPointSelected) {
        QMessageBox::information(this, "Информация", "Конечная точка уже выбрана!");
        return;
    }


    float x = static_cast<float>(ui->doubleSpinBox->value());
    float y = static_cast<float>(ui->doubleSpinBox_2->value());
    float z = selectedCurvePoint.z;

    endPoint = Point3D(x, y, z);
    endPointSelected = true;

    if (tubeViewer) {
        tubeViewer->setShowEndPoint(false);
    }

    applyCurveDeformation();

    updateDeformationStepState();

    QMessageBox::information(this, "Информация",
                             QString("Конечная точка добавлена и деформация применена!\nКоординаты: (%1, %2, %3)")
                                 .arg(endPoint.x)
                                 .arg(endPoint.y)
                                 .arg(endPoint.z));
}

void MainWindow::on_deformationpushButton_clicked()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!curvePointSelected || !endPointSelected) {
        QMessageBox::warning(this, "Предупреждение", "Не все параметры деформации заданы!");
        return;
    }

    resetDeformationState();
    updateDeformationUIState();
    QMessageBox::information(this, "Информация", "Деформация применена! Теперь можно выбрать новую точку на кривой.");

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    qDebug() << "on_deformationpushButton_clicked выполнена за:"
             << duration.count() << "мкс ("
             << duration.count() / 1000.0 << "мс)";
}

void MainWindow::setupRadiusSpinBox()
{
    ui->RadiusdoubleSpinBox->setMinimum(0.1);
    ui->RadiusdoubleSpinBox->setMaximum(2.0);
    ui->RadiusdoubleSpinBox->setValue(0.5);
    ui->RadiusdoubleSpinBox->setDecimals(2);
    ui->RadiusdoubleSpinBox->setSingleStep(0.1);
}

void MainWindow::on_RadiusdoubleSpinBox_valueChanged(double value)
{

    if (deformationMode && curvePointSelected && endPointSelected && tubeViewer) {
        applyPreviewDeformation();
    }
}

void MainWindow::applyPreviewDeformation()
{
    if (!tubeViewer || !curvePointSelected || !endPointSelected) {
        return;
    }


    std::vector<Point3D> currentCurve = originalCentersCurve;
    if (currentCurve.empty()) {
        return;
    }


    Point3D displacement(
        endPoint.x - selectedCurvePoint.x,
        endPoint.y - selectedCurvePoint.y,
        endPoint.z - selectedCurvePoint.z
        );

    try {
        float maxDistance = currentCurve[0].z;
        for (const Point3D& point : currentCurve) {
            maxDistance = std::max(maxDistance, point.z);
        }


        double radiusCoefficient = ui->RadiusdoubleSpinBox->value();
        float globalRadius = maxDistance * static_cast<float>(radiusCoefficient);


        DeformationPoint defPoint(
            selectedCurvePoint,
            displacement,
            globalRadius,
            DeformationPoint::GAUSSIAN
            );


        defPoint.setStrength(1.0f);


        deformationEngine.clearDeformationPoints();
        deformationEngine.addDeformationPoint(defPoint);


        std::vector<Point3D> deformedCurve = deformationEngine.applyDeformationToCurve(currentCurve);


        if (!deformedCurve.empty()) {
            tubeViewer->updateCentersCurve(deformedCurve);
        }

    } catch (const std::exception& e) {
        qDebug() << "Error in preview deformation:" << e.what();
    }
}

bool MainWindow::saveTubeToDatabase(const Tube& tube)
{
    qDebug() << "MainWindow::saveTubeToDatabase - Starting save operation";

    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.isConnected()) {
        qDebug() << "Database is not connected, attempting to initialize...";
        if (!db.initialize("localhost", 5432, "tube_deformation", "postgres", "12345")) {
            qDebug() << "Failed to connect to database:" << db.getLastError();
            return false;
        }
        qDebug() << "Database connection established successfully";
    }

    TubeRepository repository;

    // Определяем ID предыдущей трубки
    int64_t previousTubeId = -1;

    // Если это не первое сохранение, получаем ID предыдущей версии
    if (currentTubeId != -1) {
        previousTubeId = currentTubeId;
        qDebug() << "Previous tube ID:" << previousTubeId;
    } else {
        qDebug() << "This is the first tube version, no previous tube ID";
    }

    qDebug() << "Calling repository.saveTube with verId:" << currentVersionId
             << "previousTubeId:" << previousTubeId;

    // Сохраняем трубку с указанием предыдущей версии
    int64_t newTubeId = repository.saveTube(tube, currentVersionId, previousTubeId);

    if (newTubeId == -1) {
        QString errorMsg = repository.getLastError();
        qDebug() << "Failed to save tube to database:" << errorMsg;
        return false;
    }

    qDebug() << "Tube saved successfully to database with id:" << newTubeId;

    // Если есть предыдущая версия, связываем трубки и их сущности
    if (previousTubeId != -1) {
        qDebug() << "Linking tube versions:" << previousTubeId << "->" << newTubeId;

        // Связываем сами трубки через future_tube_id и past_tube_id
        if (!repository.linkTubeVersions(previousTubeId, newTubeId)) {
            qDebug() << "Warning: Failed to link tube versions:" << repository.getLastError();
            // Не критично, продолжаем
        } else {
            qDebug() << "Tube versions linked successfully";
        }

        // Связываем все сущности (sections, segments, points, edges)
        if (!repository.linkVersionEntities(previousTubeId, newTubeId)) {
            qDebug() << "Warning: Failed to link version entities:" << repository.getLastError();
            // Не критично, продолжаем
        } else {
            qDebug() << "Version entities linked successfully";
        }
    }

    // Обновляем текущий ID трубки
    currentTubeId = newTubeId;
    qDebug() << "Current tube ID updated to:" << currentTubeId;

    // Обновляем кнопки навигации по версиям
    updateVersionNavigationButtons();

    return true;
}

void MainWindow::applyTubeDeformation()
{
    qDebug() << "MainWindow::applyTubeDeformation - Starting tube deformation application";

    if (tube.getSectionCount() < 2) {
        QMessageBox::warning(this, "Ошибка",
                             "Невозможно применить деформацию. Трубка должна содержать минимум 2 сечения.");
        return;
    }

    if (!deformationEngine.hasActiveDeformations()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нет активных деформаций. Сначала создайте деформацию, выбрав точки на кривой центров.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                  QString("Применить деформацию к трубке?\n\n"
                                          "Это создаст новую версию трубки (версия %1).\n"
                                          "Предыдущая версия (версия %2) будет сохранена в базе данных.")
                                      .arg(currentVersionId + 1)
                                      .arg(currentVersionId),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        qDebug() << "User cancelled deformation application";
        return;
    }

    qDebug() << "=== DEBUG: Checking tube state BEFORE deformation ===";
    qDebug() << "Tube sections count:" << tube.getSectionCount();
    qDebug() << "Tube segments count:" << tube.getSegmentCount();

    for (size_t i = 0; i < tube.getSectionCount(); ++i) {
        const Section& sec = tube.getSection(static_cast<int>(i + 1));
        Point3D center = sec.getCenter();
        qDebug() << "BEFORE deformation - Section" << (i+1)
                 << "center: (" << center.x << "," << center.y << "," << center.z << ")";
        qDebug() << "  Section" << (i+1) << "has" << sec.getPointCount() << "points";

        if (sec.getPointCount() > 0) {
            const Point3D& firstPoint = sec.getPoint(1);
            qDebug() << "  First point coordinates: ("
                     << firstPoint.x << "," << firstPoint.y << "," << firstPoint.z << ")";
        }
    }
    qDebug() << "=== END DEBUG ===";

    try {
        // ВАЖНО: Сохраняем оригинальную трубку ПЕРЕД деформацией, если еще не сохранена
        qDebug() << "Saving original tube with version ID:" << currentVersionId;

        if (!tubeSavedToDatabase) {
            if (!saveTubeToDatabase(tube)) {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось сохранить исходную трубку в базу данных перед деформацией.");
                qDebug() << "Failed to save original tube to database";
                return;
            }
            tubeSavedToDatabase = true;
            qDebug() << "Original tube saved successfully with ver_id:" << currentVersionId
                     << "tube_id:" << currentTubeId;
        }

        // Сохраняем ID предыдущей трубки ПЕРЕД деформацией
        int64_t previousTubeId = currentTubeId;
        int previousVersionId = currentVersionId;

        qDebug() << "Previous version ID:" << previousVersionId;
        qDebug() << "Previous tube ID:" << previousTubeId;
        qDebug() << "Tube sections count:" << tube.getSectionCount();
        qDebug() << "Tube segments count:" << tube.getSegmentCount();

        // Увеличиваем версию ДО применения деформации
        currentVersionId++;
        qDebug() << "Incremented version to:" << currentVersionId;

        // Применяем деформацию к трубке
        // ВНИМАНИЕ: deformationEngine.applyDeformationToTube НЕ должен изменять currentVersionId
        // Он должен только деформировать геометрию
        int versionForDeformation = currentVersionId; // Передаем новую версию
        bool success = deformationEngine.applyDeformationToTube(tube, versionForDeformation);

        if (!success) {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось применить деформацию к трубке. Проверьте консоль для деталей.");
            qDebug() << "Failed to apply deformation to tube";
            currentVersionId--; // Откатываем версию
            return;
        }

        qDebug() << "Deformation applied successfully";
        qDebug() << "Current version ID:" << currentVersionId;

        // Проверяем, что версия не изменилась неожиданно
        if (currentVersionId != previousVersionId + 1) {
            qWarning() << "WARNING: Version ID mismatch! Expected" << (previousVersionId + 1)
            << "but got" << currentVersionId;
        }

        qDebug() << "=== DEBUG: Checking tube state AFTER deformation ===";
        for (size_t i = 0; i < tube.getSectionCount(); ++i) {
            const Section& sec = tube.getSection(static_cast<int>(i + 1));
            Point3D center = sec.getCenter();
            qDebug() << "AFTER deformation - Section" << (i+1)
                     << "center: (" << center.x << "," << center.y << "," << center.z << ")";
        }
        qDebug() << "=== END DEBUG ===";

        // Обновляем визуализацию
        qDebug() << "Updating visualization...";

        Tube::TubeConstructionResult result = tube.buildMesh();

        if (!result.success) {
            QMessageBox::warning(this, "Предупреждение",
                                 "Трубка деформирована, но возникли проблемы при построении визуализации.");
            qDebug() << "Mesh construction had issues";
        }

        tubeViewer->setTubeMesh(result.mesh);

        std::vector<Point3D> newCentersCurve = tube.getCentersCurve();
        tubeViewer->updateCentersCurve(newCentersCurve);

        qDebug() << "Visualization updated";

        // Сохраняем деформированную трубку с указанием previousTubeId
        qDebug() << "Saving deformed tube to database with ver_id:" << currentVersionId;
        qDebug() << "  Previous tube ID:" << previousTubeId;

        // ВАЖНО: saveTubeToDatabase теперь использует previousTubeId внутри
        // Но мы должны убедиться, что currentTubeId еще содержит previousTubeId
        int64_t savedPreviousTubeId = currentTubeId; // Сохраняем перед вызовом saveTubeToDatabase

        if (!saveTubeToDatabase(tube)) {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось сохранить деформированную трубку в базу данных.");
            qDebug() << "Failed to save deformed tube to database";
            currentVersionId--; // Откатываем версию
            return;
        }

        qDebug() << "Deformed tube saved to database successfully";
        qDebug() << "  Previous tube ID:" << savedPreviousTubeId;
        qDebug() << "  New tube ID:" << currentTubeId;

        // saveTubeToDatabase уже вызывает linkTubeVersions и linkVersionEntities
        // Но на всякий случай проверим, что связи установлены
        qDebug() << "Verifying tube version links...";

        TubeRepository verifyRepository;
        int64_t linkedFutureId = verifyRepository.getFutureTubeId(savedPreviousTubeId);
        int64_t linkedPastId = verifyRepository.getPastTubeId(currentTubeId);

        qDebug() << "  Previous tube's future_id:" << linkedFutureId << "(expected:" << currentTubeId << ")";
        qDebug() << "  Current tube's past_id:" << linkedPastId << "(expected:" << savedPreviousTubeId << ")";

        if (linkedFutureId != currentTubeId || linkedPastId != savedPreviousTubeId) {
            qWarning() << "WARNING: Tube version links are not properly set!";
        } else {
            qDebug() << "Tube version links verified successfully";
        }

        tubeSavedToDatabase = true;
        qDebug() << "Deformed tube saved to database successfully with ver_id:" << currentVersionId;

        QMessageBox::information(this, "Успех",
                                 QString("Деформация успешно применена!\n\n"
                                         "Создана новая версия трубки (версия %1).\n"
                                         "Сечений: %2\n"
                                         "Сегментов: %3\n"
                                         "Данные сохранены в базу данных.")
                                     .arg(currentVersionId)
                                     .arg(tube.getSectionCount())
                                     .arg(tube.getSegmentCount()));

        resetDeformationState();

        qDebug() << "Tube deformation application completed successfully";

        updateVersionNavigationButtons();

    } catch (const std::exception& e) {
        qDebug() << "Exception in applyTubeDeformation:" << e.what();
        QMessageBox::critical(this, "Критическая ошибка",
                              QString("Произошла ошибка при применении деформации:\n%1").arg(e.what()));
        currentVersionId--; // Откатываем версию при ошибке
    } catch (...) {
        qDebug() << "Unknown exception in applyTubeDeformation";
        QMessageBox::critical(this, "Критическая ошибка",
                              "Произошла неизвестная ошибка при применении деформации.");
        currentVersionId--; // Откатываем версию при ошибке
    }
}

bool MainWindow::findEdgePlaneIntersection(const Point3D& p1, const Point3D& p2,
                                           float zCoord, Point3D& intersection)
{

    float z1 = p1.z;
    float z2 = p2.z;


    if ((z1 < zCoord && z2 < zCoord) || (z1 > zCoord && z2 > zCoord)) {
        return false;
    }


    const float EPSILON = 0.0001f;
    if (std::abs(z1 - zCoord) < EPSILON) {
        intersection = p1;
        return true;
    }
    if (std::abs(z2 - zCoord) < EPSILON) {
        intersection = p2;
        return true;
    }


    float t = (zCoord - z1) / (z2 - z1);

    intersection.x = p1.x + t * (p2.x - p1.x);
    intersection.y = p1.y + t * (p2.y - p1.y);
    intersection.z = zCoord;

    return true;
}

Section MainWindow::findCrossSectionAtZ(float zCoord, const Tube& tube)
{
    qDebug() << "Finding cross-section at Z =" << zCoord;

    Section newSection;
    std::vector<Point3D> intersectionPoints;


    int targetSegmentIndex = -1;
    for (size_t i = 0; i < tube.getSegmentCount(); ++i) {
        const Segment& segment = tube.getSegment(static_cast<int>(i + 1));


        int startIdx = segment.getStartSectionIndex();
        int endIdx = segment.getEndSectionIndex();

        const Section& startSection = tube.getSection(startIdx);
        const Section& endSection = tube.getSection(endIdx);

        float startZ = startSection.getCenter().z;
        float endZ = endSection.getCenter().z;


        if ((startZ <= zCoord && zCoord <= endZ) ||
            (endZ <= zCoord && zCoord <= startZ)) {
            targetSegmentIndex = static_cast<int>(i + 1);

            qDebug() << "Found target segment:" << targetSegmentIndex
                     << "between sections" << startIdx << "and" << endIdx;


            for (size_t j = 0; j < segment.getConnectingEdgeCount(); ++j) {
                const Edge& edge = segment.getConnectingEdge(static_cast<int>(j + 1));

                Point3D startPoint = edge.getStartPoint();
                Point3D endPoint = edge.getEndPoint();

                Point3D intersection;
                if (findEdgePlaneIntersection(startPoint, endPoint, zCoord, intersection)) {
                    intersectionPoints.push_back(intersection);

                    qDebug() << "Found intersection point at ("
                             << intersection.x << ","
                             << intersection.y << ","
                             << intersection.z << ")";
                }
            }

            break;
        }
    }

    if (targetSegmentIndex == -1) {
        qDebug() << "ERROR: Could not find segment containing Z =" << zCoord;
        return newSection;
    }

    if (intersectionPoints.empty()) {
        qDebug() << "ERROR: No intersection points found";
        return newSection;
    }

    qDebug() << "Found" << intersectionPoints.size() << "intersection points";


    Point3D center(0, 0, zCoord);
    for (const auto& pt : intersectionPoints) {
        center.x += pt.x;
        center.y += pt.y;
    }
    center.x /= intersectionPoints.size();
    center.y /= intersectionPoints.size();


    std::sort(intersectionPoints.begin(), intersectionPoints.end(),
              [&center](const Point3D& a, const Point3D& b) {
                  float angleA = std::atan2(a.y - center.y, a.x - center.x);
                  float angleB = std::atan2(b.y - center.y, b.x - center.x);
                  return angleA < angleB;
              });


    newSection.sectionIndex = 0;
    for (const auto& pt : intersectionPoints) {
        newSection.addPoint(pt);
    }

    qDebug() << "Created new section with" << newSection.getPointCount() << "points";
    qDebug() << "Section center:" << newSection.getCenter().x
             << "," << newSection.getCenter().y
             << "," << newSection.getCenter().z;

    return newSection;
}

bool MainWindow::insertCrossSectionIntoTube(Tube& tube, const Section& newSection, float zCoord)
{
    qDebug() << "Inserting cross-section into tube at Z =" << zCoord;

    if (newSection.getPointCount() < 3) {
        qDebug() << "ERROR: New section has insufficient points:" << newSection.getPointCount();
        return false;
    }


    int insertPosition = -1;
    for (size_t i = 0; i < tube.getSectionCount(); ++i) {
        const Section& section = tube.getSection(static_cast<int>(i + 1));
        float sectionZ = section.getCenter().z;

        if (sectionZ > zCoord) {
            insertPosition = static_cast<int>(i);
            break;
        }
    }


    if (insertPosition == -1) {
        insertPosition = static_cast<int>(tube.getSectionCount());
    }

    qDebug() << "Insert position:" << insertPosition;


    Section sectionToInsert = newSection;
    sectionToInsert.sectionIndex = insertPosition + 1;


    for (size_t i = insertPosition; i < tube.getSectionCount(); ++i) {
        Section& section = tube.getSection(static_cast<int>(i + 1));
        section.sectionIndex++;
    }


    tube.sections.insert(tube.sections.begin() + insertPosition, sectionToInsert);

    qDebug() << "Section inserted. Total sections:" << tube.getSectionCount();


    tube.clearSegments();
    bool success = tube.buildAllSegments();

    if (!success) {
        qDebug() << "ERROR: Failed to rebuild segments after insertion";
        return false;
    }

    qDebug() << "Segments rebuilt successfully. Total segments:" << tube.getSegmentCount();


    currentVersionId++;
    TubeRepository tubeRepository;
    int64_t tubeId = tubeRepository.saveTube(tube, currentVersionId, -1);
    if (tubeId != -1) {
        currentTubeId = tubeId;
        qDebug() << "Tube saved successfully to database with id:" << tubeId;
    }

    if (tubeId == -1) {
        qDebug() << "ERROR: Failed to save tube to database";
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось сохранить обновлённую трубку в базу данных!");
        return false;
    }

    qDebug() << "Tube saved to database with ID:" << tubeId;

    return true;
}



void MainWindow::on_ver_backpushButton_clicked()
{
    qDebug() << "ver_back clicked - Loading previous tube version";

    if (currentTubeId == -1) {
        qDebug() << "No current tube loaded";
        return;
    }

    TubeRepository repository;
    int64_t pastTubeId = repository.getPastTubeId(currentTubeId);

    if (pastTubeId == -1) {
        qDebug() << "No previous version available";
        QMessageBox::information(this, "Информация", "Нет предыдущей версии трубки");
        return;
    }

    Tube loadedTube;
    if (!repository.loadTubeById(pastTubeId, loadedTube)) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось загрузить предыдущую версию: " + repository.getLastError());
        return;
    }


    tube = loadedTube;
    currentTubeId = pastTubeId;


    DatabaseManager& db = DatabaseManager::getInstance();
    QString query = QString("SELECT ver_id FROM tube WHERE id = %1").arg(pastTubeId);
    QSqlQuery result = db.executeQuery(query);
    if (result.next()) {
        currentVersionId = result.value(0).toInt();
    }


    Tube::TubeConstructionResult meshResult = tube.buildMesh();
    if (meshResult.success) {
        tubeViewer->setTubeMesh(meshResult.mesh);
        std::vector<Point3D> centersCurve = tube.getCentersCurve();
        tubeViewer->updateCentersCurve(centersCurve);
    }


    updateVersionNavigationButtons();

    qDebug() << "Loaded previous version. Current tube_id:" << currentTubeId
             << "ver_id:" << currentVersionId;
}

void MainWindow::on_ver_forvardpushButton_clicked()
{
    qDebug() << "ver_forward clicked - Loading next tube version";

    if (currentTubeId == -1) {
        qDebug() << "No current tube loaded";
        return;
    }

    TubeRepository repository;
    int64_t futureTubeId = repository.getFutureTubeId(currentTubeId);

    if (futureTubeId == -1) {
        qDebug() << "No next version available";
        QMessageBox::information(this, "Информация", "Нет следующей версии трубки");
        return;
    }

    Tube loadedTube;
    if (!repository.loadTubeById(futureTubeId, loadedTube)) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось загрузить следующую версию: " + repository.getLastError());
        return;
    }


    tube = loadedTube;
    currentTubeId = futureTubeId;


    DatabaseManager& db = DatabaseManager::getInstance();
    QString query = QString("SELECT ver_id FROM tube WHERE id = %1").arg(futureTubeId);
    QSqlQuery result = db.executeQuery(query);
    if (result.next()) {
        currentVersionId = result.value(0).toInt();
    }


    Tube::TubeConstructionResult meshResult = tube.buildMesh();
    if (meshResult.success) {
        tubeViewer->setTubeMesh(meshResult.mesh);
        std::vector<Point3D> centersCurve = tube.getCentersCurve();
        tubeViewer->updateCentersCurve(centersCurve);
    }


    updateVersionNavigationButtons();

    qDebug() << "Loaded next version. Current tube_id:" << currentTubeId
             << "ver_id:" << currentVersionId;
}

void MainWindow::updateVersionNavigationButtons()
{
    if (currentTubeId == -1) {
        ui->ver_backpushButton->setEnabled(false);
        ui->ver_forvardpushButton->setEnabled(false);
        return;
    }

    TubeRepository repository;


    int64_t pastId = repository.getPastTubeId(currentTubeId);
    ui->ver_backpushButton->setEnabled(pastId != -1);


    int64_t futureId = repository.getFutureTubeId(currentTubeId);
    ui->ver_forvardpushButton->setEnabled(futureId != -1);

    qDebug() << "Version navigation buttons updated. Past available:" << (pastId != -1)
             << "Future available:" << (futureId != -1);
}
