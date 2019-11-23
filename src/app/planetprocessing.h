#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include "dataqueue.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing();
    DataQueue<cv::Mat> data;

    void loadData();
};

#endif // PLANETPROCESSING_H
