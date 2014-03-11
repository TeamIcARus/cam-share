#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <opencv2/highgui/highgui.hpp>
#include "tclap/CmdLine.h"

using namespace std;
using namespace TCLAP;

class Read
{
private:
    int width, height, memorySize;

public:
    cv::Mat frame;
    Read(string logPath) {
        //Read resolution and memory size from file
        string line;
        ifstream infile;
        infile.open(logPath.c_str());
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
            throw strerror(errno);
        }

        sharedData = (uchar*)shmat(shmid, NULL, 0);

        if (sharedData == (uchar*)(-1)) {
            perror("shmat");
            throw strerror(errno);
        }

        frame = cv::Mat(height, width, CV_8UC3);

        // This lets us use the shared memory from init
        frame.data = sharedData;
    }
    ~Read() {
        // Free stuff?
    }
};
