#ifndef MAPFILERCV_H
#define MAPFILERCV_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMetaType>
class MapFileRCV : public QThread
{
    Q_OBJECT
public:
    explicit MapFileRCV(QObject *parent = 0);
    void stop();
protected:
    void run();
signals:
    void fileRcv(const bool&);
private:
    volatile bool stopped;
};

#endif // MAPFILERCV_H
