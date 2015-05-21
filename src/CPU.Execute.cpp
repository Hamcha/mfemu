#include "CPU.h"
#include <iostream>
#include <iomanip>
#include <functional>

typedef std::function<void(CPU* cpu)> CPUHandler;

enum RID {
	A, B, C, D, E, H, L
};

uint8_t* registry(CPU* cpu, RID id) {
	switch (id) {
	case A: return &(cpu->AF.Single.A);
	case B: return &(cpu->BC.Single.B);
	case C: return &(cpu->BC.Single.C);
	case D: return &(cpu->DE.Single.D);
	case E: return &(cpu->DE.Single.E);
	case H: return &(cpu->HL.Single.H);
	case L: return &(cpu->HL.Single.L);
	}
	return nullptr;
}

// Do nothing
void NOP(CPU* cpu) {
	cpu->cycles.add(1, 4);
}

// Stop or halt the processor
CPUHandler HALT(bool waitInterrupt) {
	//TODO handle waitInterrupt (for HALT)
	return [waitInterrupt](CPU *cpu) {
		// STOP takes two machine cycles
		int mcycles = waitInterrupt ? 1 : 2;
		cpu->cycles.add(mcycles, 4);
		cpu->running = false;
	};
}

// Direct Load (Register to Register)
CPUHandler LD_D(RID src, RID dst) {
	return [src, dst](CPU* cpu) {
		uint8_t* srcRes = registry(cpu, src);
		uint8_t* dstRes = registry(cpu, dst);
		*dstRes = *srcRes;
		cpu->cycles.add(1, 4);
	};
}

// Unimplemented instruction
void TODO(CPU* cpu) {
	std::cout << "Unknown Opcode: " << std::setfill('0') << std::setw(2) << std::hex << (int)cpu->Read(cpu->PC) << std::endl;
}

