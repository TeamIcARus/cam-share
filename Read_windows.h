#ifndef Read_h
#define Read_h
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "../Boost/boost/interprocess/mapped_region.hpp"

class Read
{
public:
    Read(std::string logPath);
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