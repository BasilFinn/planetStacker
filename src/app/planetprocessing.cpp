#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host): m_host(host)
{
    //host->dataReady();
}

PlanetProcessing::~PlanetProcessing()
{
    m_t_proc->join();;
}


bool PlanetProcessing::startProcessing(){
    m_t_proc = make_shared<std::thread>([this]{executeProcessing();});
    return true;
}

bool PlanetProcessing::executeProcessing()
{
    //make ref frame
    makeRefFrame();

    //    /WindowsData/Users/basil/Desktop/SharpCap Captures/2019-09-30/Saturn/
    // Check and update asyncs
    pair<double, cv::Mat> corrMat;
    std::future_status status;
    bool running = true;
    int rcnt=0;
    //TODO: set max frames or implement this in reader (while t_loadRaw running)
    m_frameCnt = 0;

    //    std::thread t_loadRaw([this]{loadRaw();});    // load raw data
    std::future<bool> asyncLoad;
    asyncLoad = std::async(std::launch::async, [this](){return loadRaw();});

    // Async init
    std::vector<std::future<pair<double, cv::Mat>>> asyncProcess(m_nThreads);
    for(auto& ap:asyncProcess)
    {
        ap = std::async(std::launch::async, [this](){return processThread();});
        m_frameCnt++;
    }

    pair<double, cv::Mat> returnV;
    while(running){
        for(int i=0; i<m_nThreads; i++)
        {
            if(asyncProcess[i].valid())
            {
                status = asyncProcess[i].wait_for(std::chrono::milliseconds(100));
                switch(status){
                case std::future_status::ready:
                    returnV = asyncProcess[i].get();
                    if(!returnV.second.empty())
                        m_data_crop.push_back(make_pair(returnV.first, returnV.second.clone()));
                    if(m_frameCnt<m_noFrames)
                    {
                        m_frameCnt++;
                        m_host->updateBar();
                        asyncProcess[i] = std::async(std::launch::async, [this](){return processThread();});
                    }
                    break;

                case std::future_status::deferred:
                    cout << "deferred: " << i <<  endl;
                    break;

                case std::future_status::timeout:
                    cout << "tiemout: " << i << endl;
                    break;

                default:
                    break;
                }
            }
            else{
                cout << "not Valid" << endl;
                rcnt = 0;
                for(auto& ap:asyncProcess)
                {
                    if(!ap.valid()){
                        rcnt++;
                        cout << "Rcnt: " << rcnt << endl;
                    }
                }
                if(rcnt==m_nThreads && asyncLoad.wait_for(std::chrono::seconds(0))==std::future_status::ready)
                {
                    cout << "All threads non Valid - all frames loaded" << endl;
                    running = false;
                    break;
                    cout << "Load status: " << asyncLoad.get() << endl;
                }
            }
        }
    }

    cout << "No. loaded frames: " << m_data_crop.size() <<  endl;

    //    imshow("First frame", m_data_crop[0]);
    //    imshow("Last frame", m_data_crop.back());

    stackFrames();
    sharpenFrame();

    m_outMat = m_stackedFrame;
    m_host->dataReady();

    return 1;
}






pair<double, cv::Mat> PlanetProcessing::processThread()
{
    cv::Mat emptyMat;
    cv::Mat frameOrg = this->m_data_raw.get();
    cv::Mat frameGray;
    cv::cvtColor(frameOrg, frameGray, CV_BGR2GRAY);
    cv::Point minLoc, maxLoc;
    double minVal, maxVal;
    const int halfCol = m_width/2;
    const int halfRow = m_height/2;

    cv::minMaxLoc(frameGray, &minVal, &maxVal, &minLoc, &maxLoc);

    //CROP FRAME
    cv::Rect roiOut;
    if(  maxLoc.x+halfCol<frameGray.cols  &&  maxLoc.x-halfCol>0  &&  maxLoc.y+halfRow<frameGray.rows  && maxLoc.y-halfRow>0){
        cv::Rect roi(maxLoc.x-halfCol, maxLoc.y-halfRow, 2*halfCol, 2*halfRow);
        roiOut = roi;
        frameGray = frameGray(roi);
    }
    else{
        cout << "Window to close to border!" << endl;
        return pair<double, cv::Mat>(0.0, emptyMat);
    }

    //REGISTER FRAME
    const int warpMode = MOTION_TRANSLATION;
    Mat warpMat = Mat::eye(2,3,CV_32F);
    int nIterations = 8000;
    double termination_eps = 1e-10; //1e-10;
    TermCriteria criteria (TermCriteria::COUNT+TermCriteria::EPS, nIterations, termination_eps);

    warpMat = Mat::eye(2,3,CV_32F);
    double corr = findTransformECC(   m_refFrame,
                                      frameGray,
                                      warpMat,
                                      warpMode,
                                      criteria);
    warpAffine(frameGray, frameGray, warpMat, frameGray.size(), INTER_LINEAR + WARP_INVERSE_MAP);
    //    return frameGray;

    cv::Mat out;
    cv::warpAffine(frameOrg(roiOut), out, warpMat, frameOrg(roiOut).size(), INTER_LINEAR + WARP_INVERSE_MAP);

    return pair<double, cv::Mat>(corr, out);
}






