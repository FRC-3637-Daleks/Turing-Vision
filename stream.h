#ifndef _STREAM_H_
#define _STREAM_H_

#include <iostream>

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "opencv2/opencv.hpp"

#define STREAM_WIDTH  640
#define STREAM_HEIGHT 480
#define STREAM_SIZE   (STREAM_WIDTH * STREAM_HEIGHT * 3)

struct stream_context {
	GstClockTime timestamp;
	GstBuffer *buffer;
	cv::Mat *mat;
	cv::Mat *mat2;
};

struct stream_configure {
	cv::Mat *mat;
	cv::Mat *mat2;
	bool	dual;
};


class Stream
{
	public:
		Stream();
		void addEndpoint(std::string path, cv::Mat &mat);
		void addDualEndpoint(std::string path, cv::Mat &mat, cv::Mat &mat2);
		void thread();
		static void need_data(GstElement *appsrc, guint arg1, stream_context *c);
		static void need_data_dual(GstElement *appsrc, guint arg1, stream_context *c);
		static void appsrc_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media, stream_configure *conf);
	private:
		GMainLoop *loop;
		GstRTSPServer *server;
		GstRTSPMountPoints *mounts;
};

#endif
