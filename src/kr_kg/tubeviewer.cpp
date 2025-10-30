 
#include "tubeviewer.h"
#include <QMouseEvent>
#include <cmath>
#include <QKeyEvent>

TubeViewer::TubeViewer(QWidget *parent)
    : QOpenGLWidget(parent)
    , cameraPosition(0.0f, 0.0f, 500.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
    , scale(1.0f)
    , cameraDistance(5.0f)
    , cameraRotation(30.0f, -45.0f, 0.0f)
    , isMousePressed(false)
    , wireframeMode(true)
    , showCentersCurve(true)
    , showDeformationPoint(false)
    , showDeformationPlane(false)
    , deformationPlaneZ(0.0f)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    showEndPoint = false;
}

TubeViewer::~TubeViewer()
{
    makeCurrent();
    doneCurrent();
}

void TubeViewer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    float light0Pos[] = {0.0f, 1000.0f, 0.0f, 0.0f};
    float light0Amb[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float light0Diff[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float light0Spec[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);

    glEnable(GL_LIGHT1);
    float light1Pos[] = {0.0f, -1000.0f, 0.0f, 0.0f};
    float light1Amb[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float light1Diff[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float light1Spec[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1Amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);

    glEnable(GL_LIGHT2);
    float light2Pos[] = {0.0f, 0.0f, 1000.0f, 0.0f};
    float light2Amb[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float light2Diff[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float light2Spec[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light2Amb);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2Diff);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2Spec);
}

void TubeViewer::setSurfaceMode(bool enabled)
{
    qDebug() << "Switching to" << (enabled ? "surface" : "wireframe") << "mode";
    surfaceMode = enabled;
    update();
}

void TubeViewer::resizeGL(int w, int h)
{
    float aspect = float(w) / float(h ? h : 1);
    const float zNear = 0.1f, zFar = 100.0f, fov = 45.0f;

    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(fov, aspect, zNear, zFar);
}

void TubeViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = width() / (height() ? (float)height() : 1);
    float zNear = 0.1f;
    float zFar = 2000.0f;
    float fov = 45.0f;

    float f = 1.0f / tan(fov * M_PI / 360.0f);
    float perspective[16] = {
        f/aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (zFar+zNear)/(zNear-zFar), -1.0f,
        0.0f, 0.0f, (2.0f*zFar*zNear)/(zNear-zFar), 0.0f
    };

    glMultMatrixf(perspective);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QVector3D front;
    front.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.setY(sin(qDegreesToRadians(pitch)));
    front.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.normalize();

    QVector3D center = cameraPosition + front;

    QVector3D up(0.0f, 1.0f, 0.0f);
    QVector3D f2 = (center - cameraPosition).normalized();
    QVector3D s = QVector3D::crossProduct(f2, up).normalized();
    QVector3D u = QVector3D::crossProduct(s, f2);

    float view[16] = {
        s.x(),  u.x(),  -f2.x(), 0.0f,
        s.y(),  u.y(),  -f2.y(), 0.0f,
        s.z(),  u.z(),  -f2.z(), 0.0f,
        -QVector3D::dotProduct(s, cameraPosition),
        -QVector3D::dotProduct(u, cameraPosition),
        QVector3D::dotProduct(f2, cameraPosition),
        1.0f
    };

    glMultMatrixf(view);

    glScalef(scale, scale, scale);

    drawAxes();

    if (surfaceMode) {
        drawSurfaceMesh();
    } else {
        drawTubeMesh();
    }
    drawCentersCurve();
    drawDeformationPlane();
    drawDeformationPoint();
    drawEndPoint();
}

void TubeViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        isMousePressed = true;
    }
}

void TubeViewer::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePressed) {
        float sensitivity = 0.1f;
        float dx = (event->x() - lastMousePos.x()) * sensitivity;
        float dy = (lastMousePos.y() - event->y()) * sensitivity;

        yaw += dx;
        pitch += dy;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        lastMousePos = event->pos();
        update();
    }
}

void TubeViewer::keyPressEvent(QKeyEvent* event)
{
    float speed = 10.0f;

    QVector3D front;
    front.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.setY(sin(qDegreesToRadians(pitch)));
    front.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.normalize();

    QVector3D right = QVector3D::crossProduct(front, QVector3D(0.0f, 1.0f, 0.0f));
    right.normalize();


     

    switch (event->key()) {
    case 1062:
        cameraPosition += front * speed;
        break;
    case 1067:
        cameraPosition -= front * speed;
        break;
    case 1060:
        cameraPosition -= right * speed;
        break;
    case 1042:
        cameraPosition += right * speed;
        break;
    case 87:
        cameraPosition += front * speed;
        break;
    case 83:
        cameraPosition -= front * speed;
        break;
    case 65:
        cameraPosition -= right * speed;
        break;
    case 68:
        cameraPosition += right * speed;
        break;
    case Qt::Key_Space:
        cameraPosition.setY(cameraPosition.y() + speed);
        break;
    case Qt::Key_Control:
        cameraPosition.setY(cameraPosition.y() - speed);
        break;
    }
    update();

    QOpenGLWidget::keyPressEvent(event);
}

void TubeViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isMousePressed = false;
    }
}

void TubeViewer::wheelEvent(QWheelEvent* event)
{
    float delta = event->angleDelta().y() / 120.0f;
    scale *= (1.0f + delta * 0.1f);
    scale = qBound(0.1f, scale, 10.0f);
    update();
}

void TubeViewer::updateMatrices()
{
    modelViewMatrix.setToIdentity();
    modelViewMatrix.translate(0.0f, 0.0f, -cameraDistance);
    modelViewMatrix.rotate(cameraRotation.x(), 1.0f, 0.0f, 0.0f);
    modelViewMatrix.rotate(cameraRotation.y(), 0.0f, 1.0f, 0.0f);
    modelViewMatrix.rotate(cameraRotation.z(), 0.0f, 0.0f, 1.0f);
}

void TubeViewer::drawWireframe()
{
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);

    const float s = 0.5f;

    glVertex3f(-s, -s, -s);  glVertex3f(s, -s, -s);
    glVertex3f(s, -s, -s);   glVertex3f(s, s, -s);
    glVertex3f(s, s, -s);    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, -s);   glVertex3f(-s, -s, -s);

    glVertex3f(-s, -s, s);   glVertex3f(s, -s, s);
    glVertex3f(s, -s, s);    glVertex3f(s, s, s);
    glVertex3f(s, s, s);     glVertex3f(-s, s, s);
    glVertex3f(-s, s, s);    glVertex3f(-s, -s, s);

    glVertex3f(-s, -s, -s);  glVertex3f(-s, -s, s);
    glVertex3f(s, -s, -s);   glVertex3f(s, -s, s);
    glVertex3f(s, s, -s);    glVertex3f(s, s, s);
    glVertex3f(-s, s, -s);   glVertex3f(-s, s, s);

    glEnd();
}

void TubeViewer::setTubeSections(const std::vector<Section>& sections)
{
    qDebug() << "Setting tube sections in TubeViewer";
    qDebug() << "Number of sections:" << sections.size();
    for (const auto& section : sections) {
        qDebug() << "Section points:" << section.points.size();
    }

    tubeSections = sections;
    update();
}

void TubeViewer::setWireframeMode(bool wireframe)
{
    wireframeMode = wireframe;
    update();
}

void TubeViewer::drawAxes()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(300.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 300.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 300.0f);

    glEnd();
    glEnable(GL_LIGHTING);
}

void TubeViewer::drawTube()
{
    qDebug() << "Drawing tube with" << tubeSections.size() << "sections";
    if (tubeSections.empty()) {
        qDebug() << "No sections to draw";
        return;
    }

    if (tubeSections.empty()) return;

    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (const auto& section : tubeSections) {
        glBegin(GL_LINE_LOOP);
        for (const auto& point : section.points) {
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }

    for (size_t i = 0; i < tubeSections.size() - 1; ++i) {
        const auto& section1 = tubeSections[i];
        const auto& section2 = tubeSections[i + 1];

        for (size_t j = 0; j < section1.points.size(); ++j) {
            const auto& p1 = section1.points[j];

            float minDist = std::numeric_limits<float>::max();
            size_t nearestIdx = 0;

            for (size_t k = 0; k < section2.points.size(); ++k) {
                const auto& p2 = section2.points[k];
                float dist = std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2);
                if (dist < minDist) {
                    minDist = dist;
                    nearestIdx = k;
                }
            }

            glBegin(GL_LINES);
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(section2.points[nearestIdx].x,
                       section2.points[nearestIdx].y,
                       section2.points[nearestIdx].z);
            glEnd();
        }
    }
}

void TubeViewer::setTubeMesh(const Tube::TubeMesh& mesh)
{
    qDebug() << "Setting mesh in TubeViewer";
    qDebug() << "Received mesh with" << mesh.vertices.size() << "vertices and" << mesh.edges.size() << "edges";
    tubeMesh = mesh;
    update();
}

