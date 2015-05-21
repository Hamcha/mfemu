#include "CPU.h"
#include <iostream>
#include <iomanip>
#include <functional>

typedef std::function<void(CPU *cpu)> CPUHandler;

void NOP(CPU* cpu) {
	cpu->cycles.add(1, 4);
}

CPUHandler HALT(bool waitInterrupt) {
	//TODO handle waitInterrupt (for HALT)
	return [waitInterrupt](CPU *cpu) {
		// STOP takes two machine cycles
		int mcycles = waitInterrupt ? 1 : 2;
		cpu->cycles.add(mcycles, 4);
		cpu->running = false;
	};
}

void TODO(CPU *cpu) {
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
	TODO, // 40
	TODO, // 41
	TODO, // 42
	TODO, // 43
	TODO, // 44
	TODO, // 45
	TODO, // 46
	TODO, // 47
	TODO, // 48
	TODO, // 49
	TODO, // 4a
	TODO, // 4b
	TODO, // 4c
	TODO, // 4d
	TODO, // 4e
	TODO, // 4f
	TODO, // 50
	TODO, // 51
	TODO, // 52
	TODO, // 53
	TODO, // 54
	TODO, // 55
	TODO, // 56
	TODO, // 57
	TODO, // 58
	TODO, // 59
	TODO, // 5a
	TODO, // 5b
	TODO, // 5c
	TODO, // 5d
	TODO, // 5e
	TODO, // 5f
	TODO, // 60
	TODO, // 61
	TODO, // 62
	TODO, // 63
	TODO, // 64
	TODO, // 65
	TODO, // 66
	TODO, // 67
	TODO, // 68
	TODO, // 69
	TODO, // 6a
	TODO, // 6b
	TODO, // 6c
	TODO, // 6d
	TODO, // 6e
	TODO, // 6f
	TODO, // 70
	TODO, // 71
	TODO, // 72
	TODO, // 73
	TODO, // 74
	TODO, // 75
	HALT(true), // 76 HALT
	TODO, // 77
	TODO, // 78
	TODO, // 79
	TODO, // 7a
	TODO, // 7b
	TODO, // 7c
	TODO, // 7d
	TODO, // 7e
	TODO, // 7f
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
