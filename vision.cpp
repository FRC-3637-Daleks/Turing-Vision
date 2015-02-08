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

	cout << "Camera opened" << endl;
	cout << "Reading first frame..." << endl;
	cv::Mat frame;
	cap.read(frame);

	cout << "First frame read, starting loop" << endl;

	long count = 0;
	Clock::time_point start, stop;
	start = Clock::now();
	while (1) {
		count += 1;
		cap.read(frame);

		// Show framerate
		if (count % 30 == 0) {
			cv::imwrite("out.jpg", frame);
			stop = Clock::now();
			cout << "Frame: " << count << "\tFPS: " << 30 / double(chrono::duration_cast<chrono::milliseconds>(stop - start).count()) * 1000 << endl;
			start = Clock::now();
		}
	}
	return 0;
}
