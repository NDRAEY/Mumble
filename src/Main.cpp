#include <vector>
#include <iostream>
#include "../include/Output/Debug.hpp"
#include "../include/Output/FindByName.hpp"
#include "../include/Base.hpp"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
	#include <termios.h>
	#include <fcntl.h>
}

std::string current_video_output_device = "None";

void show_help() {
	std::cout << "Usage: mumble <videofile>" << std::endl << std::endl;
	std::cout << "Supported Video Outputs:" << std::endl;

	for(const auto& i : Output::func_mapping) {
		std::cout << "    - " << i.first << std::endl;
	}
}

const char* inputFileName = nullptr;

struct PlayerInfo player_info;

int main(int argc, char** argv) {
	printf("Mumble Video Player by NDRAEY (c) 2023\n");

	std::vector<char*> charptr_args(argv + 1, argv + argc);

	for(size_t i = 0, list_size = charptr_args.size(); i < list_size; i++) {
		const auto& elem = std::string(charptr_args[i]);

		if(elem == "--help") { // Whut?
			show_help();

			exit(0);
		}

		if(elem == "--vo") {
			if(i + 1 >= list_size) {
				std::cerr << "error: specify video output" << std::endl;

				exit(1);
			}
			current_video_output_device = std::string(charptr_args[++i]);
		} else {
			std::cerr << "Treating as input file path: " << elem << std::endl;

			inputFileName = charptr_args[i];
		}
	}

	if(!inputFileName) {
		std::cerr << "No input file!" << std::endl;
		exit(1);
	}

    avformat_network_init();

    AVFormatContext* inputFormatContext = nullptr;
    AVCodecContext* videoCodecContext = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;

    if (avformat_open_input(&inputFormatContext, inputFileName, nullptr, nullptr) < 0) {
        printf("Could not open input file!\n");
        return -1;
    }

    if (avformat_find_stream_info(inputFormatContext, nullptr) < 0) {
        printf("No stream info!\n");
        return -1;
    }

	printf("Input: %s\n", inputFileName);
	printf("Streams: %d\n", inputFormatContext->nb_streams);

    unsigned int videoStreamIndex = 0xFFFFFFFF;
    for (unsigned int i = 0; i < inputFormatContext->nb_streams; i++) {
        if (inputFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == 0xFFFFFFFF) {
        printf("No video codec!\n");
        return -1;
    }

    AVCodecParameters* videoCodecParameters = inputFormatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec* videoCodec = avcodec_find_decoder(videoCodecParameters->codec_id);

	if (videoCodec == nullptr) {
        printf("No video codec!\n");
        return -1;
    }

    videoCodecContext = avcodec_alloc_context3(videoCodec);
    if (avcodec_parameters_to_context(videoCodecContext, videoCodecParameters) < 0) {
        printf("No video codec!\n");
        return -1;
    }

    if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
        printf("No video codec!\n");
        return -1;
    }

	AVStream* video_stream = inputFormatContext->streams[videoStreamIndex];
	player_info.current_video_stream = video_stream;

	// Print information about media file
	printf("Codec: %s\n", videoCodec->name);
	printf("Framerate: %.03f fps\n", (double)video_stream->r_frame_rate.num / (double)video_stream->r_frame_rate.den);

	player_info.framerate = (double)video_stream->r_frame_rate.num / (double)video_stream->r_frame_rate.den;

	printf("Frame size: %d\n", videoCodecContext->frame_size);
	printf("Sample rate: %d\n", videoCodecParameters->sample_rate);
	printf("Size: %dx%d\n", videoCodecContext->width, videoCodecContext->height);
	printf("Duration: %02lld:%02lld:%02lld.%.00f\n",
		   (long long int)inputFormatContext->duration / (3600 * (long long int)AV_TIME_BASE), // hour
		   (long long int)(inputFormatContext->duration / (60 * AV_TIME_BASE)) % 60, // minute
		   (long long int)(inputFormatContext->duration / AV_TIME_BASE) % 60, // seconds
		   (double)(inputFormatContext->duration % AV_TIME_BASE) / 1000.0); // millis

	player_info.window_width = videoCodecContext->width;
	player_info.window_height =  videoCodecContext->height;
	player_info.window_pixfmt = AV_PIX_FMT_RGB24;

    packet = av_packet_alloc();
    frame = av_frame_alloc();

	// Create Output object here.
	// Initialize it with some needed information.

	Output::Output* output = Output::FindByName(current_video_output_device, &player_info);

	if(output == nullptr) {
		std::cerr << "error: no suitable video output found!" << std::endl;

		exit(1);
	}

	// TODO: Controlling with keyboard.
	// FIXME: I cannot use termios because termios and O_NONBLOCK on stdin is breaking Terminal output

	std::cout << "Millis per frame: " << std::endl;

	ssize_t frameskip = 0;

	player_info.playing_started = timeInMilliseconds();

	while (av_read_frame(inputFormatContext, packet) >= 0) {
//		while(player_info.is_paused);
		auto start = timeInMilliseconds();

        if (packet->stream_index == (int)videoStreamIndex) {
            avcodec_send_packet(videoCodecContext, packet);

            while (avcodec_receive_frame(videoCodecContext, frame) >= 0) {
				player_info.frames_processed += 1;

				while (frameskip-- > 0) {
//					usleep(floor(1000000.0 / player_info.framerate));
					goto readnext;
				}

				AVFrame *rgbFrame = av_frame_alloc();
				rgbFrame->format = AV_PIX_FMT_RGB24;
				rgbFrame->width = frame->width;
				rgbFrame->height = frame->height;
				av_frame_get_buffer(rgbFrame, 32);

//				printf("SWS: %d x %d\n", player_info.window_width, player_info.window_height);

				SwsContext *swsContext = sws_getContext(
					frame->width, frame->height,
					videoCodecContext->pix_fmt,
					player_info.window_width, player_info.window_height,
					player_info.window_pixfmt,
					SWS_BILINEAR, nullptr, nullptr, nullptr
				);

				sws_scale(
					swsContext,
					frame->data,
					frame->linesize,
					0,
					frame->height,
					rgbFrame->data, rgbFrame->linesize
				);

				sws_freeContext(swsContext);


				// Writing frame data here!
				output->write(rgbFrame);

				auto end = timeInMilliseconds();
				player_info.delay = (double)(end - start);
				player_info.render_framerate = 1000.0 / player_info.delay;
				player_info.frame_is_waiting = player_info.render_framerate > player_info.framerate;

				if (player_info.frame_is_waiting) {
					usleep((int)((1000.0 / player_info.framerate) - player_info.delay) * 1000);
				} else {
					frameskip = floor(player_info.delay / player_info.framerate);
				}

				av_frame_free(&rgbFrame);
            }
        }
		readnext:
        av_packet_unref(packet);
    }

	// FIXME: Compiler warning: "Delete called on non-final 'Output::Debug' that has virtual functions but non-virtual destructor"
	delete (Output::Debug*)output;

    // Free allocated resources
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&videoCodecContext);
    avformat_close_input(&inputFormatContext);
    avformat_network_deinit();

    return 0;
}
