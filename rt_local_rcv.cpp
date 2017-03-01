#include "rt_local_rcv.h"
#include <QDebug>
#include <QFile>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
RT_local_Rcv::RT_local_Rcv(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<Mat>("cv::Mat");
    stopped = false;
}

void RT_local_Rcv::stop()
{
     stopped = true;
     WSACleanup();
}

void RT_local_Rcv::run()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return;

    //socket preparation
    struct ::sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = ::inet_addr("10.10.10.91");
    sockAddr.sin_port = ::htons(7000);

    //data preparation
    cv::Mat result_buff(1,16, CV_32FC1);
    char buffer[4*4];
    while(1){
        if(stopped) break;
        //connecting to the server
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1)
        {
            closesocket(sock);
            qDebug() << "rt_ is connecting to the server!" << endl;
            continue;
        }
        //get a frame of localizer result
        recv(sock, buffer, 16*4, NULL);
        std::string result_str(buffer, 16*4);
        result_buff.data = (uchar*) result_str.c_str();
        result_buff = result_buff.reshape(0,4);
//        for(int i = 0; i < 4; i++){
//            for(int j = 0; j < 4; j++){
//                qDebug() << result_buff.at<float>(i, j) << " ";
//            }
//        }
        result  = result_buff.clone();

        emit new_local(result);
        closesocket(sock);
    }
    stopped = false;

}
