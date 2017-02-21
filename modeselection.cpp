#include "modeselection.h"
#include <QDebug>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
struct mode{
    int fps;
    bool localization_only;
    bool saveMap;
    bool useExMap;
    bool quitAll;
    bool calib;
    bool calib_done;
};
/*
 * presentMode == 0; NONE
 * presentMode == 1; SLAM
 * presentMode == 2; LOCALIZATION_ONLY
 * presentMode == 3; quit
 * presentMode == 4; Calibration
*/
modeSelection::modeSelection(QObject *parent):QThread(parent)
{
    stopped = false;
    modeUpdated = false;
    presentMode = 0;
    quitAll = false;
}

void modeSelection::setMode(int fps, bool local_only, bool saveMap, bool useMap, bool quit, bool calib_, bool calib_done_)
{
    FPS = fps;
    localization_only = local_only;
    saveNewMap = saveMap;
    useExMap = useMap;
    quitAll = quit;
    calib_only = calib_;
    calib_done = calib_done_;
    modeUpdated = true;

}

void modeSelection::stop()
{
    stopped = true;
}

void modeSelection::run()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return;

    //socket preparation
    struct ::sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = ::inet_addr("10.10.10.91");
    sockAddr.sin_port = ::htons(6000);
    mode newMode;
    char buffer[sizeof(mode)];
    while(1){
        if(stopped) break;
        //if(!modeUpdated) continue;


        newMode.fps = FPS;
        newMode.localization_only = localization_only;
        newMode.saveMap = saveNewMap;
        newMode.useExMap = useExMap;
        newMode.quitAll = quitAll;
        newMode.calib = calib_only;
        newMode.calib_done = calib_done;

        SOCKET sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1){
            closesocket(sock);
            qDebug() << "modeSelection thread is connecting to the server!" <<  endl;
            continue;
        }

        memcpy(buffer, &newMode, sizeof(mode));
        send(sock, buffer, sizeof(mode), NULL);
        qDebug() << buffer ;
        if(localization_only)
            presentMode = 2;
        else if(quitAll)
            presentMode = 3;
        else if(calib_only)
            presentMode = 4;
        else
            presentMode = 1;
        emit modeSelected(presentMode);

        closesocket(sock);
        //if(presentMode > 0) break;
    }
    WSACleanup();
}
