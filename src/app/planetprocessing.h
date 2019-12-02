#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <tuple>
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
    void makeRefFrame();
    void stackFrames();
    void sharpenFrame();

    std::shared_ptr<std::thread> m_t_proc;

    string getPath(){return m_dataPath;}
    DataQueue<cv::Mat> m_data_raw;
    vector<cv::Mat> m_data_crop;
    vector<double> m_data_correlation;
    int m_rangeRows, m_rangeCols;
    int m_nThreads = 6;
    int m_noFrames;
    int m_frameCnt;
    int m_width = 400;  // Cols
    int m_height = 200; // rows

    // load ref image in middle (to show fov
    // process async (load img, find max, crop region, register to refFrame, pushback to vector)

private:
    string m_dataPath;
    cv::Mat m_refFrame;
    cv::Mat m_stackedFrame;
};

#endif // PLANETPROCESSING_H
