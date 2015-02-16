#include <chrono>
#include <iostream>
#include <thread>

#include "opencv2/opencv.hpp"

#include "stream.h"

using namespace std;

typedef std::chrono::high_resolution_clock Clock;

cv::Mat cam0;
cv::Mat cam1;

int main(int argc, char **argv)
{
	std::thread stream_thread(Stream::thread, std::ref(cam0));

	cv::VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;

	cout << "Camera opened" << endl;
	cout << "Reading first frame..." << endl;
	cap.read(cam0);

	cout << "First frame read, starting loop" << endl;

	long count = 0;
	Clock::time_point start, stop;
	start = Clock::now();
	while (1) {
		count += 1;
		cap.read(cam0);

		// Show framerate
		if (count % 30 == 0) {
			cv::imwrite("out.jpg", cam0);
			stop = Clock::now();
			cout << "Frame: " << count << "\tFPS: " << 30 / double(chrono::duration_cast<chrono::milliseconds>(stop - start).count()) * 1000 << endl;
			start = Clock::now();
		}
	}
	return 0;
}
