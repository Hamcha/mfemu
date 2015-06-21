#pragma once

#include <cstdint>
#include "MMU.h"

struct CycleCount {
	int machine, cpu;

	CycleCount(int m, int c) {
		machine = m; cpu = c;
	}
	void add(int m, int c) {
		machine += m; cpu += c;
	}
	void add(CycleCount c) {
		add(c.machine, c.cpu);
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
	MMU* mmu;

public:
	// Registers
	union {
		uint16_t Pair;
		struct {
			union {
				uint8_t Byte;
				FlagStruct Values;
			} Flags;
			uint8_t A;
		} Single;
	} AF;

	union {
		uint16_t Pair;
		struct { uint8_t C, B; } Single;
	} BC;

	union {
		uint16_t Pair;
		struct { uint8_t E, D; } Single;
	} DE;

	union {
		uint16_t Pair;
		struct { uint8_t L, H; } Single;
	} HL;

	uint16_t SP;    //! Stack Pointer
	uint16_t PC;    //! Program Counter
	bool maskable;  //! Are maskable interrupts enabled?

	CycleCount cycles;
	FlagStruct& Flags() { return AF.Single.Flags.Values; }

	CycleCount Execute(uint8_t opcode);

	//! Is running? (Not Halted/Paused)
	bool running;

	//! Execute single step (instruction)
	CycleCount Step();

	//! Create CPU from ROM file
	explicit CPU(MMU* _mmu);

	~CPU();
};
