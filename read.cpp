#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <opencv2/highgui/highgui.hpp>

using namespace std;

int main(int argc, char const *argv[])
{
    // TODO, get height and width from camera or something
    // Right now it reads resolution from a file created by server
    //int height = 544, width = 960;
    //int height = 480, width = 640;
    int width, height; 
    
    //Read resolution from file   
    string line;
    ifstream infile;
    infile.open("resInfo.txt");
    if (infile.is_open()) {
        getline(infile, line);
        width=atoi(line.c_str());
        getline(infile, line);
        height=atoi(line.c_str());
        infile.close();
    } else {
        cout << "Unable to open file, using default resolution" << endl;
        height = 544, width = 960;
    }

    cout << "width: " << width << " height: " << height << endl;

    int memorySize = height * width;

    int shmid;
    uchar* sharedData;

    cout << "memorySize: " << memorySize << endl;
    shmid = shmget(2581, memorySize, 0666);

    if (shmid == -1) {
        perror("shmget");
        return -1;
    }

    sharedData = (uchar*)shmat(shmid, NULL, 0);

    if (sharedData == (uchar*)(-1)) {
        perror("shmat");
        return -1;
    }

    cv::Mat frame(height, width, CV_8UC3);
    cv::namedWindow("Read", CV_WINDOW_AUTOSIZE);

    // This lets us use the shared memory from init
    frame.data = sharedData;
    while (true) {
        cv::imshow("Read", frame);
        char k = cv::waitKey(20);
        if (k == 'q' || k == 'Q') break;
    }
    return 0;
}
