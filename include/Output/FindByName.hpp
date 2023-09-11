//
// Created by ndraey on 11.09.23.
//

#pragma once
#include "Output.hpp"

#include "../Base.hpp"

namespace Output {
	Output *FindByName(const std::string& name, struct ::PlayerInfo* player_info);
}