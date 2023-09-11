//
// Created by ndraey on 11.09.23.
//

#pragma once

#include "Output.hpp"

namespace Output {

	class RawFile : public Output {
	public:
		RawFile();
		~RawFile();

		void write(const uint8_t *data, std::size_t size) {
			std::printf("%8p: Write of size %zu bytes requested!\n", data, size);
		};

		const std::string id = "RawFile";
	};

}
