#include "imageThread.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <WinSock2.h>
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
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return;
    //socket preparation
    struct ::sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = ::inet_addr("10.10.10.91");
    sockAddr.sin_port = ::htons(9000);

    //data preparation
    cv::Mat img(480, 640, CV_8UC1);
    int imgsize = img.total()*img.elemSize();
    char * sockData = new char[imgsize];
    int bytes = 0;
    while(1){
        if(stopped) break;
        //connecting to the server
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        while (:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1)
        {
                qDebug() << "imgThread is connecting to the server!"  << endl;
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
//        cv::imshow("Source Image", image);
//        cv::waitKey(1);
        emit newImage(image);
        closesocket(sock);
    }
    delete [] sockData;
    stopped = false;
    WSACleanup();

}


