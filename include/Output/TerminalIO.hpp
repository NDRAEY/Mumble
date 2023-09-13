//
// Created by ndraey on 13.09.23.
//

#pragma once

#include "Output.hpp"
#include <sys/ioctl.h>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>
#include "../Base.hpp"

static long long timeInMilliseconds(void) {
	struct timeval tv;

	gettimeofday(&tv,NULL);

	return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

namespace Output {

	class TerminalIO : public Output {
	public:
		TerminalIO(struct ::PlayerInfo* player_info) : player_info(player_info) {
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);

			std::printf("Got terminal size: %dx%d\n", winsize.ws_col, winsize.ws_row);

			player_info->window_width = winsize.ws_col;
			player_info->window_height = winsize.ws_row * 2;

			std::printf("Current format: %d\n", player_info->window_pixfmt);
		}

		~TerminalIO() = default;

		void write(const AVFrame *frame) override {
			std::printf("\x1b[0;0H");

			auto start = ::timeInMilliseconds();

			ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);

			player_info->window_width = winsize.ws_col;
			player_info->window_height = winsize.ws_row * 2;

			uint32_t bpp = 3;

			for(int y = 0; y < player_info->window_height; y += 2) {
				std::printf("\x1b[%d;%dH", y / 2, 0);

				for(int x = 0; x < player_info->window_width; x++) {
					uint32_t in_coord = y * frame->linesize[0] + (x * bpp);
					uint32_t in_coord_low = (y + 1) * frame->linesize[0] + (x * bpp);

					std::printf(
							"\x1b[48;2;%d;%d;%dm",
							frame->data[0][in_coord_low + 0],
							frame->data[0][in_coord_low + 1],
							frame->data[0][in_coord_low + 2]
					);

					std::printf(
							"\x1b[38;2;%d;%d;%dm\u2580",
							frame->data[0][in_coord + 0],
							frame->data[0][in_coord + 1],
							frame->data[0][in_coord + 2]
					);
				}
			}

			auto end = ::timeInMilliseconds();

			auto framerate = 1000.0 / (double)(end - start);

			// If diff is greater than framerate, wait for a while
			// If less, perform frameskip

			std::printf(
				"\x1b[0m%.0f fps (vs %f fps) [%s] [%dx%d] [Time: %lld ms]\x1b[K",
				framerate,
				this->player_info->framerate,
				(framerate > this->player_info->framerate) ? "Wait" : "Skip",
				player_info->window_width,
				player_info->window_height,
				av_rescale_q(frame->pts, player_info->current_video_stream->time_base, AV_TIME_BASE_Q)
//				(int)frame->pts
			);
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


