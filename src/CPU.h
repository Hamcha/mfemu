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

struct CycleCount {
	int machine, cpu;

	void add(int m, int c) {
		machine += m; cpu += c;
	}
};

struct FlagStruct {
	unsigned int Zero : 1;
	unsigned int BCD_AddSub : 1;
	unsigned int BCD_HalfCarry : 1;
	unsigned int Carry : 1;
	unsigned int _undef : 4;
};

class CPU {
private:
	ROM* rom;

	WRAMBank WRAM;
	std::vector<WRAMBank> WRAMbanks;
	uint8_t WRAMbankId = 0;

	std::vector<VRAMBank> VRAM;
	uint8_t VRAMbankId = 0;

	ZRAMBank ZRAM;

public:
	// Registers
	union {
		uint16_t Pair;
		struct {
			uint8_t A;
			union {
				uint8_t Byte;
				FlagStruct Values;
			} Flags;
		} Single;
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

	uint16_t SP;    //! Stack Pointer
	uint16_t PC;    //! Program Counter

	bool maskable;  //! Are maskable interrupts enabled?

	CycleCount cycles;
	FlagStruct& Flags() { return AF.Single.Flags.Values; }

	uint8_t Read(uint16_t location);
	void Write(uint16_t location, uint8_t value);
	void Execute(uint8_t opcode);

	//! Is running? (Not Halted/Paused)
	bool running;

	//! Execute single step (instruction)
	void Step();

	//! Create CPU from ROM file
	CPU(ROM* _rom);

	~CPU();
};
