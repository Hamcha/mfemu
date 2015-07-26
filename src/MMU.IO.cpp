#include "MMU.h"

#include <functional>

typedef std::function<uint8_t(MMU* mmu)> IOHandlerR;
typedef std::function<void(MMU* mmu, uint8_t value)> IOHandlerW;

const static IOHandlerR emptyR = [](MMU*) { return 0; };
const static IOHandlerW emptyW = [](MMU*, uint8_t) { return; };

const static IOHandlerR getters[] = {
	[](MMU* mmu) { return mmu->input->data.GetRegister(); }, // ff00 Joypad port
	emptyR, // ff01 Serial IO data
	emptyR, // ff02 Serial IO control
	emptyR, // ff03 <empty>
	[](MMU* mmu) { return mmu->divider;          }, // ff04 Divider
	[](MMU* mmu) { return mmu->timerCounter;     }, // ff05 Timer counter
	[](MMU* mmu) { return mmu->timerModulo;      }, // ff06 Timer modulo
	[](MMU* mmu) { return mmu->timerControl.raw; }, // ff07 Timer control
	emptyR, // ff08 <empty>
	emptyR, // ff09 <empty>
	emptyR, // ff0a <empty>
	emptyR, // ff0b <empty>
	emptyR, // ff0c <empty>
	emptyR, // ff0d <empty>
	emptyR, // ff0e <empty>
	emptyR, // ff0f Interrupt flags
	emptyR, // ff10 Sweep (Sound mode #1)
	emptyR, // ff11 Sound length / Pattern duty (Sound mode #1)
	emptyR, // ff12 Control (Sound mode #1)
	emptyR, // ff13 Frequency low (Sound mode #1)
	emptyR, // ff14 Frequency high (Sound mode #1)
	emptyR, // ff15 <empty>
	emptyR, // ff16 Sound length / Pattern duty (Sound mode #2)
	emptyR, // ff17 Control (Sound mode #2)
	emptyR, // ff18 Frequency low (Sound mode #2)
	emptyR, // ff19 Frequency high (Sound mode #2)
	emptyR, // ff1a Control (Sound mode #3)
	emptyR, // ff1b Sound length (Sound mode #3)
	emptyR, // ff1c Output level (Sound mode #3)
	emptyR, // ff1d Frequency low (Sound mode #3)
	emptyR, // ff1e Frequency high (Sound mode #3)
	emptyR, // ff1f <empty>
	emptyR, // ff20 Sound length / Pattern duty (Sound mode #4)
	emptyR, // ff21 Control (Sound mode #4)
	emptyR, // ff22 Polynomial counter (Sound mode #4)
	emptyR, // ff23 Frequency high (Sound mode #4)
	emptyR, // ff24 Channel / Volume control
	emptyR, // ff25 Sound output terminal selector
	emptyR, // ff26 Sound ON/OFF
	emptyR, // ff27 <empty>
	emptyR, // ff28 <empty>
	emptyR, // ff29 <empty>
	emptyR, // ff2a <empty>
	emptyR, // ff2b <empty>
	emptyR, // ff2c <empty>
	emptyR, // ff2d <empty>
	emptyR, // ff2e <empty>
	emptyR, // ff2f <empty>
	emptyR, // ff30 <empty>
	emptyR, // ff31 <empty>
	emptyR, // ff32 <empty>
	emptyR, // ff33 <empty>
	emptyR, // ff34 <empty>
	emptyR, // ff35 <empty>
	emptyR, // ff36 <empty>
	emptyR, // ff37 <empty>
	emptyR, // ff38 <empty>
	emptyR, // ff39 <empty>
	emptyR, // ff3a <empty>
	emptyR, // ff3b <empty>
	emptyR, // ff3c <empty>
	emptyR, // ff3d <empty>
	emptyR, // ff3e <empty>
	emptyR, // ff3f <empty>
	[](MMU* mmu) { return mmu->gpu->lcdControl.raw; }, // ff40 LCD Control
	[](MMU* mmu) { return mmu->gpu->lcdStatus.raw;  }, // ff41 LCD Status
	[](MMU* mmu) { return mmu->gpu->bgScrollY; },      // ff42 Background vertical scrolling
	[](MMU* mmu) { return mmu->gpu->bgScrollX; },      // ff43 Background horizontal scrolling
	[](MMU* mmu) { return mmu->gpu->line; },           // ff44 Current scanline
	emptyR, // ff45 Scanline comparison
	emptyR, // ff46 DMA transfer control
	[](MMU* mmu) { return mmu->gpu->bgPalette.raw; },      // ff47 Background palette
	[](MMU* mmu) { return mmu->gpu->spritePalette1.raw; }, // ff48 Sprite palette #0
	[](MMU* mmu) { return mmu->gpu->spritePalette2.raw; }, // ff49 Sprite palette #1
	[](MMU* mmu) { return mmu->gpu->winScrollY; },         // ff4a Window Y position
	[](MMU* mmu) { return mmu->gpu->winScrollX; },         // ff4b Window X position
	emptyR, // ff4c <empty>
	emptyR, // ff4d <empty>
	emptyR, // ff4e <empty>
	emptyR, // ff4f <empty>
	emptyR, // ff50 <empty>
	emptyR, // ff51 <empty>
	emptyR, // ff52 <empty>
	emptyR, // ff53 <empty>
	emptyR, // ff54 <empty>
	emptyR, // ff55 <empty>
	emptyR, // ff56 <empty>
	emptyR, // ff57 <empty>
	emptyR, // ff58 <empty>
	emptyR, // ff59 <empty>
	emptyR, // ff5a <empty>
	emptyR, // ff5b <empty>
	emptyR, // ff5c <empty>
	emptyR, // ff5d <empty>
	emptyR, // ff5e <empty>
	emptyR, // ff5f <empty>
	emptyR, // ff60 <empty>
	emptyR, // ff61 <empty>
	emptyR, // ff62 <empty>
	emptyR, // ff63 <empty>
	emptyR, // ff64 <empty>
	emptyR, // ff65 <empty>
	emptyR, // ff66 <empty>
	emptyR, // ff67 <empty>
	emptyR, // ff68 <empty>
	emptyR, // ff69 <empty>
	emptyR, // ff6a <empty>
	emptyR, // ff6b <empty>
	emptyR, // ff6c <empty>
	emptyR, // ff6d <empty>
	emptyR, // ff6e <empty>
	emptyR, // ff6f <empty>
	emptyR, // ff70 <empty>
	emptyR, // ff71 <empty>
	emptyR, // ff72 <empty>
	emptyR, // ff73 <empty>
	emptyR, // ff74 <empty>
	emptyR, // ff75 <empty>
	emptyR, // ff76 <empty>
	emptyR, // ff77 <empty>
	emptyR, // ff78 <empty>
	emptyR, // ff79 <empty>
	emptyR, // ff7a <empty>
	emptyR, // ff7b <empty>
	emptyR, // ff7c <empty>
	emptyR, // ff7d <empty>
	emptyR, // ff7e <empty>
	emptyR, // ff7f <empty>
};

