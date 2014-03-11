#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <opencv2/highgui/highgui.hpp>
#include "tclap/CmdLine.h"

using namespace std;
using namespace TCLAP;

int main(int argc, char const *argv[])
{
    string logPath;
    try {
        CmdLine cmd("Command description", ' ', "0.1");
        ValueArg<string> log1 ("l", "log", "Select log file", false, "caminfo.log", "string");
        cmd.add(log1);

        // Parse arguments
        cmd.parse(argc, argv);
        logPath = log1.getValue();
    } catch (ArgException &e) {
        cout << "ERROR: " << e.error() << " " << e.argId() << endl;
        return 1;
    }


    // Right now it reads resolution from a file created by server
    int width, height, memorySize;

    //Read resolution and memory size from file
    string line;
    ifstream infile;
    infile.open(logPath);
    if (infile.is_open()) {
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
    } else {
        cerr << "Unable to open file, using default resolution" << endl;
        height = 544, width = 960, memorySize = width * height * 4;
    }

    int shmid;
    uchar* sharedData;

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
