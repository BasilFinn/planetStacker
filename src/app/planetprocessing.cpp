#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host)
{
    host->dataReady();
}

bool PlanetProcessing::executeProcessing()
{
    std::thread t_loadRaw([this]{loadRaw();});    // load raw data

    // Async init
    std::vector<std::future<cv::Mat>> asyncProcess(m_nThreads);
    for(auto& ap:asyncProcess)
    {
        ap = std::async(std::launch::async, [this](){return processThread();});
    }

    // Check and update asyncs
    cv::Mat tmpMat;
    std::future_status status;
    bool running = true;
    int cnt =0, rcnt=0;
    while(running){

        rcnt = 0;
        for(auto& ap:asyncProcess)
        {
            if(ap.wait_for(std::chrono::seconds(0))==future_status::timeout)
                rcnt++;
            if(rcnt>=m_nThreads)
                running=false;
        }

//        running = none_of(asyncProcess.begin(), asyncProcess.end(),
//                          [&](std::future<cv::Mat> x)
//                          {return x.wait_for(std::chrono::seconds(0))==future_status::timeout;});

//        for(auto& el:asyncProcess){
//            if(el.wait_for(std::chrono::seconds()==future_status::timeout)
//                running = true;
//        }

        //auto bla = find_if(asyncProcess.begin(), asyncProcess.end(), [&](std::future<cv::Mat> x){return x.valid();});

        for(int i=0; i<m_nThreads; i++)
        {
            status = asyncProcess[i].wait_for(std::chrono::milliseconds(100));
            if(status==std::future_status::ready)
            {
                cout << "Get: " << cnt++ << endl;
                tmpMat = asyncProcess[i].get();
                m_data_crop.push_back(tmpMat.clone());
                asyncProcess[i] = std::async(std::launch::async, [this](){return processThread();});
            }
            else if(status==std::future_status::deferred){
                cout << "deferred: " << i <<  endl;
            }
            else if(status==std::future_status::timeout){
                cout << "tiemout: " << i << endl;
            }
        }
    }


    t_loadRaw.join();
    cout << "Loaded all frames: " << m_data_crop.size() <<  endl;

    for(int i=0; i<m_data_crop.size(); i++)
    {
        cv::imshow("Output", m_data_crop[i]);
        waitKey();
    }
    return 1;
}

Mat PlanetProcessing::processThread()
{
    cv::Mat tmpMat = this->m_data_raw.get();
    // Do stuff
    return tmpMat;

}

void PlanetProcessing::loadRaw(void)
{
    // Loads raw data to DataQueue
    Mat tmpFrame;
    VideoCapture cap(m_dataPath);
    if(!cap.isOpened())
        cout << "Error opening video" << endl;

    while(1)
    {
        cap >> tmpFrame;
        if(tmpFrame.empty())
            break;
        m_data_raw.add(tmpFrame);
    }
}

void PlanetProcessing::savePath(string path)
{
    cout << "Processing: save path" << endl;
    m_dataPath = path;
}
