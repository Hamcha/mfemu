#pragma once

#include "ROM.h"

struct WRAMBank {
	uint8_t bytes[4 * 1024];
};

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

struct ZRAMBank {
	uint8_t bytes[128];
};

class MMU {
private:
	ROM* rom;

	WRAMBank WRAM;
	std::vector<WRAMBank> WRAMbanks;
	uint8_t WRAMbankId = 0;

	std::vector<VRAMBank> VRAM;
	uint8_t VRAMbankId = 0;

	ZRAMBank ZRAM;

	bool usingBootstrap; //! Is the bootstrap ROM enabled?

public:
	MMU(ROM* romData);

	uint8_t Read(uint16_t location);
	void Write(uint16_t location, uint8_t value);
};