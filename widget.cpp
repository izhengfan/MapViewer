#include "widget.h"
#include "ui_widget.h"
#include <QImage>
#include "mattoimage.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("Odoslam MapViewer");

    lastMode = 0;

    qRegisterMetaType<Mat>("cv::Mat");
    connect(&rt_rcv_local, SIGNAL(new_local(cv::Mat)), this, SLOT(showLocal(cv::Mat)));
    connect(&imgThread, SIGNAL(newImage(cv::Mat)), this, SLOT(showNewImage(cv::Mat)));
    connect(&mode_Selection, SIGNAL(modeSelected(int)),  this, SLOT(showSystemStatus(int)));
    connect(&mapRcv, SIGNAL(fileRcv(bool)), this, SLOT(showFileTranStatus(bool)));
    //ui->quitButton->setEnabled(false);
    ui->SLAM->setEnabled(true);
    ui->LOCAL_ONLY->setEnabled(true);
    ui->cancelButton->setEnabled(true);
    QString line = "Please click Calibration, SLAM or LOCAL_ONLY Button!";
    ui->statusLineEdit->setText(line);
    imgThread.start();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::showLocal(const cv::Mat &result)
{
    ui->widget->localChangedFun(result);
    ui->widget->updateGL();
    QString line;
    line.push_back(QString::number(result.at<float>(0,3), 'g', 6));
    line.push_back(" ");
    line.push_back(QString::number(result.at<float>(1,3), 'g', 6));
    line.push_back(" ");
    double theta = atan2(result.at<float>(2,1), result.at<float>(1,1));
    line.push_back(QString::number(theta, 'g', 6));
    ui->localLineEdit->setText(line);
}

void Widget::showNewImage(const cv::Mat &image)
{
    ui->imageLabel->clear();
    QImage qimage = MatToQImage(image);
    //    QImage newImage = qimage.scaled(410, 370);
    ui->imageLabel->setPixmap(QPixmap::fromImage(qimage));
    ui->imageLabel->resize(ui->imageLabel->pixmap()->size());
}

void Widget::showSystemStatus(int mode)
{
    ui->statusLineEdit->clear();
    if(mode == 1){
        QString line = "In SLAM Mode";
        ui->statusLineEdit->setText(line);
        lastMode = mode;
    }
    else if(mode == 2){
        QString line = "In LOCAL_ONLY Mode, click the OpenMap button";
        ui->statusLineEdit->setText(line);
        lastMode = mode;
    }
    else if(mode == 3){
        if(lastMode == 1){
            QString line = "SLAM quit,  and begin to receive simple_odoslam.map file";
            ui->statusLineEdit->setText(line);
        }
        else if (lastMode == 2){
            QString line = "Localizer quit.";
            ui->statusLineEdit->setText(line);
        }
    }
    else if(mode == 4){
        QString line = "In Calibration Mode";
        ui->statusLineEdit->setText(line);
        lastMode = mode;
    }
    //mode_Selection.exit();
}

void Widget::showFileTranStatus(bool done)
{
    if(done){
        mapRcv.quit();
//        mapRcv.wait();
        ui->statusLineEdit->clear();
        QString line = "Map file has been received successfully!";
        ui->statusLineEdit->setText(line);
    }
}

void Widget::on_cancelButton_clicked()
{
    //rt_rcv_local.exit();
    //    imgThread.exit();
    close();
}

void Widget::on_openButton_clicked()
{
    ui->widget->open();
    ui->statusLineEdit->clear();
    QString line = "Localizer is running!";
    ui->statusLineEdit->setText(line);
}

void Widget::on_SLAM_clicked()
{
    int fps = 30;
    bool local_only = false;
    bool saveMap = true;
    bool useMap = false;
    bool quitAll = false;
    bool calib = false;
    bool calib_done = false;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll, calib, calib_done);
    imgThread.start();
    mode_Selection.start();
    mapRcv.start();
    ui->SLAM->setEnabled(true);
    ui->LOCAL_ONLY->setEnabled(true);
    ui->quitButton->setEnabled(true);
}

void Widget::on_LOCAL_ONLY_clicked()
{
    int fps = 30;
    bool local_only = true;
    bool saveMap = false;
    bool useMap = true;
    bool quitAll = false;
    bool calib = false;
    bool calib_done = false;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll, calib, calib_done);
    imgThread.start();
    mode_Selection.start();
    rt_rcv_local.start();
    ui->SLAM->setEnabled(true);
    ui->LOCAL_ONLY->setEnabled(true);
    ui->openButton ->setEnabled(true);
    ui->quitButton->setEnabled(true);
}

void Widget::on_quitButton_clicked()
{
    bool calib ;
    if (lastMode == 4)
        calib = true;
    else
        calib = false;
    int fps = 30;
    bool local_only = false;
    bool saveMap = false;
    bool useMap = false;
    bool quitAll = true;
    bool calib_done = false;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll, calib, calib_done);
    mode_Selection.start();


    //ui->quitButton->setEnabled(false);
}

void Widget::on_CalibrationButton_clicked()
{
    int fps = 30;
    bool local_only = false;
    bool saveMap = false;
    bool useMap = false;
    bool quitAll = false;
    bool calib = true;
    bool calib_done = false;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll, calib, calib_done);
    mode_Selection.start();
    imgThread.start();
    ui->CalibrationButton->setEnabled(true);
    ui->SLAM->setEnabled(true);
    ui->LOCAL_ONLY->setEnabled(true);
    ui->openButton ->setEnabled(false);
    ui->quitButton->setEnabled(true);
}
