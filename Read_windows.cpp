#include "Read_windows.h"
#include <iostream>
#include <fstream>
#include <signal.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "../Boost/boost/interprocess/shared_memory_object.hpp"
#include "../Boost/boost/interprocess/mapped_region.hpp"

using namespace std;
using namespace boost::interprocess;

/** Default constructor */
Read::Read(string logPath)
{

    //Set the name of the shared memory
    this->memoryName = "cam-share-memory";
    
    //Read resolution and memory size from file
    string line;
    ifstream infile;
    infile.open(logPath);
    if (infile.is_open())
	{
        getline(infile, line);
        // width
        width=atoi(line.c_str());
        getline(infile, line);
        // height
        height=atoi(line.c_str());
        // memory size
        getline(infile, line);
        memorySize=atoi(line.c_str());
        infile.close();
    }
	else
	{
        cerr << "Unable to open file, using default resolution" << endl;
        height = 480, width = 640, memorySize = width * height * 4;
    }
        
    cout << "Starting Read 2" << endl;
    shared_memory_object shm (open_only, memoryName.c_str(), read_only);
    
    cout << "Starting Read 3" << endl;
    this->region = mapped_region(shm, read_only);
    
    cout << "Starting Read 4" << endl;
    this->frame = cv::Mat(height, width, CV_8UC3);
    
    cout << "Starting Read 5" << endl;
}

Read::~Read()
{
    // Free stuff?
}

cv::Mat Read::getFrame()
{
    this->frame.data = (uchar*)this->region.get_address();
    return this->frame;
}

int Read::getWidth()
{
    return this->width;
}

int Read::getHeight()
{
    return this->height;
}

int Read::getMemorysize()
{
    return this->memorySize;
}
