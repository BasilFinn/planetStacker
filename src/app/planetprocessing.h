#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include "dataqueue.h"
#include "iprocessing.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing(Iprocessing* host);
    Iprocessing* m_host;
    DataQueue<cv::Mat> data;

    void loadData();
};

#endif // PLANETPROCESSING_H
