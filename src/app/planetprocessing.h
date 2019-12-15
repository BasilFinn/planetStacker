#ifndef PLANETPROCESSING_H
#define PLANETPROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <utility>
#include "dataqueue.h"
#include "iprocessing.h"

using namespace std;
using namespace cv;

class PlanetProcessing
{
public:
    PlanetProcessing(Iprocessing* host);
    ~PlanetProcessing();
    bool startProcessing();
    void savePath(string path);
    bool sharpenFrame();
    bool stackFrames();
    pair<double, cv::Mat> processThread();

    cv::Mat m_outMat;
    cv::Mat m_stackedFrame;
    Iprocessing* m_host;
    std::thread m_t_proc;
    string m_dataPath;

    bool m_processingDone;
    int m_scaleFactor;
    int m_rangeRows, m_rangeCols;
    int m_nThreads;
    int m_noFrames;
    int m_frameCnt;
    int m_width;  // Cols
    int m_height; // rows
    int m_sharp_gauss=3;
    double m_sharp_weightOrg=1.5;
    double m_sharp_weightBlurr=-0.5;
    double m_stackCorrThres = 1;

private:
    void executeProcessing();
    double getContrast(cv::Mat img);
    bool makeRefFrame();
    bool loadRaw(void);

    cv::Mat m_refFrame;
    DataQueue<cv::Mat> m_data_raw;
    vector<pair<double, cv::Mat>> m_data_crop;

};

#endif // PLANETPROCESSING_H
