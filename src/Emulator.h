#pragma once

#include <SDL.h>
#include <string>
#include "ROM.h"
#include "MMU.h"
#include "CPU.h"
#include "GPU.h"

class Emulator {
	friend class Debugger;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool initSDL();
public:
	ROM rom;
	MMU mmu;
	CPU cpu;
	GPU gpu;

	explicit Emulator(const std::string& romfile, bool graphics = true);
	~Emulator();

	void Run();
	void Step();
};
