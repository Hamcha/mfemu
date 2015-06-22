#pragma once

#include <SDL.h>

class GPU {
private:
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	uint32_t screen[160*144];

	void drawScreen();

public:
	int mode;
	int cycleCount;
	int line;

	void Step(int cycles);
	GPU(SDL_Renderer* _renderer);
	~GPU();
};
