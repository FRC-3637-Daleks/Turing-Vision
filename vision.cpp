#include <chrono>
#include <iostream>

#include "opencv2/opencv.hpp"

using namespace std;

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char **argv)
{
	cv::VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;

	long count = 0;
	Clock::time_point start, stop;
	start = Clock::now();
	while (1) {
		cv::Mat frame;
		cap.read(frame);
		cv::imwrite("out.jpg", frame);

		// Show framerate
		if (count == 30) {
			stop = Clock::now();
			cout << "FPS:" << double(count) /  chrono::duration_cast<chrono::seconds>(stop - start).count() << endl;
			start = Clock::now();
			count = 0;
		}
		count += 1;
	}
	return 0;
}
