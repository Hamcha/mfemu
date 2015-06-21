#include "MMU.h"

#include <functional>

typedef std::function<uint8_t(MMU* mmu)> IOHandler;

const static IOHandler empty = [](MMU* mmu) { return 0; };
const static IOHandler getters[] = {
	empty, // ff00 Joypad port
	empty, // ff01 Serial IO data
	empty, // ff02 Serial IO control
	empty, // ff03 <empty>
	empty, // ff04 Divider (?)
	empty, // ff05 Timer counter
	empty, // ff06 Timer modulo
	empty, // ff07 Timer control
	empty, // ff08 <empty>
	empty, // ff09 <empty>
	empty, // ff0a <empty>
	empty, // ff0b <empty>
	empty, // ff0c <empty>
	empty, // ff0d <empty>
	empty, // ff0e <empty>
	empty, // ff0f Interrupt flags
	empty, // ff10 Sweep (Sound mode #1)
	empty, // ff11 Sound length / Pattern duty (Sound mode #1)
	empty, // ff12 Control (Sound mode #1)
	empty, // ff13 Frequency low (Sound mode #1)
	empty, // ff14 Frequency high (Sound mode #1)
	empty, // ff15 <empty>
	empty, // ff16 Sound length / Pattern duty (Sound mode #2)
	empty, // ff17 Control (Sound mode #2)
	empty, // ff18 Frequency low (Sound mode #2)
	empty, // ff19 Frequency high (Sound mode #2)
	empty, // ff1a Control (Sound mode #3)
	empty, // ff1b Sound length (Sound mode #3)
	empty, // ff1c Output level (Sound mode #3)
	empty, // ff1d Frequency low (Sound mode #3)
	empty, // ff1e Frequency high (Sound mode #3)
	empty, // ff1f <empty>
	empty, // ff20 Sound length / Pattern duty (Sound mode #4)
	empty, // ff21 Control (Sound mode #4)
	empty, // ff22 Polynomial counter (Sound mode #4)
	empty, // ff23 Frequency high (Sound mode #4)
	empty, // ff24 Channel / Volume control
	empty, // ff25 Sound output terminal selector
	empty, // ff26 Sound ON/OFF
	empty, // ff27 <empty>
	empty, // ff28 <empty>
	empty, // ff29 <empty>
	empty, // ff2a <empty>
	empty, // ff2b <empty>
	empty, // ff2c <empty>
	empty, // ff2d <empty>
	empty, // ff2e <empty>
	empty, // ff2f <empty>
	empty, // ff30 <empty>
	empty, // ff31 <empty>
	empty, // ff32 <empty>
	empty, // ff33 <empty>
	empty, // ff34 <empty>
	empty, // ff35 <empty>
	empty, // ff36 <empty>
	empty, // ff37 <empty>
	empty, // ff38 <empty>
	empty, // ff39 <empty>
	empty, // ff3a <empty>
	empty, // ff3b <empty>
	empty, // ff3c <empty>
	empty, // ff3d <empty>
	empty, // ff3e <empty>
	empty, // ff3f <empty>
	empty, // ff40 LCD Control
	empty, // ff41 LCD Status
	empty, // ff42 Background vertical scrolling
	empty, // ff43 Background horizontal scrolling
	[](MMU* mmu) { return mmu->gpu->line; }, // ff44 Current scanline
	empty, // ff45 Scanline comparison
	empty, // ff46 DMA transfer control
	empty, // ff47 Background palette
	empty, // ff48 Sprite palette #0
	empty, // ff49 Sprite palette #1
	empty, // ff4a Window Y position
	empty, // ff4b Window X position
	empty, // ff4c <empty>
	empty, // ff4d <empty>
	empty, // ff4e <empty>
	empty, // ff4f <empty>
	empty, // ff50 <empty>
	empty, // ff51 <empty>
	empty, // ff52 <empty>
	empty, // ff53 <empty>
	empty, // ff54 <empty>
	empty, // ff55 <empty>
	empty, // ff56 <empty>
	empty, // ff57 <empty>
	empty, // ff58 <empty>
	empty, // ff59 <empty>
	empty, // ff5a <empty>
	empty, // ff5b <empty>
	empty, // ff5c <empty>
	empty, // ff5d <empty>
	empty, // ff5e <empty>
	empty, // ff5f <empty>
	empty, // ff60 <empty>
	empty, // ff61 <empty>
	empty, // ff62 <empty>
	empty, // ff63 <empty>
	empty, // ff64 <empty>
	empty, // ff65 <empty>
	empty, // ff66 <empty>
	empty, // ff67 <empty>
	empty, // ff68 <empty>
	empty, // ff69 <empty>
	empty, // ff6a <empty>
	empty, // ff6b <empty>
	empty, // ff6c <empty>
	empty, // ff6d <empty>
	empty, // ff6e <empty>
	empty, // ff6f <empty>
	empty, // ff70 <empty>
	empty, // ff71 <empty>
	empty, // ff72 <empty>
	empty, // ff73 <empty>
	empty, // ff74 <empty>
	empty, // ff75 <empty>
	empty, // ff76 <empty>
	empty, // ff77 <empty>
	empty, // ff78 <empty>
	empty, // ff79 <empty>
	empty, // ff7a <empty>
	empty, // ff7b <empty>
	empty, // ff7c <empty>
	empty, // ff7d <empty>
	empty, // ff7e <empty>
	empty, // ff7f <empty>
};

uint8_t MMU::ReadIO(const uint16_t location) {
	return getters[location](this);
}

void MMU::WriteIO(const uint16_t location, const uint8_t value) {
	return;
}