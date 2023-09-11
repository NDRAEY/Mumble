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

		void write(const uint8_t *data, std::size_t size) override;

		const std::string id = "RawFile";
	};

}
