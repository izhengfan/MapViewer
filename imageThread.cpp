#include "imageThread.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <WinSock2.h>

#include <fstream>
#include <iostream>
//#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
imageThread::imageThread(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<Mat>("cv::Mat");
    stopped = false;

}

void imageThread::stop()
{
    stopped = true;
}

void imageThread::run()
{
    std::ofstream imgRawFile;
    imgRawFile.open("img.raws", std::ofstream::binary);

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return;
    //socket preparation
    struct ::sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = ::inet_addr("10.10.10.91");
    sockAddr.sin_port = ::htons(9000);
    //data preparation
    cv::Mat img(240, 320, CV_8UC1);
    int imgsize = img.total()*img.elemSize();
    char * sockData = new char[imgsize];
    int bytes = 0;
    SYSTEMTIME sysTime;
    while(1){
        if(stopped) break;
        //connecting to the server
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1)
        {
            closesocket(sock);
            qDebug() << "imgThread is connecting to the server!"  << endl;
            continue;
        }
        //get a frame of localizer result

        for (int i = 0; i < imgsize; i += bytes) {
            if ((bytes = recv(sock, sockData + i, imgsize  - i, NULL)) == -1) {
                quit();
            }
        }

        int ptr = 0;
        for(int i = 0; i < img.rows; i++){
            for(int j = 0; j < img.cols; j++){
                img.at<uchar>(i, j) = sockData[ptr];
                ptr++;
            }
        }
        image = img.clone();

        //Record imgTime(12 Bytes) and img(240*340 Bytes)
        GetLocalTime(&sysTime);
        imgRawFile << sysTime.wMonth
                   << sysTime.wDay
                   << sysTime.wHour
                   << sysTime.wMinute
                   << sysTime.wSecond
                   << sysTime.wMilliseconds;
        imgRawFile << image.data;
        emit newImage(image);
        closesocket(sock);
    }
    imgRawFile.close();
    delete [] sockData;
    stopped = false;
    WSACleanup();

}


