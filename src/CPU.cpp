#include "CPU.h"

CycleCount CPU::Step() {
	uint8_t opcode = mmu->Read(PC);
	CycleCount c = { 1,4 };
	if (running) {
		PC += 1;
		c = Execute(opcode);
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
		running = true;
		return;
	}

	if (interrupts.flags.lcdcontrol) {
		mmu->UnsetInterrupt(IntLCDControl);
		handleInterrupt(0x48);
		running = true;
		return;
	}

	if (interrupts.flags.timer) {
		mmu->UnsetInterrupt(IntTimerOverflow);
		handleInterrupt(0x50);
		running = true;
		return;
	}

	if (interrupts.flags.serial) {
		mmu->UnsetInterrupt(IntEndSerialIO);
		handleInterrupt(0x58);
		running = true;
		return;
	}

	if (interrupts.flags.input) {
		mmu->UnsetInterrupt(IntInput);
		handleInterrupt(0x60);
		running = true;
		return;
	}
}

CPU::~CPU() {}
