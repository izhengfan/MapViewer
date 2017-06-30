#include "mapviewwidget.h"
#include <opencv2/opencv.hpp>

#include <QtWidgets>
#include <QtOpenGL>

using cv::Mat;
using cv::Point3f;
using std::vector;

float mCameraSize = 1;
const float mCameraLineWidth = 2;
const float mPointSize = 2;
const float mViewpointX = 0;
const float mViewpointY = -0.7;
const float mViewpointZ = -1.8;
const float mViewpointF = 500;
const float mOthorSize = 200;
float mRatio = 0.005;

const int mRotateRatio = 16;

MapViewWidget::MapViewWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    xTrans = 0;
    yTrans = 0;
    map = new Map;
    //map->loadFromFile("C:/Users/fzheng/Documents/QtProjects/MapViewer/odoslam_.map");
}

MapViewWidget::~MapViewWidget()
{
    delete map;
}

//! Make sure that `points.size() == 5`
void MapViewWidget::drawFrame(const std::vector<cv::Point3f>& points, const cv::Point3f &rgb){

    assert(points.size() == 5);

    glColor3f(rgb.x, rgb.y, rgb.z);

    for(int i = 1; i < 5; i++) {
        glVertex3f(points[0].x, points[0].y, points[0].z);
        glVertex3f(points[i].x, points[i].y, points[i].z);
    }

    for(int i = 1; i < 5; i++) {
        glVertex3f(points[i].x, points[i].y, points[i].z);
        int j = i == 4? 1 : (i+1);
        glVertex3f(points[j].x, points[j].y, points[j].z);
    }
}

QSize MapViewWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MapViewWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * mRotateRatio;
    while (angle > 360*mRotateRatio)
        angle -= 360 * mRotateRatio;
}

void MapViewWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void MapViewWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MapViewWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MapViewWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
}

void MapViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(xTrans, yTrans, -10.0);
    glRotatef(xRot / mRotateRatio, 1.0, 0.0, 0.0);
    glRotatef(yRot / mRotateRatio, 0.0, 1.0, 0.0);
    glRotatef(zRot / mRotateRatio, 0.0, 0.0, 1.0);
    draw();
}


void MapViewWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    double left = -mOthorSize;
    double right = mOthorSize;
    double bottom = -mOthorSize;
    double top = mOthorSize;
    double znear = -mOthorSize*2;
    double zfar = mOthorSize*2;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(left, right, bottom, top, znear, zfar);
#else
    glOrtho(left, right, bottom, top, znear, zfar);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void MapViewWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MapViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + mRotateRatio * dy /2);
        setYRotation(yRot + mRotateRatio * dx /2);
    } else if (event->buttons() & Qt::RightButton) {
        xTrans += static_cast<float>(dx);
        yTrans -= static_cast<float>(dy);
        updateGL();
    }

    lastPos = event->pos();
}

void MapViewWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->angleDelta().y();

    double ratio = 1.0 + static_cast<double>(delta) / 180.0;

    mRatio *= ratio;

    mCameraSize *= ratio;

    updateGL();
}

void MapViewWidget::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
    {
         map->loadFromFile(fileName.toStdString());
         updateGL();
    }
}

void MapViewWidget::draw()
{
    // draw poses
    glLineWidth(mCameraLineWidth);
    glBegin(GL_LINES);

    for(const cv::Mat& pose : map->Poses)
    {
        float d = mCameraSize;

        //Camera is a pyramid. Define in camera coordinate system
        cv::Mat o = (cv::Mat_<float>(4,1) << 0, 0, 0, 1);
        cv::Mat p1 = (cv::Mat_<float>(4,1) << d, d*0.8, d*0.5, 1);
        cv::Mat p2 = (cv::Mat_<float>(4,1) << d, -d*0.8, d*0.5, 1);
        cv::Mat p3 = (cv::Mat_<float>(4,1) << -d, -d*0.8, d*0.5, 1);
        cv::Mat p4 = (cv::Mat_<float>(4,1) << -d, d*0.8, d*0.5, 1);

        cv::Mat T = pose.inv();
        T.at<float>(0,3) = T.at<float>(0,3)*mRatio;
        T.at<float>(1,3) = T.at<float>(1,3)*mRatio;
        T.at<float>(2,3) = T.at<float>(2,3)*mRatio;
        cv::Mat ow = T*o;
        cv::Mat p1w = T*p1;
        cv::Mat p2w = T*p2;
        cv::Mat p3w = T*p3;
        cv::Mat p4w = T*p4;

        std::vector<cv::Point3f> vPoints;
        cv::Point3f ow3f(ow.at<float>(0),ow.at<float>(1),ow.at<float>(2));
        vPoints.push_back(ow3f);
        vPoints.push_back(Point3f(p1w.at<float>(0),p1w.at<float>(1),p1w.at<float>(2)));
        vPoints.push_back(Point3f(p2w.at<float>(0),p2w.at<float>(1),p2w.at<float>(2)));
        vPoints.push_back(Point3f(p3w.at<float>(0),p3w.at<float>(1),p3w.at<float>(2)));
        vPoints.push_back(Point3f(p4w.at<float>(0),p4w.at<float>(1),p4w.at<float>(2)));

        drawFrame(vPoints, Point3f(0,255,0));
    }

    // draw lines between poses
    // Warning: counting start from 1, not 0!
    for(int i=1, iend=map->Poses.size(); i<iend; i++)
    {
        const cv::Mat& pose = map->Poses[i-1];
        const cv::Mat& pose2 = map->Poses[i];

        cv::Mat o = (cv::Mat_<float>(4,1) << 0, 0, 0, 1);

        cv::Mat T = pose.inv();
        cv::Mat T2 = pose2.inv();
        T.at<float>(0,3) = T.at<float>(0,3)*mRatio;
        T.at<float>(1,3) = T.at<float>(1,3)*mRatio;
        T.at<float>(2,3) = T.at<float>(2,3)*mRatio;
        T2.at<float>(0,3) = T2.at<float>(0,3)*mRatio;
        T2.at<float>(1,3) = T2.at<float>(1,3)*mRatio;
        T2.at<float>(2,3) = T2.at<float>(2,3)*mRatio;
        cv::Mat ow = T*o;
        cv::Mat ow2 = T2*o;
        glColor3f(0, 255, 255);
        glVertex3f(ow.at<float>(0), ow.at<float>(1), ow.at<float>(2));
        glVertex3f(ow2.at<float>(0), ow2.at<float>(1), ow2.at<float>(2));

    }
    glEnd();


    // draw points
    glPointSize(mPointSize);
    glBegin(GL_POINTS);
    for(const Point3f& point : map->Points)
    {
        Point3f pos = point * mRatio;
        glColor3f(0.4,0.4,0.4);
        glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
}
