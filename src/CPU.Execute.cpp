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
void Nop(CPU* cpu) {
	cpu->cycles.add(1, 4);
}

// Stop or halt the processor
CPUHandler Halt(bool waitInterrupt) {
	//Todo handle waitInterrupt (for HALT)
	return [waitInterrupt](CPU *cpu) {
		// STOP takes two machine cycles
		int mcycles = waitInterrupt ? 1 : 2;

		cpu->cycles.add(mcycles, 4);
		cpu->running = false;
	};
}

// Direct Load (Register to Register)
CPUHandler LoadDirect(RID src, RID dst) {
	return [src, dst](CPU* cpu) {
		uint8_t* srcRes = registry(cpu, src);
		uint8_t* dstRes = registry(cpu, dst);
		*dstRes = *srcRes;
		cpu->cycles.add(1, 4);
	};
}

// Unimplemented instruction
void Todo(CPU* cpu) {
	std::cout << "Unknown Opcode: " << std::setfill('0') << std::setw(2) << std::hex << (int)cpu->Read(cpu->PC) << std::endl;
}

const static CPUHandler handlers[] = {
	Nop,  // 00 NOP
	Todo, // 01
	Todo, // 02
	Todo, // 03
	Todo, // 04
	Todo, // 05
	Todo, // 06
	Todo, // 07
	Todo, // 08
	Todo, // 09
	Todo, // 0a
	Todo, // 0b
	Todo, // 0c
	Todo, // 0d
	Todo, // 0e
	Todo, // 0f
	Halt(false), // 10 STOP
	Todo, // 11
	Todo, // 12
	Todo, // 13
	Todo, // 14
	Todo, // 15
	Todo, // 16
	Todo, // 17
	Todo, // 18
	Todo, // 19
	Todo, // 1a
	Todo, // 1b
	Todo, // 1c
	Todo, // 1d
	Todo, // 1e
	Todo, // 1f
	Todo, // 20
	Todo, // 21
	Todo, // 22
	Todo, // 23
	Todo, // 24
	Todo, // 25
	Todo, // 26
	Todo, // 27
	Todo, // 28
	Todo, // 29
	Todo, // 2a
	Todo, // 2b
	Todo, // 2c
	Todo, // 2d
	Todo, // 2e
	Todo, // 2f
	Todo, // 30
	Todo, // 31
	Todo, // 32
	Todo, // 33
	Todo, // 34
	Todo, // 35
	Todo, // 36
	Todo, // 37
	Todo, // 38
	Todo, // 39
	Todo, // 3a
	Todo, // 3b
	Todo, // 3c
	Todo, // 3d
	Todo, // 3e
	Todo, // 3f
	LoadDirect(B, B), // 40 LD B,B
	LoadDirect(B, C), // 41 LD B,C
	LoadDirect(B, D), // 42 LD B,D
	LoadDirect(B, E), // 43 LD B,E
	LoadDirect(B, H), // 44 LD B,H
	LoadDirect(B, L), // 45 LD B,L
	Todo, // 46
	LoadDirect(B, A), // 47 LD B,A
	LoadDirect(C, B), // 48 LD C,B
	LoadDirect(C, C), // 49 LD C,C
	LoadDirect(C, D), // 4a LD C,D
	LoadDirect(C, E), // 4b LD C,E
	LoadDirect(C, H), // 4c LD C,H
	LoadDirect(C, L), // 4d LD C,L
	Todo, // 4e
	LoadDirect(C, A), // 4f LD C,A
	LoadDirect(D, B), // 50 LD D,B
	LoadDirect(D, C), // 51 LD D,C
	LoadDirect(D, D), // 52 LD D,D
	LoadDirect(D, E), // 53 LD D,E
	LoadDirect(D, H), // 54 LD D,H
	LoadDirect(D, L), // 55 LD D,L
	Todo, // 56
	LoadDirect(D, A), // 57 LD D,A
	LoadDirect(E, B), // 58 LD E,B
	LoadDirect(E, C), // 59 LD E,C
	LoadDirect(E, D), // 5a LD E,D
	LoadDirect(E, E), // 5b LD E,E
	LoadDirect(E, H), // 5c LD E,H
	LoadDirect(E, L), // 5d LD E,L
	Todo, // 5e
	LoadDirect(E, A), // 5f LD E,A
	LoadDirect(H, B), // 60 LD H,B
	LoadDirect(H, C), // 61 LD H,C
	LoadDirect(H, D), // 62 LD H,D
	LoadDirect(H, E), // 63 LD H,E
	LoadDirect(H, H), // 64 LD H,H
	LoadDirect(H, L), // 65 LD H,L
	Todo, // 66
	LoadDirect(H, A), // 67 LD H,A
	LoadDirect(L, B), // 68 LD L,B
	LoadDirect(L, C), // 69 LD L,C
	LoadDirect(L, D), // 6a LD L,D
	LoadDirect(L, E), // 6b LD L,E
	LoadDirect(L, H), // 6c LD L,H
	LoadDirect(L, L), // 6d LD L,L
	Todo, // 6e
	LoadDirect(L, A), // 6f LD L,A
	Todo, // 70
	Todo, // 71
	Todo, // 72
	Todo, // 73
	Todo, // 74
	Todo, // 75
	Halt(true), // 76 HALT
	Todo, // 77
	LoadDirect(A, B), // 78 LD A,B
	LoadDirect(A, C), // 79 LD A,C
	LoadDirect(A, D), // 7a LD A,D
	LoadDirect(A, E), // 7b LD A,E
	LoadDirect(A, H), // 7c LD A,H
	LoadDirect(A, L), // 7d LD A,L
	Todo, // 7e
	LoadDirect(A, A), // 7f LD A,A
	Todo, // 80
	Todo, // 81
	Todo, // 82
	Todo, // 83
	Todo, // 84
	Todo, // 85
	Todo, // 86
	Todo, // 87
	Todo, // 88
	Todo, // 89
	Todo, // 8a
	Todo, // 8b
	Todo, // 8c
	Todo, // 8d
	Todo, // 8e
	Todo, // 8f
	Todo, // 90
	Todo, // 91
	Todo, // 92
	Todo, // 93
	Todo, // 94
	Todo, // 95
	Todo, // 96
	Todo, // 97
	Todo, // 98
	Todo, // 99
	Todo, // 9a
	Todo, // 9b
	Todo, // 9c
	Todo, // 9d
	Todo, // 9e
	Todo, // 9f
	Todo, // a0
	Todo, // a1
	Todo, // a2
	Todo, // a3
	Todo, // a4
	Todo, // a5
	Todo, // a6
	Todo, // a7
	Todo, // a8
	Todo, // a9
	Todo, // aa
	Todo, // ab
	Todo, // ac
	Todo, // ad
	Todo, // ae
	Todo, // af
	Todo, // b0
	Todo, // b1
	Todo, // b2
	Todo, // b3
	Todo, // b4
	Todo, // b5
	Todo, // b6
	Todo, // b7
	Todo, // b8
	Todo, // b9
	Todo, // ba
	Todo, // bb
	Todo, // bc
	Todo, // bd
	Todo, // be
	Todo, // bf
	Todo, // c0
	Todo, // c1
	Todo, // c2
	Todo, // c3
	Todo, // c4
	Todo, // c5
	Todo, // c6
	Todo, // c7
	Todo, // c8
	Todo, // c9
	Todo, // ca
	Todo, // cb
	Todo, // cc
	Todo, // cd
	Todo, // ce
	Todo, // cf
	Todo, // d0
	Todo, // d1
	Todo, // d2
	Todo, // d3
	Todo, // d4
	Todo, // d5
	Todo, // d6
	Todo, // d7
	Todo, // d8
	Todo, // d9
	Todo, // da
	Todo, // db
	Todo, // dc
	Todo, // dd
	Todo, // de
	Todo, // df
	Todo, // e0
	Todo, // e1
	Todo, // e2
	Todo, // e3
	Todo, // e4
	Todo, // e5
	Todo, // e6
	Todo, // e7
	Todo, // e8
	Todo, // e9
	Todo, // ea
	Todo, // eb
	Todo, // ec
	Todo, // ed
	Todo, // ee
	Todo, // ef
	Todo, // f0
	Todo, // f1
	Todo, // f2
	Todo, // f3
	Todo, // f4
	Todo, // f5
	Todo, // f6
	Todo, // f7
	Todo, // f8
	Todo, // f9
	Todo, // fa
	Todo, // fb
	Todo, // fc
	Todo, // fd
	Todo, // fe
	Todo  // ff
};

void CPU::Execute(uint8_t opcode) {
	handlers[opcode](this);
}
