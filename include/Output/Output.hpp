//
// Created by ndraey on 11.09.23.
//

#pragma once
#include <cstdint>
#include <string>

namespace Output {

	class Output {
	public:
		virtual void write(const uint8_t* data, std::size_t size) = 0;

		const std::string id;
	};

}