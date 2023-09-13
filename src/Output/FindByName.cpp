//
// Created by ndraey on 11.09.23.
//

#include <iostream>
#include <vector>
#include "../../include/Output/FindByName.hpp"
#include "../../include/Output/Debug.hpp"
#include "../../include/Output/FramebufferIO.hpp"
#include "../../include/Output/TerminalIO.hpp"

namespace Output {
	Output *FindByName(const std::string& name, struct ::PlayerInfo* player_info) {
		std::vector<std::pair<std::string, Output* (*)(struct ::PlayerInfo*)>> mapping = {
			{"None", nullptr},
			{"Debug", &Debug::create},
			{"Framebuffer", &FramebufferIO::create},
			{"Terminal", &TerminalIO::create}
		};

		if(name == "None") {
			// ...
		} else if(name == "Debug") {
			return new Debug(player_info);
		} else if(name == "Framebuffer") {
			return new FramebufferIO(player_info);
		} else if(name == "Terminal") {
			return new TerminalIO(player_info);
		} else {
			std::cerr << "error: No output found by name: " << name << std::endl;
		}

		return nullptr;
	}
}
