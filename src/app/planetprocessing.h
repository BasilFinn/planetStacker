#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <utility> //make_pair
#include "dataqueue.h"
#include "iprocessing.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing(Iprocessing* host);
    ~PlanetProcessing();
    void executeProcessing();
    bool startProcessing();
    pair<double, cv::Mat> processThread();
    bool loadRaw(void);
    void savePath(string path);
    bool makeRefFrame();
    bool stackFrames();
    bool sharpenFrame();
    double getContrast(cv::Mat img);

    DataQueue<cv::Mat> m_data_raw;
    vector<pair<double, cv::Mat>> m_data_crop;
    cv::Mat m_outMat;
    cv::Mat m_stackedFrame;
    Iprocessing* m_host;
    std::thread m_t_proc;
    bool m_processingDone;

    int m_rangeRows, m_rangeCols;
    int m_nThreads = 6;
    int m_noFrames;
    int m_frameCnt;
    int m_width = 400;  // Cols
    int m_height = 200; // rows
    int m_sharp_gauss=3;
    double m_sharp_weightOrg=1.5;
    double m_sharp_weightBlurr=-0.5;
    double m_stackCorrThres = 1;

    // load ref image in middle (to show fov
    // process async (load img, find max, crop region, register to refFrame, pushback to vector)

private:
    string m_dataPath;
    cv::Mat m_refFrame;
};

#endif // PLANETPROCESSING_H
