#include "MMU.h"

// Gameboy bootstrap ROM
const uint8_t bootstrap[] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21,
	0x26, 0xFF, 0x0E, 0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32,
	0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0, 0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80,
	0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B, 0xFE, 0x34, 0x20, 0xF3,
	0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9, 0x3E,
	0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32,
	0x0D, 0x20, 0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42,
	0x3E, 0x91, 0xE0, 0x40, 0x04, 0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90,
	0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2, 0x0E, 0x13, 0x24, 0x7C, 0x1E,
	0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06, 0x7B, 0xE2,
	0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05,
	0x20, 0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17,
	0xC1, 0xCB, 0x11, 0x17, 0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE,
	0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C,
	0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E,
	0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9,
	0xA5, 0x42, 0x3C, 0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20,
	0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20, 0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05,
	0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};

uint8_t MMU::Read(const uint16_t location) {
	// 0000 - 0100 => Bootstrap ROM (only if turned on)
	if (usingBootstrap && location < 0x0100) {
		return bootstrap[location];
	}

	if (location < 0x8000) {
		return rom->controller->Read(location);
	}

	// 8000 - 9fff => VRAM bank (switchable in GBC)
	if (location < 0xa000) {
		return gpu->VRAM[gpu->VRAMbankId].bytes[location - 0x8000];
	}

	// a000 - bfff => External RAM (switchable)
	if (location < 0xc000) {
		return rom->controller->Read(location);
	}

	// c000 - cfff => Work RAM fixed bank
	if (location < 0xd000) {
		return WRAM.bytes[location - 0xc000];
	}

	// d000 - dfff => Switchable Work RAM bank
	if (location < 0xe000) {
		return WRAMbanks[WRAMbankId].bytes[location - 0xd000];
	}

	// e000 - fdff => Mirror of c000 - ddff
	if (location < 0xfe00) {
		return Read(location - 0x2000);
	}

	// fe00 - fe9f => Sprite attribute table
	if (location < 0xfea0) {
		// Get OAM item
		uint8_t index = location / 4;
		OAMBlock block = gpu->sprites[index];

		// Get requested byte
		uint8_t offset = location % 4;
		switch (offset) {
			case 0: return block.x;
			case 1: return block.y;
			case 2: return block.pattern;
			case 3: return block.flags.raw;
			default: throw std::logic_error("Bad OAM offset");
		}
	}

	// fea0 - feff => Not usable
	if (location < 0xff00) {
		return 0;
	}

	// ff00 - ff7f => I/O Registers
	if (location < 0xff80) {
		return readIO(location - 0xff00);
	}

	// ff80 - fffe => High RAM (HRAM)
	if (location < 0xffff) {
		return ZRAM.bytes[location - 0xff80];
	}

	// ffff => Interrupt mask
	return interruptEnable.raw;
}

void MMU::Write(const uint16_t location, const uint8_t value) {
	// 0000 - 7fff => ROM (Not writable)
	if (location < 0x8000) {
		rom->controller->Write(location, value);
		return;
	}

	// 8000 - 9fff => VRAM bank (switchable in GBC)
	if (location < 0xa000) {
		gpu->VRAM[gpu->VRAMbankId].bytes[location - 0x8000] = value;
		return;
	}

	// a000 - bfff => External RAM (switchable)
	if (location < 0xc000) {
		rom->controller->Write(location, value);
		return;
	}

	// c000 - cfff => Work RAM fixed bank
	if (location < 0xd000) {
		WRAM.bytes[location - 0xc000] = value;
		return;
	}

	// d000 - dfff => Switchable Work RAM bank
	if (location < 0xe000) {
		WRAMbanks[WRAMbankId].bytes[location - 0xd000] = value;
		return;
	}

	// e000 - fdff => Mirror of c000 - ddff (Not writable)
	if (location < 0xfe00) { return; }

	// fe00 - fe9f => Sprite attribute table
	if (location < 0xfea0) {
		// Get OAM item
		uint8_t index = location / 4;
		OAMBlock* block = &(gpu->sprites[index]);

		// Get requested byte
		uint8_t offset = location % 4;
		switch (offset) {
			case 0:
				block->x = value;
				return;
			case 1:
				block->y = value;
				return;
			case 2:
				block->pattern = value;
				return;
			case 3:
				block->flags.raw = value;
				return;
			default:
				throw std::logic_error("Bad OAM offset");
		}
	}

	// fea0 - feff => Not usable
	if (location < 0xff00) { return; }

	// ff00 - ff7f => I/O Registers
	if (location < 0xff80) {
		writeIO(location - 0xff00, value);
		return;
	}

	// ff80 - fffe => High RAM (HRAM)
	if (location < 0xffff) {
		ZRAM.bytes[location - 0xff80] = value;
		return;
	}

	// ffff => Interrupt mask
	interruptEnable.raw = value;
}

