#include "planetprocessing.h"

PlanetProcessing::PlanetProcessing(Iprocessing* host)
{
    host->dataReady();
}


void PlanetProcessing::loadData()
{
    cout << "Load data" << endl;

//    m_data.add();
}

void PlanetProcessing::savePath(string path)
{
    m_dataPath = path;
}
