#ifndef MODESELECTION_H
#define MODESELECTION_H

#include <QObject>
#include <QThread>

class modeSelection : public QThread
{
    Q_OBJECT
public:
    explicit modeSelection(QObject *parent = 0);
    void setMode(int fps, bool local_only, bool saveMap, bool useMap, bool quit, bool calib_, bool calib_done_);
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
    bool quitAll;
    bool calib_only;
    bool calib_done;
};

#endif // MODESELECTION_H
