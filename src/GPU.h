#pragma once

#include <SDL.h>

class GPU {
private:
	SDL_Renderer* renderer;
	SDL_Surface* surface;

public:
	int mode;
	int cycleCount;
	int line;

	void Step(int cycles);
	GPU(SDL_Renderer* _renderer);
	~GPU();
};