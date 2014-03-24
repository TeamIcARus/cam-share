#include <iostream>
#include <fstream>

#include <signal.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <tclap/CmdLine.h>

#include <../Boost/boost/interprocess/shared_memory_object.hpp>
#include <../Boost/boost/interprocess/mapped_region.hpp>

using namespace std;
using namespace TCLAP;
using namespace boost::interprocess;

bool running = true;
string logPath;
cv::Mat frame;
//ID of the shared memory
string memoryID = "cam-share-memory";

void exitHandle (int sig)
{
    if (sig == SIGINT)
    {
        running = false;
        if(shared_memory_object::remove(memoryID.c_str()))
        {
            cout << "Memory released" << endl;
        }
        else
        {
            cout << "Problem releasing memory" << endl;
        }
        remove(logPath.c_str());
        frame.release();
        cout << "Shutting down" << endl;
    }
}

int main(int argc, char **argv)
{
    //Set the exit handler
    signal(SIGINT, exitHandle);

    struct shm_remove
    {
        shm_remove() {shared_memory_object::remove(memoryID.c_str());}
        ~shm_remove() {shared_memory_object::remove(memoryID.c_str());}
    } remover;

    int cflag;
    try
    {
        CmdLine cmd("Command description", ' ', "0.1");
        ValueArg<int> camera ("c", "camera", "Select camera", false, CV_CAP_ANY, "int");
        ValueArg<string> log1 ("l", "log", "Select log file", false, "caminfo.log", "string");
        cmd.add(camera);
        cmd.add(log1);

        // Parse arguments
        cmd.parse(argc, argv);
        cflag = camera.getValue();
        logPath = log1.getValue();
    }
    catch (ArgException &e)
    {
        cout << "ERROR: " << e.error() << " " << e.argId() << endl;
        return 1;
    }

    cv::VideoCapture cam(cflag);

    if (!cam.isOpened())
    {
        cout << "Error loading camera " << cflag << "." << endl;
        running = false;
    }
    else
    {
        cout << "Camera " << cflag << " loaded OK" << endl;
    }

    int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);
    int memorySize = height * width * 4;

    
    // Write resolution and memory size to file
    ofstream logFile;
    logFile.open (logPath);

    logFile << width << endl;
    logFile << height << endl;
    logFile << memorySize << endl;

    logFile.close();
    

    //Create shared memory object, if any previous memoryspace is still present, use that.
    shared_memory_object shm (open_or_create, memoryID.c_str(), read_write);

    //Set size
    shm.truncate(memorySize);

    //Map the whole shared memory in this process
    mapped_region region (shm, read_write);


    frame = cv::Mat(height, width, CV_8UC3);
    while(running)
    {
        try
        {
            cam.read(frame);

            if (!frame.empty()) {
                // This is the magic that copies everything into shared memory
                for (int row = 0; row < frame.rows; row++) {
                    int offset = row * frame.step;
                    const uchar* ptr = (const uchar*) (frame.data + offset);
                    memcpy(static_cast<char*>(region.get_address()) + offset, ptr, frame.step);
                }
            }

        } catch (cv::Exception & e)
        {
            cout << e.what() << endl;
        }
    }

    return 0;
}

