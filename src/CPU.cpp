#include "CPU.h"

CycleCount CPU::Step() {
	uint8_t opcode = mmu->Read(PC);
	CycleCount c = { 1,4 };
	if (running) {
		c = Execute(opcode);
		PC += 1;
	}
	
	cycles.add(c);
	return c;
}

CPU::CPU(MMU* _mmu)
	: cycles({ 0,0 }) {
	// Setup variables
	mmu = _mmu;
	running = true;
	paused = false;
	PC = 0;
}

void CPU::HandleInterrupts() {
	InterruptFlag interrupts = mmu->interruptFlags;
	
	// Check interrupts by priority
	if (interrupts.flags.vblank) {
		mmu->UnsetInterrupt(IntLCDVblank);
		handleInterrupt(0x40);
		return;
	}

	if (interrupts.flags.lcdcontrol) {
		mmu->UnsetInterrupt(IntLCDControl);
		handleInterrupt(0x48);
		return;
	}

	if (interrupts.flags.timer) {
		mmu->UnsetInterrupt(IntTimerOverflow);
		handleInterrupt(0x50);
		return;
	}

	if (interrupts.flags.serial) {
		mmu->UnsetInterrupt(IntEndSerialIO);
		handleInterrupt(0x58);
		return;
	}

	if (interrupts.flags.input) {
		mmu->UnsetInterrupt(IntInput);
		handleInterrupt(0x60);
		return;
	}
}

CPU::~CPU() {}
