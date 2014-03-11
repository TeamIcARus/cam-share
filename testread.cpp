#include "Read.cpp"

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

    Read* read = new Read(logPath);


    cv::namedWindow("Read", CV_WINDOW_AUTOSIZE);
    while (true) {
        cv::Mat frame = read->copyFrame();
        cv::imshow("Read", frame);
        char k = cv::waitKey(20);
        if (k == 'q' || k == 'Q') break;
    }

    return 0;
}