#pragma once

#include <SDL.h>
#include <vector>

const int WIDTH = 160, HEIGHT = 144;
const int PIXELS = WIDTH * HEIGHT;

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

//! GPU mode
enum Mode : uint8_t {
	Mode_HBlank = 0, //!< Currently on HBlank (finished line)
	Mode_VBlank = 1, //!< Currently on VBlank (finished frame)
	Mode_OAM    = 2, //!< Currently reading OAM
	Mode_VRAM   = 3  //!< Currently reading VRAM (for scanline drawing)
};

//! Opacity flag
enum Opacity : uint8_t {
	Opacity_Transparent = 0, //!< Makes color #0 act as a transparent color
	Opacity_Solid       = 1  //!< Makes color #0 opaque
};

//! Sprite size flag
enum SpriteSize : uint8_t {
	SpriteSize_8x8  = 0,//!< Sprite size 8x8 pixels
	SpriteSize_8x16 = 1 //!< Sprite size 8x16 pixels
};

//! Gameboy colors, aka shades
enum GBColor : uint8_t {
	GBColor_White     = 0,
	GBColor_LightGrey = 1,
	GBColor_DarkGrey  = 2,
	GBColor_Black     = 3
};

union LCDControl {
	uint8_t raw;
	struct Flags {
		uint8_t    displayBackground : 1; //!< Show background
		Opacity    color0Opacity     : 1; //!< Is Color #0 visible or transparent
		SpriteSize spriteSize        : 1; //!< Sprite size
		uint8_t    bgTileTable       : 1; //!< Background tilemap (#0 or #1)
		uint8_t    tilePatternTable  : 1; //!< Background tileset (#0 or #1)
		uint8_t    displayWindow     : 1; //!< Show window
		uint8_t    windowTileTable   : 1; //!< Window tilemap (#0 or #1)
		uint8_t    enableLCD         : 1; //!< Enable display
	} flags;
};

union LCDStatus {
	uint8_t raw;
	struct Flags {
		Mode    mode            : 2; //!< Current mode
		uint8_t coincidenceFlag : 1; //!< Interrupt on coincidence
		uint8_t intMode0        : 1; //!< Mode 00 interrupt flag
		uint8_t intMode1        : 1; //!< Mode 01 interrupt flag
		uint8_t intMode2        : 1; //!< Mode 10 interrupt flag
		uint8_t intScanline     : 1; //!< Scanline coincidence interrupt flag
		uint8_t _unused         : 1; //!< Unused bit
	} flags;
};

union Palette {
	uint8_t raw;
	struct Colors {
		GBColor color0 : 2;
		GBColor color1 : 2;
		GBColor color2 : 2;
		GBColor color3 : 2;
	} colors;
};

class GPU {
private:
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	uint32_t screen[PIXELS];

	uint32_t lastFrameTime;

	void drawLine();
	void drawScreen();

public:
	//! VSync speed percent (relative to real Gameboy)
	double percent;

	//! Current cycle (in machine cycles)
	uint64_t cycleCount;

	//! Current scanline
	uint8_t line;

	//! Has VBlank happened?
	bool didVblank;

	Palette bgPalette,      //!< Background color palette
	        spritePalette1, //!< Sprite color palette #0
	        spritePalette2; //!< Sprite color palette #1
	uint8_t bgScrollX,      //!< Background horizontal scrolling
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

	void Step(const uint64_t cycles);
	void InitScreen(SDL_Renderer* _renderer);
	GPU();
	~GPU();
};
