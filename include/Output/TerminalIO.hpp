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

namespace Output {

	class TerminalIO : public Output {
	public:
		explicit TerminalIO(struct ::PlayerInfo* player_info) : player_info(player_info) {
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);

			player_info->window_width = winsize.ws_col;
			player_info->window_height = winsize.ws_row * 2;

			std::printf("Video scaler set to: %dx%d\n", winsize.ws_col, winsize.ws_row);
		}

		~TerminalIO() = default;

		void write(const AVFrame *frame) override {
			std::printf("\x1b[0;0H");

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

//			if(!player_info->frame_is_waiting) {
//				std::printf(
//					"\x1b[0m%.0f fps (vs %f fps) [%dx%d] [Skip: %.0f frames] [Frame time: %.0f ms]\x1b[K",
//					player_info->render_framerate,
//					this->player_info->framerate,
//					player_info->window_width,
//					player_info->window_height,
//					player_info->delay / this->player_info->framerate,
//					player_info->delay
//				);
//			} else  {
//				std::printf(
//					"\x1b[0m%.0f fps (vs %f fps) [%dx%d] [Wait: %.0f ms] [Frame time: %.0f ms]\x1b[K",
//					player_info->render_framerate,
//					this->player_info->framerate,
//					player_info->window_width,
//					player_info->window_height,
//					(1000.0 / this->player_info->framerate) - player_info->delay,
//					player_info->delay
//				);
//			}

			auto seconds_raw = (double)player_info->frames_processed / (double)player_info->framerate;
			auto seconds = (int)seconds_raw % 60;
			auto minutes = (int)seconds_raw / 60;
			auto hours = (int)seconds_raw / 3600;

			// Synchronization quality.
			auto diff = (double)(::timeInMilliseconds() - player_info->playing_started) - seconds_raw * 1000;

			std::printf(
				"\x1b[0mTime: [%02d:%02d:%02d.%03d] [Delay: %.2f ms] [Diff: %.2f ms (%d frames)]\x1b[K",
				hours,
				minutes,
				seconds,
				(int)((seconds_raw - (int)seconds_raw) * 1000),
				player_info->delay,
				diff,
				(int)(diff / player_info->framerate)
			);

			fflush(stdout);
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