const static IOHandlerW setters[] = {
	[](MMU* mmu, uint8_t value) { mmu->input->data.SetRegister(value); }, // ff00 Joypad port
	emptyW, // ff01 Serial IO data
	emptyW, // ff02 Serial IO control
	emptyW, // ff03 <empty>
	[](MMU* mmu, uint8_t)       { mmu->divider = 0;              }, // ff04 Divider
	[](MMU* mmu, uint8_t value) { mmu->timerCounter = value;     }, // ff05 Timer counter
	[](MMU* mmu, uint8_t value) { mmu->timerModulo = value;      }, // ff06 Timer modulo
	[](MMU* mmu, uint8_t value) { mmu->timerControl.raw = value; }, // ff07 Timer control
	emptyW, // ff08 <empty>
	emptyW, // ff09 <empty>
	emptyW, // ff0a <empty>
	emptyW, // ff0b <empty>
	emptyW, // ff0c <empty>
	emptyW, // ff0d <empty>
	emptyW, // ff0e <empty>
	emptyW, // ff0f Interrupt flags
	emptyW, // ff10 Sweep (Sound mode #1)
	emptyW, // ff11 Sound length / Pattern duty (Sound mode #1)
	emptyW, // ff12 Control (Sound mode #1)
	emptyW, // ff13 Frequency low (Sound mode #1)
	emptyW, // ff14 Frequency high (Sound mode #1)
	emptyW, // ff15 <empty>
	emptyW, // ff16 Sound length / Pattern duty (Sound mode #2)
	emptyW, // ff17 Control (Sound mode #2)
	emptyW, // ff18 Frequency low (Sound mode #2)
	emptyW, // ff19 Frequency high (Sound mode #2)
	emptyW, // ff1a Control (Sound mode #3)
	emptyW, // ff1b Sound length (Sound mode #3)
	emptyW, // ff1c Output level (Sound mode #3)
	emptyW, // ff1d Frequency low (Sound mode #3)
	emptyW, // ff1e Frequency high (Sound mode #3)
	emptyW, // ff1f <empty>
	emptyW, // ff20 Sound length / Pattern duty (Sound mode #4)
	emptyW, // ff21 Control (Sound mode #4)
	emptyW, // ff22 Polynomial counter (Sound mode #4)
	emptyW, // ff23 Frequency high (Sound mode #4)
	emptyW, // ff24 Channel / Volume control
	emptyW, // ff25 Sound output terminal selector
	emptyW, // ff26 Sound ON/OFF
	emptyW, // ff27 <empty>
	emptyW, // ff28 <empty>
	emptyW, // ff29 <empty>
	emptyW, // ff2a <empty>
	emptyW, // ff2b <empty>
	emptyW, // ff2c <empty>
	emptyW, // ff2d <empty>
	emptyW, // ff2e <empty>
	emptyW, // ff2f <empty>
	emptyW, // ff30 <empty>
	emptyW, // ff31 <empty>
	emptyW, // ff32 <empty>
	emptyW, // ff33 <empty>
	emptyW, // ff34 <empty>
	emptyW, // ff35 <empty>
	emptyW, // ff36 <empty>
	emptyW, // ff37 <empty>
	emptyW, // ff38 <empty>
	emptyW, // ff39 <empty>
	emptyW, // ff3a <empty>
	emptyW, // ff3b <empty>
	emptyW, // ff3c <empty>
	emptyW, // ff3d <empty>
	emptyW, // ff3e <empty>
	emptyW, // ff3f <empty>
	[](MMU* mmu, uint8_t value) { mmu->gpu->lcdControl.raw = value; },     // ff40 LCD Control
	[](MMU* mmu, uint8_t value) { mmu->gpu->lcdStatus.raw = value;  },     // ff41 LCD Status
	[](MMU* mmu, uint8_t value) { mmu->gpu->bgScrollY = value; },          // ff42 Background vertical scrolling
	[](MMU* mmu, uint8_t value) { mmu->gpu->bgScrollX = value; },          // ff43 Background horizontal scrolling
	[](MMU* mmu, uint8_t)       { mmu->gpu->line = 0; },                   // ff44 Current scanline (reset on set)
	emptyW, // ff45 Scanline comparison
	emptyW, // ff46 DMA transfer control
	[](MMU* mmu, uint8_t value) { mmu->gpu->bgPalette.raw = value; },      // ff47 Background palette
	[](MMU* mmu, uint8_t value) { mmu->gpu->spritePalette1.raw = value; }, // ff48 Sprite palette #0
	[](MMU* mmu, uint8_t value) { mmu->gpu->spritePalette2.raw = value; }, // ff49 Sprite palette #1
	[](MMU* mmu, uint8_t value) { mmu->gpu->winScrollY = value; },         // ff4a Window Y position
	[](MMU* mmu, uint8_t value) { mmu->gpu->winScrollX = value; },         // ff4b Window X position
	emptyW, // ff4c <empty>
	emptyW, // ff4d <empty>
	emptyW, // ff4e <empty>
	emptyW, // ff4f <empty>
	[](MMU* mmu, uint8_t)       { mmu->usingBootstrap = false; },          // ff50 Disable Bootstrap ROM
	emptyW, // ff51 <empty>
	emptyW, // ff52 <empty>
	emptyW, // ff53 <empty>
	emptyW, // ff54 <empty>
	emptyW, // ff55 <empty>
	emptyW, // ff56 <empty>
	emptyW, // ff57 <empty>
	emptyW, // ff58 <empty>
	emptyW, // ff59 <empty>
	emptyW, // ff5a <empty>
	emptyW, // ff5b <empty>
	emptyW, // ff5c <empty>
	emptyW, // ff5d <empty>
	emptyW, // ff5e <empty>
	emptyW, // ff5f <empty>
	emptyW, // ff60 <empty>
	emptyW, // ff61 <empty>
	emptyW, // ff62 <empty>
	emptyW, // ff63 <empty>
	emptyW, // ff64 <empty>
	emptyW, // ff65 <empty>
	emptyW, // ff66 <empty>
	emptyW, // ff67 <empty>
	emptyW, // ff68 <empty>
	emptyW, // ff69 <empty>
	emptyW, // ff6a <empty>
	emptyW, // ff6b <empty>
	emptyW, // ff6c <empty>
	emptyW, // ff6d <empty>
	emptyW, // ff6e <empty>
	emptyW, // ff6f <empty>
	emptyW, // ff70 <empty>
	emptyW, // ff71 <empty>
	emptyW, // ff72 <empty>
	emptyW, // ff73 <empty>
	emptyW, // ff74 <empty>
	emptyW, // ff75 <empty>
	emptyW, // ff76 <empty>
	emptyW, // ff77 <empty>
	emptyW, // ff78 <empty>
	emptyW, // ff79 <empty>
	emptyW, // ff7a <empty>
	emptyW, // ff7b <empty>
	emptyW, // ff7c <empty>
	emptyW, // ff7d <empty>
	emptyW, // ff7e <empty>
	emptyW, // ff7f <empty>
};


uint8_t MMU::readIO(const uint16_t location) {
	return getters[location](this);
}

void MMU::writeIO(const uint16_t location, const uint8_t value) {
	setters[location](this, value);
	return;
}
