#pragma once

#include <SDL.h>
#include <vector>

const int
	WIDTH = 160,             //!< Gameboy screen width
	HEIGHT = 144,            //!< Gameboy screen height
	PIXELS = WIDTH * HEIGHT; //!< Gameboy framebuffer pixel count

//! Single VRAM bank
struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

//! GPU mode
enum Mode : uint8_t {
	Mode_HBlank = 0,         //!< Currently on HBlank (finished line)
	Mode_VBlank = 1,         //!< Currently on VBlank (finished frame)
	Mode_OAM    = 2,         //!< Currently reading OAM
	Mode_VRAM   = 3          //!< Currently reading VRAM (for scanline drawing)
};

//! Opacity flag
enum Opacity : uint8_t {
	Opacity_Transparent = 0, //!< Makes color #0 act as a transparent color
	Opacity_Solid       = 1  //!< Makes color #0 opaque
};

//! Sprite size flag
enum SpriteSize : uint8_t {
	SpriteSize_8x8  = 0,     //!< Sprite size 8x8 pixels
	SpriteSize_8x16 = 1      //!< Sprite size 8x16 pixels
};

//! Gameboy colors, aka shades
enum GBColor : uint8_t {
	GBColor_White     = 0,
	GBColor_LightGrey = 1,
	GBColor_DarkGrey  = 2,
	GBColor_Black     = 3
};

//! LCD Control register
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

//! LCD Status register
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

//! Palette register type (for bg and sprite #0/1 palettes)
union Palette {
	uint8_t raw;
	struct Colors {
		GBColor color0 : 2;
		GBColor color1 : 2;
		GBColor color2 : 2;
		GBColor color3 : 2;
	} colors;
};

//! Sprite Attribute Table / OAM block
struct OAMBlock {
	uint8_t y;                    //!< Y position on screen
	uint8_t x;                    //!< X position on screen
	uint8_t pattern;              //!< Pattern number
	union Flags {
		uint8_t raw;
		struct Single {
			uint8_t __unused : 4;
			uint8_t palette  : 1; //!< Palette number (#0/#1)
			uint8_t flipX    : 1; //!< Flip sprite horizontally
			uint8_t flipY    : 1; //!< Flip sprite vertically
			uint8_t priority : 1; //!< Show sprite above window
		} single;
	} flags;                      //!< Sprite flags
};

/*! \brief Game boy LCD emulation
 *
 *  Emulates the Game boy graphics behavior and LCD blitting
 *  using cycles to sync up with the rest of the machine (mainly CPU)
 */
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

	//! Sprite OAM table
	OAMBlock sprites[40];

	/*! \brief Step a number of cycles
	 *
	 *  Advances a number of cycles (relative to machine cycles)
	 *  and do OAM reading / screen blitting when necessary
	 *
	 *  \param cycles Machine cycles that have been passed
	 */
	void Step(const uint64_t cycles);

	/*! \brief Set up LCD renderer
	 *
	 *  Sets up the given renderer for blitting the Game boy
	 *  LCD output into it.
	 *  Most of the blitting is done in a texture, so the
	 *  renderer is only used to display the texture.
	 *
	 *  \param _renderer Renderer to blit LCD onto
	 */
	void InitScreen(SDL_Renderer* _renderer);

	GPU();
	~GPU();
};
