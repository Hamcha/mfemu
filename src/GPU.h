#pragma once

#include <SDL.h>
#include <vector>

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

enum Opacity {
	Opacity_Transparent = 0,
	Opacity_Solid = 1
};

enum SpriteSize {
	SpriteSize_8x8 = 0,
	SpriteSize_8x16 = 1
};

union LCDControl {
	uint8_t raw;
	struct Flags {
		unsigned int displayBackground : 1;
		Opacity color0Opacity : 1;
		SpriteSize spriteSize : 1;
		unsigned int bgTileTable : 1;
		unsigned int tilePatternTable : 1;
		unsigned int displayWindow : 1;
		unsigned int windowTileTable : 1;
		unsigned int enableLCD : 1;
	} flags;
};

class GPU {
private:
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	uint32_t screen[160*144];

	void drawLine();
	void drawScreen();

public:
	int mode;
	int cycleCount;
	int line;
	uint8_t bgPalette, spritePalette1, spritePalette2;
	uint8_t bgScrollX, bgScrollY, winScrollX, winScrollY;
	LCDControl lcdControl;

	std::vector<VRAMBank> VRAM;
	uint8_t VRAMbankId = 0;

	void Step(int cycles);
	GPU();
	void InitScreen(SDL_Renderer* _renderer);
	~GPU();
};
