#include "CPU.h"

CycleCount CPU::Step() {
	uint8_t opcode = mmu->Read(PC);
	CycleCount c = Execute(opcode);
	cycles.add(c);
	PC += 1;
	return c;
}

CPU::CPU(MMU* _mmu)
	: cycles({0,0}) {
	// Setup variables
	mmu = _mmu;
	running = true;
	PC = 0;
	maskable = true;
}

CPU::~CPU() {}
