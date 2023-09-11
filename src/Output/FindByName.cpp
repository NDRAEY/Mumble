//
// Created by ndraey on 11.09.23.
//

#include <iostream>
#include "../../include/Output/FindByName.hpp"
#include "../../include/Output/Debug.hpp"
#include "../../include/Output/FramebufferIO.hpp"

namespace Output {
	Output *FindByName(const std::string& name, struct ::PlayerInfo* player_info) {
		if(name == "None") {
			// ...
		} else if(name == "Debug") {
			return new Debug(player_info);
		} else if(name == "Framebuffer") {
			return new FramebufferIO(player_info);
		} else {
			std::cerr << "error: No output found by name: " << name << std::endl;
		}

		return nullptr;
	}
}