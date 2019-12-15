#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host)
    : m_host(host), m_width(300), m_height(200), m_scaleFactor(1)
{
    m_nThreads = std::thread::hardware_concurrency()-2;     // Set maximum available threads for processing as max-2
}

PlanetProcessing::~PlanetProcessing()
{
    if(m_t_proc.joinable())
        m_t_proc.join();
}


bool PlanetProcessing::startProcessing(){
    m_t_proc = std::thread(&PlanetProcessing::executeProcessing,this);
    m_data_crop.clear();
    return true;
}

void PlanetProcessing::executeProcessing()
{
    m_processingDone = false;
    makeRefFrame();

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
                status = asyncProcess[i].wait_for(std::chrono::milliseconds(0));
                switch(status){
                case std::future_status::ready:
                    returnV = asyncProcess[i].get();
                    if(!returnV.second.empty())
                        m_data_crop.push_back(make_pair(returnV.first, returnV.second.clone()));
                    if(m_frameCnt<m_noFrames)
                    {
                        m_frameCnt++;
                        m_host->updateBar(m_frameCnt);
                        asyncProcess[i] = std::async(std::launch::async, [this](){return processThread();});
                    }
                    break;

                case std::future_status::deferred:
                    //cout << "deferred: " << i <<  endl;
                    break;

                case std::future_status::timeout:
                    //cout << "tiemout: " << i << endl;
                    break;

                default:
                    break;
                }
            }
            else{
                rcnt = 0;
                for(auto& ap:asyncProcess)
                {
                    if(!ap.valid()){
                        rcnt++;
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
    m_processingDone = true;
    cout << "No. loaded frames: " << m_data_crop.size() <<  endl;

    // Sort data_crop by correlation
    std::sort(m_data_crop.begin(), m_data_crop.end(),
              [&](pair<double, cv::Mat> &x, pair<double, cv::Mat> &y){return x.first>y.first;});

    stackFrames();
//    return 1;
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
    int nIterations = 4000;
    double termination_eps = 1e-3; //1e-10;
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
    m_data_crop.reserve(m_noFrames);
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
    if(m_t_proc.joinable())
        m_t_proc.join();
    m_dataPath = path;
}

bool PlanetProcessing::makeRefFrame()
{
    int noTestFrames = 10;
    Mat frameOrg, frameGray, emptyMat;
    double minVal, maxVal, tmpContrast, contrast=0;
    cv::Point minLoc,maxLoc;
    const int halfCol = m_width/2;
    const int halfRow = m_height/2;

    VideoCapture cap(m_dataPath);
    if(!cap.isOpened()){
        cout << "Error opening video" << endl;
        return false;
    }
    int nFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    cap.set(CV_CAP_PROP_POS_FRAMES, int(nFrames/2));

    for(int i=0; i<noTestFrames; i++)
    {
        cap >> frameOrg;
        cv::cvtColor(frameOrg, frameGray, CV_BGR2GRAY);
        cv::minMaxLoc(frameGray, &minVal, &maxVal, &minLoc, &maxLoc);
        if(  maxLoc.x+halfCol<frameGray.cols  &&  maxLoc.x-halfCol>0  &&  maxLoc.y+halfRow<frameGray.rows  && maxLoc.y-halfRow>0){
            cv::Rect roi(maxLoc.x-halfCol, maxLoc.y-halfRow, 2*halfCol, 2*halfRow);
            frameGray = frameGray(roi);
        }
        else{
            cout << "Test frame to close to border!" << endl;
            frameGray = emptyMat;
        }

        tmpContrast = getContrast(frameGray);
        if(tmpContrast>contrast)
        {
            contrast = tmpContrast;
            m_refFrame =frameGray.clone();
        }
    }
    return true;
}

bool PlanetProcessing::stackFrames()
{
    if(m_processingDone == false)
        return false;
    int factor = m_scaleFactor;
    int stackFrameCount=0;
    Mat matSum = Mat::zeros(cv::Size(m_data_crop[0].second.cols*factor, m_data_crop[0].second.rows*factor), 22);
    for(size_t i=0;i<m_data_crop.size(); i++)
    {
        cout << i << endl;
        if(m_data_crop[i].first>=m_stackCorrThres)
        {
            cout << "Correlation: " << i << m_data_crop[i].first << endl;
            stackFrameCount++;
            cv::Mat rsMat = m_data_crop[i].second.clone();
            cv::resize(rsMat, rsMat, cv::Size(rsMat.cols*factor, rsMat.rows*factor));
            cv::accumulate(rsMat, matSum);
        }
        else{
            cout << "Break" << endl;
            break;
        }
    }

    m_stackedFrame = matSum / stackFrameCount;
    rotate(m_stackedFrame, m_stackedFrame, 1);

    m_outMat = m_stackedFrame.clone();
    m_host->dataReady();
    return true;
}

bool PlanetProcessing::sharpenFrame()
{
    if(m_processingDone == false)
        return false;
    cv::Mat imgSharp;
    cv::GaussianBlur(m_stackedFrame, imgSharp, cv::Size(0, 0), m_sharp_gauss);
    cv::addWeighted(m_stackedFrame, m_sharp_weightOrg, imgSharp, -m_sharp_weightBlurr, 0, imgSharp);

    //cv::imshow("sharp img", imgSharp/255);

    m_outMat = imgSharp.clone();
    m_host->dataReady();
    return true;
}

double PlanetProcessing::getContrast(cv::Mat img)
{
    // Used for reference frame generation
    Mat dx, dy;
    Sobel( img, dx, CV_32F, 1, 0, 3 );
    Sobel( img, dy, CV_32F, 0, 1, 3 );
    magnitude( dx, dy, dx );
    return sum(dx)[0];
}
