//
// Created by ndraey on 11.09.23.
//

#pragma once

#include "Output.hpp"

#include "../Base.hpp"

namespace Output {

	class Debug : public Output {
	public:
		Debug(struct ::PlayerInfo* player_info);
		~Debug();

		void write(const AVFrame* frame) override;

		const std::string id = "Debug";
	};

}
