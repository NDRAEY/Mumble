//
// Created by ndraey on 11.09.23.
//

#pragma once
#include <cstdint>
#include <string>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

namespace Output {

	class Output {
	public:
		virtual void write(const AVFrame* frame) = 0;

		const std::string id;
	};

}
