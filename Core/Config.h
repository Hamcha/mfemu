#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include "SDL.h"
#include "Input.h"

/*! \class Configuration for mfemu.
 *
 * On startup, mfemu can optionally load a conf file containing options like
 * keybindings etc. By default, this is mfemu.conf.
 */
class Config final {
private:
	static std::unordered_map<Button, SDL_Scancode> keybindings; 
	static void parseLine(const uint32_t lineno, const std::string& line);

public:
	Config() = delete;
	Config(const Config&) = delete;
	
	const static std::string DEFAULT_FILE;

	/*! \brief Loads conigurationf from file
	 *
	 * Tries to load Config from fname. Returns false if the config wasn't
	 * properly loaded.
	 */
	static bool LoadFromFile(const std::string& fname);

	static SDL_Scancode Binding(const Button b);
};
