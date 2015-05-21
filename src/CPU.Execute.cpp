#include "CPU.h"
#include <iostream>
#include <iomanip>

uint8_t CPU::Execute(uint8_t opcode) {
	switch (opcode) {
	// NOP
	case 0x00:
		return 0;

	// STOP
	case 0x10:
		running = false;
		return 0;

	// HALT
	case 0x76:
		running = false;
		//TODO Listen for Interrupt
		return 0;

	default:
		std::cout << "Unknown opcode: " << std::setfill('0') << std::setw(2) << std::hex << (int)opcode << std::endl;
	}
}