bool PlanetProcessing::loadRaw(void)
{
    // Loads raw data to DataQueue
    Mat tmpFrame;
    VideoCapture cap(m_dataPath);
    if(!cap.isOpened()){
        cout << "Error opening video" << endl;
        return false;
    }
    m_noFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    m_host->initBar(m_noFrames);
    cout << "Numbers: " << m_noFrames << endl;

    while(1)
    {
        cap >> tmpFrame;
        if(tmpFrame.empty())
            break;
        m_data_raw.add(tmpFrame.clone());
    }
    cap.release();
    return true;
}

void PlanetProcessing::savePath(string path)
{
    cout << "Processing: save path" << endl;
    m_dataPath = path;
}

void PlanetProcessing::makeRefFrame()
{
    Mat frameOrg;
    VideoCapture cap(m_dataPath);
    if(!cap.isOpened()){
        cout << "Error opening video" << endl;
        exit;
    }
    int nFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    cap.set(CV_CAP_PROP_POS_FRAMES, int(nFrames/2));
    cap >> frameOrg;

    cv::Mat frameGray, emptyMat;
    cv::cvtColor(frameOrg, frameGray, CV_BGR2GRAY);
    double minVal, maxVal;
    cv::Point minLoc,maxLoc;
    const int halfCol = m_width/2;
    const int halfRow = m_height/2;
    cv::minMaxLoc(frameGray, &minVal, &maxVal, &minLoc, &maxLoc);
    if(  maxLoc.x+halfCol<frameGray.cols  &&  maxLoc.x-halfCol>0  &&  maxLoc.y+halfRow<frameGray.rows  && maxLoc.y-halfRow>0){
        cv::Rect roi(maxLoc.x-halfCol, maxLoc.y-halfRow, 2*halfCol, 2*halfRow);
        frameGray = frameGray(roi);
    }
    else{
        cout << "Reference frame to close to border!" << endl;
        m_refFrame = emptyMat;
    }

    m_refFrame = frameGray;
}

void PlanetProcessing::stackFrames()
{
    int factor = 2;
    int count = 0;
    //    Mat matSum = Mat::zeros(m_data_crop[0].size(), CV_32F);
    Mat matSum = Mat::zeros(cv::Size(m_data_crop[0].second.cols*factor, m_data_crop[0].second.rows*factor), 22);// m_data_crop[0].type());
    m_corrThres = 0.997;
    for(int i=0;i<m_data_crop.size(); i++)
    {
        cout << m_data_crop[i].first << endl;
        if(m_data_crop[i].first>=m_corrThres)
        {
            cout << "good correlation\n";
            count++;
            cv::Mat rsMat = m_data_crop[i].second.clone();
            cv::resize(rsMat, rsMat, cv::Size(rsMat.cols*factor, rsMat.rows*factor));
            cv::accumulate(rsMat, matSum);
        }
    }
    cout << "N-Frames stacked: " << count << endl;

//    for(auto el:m_data_crop){
//        cv::Mat rsMat = el;
//        cv::resize(rsMat, rsMat, cv::Size(rsMat.cols*factor, rsMat.rows*factor));
//        cv::accumulate(rsMat, matSum);
//    }
    //m_stackedFrame = matSum / m_data_crop.size();
    m_stackedFrame = matSum / count;
    rotate(m_stackedFrame, m_stackedFrame, 1);
    cv::imshow("mean img", m_stackedFrame/255);
}

void PlanetProcessing::sharpenFrame()
{
    cv::Mat imgSharp;
    cv::GaussianBlur(m_stackedFrame, imgSharp, cv::Size(0, 0), 9);      // 3
    cv::addWeighted(m_stackedFrame, 3, imgSharp, -2, 0, imgSharp);  // 1.5 -0.5 0

    cv::imshow("sharp img", imgSharp/255);
}
