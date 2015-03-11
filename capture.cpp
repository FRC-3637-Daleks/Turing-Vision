#include "capture.h"
#include "stream.h"

Capture::Capture(int id, cv::Mat &mat) : m_id(id), m_mat(mat)
{
	std::cout << "[" << m_id << "] Opening camera" << std::endl;
	m_start = Clock::now();
	m_cap = new cv::VideoCapture(m_id);
	if (!m_cap->isOpened())
		std::cout << "Failed to open camera" << std::endl;

	m_cap->set(CV_CAP_PROP_FRAME_WIDTH, STREAM_WIDTH);
	m_cap->set(CV_CAP_PROP_FRAME_HEIGHT, STREAM_HEIGHT);

	std::cout << "[" << m_id << "] Reading first frame..." << std::endl;
	m_start = Clock::now();
	cv::Mat capture;
	m_cap->read(capture);
	m_stop = Clock::now();
	std::cout << "[" << m_id << "] Reading took " << double(std::chrono::duration_cast<std::chrono::milliseconds>(m_stop - m_start).count()) / 1000 << " seconds" << std::endl;
}

Capture::Capture(int id, std::string addr, cv::Mat &mat) : m_id(id), m_mat(mat)
{
	std::cout << "[" << m_id << "] Opening camera" << std::endl;
	m_start = Clock::now();
	m_cap = new cv::VideoCapture(addr);
	if (!m_cap->isOpened())
		std::cout << "Failed to open camera" << std::endl;

	m_cap->set(CV_CAP_PROP_FRAME_WIDTH, STREAM_WIDTH);
	m_cap->set(CV_CAP_PROP_FRAME_HEIGHT, STREAM_HEIGHT);

	std::cout << "[" << m_id << "] Reading first frame..." << std::endl;
	m_start = Clock::now();
	cv::Mat capture;
	m_cap->read(capture);
	m_stop = Clock::now();
	std::cout << "[" << m_id << "] Reading took " << double(std::chrono::duration_cast<std::chrono::milliseconds>(m_stop - m_start).count()) / 1000 << " seconds" << std::endl;
}

void Capture::thread()
{
	cv::Mat capture;
	long count = 0;
	m_start = Clock::now();
	while (1) {
		count += 1;
		m_cap->read(capture);

		if (m_transform != NULL) {
			m_transform(capture);
		}
		capture.copyTo(m_mat);

		// Show framerate
		if (count % 30 == 0) {
			m_stop = Clock::now();
			std::cout << "[" << m_id << "] Frame: " << count << "\tFPS: " << 30 / double(std::chrono::duration_cast<std::chrono::milliseconds>(m_stop - m_start).count()) * 1000 << std::endl;
			m_start = Clock::now();
		}
	}
}

void Capture::setTransform(void (*func)(cv::Mat &))
{
	m_transform = func;
}
