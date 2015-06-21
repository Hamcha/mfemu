#include "GPU.h"

void GPU::Step(int cycles) {
	cycleCount += cycles;

	switch (mode) {
	// Hblank
	case 0:
		if (cycleCount >= 204) {
			// Next scanline
			cycleCount = 0;
			line++;

			if (line == 143) {
				// Go into Vblank
				mode = 1;
				// DRAW
			} else {
				mode = 2;
			}
		}
		break;
	// Vblank (lasts 10 lines)
	case 1:
		if (cycleCount >= 456) {
			cycleCount = 0;
			line++;

			if (line > 153) {
				// Last line, go back up
				line = 0;
				mode = 2;
			}
		}
		break;
	// OAM read
	case 2:
		if (cycleCount >= 80) {
			// Go into VRAM read
			cycleCount = 0;
			mode = 3;
		}
		break;
	// VRAM read
	case 3:
		if (cycleCount >= 172) {
			// Go into Hblank
			cycleCount = 0;
			mode = 0;

			//TODO Write scanline
		}
		break;
	}
}

GPU::GPU(SDL_Renderer* _renderer) {
	line = 0;
	cycleCount = 0;
	mode = 0;
	renderer = _renderer;

	uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	surface = SDL_CreateRGBSurface(0, 160, 144, 32, rmask, gmask, bmask, amask);
}

GPU::~GPU() {
	SDL_FreeSurface(surface);
}