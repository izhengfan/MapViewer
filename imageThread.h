#ifndef IMAGETHREAD_H
#define IMAGETHREAD_H

#include <QObject>
#include <QThread>
//#include "mythread.h"
#include <opencv2/opencv.hpp>
#include <QMetaType>
using cv::Mat;
class imageThread : public QThread
{
    Q_OBJECT
public:
    explicit imageThread(QObject *parent = 0);
    void stop();
protected:
    void run();
signals:
    void newImage(const cv::Mat & image);
private:
    volatile bool stopped;
    cv::Mat image;

};
#endif //IMAGETHREAD_H
