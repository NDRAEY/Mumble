//
// Created by ndraey on 11.09.23.
//

#include "../../include/Output/Debug.hpp"

#include <cstdio>

namespace Output {
	Debug::Debug(struct ::PlayerInfo* player_info) {};
	Debug::~Debug() = default;

	void Debug::write(const AVFrame* frame) {
		std::printf(
				"DebugOut: 0x%08lx: Write of size %zu bytes requested!\n",
				(size_t)frame->data[0],
				(size_t)frame->linesize[0] * frame->height
		);
	}

} // Output
