#include "stream.h"

Stream::Stream()
{
	gst_init(NULL, NULL);

	loop = g_main_loop_new (NULL, FALSE);

	/* create a server instance */
	server = gst_rtsp_server_new ();
	gst_rtsp_server_set_service(server, "554");

	/* get the mount points for this server, every server has a default object
	 * that be used to map uri mount points to media factories */
	mounts = gst_rtsp_server_get_mount_points (server);
}

void Stream::addEndpoint(std::string path, cv::Mat &mat)
{
	stream_configure *conf;
	conf = g_new0 (stream_configure, 1);
	conf->mat = &mat;

	GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
	gst_rtsp_media_factory_set_launch (factory,
			"( appsrc name=mysrc ! videoconvert ! video/x-raw,format=I420 ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay name=pay0 pt=96 )");

	g_signal_connect (factory, "media-configure", (GCallback) Stream::appsrc_configure, conf);

	gst_rtsp_mount_points_add_factory(mounts, path.c_str(), factory);

	std::cout << "[Stream] Creating new endpoint at: " << path << std::endl;
}

void Stream::addDualEndpoint(std::string path, cv::Mat &mat, cv::Mat &mat2)
{
	stream_configure *conf;
	conf = g_new0 (stream_configure, 1);
	conf->mat = &mat;
	conf->mat2 = &mat2;
        conf->dual = true;

	GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
	gst_rtsp_media_factory_set_launch (factory,
			"( appsrc name=mysrc ! videoconvert ! video/x-raw,format=I420 ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay name=pay0 pt=96 )");

	g_signal_connect (factory, "media-configure", (GCallback) Stream::appsrc_configure, conf);

	gst_rtsp_mount_points_add_factory(mounts, path.c_str(), factory);

	std::cout << "[Stream] Creating new endpoint at: " << path << std::endl;
}

void Stream::thread()
{
	/* don't need the ref to the mounts anymore */
	g_object_unref (mounts);

	/* attach the server to the default maincontext */
	gst_rtsp_server_attach (server, NULL);

	/* start serving */
	g_main_loop_run (loop);
}

void Stream::need_data(GstElement *appsrc, guint arg1, stream_context *c)
{
	GstMapInfo info;
	GstFlowReturn ret;

	gst_buffer_map(c->buffer, &info, GST_MAP_WRITE);
	memcpy(info.data, c->mat->data, STREAM_SIZE);
	gst_buffer_unmap(c->buffer, &info);

	// 30 FPS
	GST_BUFFER_PTS (c->buffer) = c->timestamp;
	GST_BUFFER_DURATION (c->buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 30);
	c->timestamp += GST_BUFFER_DURATION (c->buffer);

	g_signal_emit_by_name (appsrc, "push-buffer", c->buffer, &ret);
}

void Stream::need_data_dual(GstElement *appsrc, guint arg1, stream_context *c)
{
	GstMapInfo info;
	GstFlowReturn ret;

	cv::Mat dual(cv::Size(640*2,480), CV_8UC3);
	cv::Mat left(dual, cv::Rect(0, 0, 640, 480));
	cv::Mat right(dual, cv::Rect(640, 0, 640, 480));

	c->mat->copyTo(left);
	c->mat2->copyTo(right);

	gst_buffer_map(c->buffer, &info, GST_MAP_WRITE);
	memcpy(info.data, dual.data, STREAM_SIZE*2);
	gst_buffer_unmap(c->buffer, &info);

	// 30 FPS
	GST_BUFFER_PTS (c->buffer) = c->timestamp;
	GST_BUFFER_DURATION (c->buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 30);
	c->timestamp += GST_BUFFER_DURATION (c->buffer);

	g_signal_emit_by_name (appsrc, "push-buffer", c->buffer, &ret);
}

void Stream::appsrc_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media, stream_configure *conf)
{

	GstElement *element, *appsrc;
	stream_context *c;

	/* get the element used for providing the streams of the media */
	element = gst_rtsp_media_get_element (media);

	/* get our appsrc, we named it 'mysrc' with the name property */
	appsrc = gst_bin_get_by_name_recurse_up (GST_BIN (element), "mysrc");

	/* this instructs appsrc that we will be dealing with timed buffer */
	gst_util_set_object_arg (G_OBJECT (appsrc), "format", "time");
	/* configure the caps of the video */
	if (conf->dual) {
		g_object_set (G_OBJECT (appsrc), "caps",
			gst_caps_new_simple ("video/x-raw",
				"format", G_TYPE_STRING, "BGR",
				"width", G_TYPE_INT, STREAM_WIDTH*2,
				"height", G_TYPE_INT, STREAM_HEIGHT,
				"framerate", GST_TYPE_FRACTION, 0, 1, NULL), NULL);
	} else {
		g_object_set (G_OBJECT (appsrc), "caps",
			gst_caps_new_simple ("video/x-raw",
				"format", G_TYPE_STRING, "BGR",
				"width", G_TYPE_INT, STREAM_WIDTH,
				"height", G_TYPE_INT, STREAM_HEIGHT,
				"framerate", GST_TYPE_FRACTION, 0, 1, NULL), NULL);
	}

	c = g_new0 (stream_context, 1);
	c->timestamp = 0;
	if (conf->dual) {
		c->buffer = gst_buffer_new_allocate(NULL, STREAM_SIZE*2, NULL);
	} else {
		c->buffer = gst_buffer_new_allocate(NULL, STREAM_SIZE, NULL);
	}
	c->mat = conf->mat;
	c->mat2 = conf->mat2;

	/* make sure ther data is freed when the media is gone */
	g_object_set_data_full (G_OBJECT (media), "my-extra-data", c,
			(GDestroyNotify) g_free);

	/* install the callback that will be called when a buffer is needed */
	if (conf->dual) {
		g_signal_connect (appsrc, "need-data", (GCallback) Stream::need_data_dual, c);
	} else {
		g_signal_connect (appsrc, "need-data", (GCallback) Stream::need_data, c);
	}

	gst_object_unref (appsrc);
	gst_object_unref (element);
}
