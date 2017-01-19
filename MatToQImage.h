
#ifndef MATTOQIMAGE_H
#define MATTOQIMAGE_H

#include <QtGui>
#include <QDebug>

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

QImage MatToQImage(const Mat&);

#endif
