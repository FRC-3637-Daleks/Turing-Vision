#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include <chrono>
#include <iostream>

#include "opencv2/opencv.hpp"

typedef std::chrono::high_resolution_clock Clock;

class Capture
{
	public:
		Capture(int id, cv::Mat &mat);
		void setTransform(void (*func)(cv::Mat &));
		void thread();
	private:
		int m_id;
		cv::Mat &m_mat;
		void (*m_transform)(cv::Mat &) = NULL;
		cv::VideoCapture *m_cap;
		Clock::time_point m_start, m_stop;
};

#endif
