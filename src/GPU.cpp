#include "GPU.h"

const uint8_t shades[] = { 0xff, 0xc0, 0x60, 0x00 };

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
				drawScreen();
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

			if (lcdControl.flags.enableLCD) {
				drawLine();
			}
		}
		break;
	}
}

GPU::GPU() {
	line = 0;
	cycleCount = 0;
	mode = 0;

	// Push at least one VRAM bank (GB classic)
	VRAMBank vbank1;
	VRAM.push_back(vbank1);
}

void GPU::InitScreen(SDL_Renderer* _renderer) {
	renderer = _renderer;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

void GPU::drawLine() {
	uint16_t mapOffset = lcdControl.flags.bgTileTable ? 0x1c00 : 0x1800;
	uint8_t tile = VRAM[VRAMbankId].bytes[mapOffset];
}

void GPU::drawScreen() {
	// Put buffer to texture
	SDL_UpdateTexture(texture, NULL, &screen, 160 * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

GPU::~GPU() {}
