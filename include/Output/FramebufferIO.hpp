//
// Created by ndraey on 11.09.23.
//

#pragma once

#include "Output.hpp"
#include <linux/fb.h>

#include "../Base.hpp"

namespace Output {
	class FramebufferIO : public Output {
	public:
		explicit FramebufferIO(struct ::PlayerInfo* player_info);
		~FramebufferIO();

		void write(const AVFrame *frame) override;
		static Output* create(struct ::PlayerInfo* info);
		
	private:
		int fd = -1;

		struct fb_var_screeninfo vinfo = {};

		struct PlayerInfo* player_info = nullptr;

		char* vbuf = nullptr;
	};
}
