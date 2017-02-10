#include "widget.h"
#include "ui_widget.h"
#include <QImage>
#include "mattoimage.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    lastMode = 0;
    qRegisterMetaType<Mat>("cv::Mat");
    connect(&rt_rcv_local, SIGNAL(new_local(cv::Mat)), this, SLOT(showLocal(cv::Mat)));
    connect(&imgThread, SIGNAL(newImage(cv::Mat)), this, SLOT(showNewImage(cv::Mat)));
    connect(&mode_Selection, SIGNAL(modeSelected(int)),  this, SLOT(showSystemStatus(int)));
    ui->openButton ->setEnabled(false);
    ui->quitButton->setEnabled(false);
    ui->SLAM->setEnabled(true);
    ui->LOCAL_ONLY->setEnabled(true);
    ui->cancelButton->setEnabled(true);
    QString line = "Please click SLAM or LOCAL_ONLY Button!";
    ui->statusLineEdit->setText(line);
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
    line.push_back(QString::number(result.at<float>(2,3), 'g', 6));
    ui->localLineEdit->setText(line);
}

void Widget::showNewImage(const cv::Mat &image)
{
    ui->imageLabel->clear();
    QImage qimage = MatToQImage(image);
    QImage newImage = qimage.scaled(410, 370);
    ui->imageLabel->setPixmap(QPixmap::fromImage(newImage));
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

    mode_Selection.exit();
}

void Widget::on_cancelButton_clicked()
{
    rt_rcv_local.exit();
    imgThread.exit();
    close();
}

void Widget::on_openButton_clicked()
{
    ui->widget->open();
    ui->openButton->setEnabled(false);
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
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll);
    imgThread.start();
    mode_Selection.start();
    ui->SLAM->setEnabled(false);
    ui->LOCAL_ONLY->setEnabled(false);
    ui->quitButton->setEnabled(true);
}

void Widget::on_LOCAL_ONLY_clicked()
{
    int fps = 30;
    bool local_only = true;
    bool saveMap = false;
    bool useMap = true;
    bool quitAll = false;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll);
    imgThread.start();
    mode_Selection.start();
    rt_rcv_local.start();
    ui->SLAM->setEnabled(false);
    ui->LOCAL_ONLY->setEnabled(false);
    ui->openButton ->setEnabled(true);
    ui->quitButton->setEnabled(true);
}

void Widget::on_quitButton_clicked()
{
    int fps = 30;
    bool local_only = false;
    bool saveMap = false;
    bool useMap = false;
    bool quitAll = true;
    mode_Selection.setMode(fps, local_only, saveMap, useMap, quitAll);
    mode_Selection.start();
    ui->quitButton->setEnabled(false);
}
