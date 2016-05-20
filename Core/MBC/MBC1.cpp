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
		//TODO should limit rom bank id if ramBanking is enabled
		return banks[romBankId].bytes[location - 0x4000];
	}
	// Not applicable (used by VRAM)
	if (location < 0xa000) {
		throw std::domain_error("Trying to access VRAM in ROM");
	}
	// External RAM (on cartridge)
	if (location < 0xc000) {
		if (hasRam) {
			// Check if we have all the 8k of the RAM bank available or only 2k
			if (header.RAMSize == RAM_2KB && location > 0xa800) {
				throw std::domain_error("Trying to access inexistant RAM memory (>2k)");
			}

			return ram[ramBankId].bytes[location - 0xa000];
		} else {
			throw std::domain_error("Trying to access inexistent RAM memory (RAM not available)");
		}
	}

	throw std::logic_error("Trying to access non-ROM memory in ROM");
}

void MBC1::Write(const uint16_t location, const uint8_t value) {
	// RAM Enable flag (true if lower 4 bits are 0xa)
	if (location < 0x2000) {
		ramEnabled = (value & 0xf) == 0xa;
		return;
	}

	// ROM Bank select (lower 5 bits)
	if (location < 0x4000) {
		// Select lower 5 bits
		uint8_t maskedValue = value & 0x1f;

		// If 0, add 1
		if (maskedValue == 0) {
			maskedValue += 1;
		}

		// Set lower 5 bits of the ROM bank register
		romBankId &= 0xe0;
		romBankId |= maskedValue;
		return;
	}

	// RAM Bank select OR ROM Bank select (upper 2 bits)
	if (location < 0x6000) {
		if (ramBankingEnable) {
			ramBankId = value & 0x3;
		} else {
			romBankId &= 0x1f;
			romBankId |= (value & 0x3) << 5;
		}
		return;
	}

	// ROM/RAM Banking select
	if (location < 0x8000) {
		ramBankingEnable = value == 0x1;
		return;
	}

	// External RAM (on cartridge)
	if (location < 0xc000) {
		if (hasRam) {
			// Check if we have all the 8k of the RAM bank available or only 2k
			if (header.RAMSize == RAM_2KB && location > 0xa800) {
				throw std::domain_error("Trying to write to inexistant RAM memory (>2k)");
			}

			ram[ramBankId].bytes[location - 0xa000] = value;
			return;
		} else {
			throw std::domain_error("Trying to write to inexistent RAM memory (RAM not available)");
		}
	}

	throw std::logic_error("Trying to write non-ROM memory in ROM");
}
