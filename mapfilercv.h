#ifndef MAPFILERCV_H
#define MAPFILERCV_H

#include <QObject>
#include <QThread>

class MapFileRCV : public QThread
{
    Q_OBJECT
public:
    MapFileRCV();
};

#endif // MAPFILERCV_H
