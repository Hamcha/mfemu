#include "GPU.h"

const uint32_t shades[] = { 0xffffffff, 0xffc0c0c0, 0xff606060, 0xff000000 };

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

void GPU::drawLine() {
	if (lcdControl.flags.displayBackground) {
		uint16_t mapOffset = lcdControl.flags.bgTileTable ? 0x1c00 : 0x1800;
		uint16_t dataOffset = lcdControl.flags.tilePatternTable ? 0x0800 : 0x0000;

		uint8_t lineOffset = line % 8;

		//TODO Calculate scroll offset

		for (int tx = 0; tx <= WIDTH / 8; tx += 1) {
			// Get tile id
			uint8_t tileId = VRAM[VRAMbankId].bytes[mapOffset + tx];

			// Convert signed to unsigned if Pattern table #1
			if (lcdControl.flags.tilePatternTable == 1) {
				tileId ^= 0x80;
			}

			// Get colors (2 bytes) of the current tile's line
			uint16_t colorOffset = dataOffset + tileId * 16 + lineOffset * 2;
			uint8_t color0 = VRAM[VRAMbankId].bytes[colorOffset];
			uint8_t color1 = VRAM[VRAMbankId].bytes[colorOffset+1];

			//TODO Plot line
			for (int pixel = 0; pixel < 8; pixel += 1) {
				// This is a 2 bit number, MSB is color0[pixel], LSB is color1[pixel]
				uint8_t colorId = (color0 >> pixel & 0x1 << 1) | (color1 >> pixel & 0x1);

				// Get actual color from the palette
				uint8_t actualColor = (bgPalette.raw >> (colorId * 2)) & 0x3;

				// Set pixel to shade defined by the color
				screen[line * WIDTH + tx * 8 + pixel] = shades[actualColor];
			}
		}
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
