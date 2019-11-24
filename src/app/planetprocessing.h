#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include "dataqueue.h"
#include "iprocessing.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing(Iprocessing* host);
    DataQueue<cv::Mat> data;
    void loadData();
    void savePath(string path);
    string getPath(){return m_dataPath;}

private:
    string m_dataPath;
};

#endif // PLANETPROCESSING_H
