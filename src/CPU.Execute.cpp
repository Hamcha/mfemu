#include "CPU.h"
#include <iostream>
#include <iomanip>
#include <functional>

typedef std::function<void(CPU* cpu)> CPUHandler;

enum RID {
	A, B, C, D, E, H, L
};
enum PID {
	AF, BC, DE, HL, SP, PC
};

uint8_t* getRegister(CPU* cpu, RID id) {
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

uint16_t* getPair(CPU* cpu, PID id) {
	switch (id) {
	case AF: return &(cpu->AF.Pair);
	case BC: return &(cpu->BC.Pair);
	case DE: return &(cpu->DE.Pair);
	case HL: return &(cpu->HL.Pair);
	case SP: return &(cpu->SP);
	case PC: return &(cpu->PC);
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

// Direct Load (8bit Register to 8bit Register)
CPUHandler LoadDirect(RID dst, RID src) {
	return [src, dst](CPU* cpu) {
		uint8_t* srcRes = getRegister(cpu, src);
		uint8_t* dstRes = getRegister(cpu, dst);
		*dstRes = *srcRes;
		cpu->cycles.add(1, 4);
	};
}

// Direct Load (16bit Register to 16bit Register)
CPUHandler LoadDirect(PID dst, PID src) {
	return [src, dst](CPU* cpu) {
		uint16_t* srcRes = getPair(cpu, src);
		uint16_t* dstRes = getPair(cpu, dst);
		*dstRes = *srcRes;
		cpu->cycles.add(1, 8);
	};
}

// Indirect Load (Register offset to Register)
CPUHandler LoadIndirect(RID dst, PID ind) {
	return [dst, ind](CPU* cpu) {
		uint8_t* res = getRegister(cpu, dst);
		uint16_t* addr = getPair(cpu, ind);
		uint8_t value = cpu->Read(*addr);
		*res = value;
		cpu->cycles.add(1, 8);
	};
}

// Indirect Load (Register to Register offset)
CPUHandler LoadIndirect(PID dst, RID src) {
	return [dst, src](CPU* cpu) {
		uint8_t* value = getRegister(cpu, src);
		uint16_t* addr = getPair(cpu, dst);
		cpu->Write(*addr, *value);
		cpu->cycles.add(1, 8);
	};
}

// Immediate Load (8bit constant to Register)
CPUHandler LoadImmediate(RID dst) {
	return [dst](CPU* cpu) {
		uint8_t* dstRes = getRegister(cpu, dst);
		// Get next byte
		uint8_t value = cpu->Read(++cpu->PC);

		// Assign to register
		*dstRes = value;
		cpu->cycles.add(2, 8);
	};
}

// Immediate Load (16bit constant to register pair)
CPUHandler LoadImmediate(PID dst) {
	return [dst](CPU* cpu) {
		uint16_t* dstRes = getPair(cpu, dst);
		// Get next bytes
		uint8_t  low  = cpu->Read(++cpu->PC);
		uint8_t  high = cpu->Read(++cpu->PC);
		uint16_t word = (high << 8) + low;

		*dstRes = word;
		cpu->cycles.add(3, 12);
	};
}

// Increment register (8bit, immediate)
CPUHandler Increment(RID dst) {
	return [dst](CPU* cpu) {
		uint8_t* dstRes = getRegister(cpu, dst);
		dstRes++;
		cpu->Flags().Zero = *dstRes == 0;
		cpu->Flags().BCD_AddSub = 0;
		cpu->Flags().BCD_HalfCarry = (*dstRes & 0x0f) > 9;
		cpu->cycles.add(1,4);
	};
}

// Increment register (16bit, immediate)
CPUHandler Increment(PID dst) {
	return [dst](CPU* cpu) {
		uint16_t* dstRes = getPair(cpu, dst);
		dstRes++;
		cpu->cycles.add(1,8);
	};
}

// Increment register (8bit, immediate)
CPUHandler Decrement(RID dst) {
	return [dst](CPU* cpu) {
		uint8_t* dstRes = getRegister(cpu, dst);
		dstRes--;
		cpu->Flags().Zero = *dstRes == 0;
		cpu->Flags().BCD_AddSub = 1;
		cpu->Flags().BCD_HalfCarry = (*dstRes & 0x0f) > 9;
		cpu->cycles.add(1,4);
	};
}

// Increment register (16bit, immediate)
CPUHandler Decrement(PID dst) {
	return [dst](CPU* cpu) {
		uint16_t* dstRes = getPair(cpu, dst);
		dstRes--;
		cpu->cycles.add(1,8);
	};
}

// Add function (called by AddDirect etc)
void Add(CPU* cpu, uint8_t* a, uint8_t* b, bool useCarry) {
	uint8_t orig = *a;
	*a += *b;
	if (useCarry && cpu->Flags().Carry) {
		*a++;
	}
	cpu->Flags().Carry = *a < orig;
	cpu->Flags().Zero = *a == 0;
	cpu->Flags().BCD_AddSub = 0;
	cpu->Flags().BCD_HalfCarry = (*a & 0x0f) > 9;
}

void Add(CPU* cpu, uint16_t* a, uint16_t* b, bool setZero) {
	uint16_t orig = *a;
	*a += *b;
	cpu->Flags().Carry = *a < orig;
	if (setZero) cpu->Flags().Zero = 0;
	cpu->Flags().BCD_AddSub = 0;
	cpu->Flags().BCD_HalfCarry = (*a & 0x000f) > 9;
}

// Direct Add (8bit, register to register)
CPUHandler AddDirect(RID a, RID b, bool useCarry) {
	return [a,b,useCarry](CPU* cpu){
		uint8_t* aRes = getRegister(cpu, a);
		uint8_t* bRes = getRegister(cpu, b);
		Add(cpu, aRes, bRes, useCarry);
		cpu->cycles.add(1, 4);
	};
}

// Direct Add (16bit, register to register)
CPUHandler AddDirect(PID a, PID b) {
	return[a, b](CPU* cpu) {
		uint16_t* aRes = getPair(cpu, a);
		uint16_t* bRes = getPair(cpu, b);
		Add(cpu, aRes, bRes, false);
		cpu->cycles.add(1, 8);
	};
}

// Indirect Add (register offset to register)
CPUHandler AddIndirect(RID a, PID ind, bool useCarry) {
	return[a, ind, useCarry](CPU* cpu) {
		uint8_t*  aRes = getRegister(cpu, a);
		uint16_t* addr = getPair(cpu, ind);
		uint8_t   bRes = cpu->Read(*addr);
		Add(cpu, aRes, &bRes, useCarry);
		cpu->cycles.add(1, 8);
	};
}

// Add Immediate (8bit constant value to 8bit register)
CPUHandler AddImmediate(RID a, bool useCarry) {
	return[a, useCarry](CPU* cpu) {
		uint8_t* aRes = getRegister(cpu, a);
		uint8_t  bRes = cpu->Read(++cpu->PC);
		Add(cpu, aRes, &bRes, useCarry);
		cpu->cycles.add(2, 8);
	};
}

// Add Immediate (8bit constant value to 16bit register)
CPUHandler AddImmediate(PID a) {
	return[a](CPU* cpu) {
		uint16_t* aRes = getPair(cpu, a);
		uint16_t  bRes = 0 + cpu->Read(++cpu->PC);
		Add(cpu, aRes, &bRes, false);
		cpu->cycles.add(2, 16);
	};
}

// Subtract function (called by SubDirect etc)
void Subtract(CPU* cpu, uint8_t* a, uint8_t* b, bool useCarry) {
	uint8_t orig = *a;
	*a -= *b;
	if (useCarry && cpu->Flags().Carry) {
		*a--;
	}
	cpu->Flags().Carry = *a > orig;
	cpu->Flags().Zero = *a == 0;
	cpu->Flags().BCD_AddSub = 1;
	cpu->Flags().BCD_HalfCarry = (*a & 0x0f) > 9;
}

// Direct Subtract (8bit, register to register)
CPUHandler SubDirect(RID a, RID b, bool useCarry) {
	return [a, b, useCarry](CPU* cpu) {
		uint8_t* aRes = getRegister(cpu, a);
		uint8_t* bRes = getRegister(cpu, b);
		Subtract(cpu, aRes, bRes, useCarry);
		cpu->cycles.add(1, 4);
	};
}

CPUHandler SubIndirect(RID a, PID ind, bool useCarry) {
	return[a, ind, useCarry](CPU* cpu) {
		uint8_t*  aRes = getRegister(cpu, a);
		uint16_t* addr = getPair(cpu, ind);
		uint8_t   bRes = cpu->Read(*addr);
		Subtract(cpu, aRes, &bRes, useCarry);
		cpu->cycles.add(1, 8);
	};
}

// Subtract Immediate (8bit constant value to 8bit register)
CPUHandler SubImmediate(RID a, bool useCarry) {
	return[a](CPU* cpu) {
		uint8_t* aRes = getRegister(cpu, a);
		uint8_t  bRes = cpu->Read(++cpu->PC);
		Subtract(cpu, aRes, &bRes, false);
		cpu->cycles.add(2, 8);
	};
}

// Unimplemented instruction
void Todo(CPU* cpu) {
	std::cout << "Unknown Opcode: " << std::setfill('0') << std::setw(2) << std::hex << (int)cpu->Read(cpu->PC) << std::endl;
}

const static CPUHandler handlers[] = {
	Nop,                 // 00 NOP
	LoadImmediate(BC),   // 01 LD BC,d16
	LoadIndirect(BC, A), // 02 LD (BC),A
	Increment(BC),       // 03 INC BC
	Increment(B),        // 04 INC B
	Decrement(B),        // 05 DEC B
	LoadImmediate(B),    // 06 LD B,d8
	Todo, // 07
	Todo, // 08
	AddDirect(HL, BC),   // 09 ADD HL,BC
	Todo, // 0a
	Decrement(BC),       // 0b DEC BC
	Increment(C),        // 0c INC C
	Decrement(C),        // 0d DEC C
	LoadImmediate(C),    // 0e LD C,d8
	Todo,                // 0f
	Halt(false),         // 10 STOP
	LoadImmediate(DE),   // 11 LD DE,d16
	LoadIndirect(DE, A), // 12 LD (DE),A
	Increment(DE),       // 13 INC DE
	Increment(D),        // 14 INC D
	Decrement(D),        // 15 DEC D
	LoadImmediate(D),    // 16 LD D,d8
	Todo, // 17
	Todo, // 18
	AddDirect(HL,DE),    // 19 ADD HL,DE
	Todo, // 1a
	Decrement(DE),       // 1b DEC DE
	Increment(E),        // 1c DEC E
	Decrement(E),        // 1d DEC E
	LoadImmediate(E),    // 1e LD E,d8
	Todo, // 1f
	Todo, // 20
	LoadImmediate(HL),   // 21 LD HL,d16
	Todo, // 22
	Increment(HL),       // 23 INC HL
	Increment(H),        // 24 INC H
	Decrement(H),        // 25 DEC H
	LoadImmediate(H),    // 26 LD H,d8
	Todo, // 27
	Todo, // 28
	AddDirect(HL,HL),    // 29 ADD HL,HL
	Todo, // 2a
	Decrement(HL),       // 2b DEC HL
	Increment(L),        // 2c INC L
	Decrement(L),        // 2d DEC L
	LoadImmediate(L),    // 2e LD L,d8
	Todo, // 2f
	Todo, // 30
	LoadImmediate(SP),   // 31 LD SP,d16
	Todo, // 32
	Increment(SP),       // 33 INC SP
	Todo, // 34
	Todo, // 35
	Todo, // 36
	Todo, // 37
	Todo, // 38
	AddDirect(HL,SP),    // 39 ADD HL,SP
	Todo, // 3a
	Decrement(SP),       // 3b DEC SP
	Increment(A),        // 3c INC A
	Decrement(A),        // 3d DEC A
	LoadImmediate(A),    // 3e LD A,d8
	Todo, // 3f
	LoadDirect(B, B),    // 40 LD B,B
	LoadDirect(B, C),    // 41 LD B,C
	LoadDirect(B, D),    // 42 LD B,D
	LoadDirect(B, E),    // 43 LD B,E
	LoadDirect(B, H),    // 44 LD B,H
	LoadDirect(B, L),    // 45 LD B,L
	LoadIndirect(B, HL), // 46 LD B,(HL)
	LoadDirect(B, A),    // 47 LD B,A
	LoadDirect(C, B),    // 48 LD C,B
	LoadDirect(C, C),    // 49 LD C,C
	LoadDirect(C, D),    // 4a LD C,D
	LoadDirect(C, E),    // 4b LD C,E
	LoadDirect(C, H),    // 4c LD C,H
	LoadDirect(C, L),    // 4d LD C,L
	LoadIndirect(C, HL), // 4e LD C,(HL)
	LoadDirect(C, A),    // 4f LD C,A
	LoadDirect(D, B),    // 50 LD D,B
	LoadDirect(D, C),    // 51 LD D,C
	LoadDirect(D, D),    // 52 LD D,D
	LoadDirect(D, E),    // 53 LD D,E
	LoadDirect(D, H),    // 54 LD D,H
	LoadDirect(D, L),    // 55 LD D,L
	LoadIndirect(D, HL), // 56 LD D,(HL)
	LoadDirect(D, A),    // 57 LD D,A
	LoadDirect(E, B),    // 58 LD E,B
	LoadDirect(E, C),    // 59 LD E,C
	LoadDirect(E, D),    // 5a LD E,D
	LoadDirect(E, E),    // 5b LD E,E
	LoadDirect(E, H),    // 5c LD E,H
	LoadDirect(E, L),    // 5d LD E,L
	LoadIndirect(E, HL), // 5e LD E,(HL)
	LoadDirect(E, A),    // 5f LD E,A
	LoadDirect(H, B),    // 60 LD H,B
	LoadDirect(H, C),    // 61 LD H,C
	LoadDirect(H, D),    // 62 LD H,D
	LoadDirect(H, E),    // 63 LD H,E
	LoadDirect(H, H),    // 64 LD H,H
	LoadDirect(H, L),    // 65 LD H,L
	LoadIndirect(H, HL), // 66 LD H,(HL)
	LoadDirect(H, A),    // 67 LD H,A
	LoadDirect(L, B),    // 68 LD L,B
	LoadDirect(L, C),    // 69 LD L,C
	LoadDirect(L, D),    // 6a LD L,D
	LoadDirect(L, E),    // 6b LD L,E
	LoadDirect(L, H),    // 6c LD L,H
	LoadDirect(L, L),    // 6d LD L,L
	LoadIndirect(L, HL), // 6e LD L,(HL)
	LoadDirect(L, A),    // 6f LD L,A
	LoadIndirect(HL, B), // 70 LD (HL),B
	LoadIndirect(HL, C), // 71 LD (HL),C
	LoadIndirect(HL, D), // 72 LD (HL),D
	LoadIndirect(HL, E), // 73 LD (HL),E
	LoadIndirect(HL, H), // 74 LD (HL),H
	LoadIndirect(HL, L), // 75 LD (HL),L
	Halt(true),          // 76 HALT
	LoadIndirect(HL, A), // 77 LD (HL),A
	LoadDirect(A, B),    // 78 LD A,B
	LoadDirect(A, C),    // 79 LD A,C
	LoadDirect(A, D),    // 7a LD A,D
	LoadDirect(A, E),    // 7b LD A,E
	LoadDirect(A, H),    // 7c LD A,H
	LoadDirect(A, L),    // 7d LD A,L
	LoadIndirect(A, HL), // 7e LD A,(HL)
	LoadDirect(A, A),    // 7f LD A,A
	AddDirect(A, B, false),    // 80 ADD A,B
	AddDirect(A, C, false),    // 81 ADD A,C
	AddDirect(A, D, false),    // 82 ADD A,D
	AddDirect(A, E, false),    // 83 ADD A,E
	AddDirect(A, H, false),    // 84 ADD A,H
	AddDirect(A, L, false),    // 85 ADD A,L
	AddIndirect(A, HL, false), // 86 ADD A,(HL)
	AddDirect(A, A, false),    // 87 ADD A,A
	AddDirect(A, B, true),     // 88 ADC A,B
	AddDirect(A, C, true),     // 89 ADC A,C
	AddDirect(A, D, true),     // 8a ADC A,D
	AddDirect(A, E, true),     // 8b ADC A,E
	AddDirect(A, H, true),     // 8c ADC A,H
	AddDirect(A, L, true),     // 8d ADC A,L
	AddIndirect(A, HL, false), // 8e ADC A,(HL)
	AddDirect(A, A, true),     // 8f ADC A,A
	SubDirect(A, B, false),    // 90 SUB A,B
	SubDirect(A, C, false),    // 91 SUB A,C
	SubDirect(A, D, false),    // 92 SUB A,D
	SubDirect(A, E, false),    // 93 SUB A,E
	SubDirect(A, H, false),    // 94 SUB A,H
	SubDirect(A, L, false),    // 95 SUB A,L
	SubIndirect(A, HL, false), // 96 SUB A,(HL)
	SubDirect(A, A, false),    // 97 SUB A,A
	SubDirect(A, B, true),     // 98 SBC A,B
	SubDirect(A, C, true),     // 99 SBC A,C
	SubDirect(A, D, true),     // 9a SBC A,D
	SubDirect(A, E, true),     // 9b SBC A,E
	SubDirect(A, H, true),     // 9c SBC A,H
	SubDirect(A, L, true),     // 9d SBC A,L
	SubIndirect(A, HL, true),  // 9e SBC A,(HL)
	SubDirect(A, A, true),     // 9f SBC A,A
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
	AddImmediate(A, false), // c6 ADD A,d8
	Todo, // c7
	Todo, // c8
	Todo, // c9
	Todo, // ca
	Todo, // cb
	Todo, // cc
	Todo, // cd
	AddImmediate(A, true),  // ce ADC A,d8
	Todo, // cf
	Todo, // d0
	Todo, // d1
	Todo, // d2
	Todo, // d3
	Todo, // d4
	Todo, // d5
	SubImmediate(A, false), // d6 SUB A,d8
	Todo, // d7
	Todo, // d8
	Todo, // d9
	Todo, // da
	Todo, // db
	Todo, // dc
	Todo, // dd
	SubImmediate(A, true),  // de SBC A,d8
	Todo, // df
	Todo, // e0
	Todo, // e1
	Todo, // e2
	Todo, // e3
	Todo, // e4
	Todo, // e5
	Todo, // e6
	Todo, // e7
	AddImmediate(SP),       // e8 ADD SP,r8
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
	LoadDirect(SP, HL), // f9 LD SP,HL
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
