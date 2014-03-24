#ifndef Read_h
#define Read_h

#include <iostream>
#include <fstream>
#include <signal.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <../Boost/boost/interprocess/shared_memory_object.hpp>
#include <../Boost/boost/interprocess/mapped_region.hpp>
#include <string>

class Read
{
public:
    Read(string logPath);
    ~Read();
    cv::Mat Read::getFrame();
    int getWidth();
    int getHeight();
    int getMemorysize();
private:
    int width, height, memorySize;
	std::string memoryName;
    cv::Mat frame;
    boost::interprocess::mapped_region region;
};

#endif