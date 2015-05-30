#pragma once

#include <SDL.h>
#include <string>
#include "CPU.h"

class Emulator {
	friend class Debugger;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool initSDL();
public:
	ROM rom;
	CPU cpu;

	explicit Emulator(const std::string& romfile, bool graphics = true);
	~Emulator();

	void Run();
};
