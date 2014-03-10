/**
 * Awesome resources for this:
 * http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html#shm
 * http://stackoverflow.com/questions/3906437/opencv-matrix-into-shared-memory
 */
#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "tclap/CmdLine.h"

using namespace std;
using namespace TCLAP;

bool running = true;

void exitHandle (int sig) {
    if (sig == SIGINT) {
        running = false;
        cout << "Shutting down" << endl;
    }
}

int main(int argc, char **argv) {
    int cflag=CV_CAP_ANY;
    try {
        CmdLine cmd("Command description", ' ', "0.1");
        ValueArg<int> camera ("c", "camera", "Select camera", false, CV_CAP_ANY, "int");
        cmd.add(camera);

        // Parse arguments
        cmd.parse(argc, argv);
        cflag=camera.getValue();
    } catch (ArgException &e) {
        cout << "ERROR: " << e.error() << " " << e.argId() << endl;
        return 1;
    }

    cv::VideoCapture cam(cflag);

    if (!cam.isOpened()) {
        cout << "Error loading camera " << cflag << "." << endl;
        return 1;
    } else {
        cout << "Camera " << cflag << " loaded OK" << endl;
    }
  
    int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);

    // Write resolution to file
    ofstream resInfo;
    resInfo.open ("resInfo.txt");
    resInfo << width << endl;
    resInfo << height << endl;
    resInfo.close();

    // Multiply by four since color image is three extra channels
    int memorySize = height * width * 4;

    int shmid;
    char* sharedData;

    cout << "memorySize: " << memorySize << "(" << height << ", " << width <<  ")" << endl;
    shmid = shmget(2581, memorySize, 0666 | IPC_CREAT);

    if (shmid == -1) {
        perror("shmget");
        return -1;
    }


    sharedData = (char*)shmat(shmid, NULL, 0);

    if (sharedData == (char*)(-1)) {
        perror("shmat");
        return -1;
    }

    signal(SIGINT, exitHandle);

    cv::Mat frame(height, width, CV_8UC3);
    while (running) {
        cam.read(frame);

        if (!frame.empty()) {
            // This is the magic that copies everything into shared memory
            for (int row = 0; row < frame.rows; row++) {
                int offset = row * frame.step;
                const uchar* ptr = (const uchar*) (frame.data + offset);
                memcpy((uchar*)(sharedData + offset), ptr, frame.step);
            }
        }
    }

    // Remove the data segment and delete it
    if (shmdt(sharedData) == -1) {
        perror("shmdt");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }
    return 0;
}
