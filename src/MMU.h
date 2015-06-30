#pragma once

#include "ROM.h"
#include "GPU.h"

struct WRAMBank {
	uint8_t bytes[4 * 1024];
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

	ZRAMBank ZRAM;

	uint8_t ReadIO(const uint16_t location);
	void WriteIO(const uint16_t location, const uint8_t value);

public:
	GPU* gpu;
	bool usingBootstrap; //! Is the bootstrap ROM enabled?

	MMU(ROM* romData, GPU* _gpu);

	uint8_t Read(const uint16_t location);
	void Write(const uint16_t location, const uint8_t value);
};