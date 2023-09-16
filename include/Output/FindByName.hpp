//
// Created by ndraey on 11.09.23.
//

#pragma once
#include "Output.hpp"

#include "../Base.hpp"

#include "../../include/Output/FindByName.hpp"
#include "../../include/Output/Debug.hpp"
#include "../../include/Output/FramebufferIO.hpp"
#include "../../include/Output/TerminalIO.hpp"

namespace Output {
	typedef Output* (*outfunc_t)(struct ::PlayerInfo*);
	
	static std::vector<std::pair<std::string, outfunc_t>> func_mapping = {
		{"None", nullptr},
		{"Debug", &Debug::create},
		{"Framebuffer", &FramebufferIO::create},
		{"Terminal", &TerminalIO::create}
	};

	Output *FindByName(const std::string& name, struct ::PlayerInfo* player_info);
}
