//
// Created by ndraey on 11.09.23.
//

#pragma once
#include <cstdint>
#include <string>

namespace Output {

	class Output {
	public:
		void write(const uint8_t* data, std::size_t size);

		const std::string id;
	};

}
