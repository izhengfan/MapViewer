#ifndef MAP_H
#define MAP_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
class Map
{
public:
    Map();

    std::vector<cv::Point3f> Points;

    std::vector<cv::Mat>     Poses;

    bool loadFromFile(const std::string &filename);
private:
    size_t size_pose;
    size_t size_point;
    std::string mapfile;

};

#endif // MAP_H
