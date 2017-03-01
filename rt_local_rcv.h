#ifndef RT_LOCAL_RCV_H
#define RT_LOCAL_RCV_H

#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <QMetaType>
using cv::Mat;
class RT_local_Rcv : public QThread
{
    Q_OBJECT
public:
    explicit RT_local_Rcv(QObject *parent = 0);
    void stop();
protected:
    void run();
signals:
    void new_local(const cv::Mat&);
private:
    volatile bool stopped;
    cv::Mat result;
public slots:
};

#endif // RT_LOCAL_RCV_H
