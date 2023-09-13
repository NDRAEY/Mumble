//
// Created by ndraey on 11.09.23.
//

#include "../../include/Output/FramebufferIO.hpp"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdexcept>
#include <unistd.h>

std::vector<std::string> possible_fb_devices = {
		"/dev/fb0",
		"/dev/graphics/fb0"
};

namespace Output {

	FramebufferIO::FramebufferIO(struct ::PlayerInfo* player_info) : player_info(player_info) {
//		fd = open("/dev/fb0", O_RDWR);

		for(const auto& i : possible_fb_devices) {
			fd = open(i.c_str(), O_RDWR);

			if(fd >= 0) {
				std::printf("Found fb device: %s\n", i.c_str());

				break;
			}
		}

		if(fd < 0) {
			throw std::runtime_error("Could not open /dev/fb0");
		}

		ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);


		printf("Framebuffer size: %dx%d\n", vinfo.width, vinfo.height);
		printf("Framebuffer BPP: %d bits\n", vinfo.bits_per_pixel);

		player_info->window_width = (int)vinfo.width;
		player_info->window_height = (int)vinfo.height;
		player_info->window_pixfmt = (vinfo.bits_per_pixel == 32 ? (vinfo.red.offset == 16 ? AV_PIX_FMT_BGRA : AV_PIX_FMT_RGBA) : AV_PIX_FMT_RGB24);

		printf("R: %d; G: %d; B: %d; T: %d\n", vinfo.red.offset, vinfo.green.offset, vinfo.blue.offset, vinfo.transp.offset);

		vbuf = static_cast<char *>(mmap(nullptr, vinfo.width * vinfo.height * (vinfo.bits_per_pixel / 8),
										PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

		printf("Vbuf at: %p\n", vbuf);
		printf("buffer size is: %d bytes\n", vinfo.width * vinfo.height * (vinfo.bits_per_pixel / 8));
	}

	FramebufferIO::~FramebufferIO() {
		close(fd);

		if(vbuf)
			munmap(vbuf, vinfo.width * vinfo.height * (vinfo.bits_per_pixel / 8));
	};

	// TODO: !!!: Not tested yet! My framebuffer gives me invalid configuration!
	void FramebufferIO::write(const AVFrame *frame) {
//		memcpy(vbuf, frame->data[0], vinfo.width * vinfo.height * (vinfo.bits_per_pixel / 8));

		uint32_t bpp = vinfo.bits_per_pixel >> 3;
		uint32_t pitch = (bpp * vinfo.width);

		for(size_t y = 0; y < vinfo.height; y++) {
			for(size_t x = 0; x < vinfo.width; x++) {
				uint32_t in_coord = y * (bpp * frame->width) + (x * bpp);
				uint32_t out_coord = y * pitch + (x * bpp);

				vbuf[out_coord + 0] = (char)frame->data[0][in_coord + 0];
				vbuf[out_coord + 1] = (char)frame->data[0][in_coord + 1];
				vbuf[out_coord + 2] = (char)frame->data[0][in_coord + 2];

				if(bpp == 4)
					vbuf[out_coord + 3] = (char)frame->data[0][in_coord + 3];
			}
		}
	}

	Output* FramebufferIO::create(struct ::PlayerInfo* info) {
		return new FramebufferIO(info);
	}
}
