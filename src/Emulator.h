#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "CPU.h"

class Emulator {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool InitSDL();
public:
	ROM rom;
	CPU cpu;

	Emulator(std::string romfile);
	~Emulator();

	void Run();
};
