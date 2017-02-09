#ifndef MODESELECTION_H
#define MODESELECTION_H

#include <QObject>
#include <QThread>

class modeSelection : public QThread
{
    Q_OBJECT
public:
    explicit modeSelection(QObject *parent = 0);
    void setMode(int fps, bool local_only, bool saveMap, bool useMap);
    void stop();
protected:
    void run();
signals:
    void modeSelected(int mode);
private:
    bool modeUpdated;
    volatile bool stopped;
    int presentMode;
    int FPS;
    bool localization_only;
    bool saveNewMap;
    bool useExMap;
};

#endif // MODESELECTION_H
