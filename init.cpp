/**
 * Awesome resources for this:
 * http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html#shm
 * http://stackoverflow.com/questions/3906437/opencv-matrix-into-shared-memory
 */

#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"

bool running = true;

void exitHandle (int sig) {
    if (sig == SIGINT) {
        running = false;
        std::cout << "Shutting down" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    cv::VideoCapture cam(CV_CAP_ANY);

    int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);

    // Multiply by four since color image is three extra channels
    int memorySize = height * width * 4;

    int shmid;
    char* sharedData;

    std::cout << "memorySize: " << memorySize << "(" << height << ", " << width <<  ")" << std::endl;
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

        // This is the magic that copies everything into shared memory
        for (int row = 0; row < frame.rows; row++) {
            int offset = row * frame.step;
            const uchar* ptr = (const uchar*) (frame.data + offset);
            memcpy((uchar*)(sharedData + offset), ptr, frame.step);
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
