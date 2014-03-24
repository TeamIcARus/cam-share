#include <Read_unix.h>

using namespace std;
using namespace TCLAP;

/** Default constructor */
Read::Read(string logPath)
{

    //Set the name of the shared memory
    memoryName = "cam-share-memory";

	//Read resolution and memory size from file
	string line;
	ifstream infile;
	infile.open(logPath.c_str());
	if (infile.is_open())
	{
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
	}
	else
	{
		cerr << "Unable to open file, using default resolution" << endl;
		height = 544, width = 960, memorySize = width * height * 4;
	}

	int shmid;
	uchar* sharedData;

	shmid = shmget(2581, memorySize, 0666);

	if (shmid == -1)
	{
		perror("shmget");
		throw strerror(errno);
	}

	sharedData = (uchar*)shmat(shmid, NULL, 0);

	if (sharedData == (uchar*)(-1))
	{
		perror("shmat");
		throw strerror(errno);
	}

	frame = cv::Mat(height, width, CV_8UC3);

	// This lets us use the shared memory from init
	frame.data = sharedData;
}

Read::~Read()
{
	// Free stuff?
}

cv::Mat Read::getFrame()
{
    return frame;
}

int Read::getWidth()
{
	return width;
}

int Read::getHeight()
{
	return height;
}
	
int Read::getMemorysize()
{
    return memorySize;
}
