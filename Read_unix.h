#ifndef Read_h
#define Read_h

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

class Read
{
public:
    Read(std::string logPath);
    ~Read();
    cv::Mat getFrame();
    int getWidth();
    int getHeight();
    int getMemorysize();
private:
    int width, height, memorySize;
	cv::Mat frame;
	std::string memoryName;
};

#endif