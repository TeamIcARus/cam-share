#include <sys/ipc.h>
#include <sys/shm.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <opencv2/highgui/highgui.hpp>

int main(int argc, char const *argv[])
{
    // TODO, get height and width from camera or something
    int height = 544, width = 960;
    int memorySize = height * width;

    int shmid;
    uchar* sharedData;

    std::cout << "memorySize: " << memorySize << std::endl;
    shmid = shmget(2581, memorySize, 0666);

    if (shmid == -1) {
        perror("shmget");
        return -1;
    }

    sharedData = (uchar*)shmat(shmid, (void *)0, 0);

    if (sharedData == (uchar*)(-1)) {
        perror("shmat");
        return -1;
    }

    cv::Mat frame(height, width, CV_8U); // does not copy
    cv::namedWindow("Read", CV_WINDOW_AUTOSIZE);

    // This lets us use the shared memory from init
    frame.data = sharedData;
    while (true) {
        cv::imshow("Read", frame);
        if (cv::waitKey(20) == 'q') break;
    }
    return 0;
}