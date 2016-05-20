#include "Config.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>

static std::unordered_map<std::string, Button> nameToButton = {
	{ "A",      ButtonA      },
	{ "B",      ButtonB      },
	{ "Up",     ButtonUp     },
	{ "Down",   ButtonDown   },
	{ "Left",   ButtonLeft   },
	{ "Right",  ButtonRight  },
	{ "Start",  ButtonStart  },
	{ "Select", ButtonSelect }
};

const std::string Config::DEFAULT_FILE = "mfemu.conf";

std::unordered_map<Button, SDL_Scancode> Config::keybindings = {
	// Default configuration
	{ ButtonB,      SDL_SCANCODE_Z         },
	{ ButtonA,      SDL_SCANCODE_X         },
	{ ButtonStart,  SDL_SCANCODE_RETURN    },
	{ ButtonSelect, SDL_SCANCODE_BACKSPACE },
	{ ButtonUp,     SDL_SCANCODE_UP        },
	{ ButtonDown,   SDL_SCANCODE_DOWN      },
	{ ButtonLeft,   SDL_SCANCODE_LEFT      },
	{ ButtonRight,  SDL_SCANCODE_RIGHT     }
};

bool Config::LoadFromFile(const std::string& fname) {
	std::ifstream confFile(fname);
	if (!confFile.good())
		return false;

	std::string line;
	
	uint32_t lineno = 1;
	std::getline(confFile, line);
	while (!confFile.eof()) {
		if (line.length() > 0 && line[0] != '#') {
			parseLine(lineno, line);
		}
		std::getline(confFile, line);
		++lineno;
	}

	return true;	
}

SDL_Scancode Config::Binding(const Button b) {
	auto code = keybindings.find(b);
	if (code != keybindings.end())
		return code->second;

	return SDL_SCANCODE_UNKNOWN;
}

void Config::parseLine(const uint32_t lineno, const std::string& line) {
	std::istringstream ss(line);
	std::string cmd;

	std::stringstream errPrelude;
	errPrelude << "[At line " << lineno << "] ";

	auto startsWith = [] (const std::string& str, const std::string& substr) -> bool {
		return strncmp(str.c_str(), substr.c_str(), substr.length()) == 0;
	};

	ss >> cmd;
	if (startsWith(cmd, "button.")) {
		auto key = cmd.substr(7);
		auto it = nameToButton.find(key);
		if (it == nameToButton.end()) {
			std::cerr << errPrelude.str() << "Unknown button: " << key << std::endl;
		} else {
			std::string name;
			ss >> name; // eat '='
			ss >> name;
			keybindings[it->second] = SDL_GetScancodeFromName(name.c_str());
			std::clog << "[INFO] Bound " << it->first << " to "
				<< SDL_GetScancodeName(keybindings[it->second]) << "\r\n";
		}	
	}
}
