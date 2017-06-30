#include "map.h"
#include <cassert>
using namespace cv;
using namespace std;

Map::Map()
{

}

bool Map::loadFromFile(const string &filename)
{
    mapfile = filename;
    FileStorage fs(mapfile, FileStorage::READ);

    assert(fs.isOpened());

    FileNode nodeKFs = fs["KeyFrames"];
    FileNodeIterator it = nodeKFs.begin(), itend = nodeKFs.end();
    size_pose = nodeKFs.size();
    Poses.clear();
    Poses.reserve(size_pose);
    for(; it != itend; it++) {
        FileNode nodeKF = *it;
        Mat pose;
        nodeKF["Pose"] >> pose;
        Poses.push_back(pose);
    }

    FileNode nodeMPs = fs["MapPoints"];
    it = nodeMPs.begin(), itend = nodeMPs.end();
    size_point = nodeMPs.size();
    Points.clear();
    Points.reserve(size_point);
    for(; it != itend; it++) {
        FileNode nodeMP = *it;

        Point3f pos;
        nodeMP["Pos"] >> pos;
        Points.push_back(pos);
    }

    fs.release();

    return true;
}
