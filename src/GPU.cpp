#include "GPU.h"

// BGB palette
const uint32_t shades[] = { 0xffe7ffd6, 0xff88c070, 0xff346856, 0xff081820 };

void GPU::Step(const uint64_t cycles) {
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

			// Trigger LCD interrupt if the Scanline coincidence int mode flag is on
			if (lcdStatus.flags.intScanline) {
				// If the Concidence flag is set to COINCIDENCE, trigger if the current
				// line is the same as the coincidence register, otherwise if isn't
				if (lcdStatus.flags.coincidenceFlag) {
					didLCDInterrupt = line == coincidence;
				} else {
					didLCDInterrupt = line != coincidence;
				}
			}

			if (line == 143) {
				// Go into Vblank
				lcdStatus.flags.mode = Mode_VBlank;
				drawScreen();
				didVblank = true;

				// Trigger LCD interrupt if the Vblank int mode flag is on
				if (lcdStatus.flags.intMode1) {
					didLCDInterrupt = true;
				}
			} else {
				lcdStatus.flags.mode = Mode_OAM;

				// Trigger LCD interrupt if the OAM int mode flag is on
				if (lcdStatus.flags.intMode2) {
					didLCDInterrupt = true;
				}
			}
		}
		break;
	// Vblank (lasts 10 lines)
	case Mode_VBlank:
		if (cycleCount >= 456) {

			cycleCount = 0;
			line++;

			// Trigger LCD interrupt if the Scanline coincidence int mode flag is on
			if (lcdStatus.flags.intScanline) {
				// If the Concidence flag is set to COINCIDENCE, trigger if the current
				// line is the same as the coincidence register, otherwise if isn't
				if (lcdStatus.flags.coincidenceFlag) {
					didLCDInterrupt = line == coincidence;
				} else {
					didLCDInterrupt = line != coincidence;
				}
			}

			if (line > 153) {
				// Last line, go back up
				line = 0;
				lcdStatus.flags.mode = Mode_OAM;

				// Trigger LCD interrupt if the OAM int mode flag is on
				if (lcdStatus.flags.intMode2) {
					didLCDInterrupt = true;
				}
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

			// Trigger LCD interrupt if the HBlank int mode flag is on
			if (lcdStatus.flags.intMode0) {
				didLCDInterrupt = true;
			}
		}
		break;
	}
}

GPU::GPU() {
	line = 0;
	cycleCount = 0;
	bgScrollX = bgScrollY = 0;
	didVblank = didLCDInterrupt = false;
	lcdStatus.flags.mode = Mode_HBlank;
	lastFrameTime = SDL_GetTicks();

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
		uint16_t dataOffset = lcdControl.flags.tilePatternTable ? 0x0000 : 0x0800;

		uint8_t ty = line + bgScrollY;
		uint16_t tileRow = ty / 8;
		uint8_t lineOffset = ty % 8;

		//TODO Calculate scroll offset

		for (uint8_t x = 0; x <= WIDTH; x += 1) {
			uint8_t tx = x + bgScrollX;

			// Get tile id
			uint8_t tileCol = tx / 8;
			uint8_t tileId = VRAM[VRAMbankId].bytes[mapOffset + (tileRow * 32) + tileCol];

			// Convert signed to unsigned if Pattern table #1
			if (lcdControl.flags.tilePatternTable == 0) {
				tileId ^= 0x80;
			}

			// Get colors (2 bytes) of the current tile's line
			uint16_t colorOffset = dataOffset + (tileId * 16) + (lineOffset * 2);
			uint8_t color0 = VRAM[VRAMbankId].bytes[colorOffset];
			uint8_t color1 = VRAM[VRAMbankId].bytes[colorOffset + 1];

			// Get palette color id of current pixel
			// This is a 2 bit number, MSB is color1[pixel], LSB is color0[pixel]
			uint8_t tileOffset = 7 - tx % 8;
			uint8_t colorId = ((color1 >> tileOffset & 0x1) << 1) | (color0 >> tileOffset & 0x1);

			// Get actual color from the palette
			uint8_t actualColor = (bgPalette.raw >> (colorId * 2)) & 0x3;

			// Set pixel to shade defined by the color
			screen[line * WIDTH + x] = shades[actualColor];
		}
	}
}

void GPU::drawScreen() {
	// Update speed %
	uint32_t now = SDL_GetTicks();
	uint32_t diff = now - lastFrameTime;
	percent = 1666.66 / diff;
	lastFrameTime = now;

	// Put buffer to texture
	SDL_UpdateTexture(texture, NULL, &screen, WIDTH * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

GPU::~GPU() {}
