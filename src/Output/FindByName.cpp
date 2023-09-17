//
// Created by ndraey on 11.09.23.
//

#include <iostream>
#include <vector>

#include "../../include/Output/FindByName.hpp"

namespace Output {
	outfunc_t FindFuncByName(const std::string& name) {
		for(const auto& i : func_mapping) {
			if(i.first == name) {
				return i.second;
			}
		}
		
		return nullptr;
	}

	Output *FindByName(const std::string& name, struct ::PlayerInfo* player_info) {
		/*if(name == "None") {
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
	*/
		auto func = FindFuncByName(name);
		if(!func) {
			return nullptr;
		}

		return func(player_info);
	}

	
}
