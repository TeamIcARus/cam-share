#ifndef Read_h
#define Read_h


#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <tclap/CmdLine.h>

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
	cv::Mat frame;
	std::string memoryName;
};

#endif