#pragma once

#include <SDL.h>

class GPU {
private:
	int mode;
	int cycleCount;
	int line;
	SDL_Renderer* renderer;

public:
	void Step(int cycles);
	GPU(SDL_Renderer* _renderer);
};