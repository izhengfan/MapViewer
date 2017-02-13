#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "rt_local_rcv.h"
#include "mapviewwidget.h"
#include <QMetaType>
#include "rt_local_rcv.h"
#include "imageThread.h"
#include "modeselection.h"
#include "mapfilercv.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:

    void showLocal(const cv::Mat &result);

    void showNewImage(const cv::Mat&image);

    void showSystemStatus(int mode);

    void showFileTranStatus(bool done);

    void on_cancelButton_clicked();

    void on_openButton_clicked();

    void on_SLAM_clicked();

    void on_LOCAL_ONLY_clicked();

    void on_quitButton_clicked();

private:
    Ui::Widget *ui;
    RT_local_Rcv rt_rcv_local;
    imageThread imgThread;
    modeSelection mode_Selection;
    MapFileRCV mapRcv;
    int lastMode;
};

#endif // WIDGET_H
