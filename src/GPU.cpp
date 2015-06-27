#include "GPU.h"

const uint8_t shades[] = { 0xff, 0xc0, 0x60, 0x00 };

void GPU::Step(const int cycles) {
	if (!lcdControl.flags.enableLCD) {
		lcdStatus.flags.mode = Mode_VBlank;
		line = 0;
		return;
	}

	cycleCount += cycles;

	switch (lcdStatus.flags.mode) {
	// Hblank
	case Mode_HBlank:
		if (cycleCount >= 204) {
			// Next scanline
			cycleCount = 0;
			line++;

			if (line == 143) {
				// Go into Vblank
				lcdStatus.flags.mode = Mode_VBlank;
				drawScreen();
			} else {
				lcdStatus.flags.mode = Mode_OAM;
			}
		}
		break;
	// Vblank (lasts 10 lines)
	case Mode_VBlank:
		if (cycleCount >= 456) {
			cycleCount = 0;
			line++;

			if (line > 153) {
				// Last line, go back up
				line = 0;
				lcdStatus.flags.mode = Mode_OAM;
			}
		}
		break;
	// OAM read
	case Mode_OAM:
		if (cycleCount >= 80) {
			// Go into VRAM read
			cycleCount = 0;
			lcdStatus.flags.mode = Mode_VRAM;
		}
		break;
	// VRAM read
	case Mode_VRAM:
		if (cycleCount >= 172) {
			// Go into Hblank
			cycleCount = 0;
			lcdStatus.flags.mode = Mode_HBlank;

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
	lcdStatus.flags.mode = Mode_HBlank;

	// Push at least one VRAM bank (GB classic)
	VRAMBank vbank1;
	VRAM.push_back(vbank1);
}

void GPU::InitScreen(SDL_Renderer* _renderer) {
	renderer = _renderer;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
}

void GPU::drawTile(const uint8_t tile, const int x, const int y) {
	for (int i = 0; i < 8, i < WIDTH - x; i++) {
		screen[i] = 0xff000000       // Alpha (always 1)
			|  shades[i % 4]         // Red
			| (shades[i % 4] << 8)   // Green
			| (shades[i % 4] << 16); // Blue
	}
}

void GPU::drawLine() {
	uint16_t mapOffset = lcdControl.flags.bgTileTable ? 0x1c00 : 0x1800;
	uint8_t backgroundY = (bgScrollY + line) & 0xff;
	uint8_t offsetX = bgScrollX & 0x07;
	uint8_t offsetY = backgroundY & 0x07;

	// Draw tile per tile
	for (int tx = bgScrollX; tx <= bgScrollX + WIDTH; tx += 8) {
		uint8_t backgroundX = (uint8_t)tx;

		uint8_t tileX = backgroundX / 8;
		uint8_t tileY = backgroundY / 8;

		// TODO Check for window
		uint8_t tile = VRAM[VRAMbankId].bytes[mapOffset + (tileY * 32) + tileX];

		drawTile(tile, tx, offsetY);
	}
}

void GPU::drawScreen() {
	// Put buffer to texture
	SDL_UpdateTexture(texture, NULL, &screen, WIDTH * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

GPU::~GPU() {}
