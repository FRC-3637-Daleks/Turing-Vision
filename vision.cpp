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

void init_cam0(cv::Mat &cam0)
{
	Capture *cap0 = new Capture(0, cam0);
	/*cap0->setTransform([](cv::Mat &mat) {
		if (mat.rows > 0) {
			cv::Mat clone(mat);
			cv::Mat rotate = cv::getRotationMatrix2D(cv::Point(mat.rows/2.0, mat.cols/2.0), 180, 1);
			cv::warpAffine(clone, mat, rotate, mat.size());
		}
	});*/

	cap0->thread();
}

void init_cam1(cv::Mat &cam1)
{
	Capture *cap1 = new Capture(1, "rtsp://localhost/cam0", cam1);
	/*cap1->setTransform([](cv::Mat &mat) {
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
		cv::line(mat, cv::Point(243, 410), cv::Point(243, 450), COLOR_GB, 3);
		cv::line(mat, cv::Point(188, 410), cv::Point(188, 450), COLOR_GB, 3);
		cv::line(mat, cv::Point(217, 410), cv::Point(217, 450), COLOR_GB, 3);
	});*/

	cap1->thread();
}

int main(int argc, char **argv)
{
	cv::Mat cam0(cv::Size(640,480), CV_8UC3);
	cv::Mat cam1(cv::Size(640,480), CV_8UC3);

	std::thread init_thread_cam0(&init_cam0, std::ref(cam0));
	init_thread_cam0.detach();

	std::thread init_thread_cam1(&init_cam1, std::ref(cam1));
	init_thread_cam1.detach();

	Stream *str = new Stream();
	str->addEndpoint("/cam0", cam0);
	str->addEndpoint("/cam1", cam1);

	std::thread stream_thread(&Stream::thread, str);

	// Allow server to start
	//Capture *cap1 = new Capture(1, "rtsp://localhost/cam0", cam1);
	//std::thread cam1_thread(&Capture::thread, cap1);

	while (1) { std::this_thread::sleep_for(std::chrono::seconds(1)); }

	return 0;
}
