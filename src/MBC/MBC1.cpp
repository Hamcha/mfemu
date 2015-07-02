#include "../MBC.h"

#include <stdexcept>

MBC1::MBC1(const ROMType type) {
	switch (type) {
	case ROM_MBC1_RAM: hasRam = true; break;
	case ROM_MBC1_R_B: hasRam = hasBattery = true; break;
	default: hasRam = hasBattery = false;
	}
}

uint8_t MBC1::Read(const uint16_t location) const {
	if (location < 0x4000) {
		return banks[0].bytes[location];
	}
	if (location < 0x8000) {
		return banks[romBankId].bytes[location - 0x4000];
	}
	// Not applicable (used by VRAM)
	if (location < 0xa000) {
		throw std::domain_error("Trying to access VRAM in ROM");
	}
	// External RAM (on cartridge)
	if (hasRam) {
		if (location < 0xc000) {
			ram[ramBankId].bytes[location - 0xa000];
		}
	} else {
		throw std::domain_error("Trying to access inexistent RAM memory");
	}

	throw std::domain_error("Trying to access non-ROM memory in ROM");
}

void MBC1::Write(const uint16_t location, const uint8_t value) {
	//TODO
}
