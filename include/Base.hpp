//
// Created by ndraey on 11.09.23.
//

#pragma once

#include <vector>
#include <string>
#include <sys/time.h>

extern "C" {
#include <libavfilter/avfilter.h>
}

struct PlayerInfo {
	int window_width;
	int window_height;

	enum AVPixelFormat window_pixfmt;

	double framerate;
	AVStream* current_video_stream;

	bool is_paused;

	bool frame_is_waiting;
	double delay;

	double render_framerate;

	size_t frames_processed;

	size_t playing_started;
};

static long long timeInMilliseconds() {
	struct timeval tv = {};

	gettimeofday(&tv, nullptr);

	return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}
