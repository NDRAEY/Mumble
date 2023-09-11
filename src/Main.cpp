#include "../include/Output/RawFile.hpp"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
}

int main(int argc, char** argv) {
	printf("Mumble Video Player by NDRAEY (c) 2023\n");

	if(argc < 3) {
		printf("Not enough args!\n");
		exit(1);
	}

    avformat_network_init();

    const char* inputFileName = argv[1];
    const char* outputFileName = argv[2];

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

	// Print information about media file
	printf("Codec: %s\n", videoCodec->name);
	printf("Framerate: %.03f fps\n", (double)video_stream->r_frame_rate.num / (double)video_stream->r_frame_rate.den);

	printf("Frame size: %d\n", videoCodecContext->frame_size);
	printf("Sample rate: %d\n", videoCodecParameters->sample_rate);
	printf("Size: %dx%d\n", videoCodecContext->width, videoCodecContext->height);
	printf("Duration: %02ld:%02ld:%02ld.%.00f\n",
		   inputFormatContext->duration / 3600000000,
		   (inputFormatContext->duration / 60000000) % 60,
		   (inputFormatContext->duration / 1000000) % 60,
		   (double)(inputFormatContext->duration % 1000000) / 1000.0);

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    // Open output file
    FILE* outputFile = fopen(outputFileName, "wb");
    if (outputFile == nullptr) {
        printf("Could not open output stream!\n");
        return -1;
    }

    SwsContext* swsContext = nullptr;

	// Create Output object here.
	// Initialize it with some needed information.

//	Output::RawFile* output = new Output::RawFile();
	Output::RawFile* output = new Output::RawFile;

    while (av_read_frame(inputFormatContext, packet) >= 0) {
        if (packet->stream_index == (int)videoStreamIndex) {
            avcodec_send_packet(videoCodecContext, packet);

			printf("Packet: %ld\n", packet->pos);

            while (avcodec_receive_frame(videoCodecContext, frame) >= 0) {
                AVFrame* rgbFrame = av_frame_alloc();
                rgbFrame->format = AV_PIX_FMT_RGB24;
                rgbFrame->width = frame->width;
                rgbFrame->height = frame->height;
                av_frame_get_buffer(rgbFrame, 32);

                swsContext = sws_getContext(
                    frame->width, frame->height,
					videoCodecContext->pix_fmt,
                    frame->width, frame->height,
					AV_PIX_FMT_RGB24,
                    SWS_BILINEAR, nullptr, nullptr, nullptr
                );

                sws_scale(
                    swsContext, frame->data, frame->linesize, 0, frame->height,
                    rgbFrame->data, rgbFrame->linesize
                );

                sws_freeContext(swsContext);

                // Writing RGB24 data here!

				// Data and size.
				output->write(rgbFrame->data[0], rgbFrame->linesize[0] * rgbFrame->height);

				fwrite(
                	rgbFrame->data[0],
                	1,
                	rgbFrame->linesize[0] * rgbFrame->height,
                	outputFile
				);

                av_frame_free(&rgbFrame);
            }
        }
        av_packet_unref(packet);
    }

	delete (Output::RawFile*)output;

    // Free allocated resources
    fclose(outputFile);
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&videoCodecContext);
    avformat_close_input(&inputFormatContext);
    avformat_network_deinit();

    return 0;
}
