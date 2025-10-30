#ifndef TUBEVIEWER_H
#define TUBEVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include "tube.h"
#include "section.h"
#include "point3d.h"

class TubeViewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit TubeViewer(QWidget *parent = nullptr);
    ~TubeViewer();

     
    void setTube(const Tube& tube);
    void setTubeSections(const std::vector<Section>& sections);
    void setWireframeMode(bool wireframe);
    void setTubeMesh(const Tube::TubeMesh& mesh);

    void setColor(const QColor& color) {
        modelColor = color;
        update();
    }

    const Tube::TubeMesh& getTubeMesh() const { return tubeMesh; }

    void setSurfaceMode(bool enabled);   

    struct TestVisualization {
        std::vector<Point3D> section1;
        std::vector<Point3D> section2;
        std::vector<std::pair<int, int>> edges;
        bool enabled = false;
    };
    void setTestVisualization(const std::vector<Point3D>& section1,
                              const std::vector<Point3D>& section2,
                              const std::vector<std::pair<int, int>>& edges);

     
    void setCentersCurve(const std::vector<Point3D>& centers);
    void setShowCentersCurve(bool show);
    bool isShowingCentersCurve() const { return showCentersCurve; }

    void setDeformationPoint(const Point3D& point);
    void setShowDeformationPoint(bool show);
    Point3D getDeformationPointOnCurve(float zCoord) const;

    void setDeformationPlaneZ(float z);
    void setShowDeformationPlane(bool show);

    void setEndPoint(const Point3D& point);
    void setShowEndPoint(bool show);
    void drawEndPoint();

    void updateCentersCurve(const std::vector<Point3D>& newCurve);

    const std::vector<Point3D>& getCentersCurve() const { return centersCurve; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

     
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
     
    Tube currentTube;                
    std::vector<Section> sections;   
    std::vector<Point3D> vertices;   
    std::vector<std::pair<int, int>> edges;   

    QColor modelColor;

     
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;

     
    float cameraDistance;
    QVector3D cameraRotation;

     
    QPoint lastMousePos;
    bool isMousePressed;

     
    bool wireframeMode;

    float scale = 1.0f;

    void updateMatrices();
    void drawWireframe();
    void drawSurface();
    void drawAxes();

    std::vector<Section> tubeSections;   
    void drawTube();   

    Tube::TubeMesh tubeMesh;
    void drawTubeMesh();

    TestVisualization testVis;
    void drawTestVisualization();

    bool surfaceMode = false;   
    void drawSurfaceMesh();     

    QVector3D cameraPosition;     
    float yaw;                   
    float pitch;                 

    std::vector<Point3D> centersCurve;
    bool showCentersCurve;

    void drawCentersCurve();

    Point3D deformationPoint;
    bool showDeformationPoint;

    void drawDeformationPoint();
    void drawCube(float x, float y, float z, float size);

    float deformationPlaneZ;
    bool showDeformationPlane;

    void drawDeformationPlane();

    Point3D endPoint;
    bool showEndPoint;
};

#endif  
