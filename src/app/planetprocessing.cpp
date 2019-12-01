#include "planetprocessing.h"
#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host)
{
    host->dataReady();
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
//    /WindowsData/Users/basil/Desktop/SharpCap Captures/2019-09-30/Saturn/
    // Check and update asyncs
    cv::Mat tmpMat;
    std::future_status status;
    bool running = true;
    int frameCnt =0, rcnt=0;
    //TODO: set max frames or implement this in reader (while t_loadRaw running)
    m_frameCnt = 0;

    //    std::thread t_loadRaw([this]{loadRaw();});    // load raw data
    std::future<bool> asyncLoad;
    asyncLoad = std::async(std::launch::async, [this](){return loadRaw();});

    // Async init
    std::vector<std::future<cv::Mat>> asyncProcess(m_nThreads);
    for(auto& ap:asyncProcess)
    {
        ap = std::async(std::launch::async, [this](){return processThread();});
        m_frameCnt++;
    }


    while(running){
        for(int i=0; i<m_nThreads; i++)
        {
            if(asyncProcess[i].valid())
            {
                status = asyncProcess[i].wait_for(std::chrono::milliseconds(100));
                switch(status){
                case std::future_status::ready:
                    tmpMat = asyncProcess[i].get();
                    m_data_crop.push_back(tmpMat.clone());
                    if(m_frameCnt<m_noFrames)
                    {
                        m_frameCnt++;
                        cout << "Thread["<<i<<"]: "<<m_frameCnt << endl;
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

    imshow("First frame", m_data_crop[0]);
    imshow("Last frame", m_data_crop.back());

    return 1;
}


Mat PlanetProcessing::processThread()
{
    cv::Mat tmpMat = this->m_data_raw.get();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));    // Do processing here
    return tmpMat;

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
    cout << "Numbers: " << m_noFrames << endl;

    while(1)
    {
        cap >> tmpFrame;
        if(tmpFrame.empty())
            break;
        m_data_raw.add(tmpFrame.clone());
    }
    return true;
}

void PlanetProcessing::savePath(string path)
{
    cout << "Processing: save path" << endl;
    m_dataPath = path;
}
