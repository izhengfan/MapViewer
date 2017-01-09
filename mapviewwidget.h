#ifndef MAPVIEWWIDGET_H
#define MAPVIEWWIDGET_H


#include <QWidget>
#include <QGLWidget>
#include "map.h"

class MapViewWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MapViewWidget(QWidget *parent = 0);
    ~MapViewWidget();
signals:

public slots:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void drawFrame(const std::vector<cv::Point3f>& points, const cv::Point3f &rgb);

public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

signals:
    // signaling rotation from mouse movement, currently not used.
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

public slots:
    void open();

private:
    void draw();

    Map* map;

    int xRot;
    int yRot;
    int zRot;

    float xTrans;
    float yTrans;

    QPoint lastPos;
};

#endif // MAPVIEWWIDGET_H
