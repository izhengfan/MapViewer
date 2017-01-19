#include "widget.h"
#include "ui_widget.h"
#include <QImage>
#include "mattoimage.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qRegisterMetaType<Mat>("cv::Mat");
    connect(&rt_rcv_local, SIGNAL(new_local(cv::Mat)), this, SLOT(showLocal(cv::Mat)));
    connect(&imgThread, SIGNAL(newImage(cv::Mat)), this, SLOT(showNewImage(cv::Mat)));
    ui->openButton ->setEnabled(true);
    ui->cancelButton->setEnabled(true);
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

void Widget::on_cancelButton_clicked()
{
    close();
}

void Widget::on_openButton_clicked()
{
    ui->widget->open();
}

void Widget::on_localRcvButton_clicked()
{
    rt_rcv_local.start();
    ui->localRcvButton->setEnabled(false);
}

void Widget::on_ImageRcvButton_clicked()
{
    imgThread.start();
    ui->ImageRcvButton->setEnabled(false);
}
