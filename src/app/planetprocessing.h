#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include "dataqueue.h"
#include "iprocessing.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing(Iprocessing* host);
    ~PlanetProcessing();
    bool executeProcessing();
    bool startProcessing();
    cv::Mat processThread();
    bool loadRaw(void);
    void savePath(string path);

    std::shared_ptr<std::thread> m_t_proc;

    string getPath(){return m_dataPath;}
    DataQueue<cv::Mat> m_data_raw;
    vector<cv::Mat> m_data_crop;
    int m_rangeRows, m_rangeCols;
    int m_nThreads = 4;
    int m_noFrames;
    int m_frameCnt;
    // load ref image in middle (to show fov
    // process async (load img, find max, crop region, register to refFrame, pushback to vector)

private:
    string m_dataPath;
};

#endif // PLANETPROCESSING_H
