#pragma once

#include "ROM.h"

struct WRAMBank {
	uint8_t bytes[4 * 1024];
};

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

class CPU {
private:
	// Registers
	union AF {
		uint16_t Pair;
		struct Single { uint8_t A, Flags; };
	};
	union BC {
		uint16_t Pair;
		struct Single { uint8_t B, C; };
	};
	union DE {
		uint16_t Pair;
		struct Single { uint8_t D, E; };
	};
	union HL {
		uint16_t Pair;
		struct Single { uint8_t H, L; };
	};

	uint16_t SP;    // Stack Pointer
	uint16_t PC;    // Program Counter
	union Status {  // Status Register
		uint8_t Byte;
		struct Flags {
			unsigned int Zero : 1;
			unsigned int BCD_AddSub : 1;
			unsigned int BCD_HalfCarry : 1;
			unsigned int Carry : 1;
			unsigned int _undef : 4;
		};
	};

	WRAMBank WRAM;
	std::vector<WRAMBank> WRAMbanks;
	uint8_t WRAMbankId;

	std::vector<VRAMBank> VRAM;
	uint8_t VRAMbankId;

	ROM* rom;

	uint8_t Read(uint16_t location);
public:
	void Step();

	CPU(ROM* _rom);
	~CPU();
};