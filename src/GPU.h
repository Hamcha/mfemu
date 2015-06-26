#pragma once

#include <SDL.h>
#include <vector>

const int PIXELS = 160*144;

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

//! GPU mode
enum Mode {
	Mode_HBlank = 0, //!< Currently on HBlank (finished line)
	Mode_VBlank = 1, //!< Currently on VBlank (finished frame)
	Mode_OAM    = 2, //!< Currently reading OAM
	Mode_VRAM   = 3  //!< Currently reading VRAM (for scanline drawing)
};

//! Opacity flag
enum Opacity {
	Opacity_Transparent = 0, //!< Makes color #0 act as a transparent color
	Opacity_Solid = 1        //!< Makes color #0 opaque
};

//! Sprite size flag
enum SpriteSize {
	SpriteSize_8x8 = 0, //!< Sprite size 8x8 pixels
	SpriteSize_8x16 = 1 //!< Sprite size 8x16 pixels
};

union LCDControl {
	uint8_t raw;
	struct Flags {
		unsigned int displayBackground : 1; //!< Show background
		Opacity      color0Opacity     : 1; //!< Is Color #0 visible or transparent
		SpriteSize   spriteSize        : 1; //!< Sprite size
		unsigned int bgTileTable       : 1; //!< Background tilemap (#0 or #1)
		unsigned int tilePatternTable  : 1; //!< Background tileset (#0 or #1)
		unsigned int displayWindow     : 1; //!< Show window
		unsigned int windowTileTable   : 1; //!< Window tilemap (#0 or #1)
		unsigned int enableLCD         : 1; //!< Enable display
	} flags;
};

union LCDStatus {
	uint8_t raw;
	struct Flags {
		Mode mode                    : 2; //!< Current mode
		unsigned int coincidenceFlag : 1; //!< Interrupt on coincidence
		unsigned int intMode0        : 1; //!< Mode 00 interrupt flag
		unsigned int intMode1        : 1; //!< Mode 01 interrupt flag
		unsigned int intMode2        : 1; //!< Mode 10 interrupt flag
		unsigned int intScanline     : 1; //!< Scanline coincidence interrupt flag
		unsigned int _unused         : 1; //!< Unused bit
	} flags;
};

class GPU {
private:
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	uint32_t screen[PIXELS];

	void drawLine();
	void drawScreen();

public:
	//! Current cycle (in machine cycles)
	int cycleCount;

	//! Current scanline
	int line;

	uint8_t bgPalette,      //!< Background color palette
	        spritePalette1, //!< Sprite color palette #0
	        spritePalette2, //!< Sprite color palette #1
	        bgScrollX,      //!< Background horizontal scrolling
	        bgScrollY,      //!< Background vertical scrolling
	        winScrollX,     //!< Window horizontal scrolling
	        winScrollY;     //!< Window vertical scrolling

	//! LCD control flags
	LCDControl lcdControl;

	//! LCD status flags
	LCDStatus  lcdStatus;

	//! Video RAM (1 bank on GB, 2 on GBC)
	std::vector<VRAMBank> VRAM;
	//! Current VRAM Bank (only changes on GBC)
	uint8_t VRAMbankId = 0;

	void Step(int cycles);
	void InitScreen(SDL_Renderer* _renderer);
	GPU();
	~GPU();
};
