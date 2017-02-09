#-------------------------------------------------
#
# Project created by QtCreator 2017-01-06T23:42:57
#
#-------------------------------------------------

QT       += core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = MapViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32{
INCLUDEPATH += D:/opencv2413/opencv/sources/build/install/include/opencv D:/opencv2413/opencv/sources/build/install/include/
Debug{
LIBS += -LD:/opencv2413/opencv/sources/build/install/x86/vc14/lib/ -lopencv_core2413d -lopencv_highgui2413d
}
#Release{
#LIBS += -LC:/opencv/build/x86/vc14/lib/ -lopencv_core249
#}
} # win32 config end

SOURCES += main.cpp\
        widget.cpp \
    mapviewwidget.cpp \
    map.cpp \
    rt_local_rcv.cpp \
    imageThread.cpp \
    mattoimage.cpp \
    modeselection.cpp \
    mapfilercv.cpp

HEADERS  += widget.h \
    mapviewwidget.h \
    map.h \
    rt_local_rcv.h \
    imageThread.h \
    mattoimage.h \
    modeselection.h \
    mapfilercv.h

FORMS    += widget.ui
LIBS += opengl32.lib
