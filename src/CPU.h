#pragma once

#include <cstdint>
#include <SDL.h>
#include "MMU.h"

struct CycleCount {
	int machine, cpu;

	CycleCount(const int m, const int c) {
		machine = m; cpu = c;
	}
	void add(const int m, const int c) {
		machine += m; cpu += c;
	}
	void add(const CycleCount c) {
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
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			uint8_t A;
			union {
				uint8_t Byte;
				FlagStruct Values;
			} Flags;
#else
			union {
					uint8_t Byte;
					FlagStruct Values;
			} Flags;
			uint8_t A;
#endif
		} Single;
	} AF;

	union {
		uint16_t Pair;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		struct { uint8_t B, C; } Single;
#else
		struct { uint8_t C, B; } Single;
#endif
	} BC;

	union {
		uint16_t Pair;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		struct { uint8_t D, E; } Single;
#else
		struct { uint8_t E, D; } Single;
#endif
	} DE;

	union {
		uint16_t Pair;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		struct { uint8_t H, L; } Single;
#else
		struct { uint8_t L, H; } Single;
#endif
	} HL;

	uint16_t SP;    //! Stack Pointer
	uint16_t PC;    //! Program Counter
	bool maskable;  //! Are maskable interrupts enabled?

	CycleCount cycles;
	FlagStruct& Flags() { return AF.Single.Flags.Values; }

	CycleCount Execute(const uint8_t opcode);

	//! Is running? (Not Halted/Paused)
	bool running;

	//! Execute single step (instruction)
	CycleCount Step();

	//! Create CPU from ROM file
	explicit CPU(MMU* _mmu);

	~CPU();
};
