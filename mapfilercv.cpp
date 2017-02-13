#include "mapfilercv.h"
#include <WinSock2.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <QString>
#include <QDebug>
#pragma comment(lib, "ws2_32.lib")

#define Point3f_size sizeof(Point3f)
using namespace std;
using namespace cv;
struct MP_Pose{
    float x;
    float y;
    float z;
};
MapFileRCV::MapFileRCV(QObject *parent) : QThread(parent)
{
    qRegisterMetaType<Mat>("cv::Mat");
    qRegisterMetaType<Point3f>("cv::Point3f");
    stopped = false;
}

void MapFileRCV::stop()
{
    stopped = true;
    WSACleanup();
}

void MapFileRCV::run()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return;

    FileStorage file("E:/SourceCode/MapViewer/MapViewer/odoslam.map", FileStorage::WRITE);

    //socket preparation
    struct ::sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.S_un.S_addr = ::inet_addr("10.10.10.91");
    sockAddr.sin_port = ::htons(6666);

    //data preparation
    cv::Mat KF_Pose(1, 16, CV_32FC1);
    char buffer[4*4];
    int num = -1;
    file << "KeyFrames" << "[";
    int i = 0;
    while(1){
        if(stopped) break;
        //connecting to the server
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        while (:: connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1)
        {
            qDebug() << "mapRcv is connecting to the server!" << endl;
        }
        //get a frame of localizer result
        num = recv(sock, buffer, 16*4, NULL);

        if(num == 16*4){
            std::string KF_Pose_str(buffer, 16*4);
            KF_Pose.data = (uchar*) KF_Pose_str.c_str();
            KF_Pose = KF_Pose.reshape(0, 4);
            file << "{";
            file << "Pose" << KF_Pose;
            file << "}";
        }
        else if(num == Point3f_size){
            if(i == 0){
                file << "]";
                file << "MapPoints" << "[";
            }
            i++;
            Point3f * MP_buffer = (Point3f *) buffer;
            Point3f MP;
            MP.x = MP_buffer->x;
            MP.y = MP_buffer->y;
            MP.z = MP_buffer->z;
            file << "{";
            file << "Pos" << MP;
            file << "}";
        }else if(num < Point3f_size){
            file << "]";
            qDebug()<< "end" << endl;
            file.release();
            closesocket(sock);
            break;
        }
        closesocket(sock);
    }
    bool done = true;
    emit fileRcv(done);

}