void MMU::UpdateTimers(CycleCount delta) {
	// Update divider (one increment every 256 clocks), include past extra clocks (dividerRest)
	divider += (uint8_t) ((delta.cpu + dividerRest) / 256);

	// Store eventual extra clocks into the dividerRest to add at a later time
	// Beware of nasty trick:
	//   dividerRest is uint8, so it will overflow every 256 clocks
	//   I exploit this as an automatic "% 256"
	uint8_t oldDivider = dividerRest;
	dividerRest += (uint8_t)delta.cpu;

	// If dividerRest overflows, increment the timer one more time
	if (dividerRest < oldDivider) {
		divider += 1;
	}

	// Check if the controlled timer is enabled
	if (timerControl.values.enabled == 1) {
		// Make a copy of the timer for the modulo comparison
		uint8_t originalTimer = timerCounter;
		// Make a copy of the counter rest for overflow checking
		uint8_t originalRest = counterRest;

		// Add ticks depending on timer configuration
		switch (timerControl.values.clock) {
		case ClockDiv1024:
			timerCounter += (uint8_t) ((delta.cpu + counterRest) / 1024);
			counterRest = (counterRest + delta.cpu) % 1024;
			break;
		case ClockDiv256:
			timerCounter += (uint8_t) ((delta.cpu + counterRest) / 256);
			counterRest = (counterRest + delta.cpu) % 256;
			break;
		case ClockDiv64:
			timerCounter += (uint8_t) ((delta.cpu + counterRest) / 64);
			counterRest = (counterRest + delta.cpu) % 64;
			break;
		case ClockDiv16:
			timerCounter += (uint8_t) ((delta.cpu + counterRest) / 16);
			counterRest = (counterRest + delta.cpu) % 16;
			break;
		}

		// Check for modulo increment
		if (originalTimer > timerCounter) {
			timerModulo += 1;
		}

		// Check for counterRest overflow
		if (originalRest > counterRest) {
			timerCounter += 1;
		}
	}
}

MMU::MMU(ROM* romData, GPU* _gpu, Input* _input) {
	// Setup variables
	rom = romData;
	gpu = _gpu;
	input = _input;
	usingBootstrap = true;

	// Reset timers
	timerModulo = timerCounter = timerControl.raw = divider = 0;
	counterRest = dividerRest = 0;

	// Reset interrupts
	interruptFlags.raw = interruptEnable.raw = 0;
	interruptsEnabled = true;

	// Push at least one WRAM bank (GB classic)
	WRAMBank wbank1;
	WRAMbanks.push_back(wbank1);
}

void MMU::SetInterrupt(InterruptType type) {
	// Check if the interrupt is enabled
	uint8_t index = 1 << (uint8_t) type;
	if (interruptEnable.raw & index) {
		interruptFlags.raw |= index;
	}
}

void MMU::UnsetInterrupt(InterruptType type) {
	uint8_t index = 1 << (uint8_t) type;
	interruptFlags.raw &= ~index;
}
