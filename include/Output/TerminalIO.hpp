//
// Created by ndraey on 13.09.23.
//

#pragma once

#include "Output.hpp"
#include <sys/ioctl.h>
#include <csignal>
#include "../Base.hpp"

namespace Output {

	class TerminalIO : public Output {
	public:
		TerminalIO(struct ::PlayerInfo* player_info) : player_info(player_info) {
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);

			std::printf("Got terminal size: %dx%d\n", winsize.ws_col, winsize.ws_row);

			player_info->window_width = winsize.ws_col;
			player_info->window_height = winsize.ws_row;

			std::printf("Current format: %d\n", player_info->window_pixfmt);
		}

		~TerminalIO() = default;

		void write(const AVFrame *frame) override {
			std::printf("\x1b[0;0H");

			uint32_t bpp = 3;

			for(int y = 0; y < player_info->window_height; y++) {
				std::printf("\x1b[%d;%dH", y, 0);

				for(int x = 0; x < player_info->window_width; x++) {
					uint32_t in_coord = y * (bpp * frame->width) + (x * bpp);

					if(frame->data[0][in_coord + 0] == 0
					   || frame->data[0][in_coord + 1] == 0
					   || frame->data[0][in_coord + 2] == 0) {
						std::printf(" ");
					} else {
						std::printf(
								"\x1b[38;2;%d;%d;%dm@",
								frame->data[0][in_coord + 0],
								frame->data[0][in_coord + 1],
								frame->data[0][in_coord + 2]
						);
					}
				}
			}
		}

		static Output* create(struct ::PlayerInfo* pi) {
			return new TerminalIO(pi);
		}

		std::string id = "Terminal";

	protected:
		struct ::PlayerInfo* player_info = nullptr;

		struct winsize winsize = {};
	};

} // Output

