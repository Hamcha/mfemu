#pragma once

#include "ROM.h"

struct WRAMBank {
	uint8_t bytes[4 * 1024];
};

struct VRAMBank {
	uint8_t bytes[8 * 1024];
};

struct CycleCount {
	int machine, cpu;

	void add(int m, int c) {
		machine += m; cpu += c;
	}
};

class CPU {
private:
	ROM* rom;

	WRAMBank WRAM;
	std::vector<WRAMBank> WRAMbanks;
	uint8_t WRAMbankId;

	std::vector<VRAMBank> VRAM;
	uint8_t VRAMbankId;

public:
	// Registers
	union {
		uint16_t Pair;
		struct { uint8_t A, Flags; } Single;
	} AF;

	union {
		uint16_t Pair;
		struct { uint8_t B, C; } Single;
	} BC;

	union {
		uint16_t Pair;
		struct { uint8_t D, E; } Single;
	} DE;

	union {
		uint16_t Pair;
		struct { uint8_t H, L; } Single;
	} HL;

	uint16_t SP;    // Stack Pointer
	uint16_t PC;    // Program Counter
	union Status {  // Status Register
		uint8_t Byte;
		struct {
			unsigned int Zero : 1;
			unsigned int BCD_AddSub : 1;
			unsigned int BCD_HalfCarry : 1;
			unsigned int Carry : 1;
			unsigned int _undef : 4;
		};
	};

	CycleCount cycles;

	uint8_t Read(uint16_t location);
	void Execute(uint8_t opcode);

	//! Is running? (Not Halted/Paused)
	bool running;

	//! Execute single step (instruction)
	void Step();

	//! Create CPU from ROM file
	CPU(ROM* _rom);

	~CPU();
};
