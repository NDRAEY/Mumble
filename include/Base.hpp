//
// Created by ndraey on 11.09.23.
//

#pragma once

#include <vector>
#include <string>

extern "C" {
#include <libavfilter/avfilter.h>
}

struct PlayerInfo {
	int window_width;
	int window_height;

	enum AVPixelFormat window_pixfmt;
};