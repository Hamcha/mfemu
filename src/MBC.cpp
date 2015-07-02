#include "MBC.h"

#include <stdexcept>

void MBC::LoadROM(const ROMHeader& header, const std::vector<uint8_t>& bytes) {
	// Get banks from buffer
	int bankCount;
	switch (header.ROMSize) {
	case ROM_32K:  bankCount = 2; break;
	case ROM_64K:  bankCount = 4; break;
	case ROM_128K: bankCount = 8; break;
	case ROM_256K: bankCount = 16; break;
	case ROM_512K: bankCount = 32; break;
	case ROM_1M:   bankCount = header.Type == ROM_MBC1 ? 63 : 64; break;
	case ROM_2M:   bankCount = header.Type == ROM_MBC1 ? 125 : 128; break;
	case ROM_4M:   bankCount = 256; break;
	default:       throw std::runtime_error("Erroneous ROM Type");
	}

	// Read banks from buffer
	banks.reserve(bankCount);
	for (int i = 0; i < bankCount; i += 1) {
		ROMBank b;
		std::copy(bytes.begin() + 16 * 1024 * i, bytes.begin() + 16 * 1024 * (i + 1), b.bytes);
		banks.push_back(b);
	}

	// Setup RAM banks
	int ramcount = 0;
	switch (header.RAMSize) {
	case RAM_NONE:
		ramcount = 0; break;
	case RAM_2KB: case RAM_8KB:
		ramcount = 1; break;
	case RAM_32KB:
		ramcount = 4; break;
	case RAM_64KB:
		ramcount = 8; break;
	case RAM_128KB:
		ramcount = 16; break;
	}
	ram.reserve(ramcount);
	for (int i = 0; i < ramcount; i++) {
		RAMBank bank;
		ram.push_back(bank);
	}
}
