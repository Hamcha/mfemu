#include "CPU.h"

uint8_t CPU::Execute(uint8_t opcode) {
	switch (opcode) {
	// NOP
	case 0x00:
		return 0;

	// STOP
	case 0x10:
		//TODO Halt CPU
		return 0;

	// HALT
	case 0x76:
		//TODO Halt until Interrupt
		return 0;
	}
}