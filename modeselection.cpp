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
};
/*
 * presentMode == 0; NONE
 * presentMode == 1; SLAM
 * presentMode == 2; LOCALIZATION_ONLY
 * presentMode == 3; quit
*/
modeSelection::modeSelection(QObject *parent):QThread(parent)
{
    stopped = false;
    modeUpdated = false;
    presentMode = 0;
    quitAll = false;
}

void modeSelection::setMode(int fps, bool local_only, bool saveMap, bool useMap, bool quit)
{
    FPS = fps;
    localization_only = local_only;
    saveNewMap = saveMap;
    useExMap = useMap;
    quitAll = quit;
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

    char buffer[sizeof(mode)];
    while(1){
        if(stopped) break;
        if(!modeUpdated) continue;

        mode newMode;
        newMode.fps = FPS;
        newMode.localization_only = localization_only;
        newMode.saveMap = saveNewMap;
        newMode.useExMap = useExMap;
        newMode.quitAll = quitAll;

        SOCKET sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        while(:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1){
            qDebug() << "modeSelection thread is connecting to the server!" <<  endl;
        }

        memcpy(buffer, &newMode, sizeof(mode));
        send(sock, buffer, sizeof(mode), NULL);
        qDebug() << buffer ;
        if(localization_only)
            presentMode = 2;
        else if(quitAll)
            presentMode = 3;
        else
            presentMode = 1;
        emit modeSelected(presentMode);

        closesocket(sock);
        if(presentMode > 0) break;
    }
    WSACleanup();
}