const static CPUHandler handlers[] = {
	NOP,  // 00 NOP
	TODO, // 01
	TODO, // 02
	TODO, // 03
	TODO, // 04
	TODO, // 05
	TODO, // 06
	TODO, // 07
	TODO, // 08
	TODO, // 09
	TODO, // 0a
	TODO, // 0b
	TODO, // 0c
	TODO, // 0d
	TODO, // 0e
	TODO, // 0f
	HALT(false), // 10 STOP
	TODO, // 11
	TODO, // 12
	TODO, // 13
	TODO, // 14
	TODO, // 15
	TODO, // 16
	TODO, // 17
	TODO, // 18
	TODO, // 19
	TODO, // 1a
	TODO, // 1b
	TODO, // 1c
	TODO, // 1d
	TODO, // 1e
	TODO, // 1f
	TODO, // 20
	TODO, // 21
	TODO, // 22
	TODO, // 23
	TODO, // 24
	TODO, // 25
	TODO, // 26
	TODO, // 27
	TODO, // 28
	TODO, // 29
	TODO, // 2a
	TODO, // 2b
	TODO, // 2c
	TODO, // 2d
	TODO, // 2e
	TODO, // 2f
	TODO, // 30
	TODO, // 31
	TODO, // 32
	TODO, // 33
	TODO, // 34
	TODO, // 35
	TODO, // 36
	TODO, // 37
	TODO, // 38
	TODO, // 39
	TODO, // 3a
	TODO, // 3b
	TODO, // 3c
	TODO, // 3d
	TODO, // 3e
	TODO, // 3f
	LD_D(B, B), // 40 LD B,B
	LD_D(B, C), // 41 LD B,C
	LD_D(B, D), // 42 LD B,D
	LD_D(B, E), // 43 LD B,E
	LD_D(B, H), // 44 LD B,H
	LD_D(B, L), // 45 LD B,L
	TODO, // 46
	LD_D(B, A), // 47 LD B,A
	LD_D(C, B), // 48
	LD_D(C, C), // 49
	LD_D(C, D), // 4a
	LD_D(C, E), // 4b
	LD_D(C, H), // 4c
	LD_D(C, L), // 4d
	TODO, // 4e
	LD_D(C, A), // 4f
	LD_D(D, B), // 50
	LD_D(D, C), // 51
	LD_D(D, D), // 52
	LD_D(D, E), // 53
	LD_D(D, H), // 54
	LD_D(D, L), // 55
	TODO, // 56
	LD_D(D, A), // 57
	LD_D(E, B), // 58
	LD_D(E, C), // 59
	LD_D(E, D), // 5a
	LD_D(E, E), // 5b
	LD_D(E, H), // 5c
	LD_D(E, L), // 5d
	TODO, // 5e
	LD_D(E, A), // 5f
	LD_D(H, B), // 60
	LD_D(H, C), // 61
	LD_D(H, D), // 62
	LD_D(H, E), // 63
	LD_D(H, H), // 64
	LD_D(H, L), // 65
	TODO, // 66
	LD_D(H, A), // 67
	LD_D(L, B), // 68
	LD_D(L, C), // 69
	LD_D(L, D), // 6a
	LD_D(L, E), // 6b
	LD_D(L, H), // 6c
	LD_D(L, L), // 6d
	TODO, // 6e
	LD_D(L, A), // 6f
	TODO, // 70
	TODO, // 71
	TODO, // 72
	TODO, // 73
	TODO, // 74
	TODO, // 75
	HALT(true), // 76 HALT
	TODO, // 77
	LD_D(A, B), // 78
	LD_D(A, C), // 79
	LD_D(A, D), // 7a
	LD_D(A, E), // 7b
	LD_D(A, H), // 7c
	LD_D(A, L), // 7d
	TODO, // 7e
	LD_D(A, A), // 7f
	TODO, // 80
	TODO, // 81
	TODO, // 82
	TODO, // 83
	TODO, // 84
	TODO, // 85
	TODO, // 86
	TODO, // 87
	TODO, // 88
	TODO, // 89
	TODO, // 8a
	TODO, // 8b
	TODO, // 8c
	TODO, // 8d
	TODO, // 8e
	TODO, // 8f
	TODO, // 90
	TODO, // 91
	TODO, // 92
	TODO, // 93
	TODO, // 94
	TODO, // 95
	TODO, // 96
	TODO, // 97
	TODO, // 98
	TODO, // 99
	TODO, // 9a
	TODO, // 9b
	TODO, // 9c
	TODO, // 9d
	TODO, // 9e
	TODO, // 9f
	TODO, // a0
	TODO, // a1
	TODO, // a2
	TODO, // a3
	TODO, // a4
	TODO, // a5
	TODO, // a6
	TODO, // a7
	TODO, // a8
	TODO, // a9
	TODO, // aa
	TODO, // ab
	TODO, // ac
	TODO, // ad
	TODO, // ae
	TODO, // af
	TODO, // b0
	TODO, // b1
	TODO, // b2
	TODO, // b3
	TODO, // b4
	TODO, // b5
	TODO, // b6
	TODO, // b7
	TODO, // b8
	TODO, // b9
	TODO, // ba
	TODO, // bb
	TODO, // bc
	TODO, // bd
	TODO, // be
	TODO, // bf
	TODO, // c0
	TODO, // c1
	TODO, // c2
	TODO, // c3
	TODO, // c4
	TODO, // c5
	TODO, // c6
	TODO, // c7
	TODO, // c8
	TODO, // c9
	TODO, // ca
	TODO, // cb
	TODO, // cc
	TODO, // cd
	TODO, // ce
	TODO, // cf
	TODO, // d0
	TODO, // d1
	TODO, // d2
	TODO, // d3
	TODO, // d4
	TODO, // d5
	TODO, // d6
	TODO, // d7
	TODO, // d8
	TODO, // d9
	TODO, // da
	TODO, // db
	TODO, // dc
	TODO, // dd
	TODO, // de
	TODO, // df
	TODO, // e0
	TODO, // e1
	TODO, // e2
	TODO, // e3
	TODO, // e4
	TODO, // e5
	TODO, // e6
	TODO, // e7
	TODO, // e8
	TODO, // e9
	TODO, // ea
	TODO, // eb
	TODO, // ec
	TODO, // ed
	TODO, // ee
	TODO, // ef
	TODO, // f0
	TODO, // f1
	TODO, // f2
	TODO, // f3
	TODO, // f4
	TODO, // f5
	TODO, // f6
	TODO, // f7
	TODO, // f8
	TODO, // f9
	TODO, // fa
	TODO, // fb
	TODO, // fc
	TODO, // fd
	TODO, // fe
	TODO  // ff
};

void CPU::Execute(uint8_t opcode) {
	handlers[opcode](this);
}
