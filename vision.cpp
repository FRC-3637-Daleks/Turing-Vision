#include <chrono>
#include <iostream>
#include <thread>

#include "opencv2/opencv.hpp"

#include "stream.h"
#include "capture.h"

#define COLOR_R		CV_RGB(255,0,0)
#define COLOR_G		CV_RGB(0,255,0)
#define COLOR_B		CV_RGB(0,0,255)
#define COLOR_RG	CV_RGB(255,255,0)
#define COLOR_GB	CV_RGB(0,255,255)
#define COLOR_RB	CV_RGB(255,0,255)

using namespace std;

int main(int argc, char **argv)
{
	//cv::Mat cam0;
	cv::Mat cam1;

	/*Capture *cap0 = new Capture(0, cam0);
	cap0->setTransform([](cv::Mat &mat) {
		if (mat.rows > 0) {
			cv::Mat clone(mat);
			cv::Mat rotate = cv::getRotationMatrix2D(cv::Point(mat.rows/2.0, mat.cols/2.0), 180, 1);
			cv::warpAffine(clone, mat, rotate, mat.size());
		}
	});*/

	Capture *cap1 = new Capture(0, cam1);
	cap1->setTransform([](cv::Mat &mat) {
		// Horizontal depth lines
		cv::line(mat, cv::Point(0, 360), cv::Point(STREAM_WIDTH, 360), COLOR_R, 3);
		cv::line(mat, cv::Point(0, 290), cv::Point(STREAM_WIDTH, 290), COLOR_R, 3);

		// Slanted orientation lines
		cv::line(mat, cv::Point(200, 290), cv::Point(10, 360), COLOR_G, 3);
		cv::line(mat, cv::Point(420, 290), cv::Point(610, 360), COLOR_G, 3);

		// Vertical orientation lines continuation
		cv::line(mat, cv::Point(200, 290), cv::Point(200, 0), COLOR_G, 3);
		cv::line(mat, cv::Point(420, 290), cv::Point(420, 0), COLOR_G, 3);

		// Horizontal tote alignment lines
		cv::line(mat, cv::Point(243, 410), cv::Point(243, 450), COLOR_BG, 3);
		cv::line(mat, cv::Point(188, 410), cv::Point(188, 450), COLOR_BG, 3);
		cv::line(mat, cv::Point(217, 410), cv::Point(217, 450), COLOR_BG, 3);
	});

	//std::thread cam0_thread(&Capture::thread, cap0);
	//std::this_thread::sleep_for(std::chrono::milliseconds(200));
	std::thread cam1_thread(&Capture::thread, cap1);

	Stream *str = new Stream();
	str->addEndpoint("/cam0", cam1);
	//str->addEndpoint("/cam1", cam1);

	std::thread stream_thread(&Stream::thread, str);

	while (1) { std::this_thread::sleep_for(std::chrono::seconds(1)); }

	return 0;
}