void TubeViewer::drawTubeMesh()
{
    if (tubeMesh.vertices.empty()) {
        return;
    }

    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor3f(modelColor.redF(), modelColor.greenF(), modelColor.blueF());

    glBegin(GL_LINES);
    for (const auto& edge : tubeMesh.edges) {
        if (edge.first < tubeMesh.vertices.size() && edge.second < tubeMesh.vertices.size()) {
            const auto& v1 = tubeMesh.vertices[edge.first];
            const auto& v2 = tubeMesh.vertices[edge.second];
            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void TubeViewer::drawTestVisualization()
{
    if (!testVis.enabled) return;

    glLineWidth(2.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : testVis.section1) {
        glVertex3f(p.x, p.y, 0.0f);
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : testVis.section2) {
        glVertex3f(p.x, p.y, 0.0f);
    }
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    for (const auto& edge : testVis.edges) {
        const auto& p1 = testVis.section1[edge.first];
        const auto& p2 = testVis.section2[edge.second];
        glVertex3f(p1.x, p1.y, 0.0f);
        glVertex3f(p2.x, p2.y, 0.0f);
    }
    glEnd();
}

void TubeViewer::setTestVisualization(const std::vector<Point3D>& section1,
                                      const std::vector<Point3D>& section2,
                                      const std::vector<std::pair<int, int>>& edges)
{
    testVis.section1 = section1;
    testVis.section2 = section2;
    testVis.edges = edges;
    testVis.enabled = true;
    update();
}

void TubeViewer::drawSurfaceMesh()
{
    if (tubeMesh.vertices.empty()) return;

    glEnable(GL_LIGHTING);

    float matAmb[] = {modelColor.redF() * 0.2f, modelColor.greenF() * 0.2f, modelColor.blueF() * 0.2f, 1.0f};
    float matDiff[] = {modelColor.redF() * 0.7f, modelColor.greenF() * 0.7f, modelColor.blueF() * 0.7f, 1.0f};
    float matSpec[] = {0.1f, 0.1f, 0.1f, 1.0f};
    float matShininess = 8.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

    std::vector<std::vector<Point3D>> vertexNormals(tubeMesh.vertices.size());

    for (const auto& face : tubeMesh.faces) {
        const auto& v1 = tubeMesh.vertices[face[0]];
        const auto& v2 = tubeMesh.vertices[face[1]];
        const auto& v3 = tubeMesh.vertices[face[2]];

        float ux = v2.x - v1.x;
        float uy = v2.y - v1.y;
        float uz = v2.z - v1.z;
        float vx = v3.x - v1.x;
        float vy = v3.y - v1.y;
        float vz = v3.z - v1.z;

        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;

        float len = sqrt(nx * nx + ny * ny + nz * nz);
        nx /= len;
        ny /= len;
        nz /= len;

        vertexNormals[face[0]].push_back({nx, ny, nz});
        vertexNormals[face[1]].push_back({nx, ny, nz});
        vertexNormals[face[2]].push_back({nx, ny, nz});
    }

    std::vector<Point3D> averageNormals(tubeMesh.vertices.size());
    for (size_t i = 0; i < vertexNormals.size(); ++i) {
        if (!vertexNormals[i].empty()) {
            Point3D avg{0, 0, 0};
            for (const auto& n : vertexNormals[i]) {
                avg.x += n.x;
                avg.y += n.y;
                avg.z += n.z;
            }
            float len = sqrt(avg.x * avg.x + avg.y * avg.y + avg.z * avg.z);
            avg.x /= len;
            avg.y /= len;
            avg.z /= len;
            averageNormals[i] = avg;
        }
    }

    glBegin(GL_TRIANGLES);
    for (const auto& face : tubeMesh.faces) {
        for (int i = 0; i < 3; ++i) {
            const auto& normal = averageNormals[face[i]];
            const auto& vertex = tubeMesh.vertices[face[i]];
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void TubeViewer::setCentersCurve(const std::vector<Point3D>& centers)
{
    centersCurve = centers;
    update();
}

void TubeViewer::setShowCentersCurve(bool show)
{
    showCentersCurve = show;
    update();
}

void TubeViewer::drawCentersCurve()
{
    if (!showCentersCurve || centersCurve.empty()) {
        return;
    }

    glDisable(GL_LIGHTING);
    glLineWidth(10.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_LINE_STRIP);
    for (const auto& center : centersCurve) {
        glVertex3f(center.x, center.y, center.z);
    }
    glEnd();

    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto& center : centersCurve) {
        glVertex3f(center.x, center.y, center.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void TubeViewer::setDeformationPoint(const Point3D& point)
{
    deformationPoint = point;
    update();
}

void TubeViewer::setShowDeformationPoint(bool show)
{
    showDeformationPoint = show;
    update();
}

Point3D TubeViewer::getDeformationPointOnCurve(float zCoord) const
{
    if (centersCurve.empty()) {
        return Point3D(0.0f, 0.0f, zCoord);
    }

     
    if (zCoord <= centersCurve.front().z) {
        return centersCurve.front();
    }

    if (zCoord >= centersCurve.back().z) {
        return centersCurve.back();
    }

     
    for (size_t i = 0; i < centersCurve.size() - 1; ++i) {
        const Point3D& p1 = centersCurve[i];
        const Point3D& p2 = centersCurve[i + 1];

        if (zCoord >= p1.z && zCoord <= p2.z) {
             
            float t = (zCoord - p1.z) / (p2.z - p1.z);
            Point3D result;
            result.x = p1.x + t * (p2.x - p1.x);
            result.y = p1.y + t * (p2.y - p1.y);
            result.z = zCoord;
            return result;
        }
    }

    return Point3D(0.0f, 0.0f, zCoord);
}

void TubeViewer::drawDeformationPoint()
{
    if (!showDeformationPoint) {
        return;
    }

    drawCube(deformationPoint.x, deformationPoint.y, deformationPoint.z, 5.0f);
}

void TubeViewer::drawCube(float x, float y, float z, float size)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);  

    float s = size / 2.0f;  

    glPushMatrix();
    glTranslatef(x, y, z);

     
    glLineWidth(3.0f);
    glBegin(GL_LINES);

     
    glVertex3f(-s, -s, -s);  glVertex3f(s, -s, -s);
    glVertex3f(s, -s, -s);   glVertex3f(s, s, -s);
    glVertex3f(s, s, -s);    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, -s);   glVertex3f(-s, -s, -s);

     
    glVertex3f(-s, -s, s);   glVertex3f(s, -s, s);
    glVertex3f(s, -s, s);    glVertex3f(s, s, s);
    glVertex3f(s, s, s);     glVertex3f(-s, s, s);
    glVertex3f(-s, s, s);    glVertex3f(-s, -s, s);

     
    glVertex3f(-s, -s, -s);  glVertex3f(-s, -s, s);
    glVertex3f(s, -s, -s);   glVertex3f(s, -s, s);
    glVertex3f(s, s, -s);    glVertex3f(s, s, s);
    glVertex3f(-s, s, -s);   glVertex3f(-s, s, s);

    glEnd();

     
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.0f, 0.3f);  

    glBegin(GL_QUADS);

     
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);

     
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, -s, -s);

     
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, -s);

     
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(s, -s, s);

     
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, -s);

     
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(-s, -s, s);

    glEnd();

    glDisable(GL_BLEND);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void TubeViewer::setDeformationPlaneZ(float z)
{
    deformationPlaneZ = z;
    update();
}

void TubeViewer::setShowDeformationPlane(bool show)
{
    showDeformationPlane = show;
    update();
}

void TubeViewer::drawDeformationPlane()
{
    if (!showDeformationPlane) {
        return;
    }

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     
    glColor4f(0.0f, 0.8f, 1.0f, 0.3f);

     
    float planeSize = 300.0f;

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, deformationPlaneZ);

     
    glBegin(GL_QUADS);
    glVertex3f(-planeSize, -planeSize, 0.0f);
    glVertex3f(planeSize, -planeSize, 0.0f);
    glVertex3f(planeSize, planeSize, 0.0f);
    glVertex3f(-planeSize, planeSize, 0.0f);
    glEnd();

     
    glColor4f(0.0f, 0.6f, 0.8f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-planeSize, -planeSize, 0.0f);
    glVertex3f(planeSize, -planeSize, 0.0f);
    glVertex3f(planeSize, planeSize, 0.0f);
    glVertex3f(-planeSize, planeSize, 0.0f);
    glEnd();

     
    glColor4f(0.0f, 0.6f, 0.8f, 0.4f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);

     
    for (float x = -planeSize; x <= planeSize; x += 50.0f) {
        glVertex3f(x, -planeSize, 0.0f);
        glVertex3f(x, planeSize, 0.0f);
    }

     
    for (float y = -planeSize; y <= planeSize; y += 50.0f) {
        glVertex3f(-planeSize, y, 0.0f);
        glVertex3f(planeSize, y, 0.0f);
    }

    glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void TubeViewer::setEndPoint(const Point3D& point)
{
    endPoint = point;
    update();
}

void TubeViewer::setShowEndPoint(bool show)
{
    showEndPoint = show;
    update();
}

void TubeViewer::drawEndPoint()
{
    if (!showEndPoint) {
        return;
    }

     
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);  

    drawCube(endPoint.x, endPoint.y, endPoint.z, 4.0f);

    glEnable(GL_LIGHTING);
}

void TubeViewer::updateCentersCurve(const std::vector<Point3D>& newCurve)
{
    centersCurve = newCurve;
    update();  
}
