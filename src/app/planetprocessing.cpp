#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host)
{
    host->dataReady();
}


void PlanetProcessing::loadData()
{
    cout << "Processing: load data" << endl;
    Mat tmpFrame;


}

void PlanetProcessing::loadRaw()
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
        m_data_raw.push_back(tmpFrame);
    }
}

void PlanetProcessing::savePath(string path)
{
    cout << "Processing: save path" << endl;

    m_dataPath = path;
}
