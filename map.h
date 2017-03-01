#ifndef MAP_H
#define MAP_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <QString>
#include <string>
class Map
{
public:
    Map();

    std::vector<cv::Point3f> Points;

    std::vector<cv::Mat>     Poses;

    bool loadFromFile(QString filename);
private:
    size_t size_pose;
    size_t size_point;

};

#endif // MAP_H
