#include "capture.h"
#include "stream.h"

Capture::Capture(int id, cv::Mat &mat) : m_id(id), m_mat(mat)
{
}

void Capture::thread()
{
	Clock::time_point start, stop;
	cv::Mat capture;

	std::cout << "[" << m_id << "] Opening camera" << std::endl;
	start = Clock::now();
	cv::VideoCapture cap(m_id);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, STREAM_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, STREAM_HEIGHT);
	if (!cap.isOpened())
		return;

	std::cout << "[" << m_id << "] Reading first frame..." << std::endl;
	cap.read(capture);
	stop = Clock::now();
	std::cout << "[" << m_id << "] Reading took " << double(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) / 1000 << " seconds" << std::endl;

	long count = 0;
	start = Clock::now();
	while (1) {
		count += 1;
		cap.read(capture);

		if (m_transform != NULL) {
			m_transform(capture);
		}
		capture.copyTo(m_mat);

		// Show framerate
		if (count % 30 == 0) {
			stop = Clock::now();
			std::cout << "[" << m_id << "] Frame: " << count << "\tFPS: " << 30 / double(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) * 1000 << std::endl;
			start = Clock::now();
		}
	}
}

void Capture::setTransform(void (*func)(cv::Mat &))
{
	m_transform = func;
}
