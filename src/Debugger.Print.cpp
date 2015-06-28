#include "Debugger.h"
#include "CPU.Defines.h"
#include <iostream>
#include <iomanip>
#include <string>

enum DebugRegisterType {
	Direct, Indirect
};

enum DebugFlags {
	Comma, IndStart, IndFinish
};

enum DebugIntType {
	Hex8, Hex16, HexOffset8, HexOffset16, Offset8, Offset16, Absolute
};

std::string getRegisterName(const RID id) {
	switch (id) {
	case A: return "A";
	case B: return "B";
	case C: return "C";
	case D: return "D";
	case E: return "E";
	case H: return "H";
	case L: return "L";
	}
	return "<REG>";
}

std::string getPairName(const PID id) {
	switch (id) {
	case AF: return "AF";
	case BC: return "BC";
	case DE: return "DE";
	case HL: return "HL";
	case SP: return "SP";
	case PC: return "PC";
	}
	return "<PAIR>";
}

std::string getJumpConditionName(const JumpCondition condition) {
	switch (condition) {
	case CA: return " C";
	case NC: return " NC";
	case ZE: return " Z";
	case NZ: return " NZ";
	case NO: default: return "";
	}
}

void debugPrintArgument(CPU*, MMU*, const uint16_t) {
	std::cout << "\r\n";
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const DebugRegisterType type, const RID registerId, const Args... args) {
	std::string registerName = getRegisterName(registerId);
	if (type == Indirect) {
		registerName = "(" + registerName + ")";
	}
	std::cout << " " << registerName;
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const DebugRegisterType type, const PID pairId, const Args... args) {
	std::string pairName = getPairName(pairId);
	if (type == Indirect) {
		pairName = "(" + pairName + ")";
	}
	std::cout << " " << pairName;
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const DebugFlags flag, const Args... args) {
	switch (flag) {
	case Comma: std::cout << ","; break;
	case IndStart: std::cout << " ("; break;
	case IndFinish: std::cout << " )"; break;
	}
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const JumpCondition condition, const Args... args) {
	std::cout << getJumpConditionName(condition);
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const DebugIntType type, const int data, const Args... args) {
	uint8_t  low = mmu->Read(addr + (uint16_t) data);
	uint8_t  high = mmu->Read(addr + (uint16_t) data + 1);
	uint16_t word = (high << 8) | low;

	std::ios::fmtflags fmt(std::cout.flags());
	switch (type) {
	case Absolute:
		std::cout << " " << std::dec << data;
		break;
	case Offset8:
		std::cout << " " << std::dec << (int) mmu->Read(addr + (uint16_t) data);
		break;
	case Offset16:
		std::cout << " " << std::dec << (int) word;
		break;
	case Hex8:
		std::cout << " $" << std::setfill('0') << std::setw(2) << std::hex << (int) data;
		break;
	case Hex16:
		std::cout << " $" << std::setfill('0') << std::setw(2) << std::hex << (int) data;
		break;
	case HexOffset8:
		std::cout << " $" << std::setfill('0') << std::setw(2) << std::hex << (int) mmu->Read(addr + (uint16_t) data);
		break;
	case HexOffset16:
		std::cout << " $" << std::setfill('0') << std::setw(4) << std::hex << (int) word;
		break;

	}
	std::cout.flags(fmt);
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
void debugPrintArgument(CPU* cpu, MMU* mmu, const uint16_t addr, const std::string& absolute, const Args... args) {
	std::cout << " " << absolute;
	debugPrintArgument(cpu, mmu, addr, args...);
}

template<typename... Args>
Debug::InstructionPrinter debugPrintInstruction(const Args... args) {
	return[args...](CPU* cpu, MMU* mmu, const uint16_t addr) {
		std::ios::fmtflags fmt(std::cout.flags());
		std::cout << std::setfill('0') << std::setw(4) << std::hex << (int) addr << " |";
		std::cout.flags(fmt);
		debugPrintArgument(cpu, mmu, addr, args...);
	};
}


const static Debug::InstructionPrinter cbhandlers[] = {
	debugPrintInstruction("RLC", Direct, B),   // 00 RLC B
	debugPrintInstruction("RLC", Direct, C),   // 01 RLC C
	debugPrintInstruction("RLC", Direct, D),   // 02 RLC D
	debugPrintInstruction("RLC", Direct, E),   // 03 RLC E
	debugPrintInstruction("RLC", Direct, H),   // 04 RLC H
	debugPrintInstruction("RLC", Direct, L),   // 05 RLC L
	debugPrintInstruction("RLC", Indirect, HL),// 06 RLC (HL)
	debugPrintInstruction("RLC", Direct, A),   // 07 RLC A
	debugPrintInstruction("RRC", Direct, B),   // 08 RRC B
	debugPrintInstruction("RRC", Direct, C),   // 09 RRC C
	debugPrintInstruction("RRC", Direct, D),   // 0a RRC D
	debugPrintInstruction("RRC", Direct, E),   // 0b RRC E
	debugPrintInstruction("RRC", Direct, H),   // 0c RRC H
	debugPrintInstruction("RRC", Direct, L),   // 0d RRC L
	debugPrintInstruction("RRC", Indirect, HL),// 0e RRC (HL)
	debugPrintInstruction("RRC", Direct, A),   // 0f RRC A
	debugPrintInstruction("RL ", Direct, B),   // 10 RL  B
	debugPrintInstruction("RL ", Direct, C),   // 11 RL  C
	debugPrintInstruction("RL ", Direct, D),   // 12 RL  D
	debugPrintInstruction("RL ", Direct, E),   // 13 RL  E
	debugPrintInstruction("RL ", Direct, H),   // 14 RL  H
	debugPrintInstruction("RL ", Direct, L),   // 15 RL  L
	debugPrintInstruction("RL ", Indirect, HL),// 16 RL  (HL)
	debugPrintInstruction("RL ", Direct, A),   // 17 RL  A
	debugPrintInstruction("RR ", Direct, B),   // 18 RR  B
	debugPrintInstruction("RR ", Direct, C),   // 19 RR  C
	debugPrintInstruction("RR ", Direct, D),   // 1a RR  D
	debugPrintInstruction("RR ", Direct, E),   // 1b RR  E
	debugPrintInstruction("RR ", Direct, H),   // 1c RR  H
	debugPrintInstruction("RR ", Direct, L),   // 1d RR  L
	debugPrintInstruction("RR ", Indirect, HL),// 1e RR  (HL)
	debugPrintInstruction("RR ", Direct, A),   // 1f RR  A
	debugPrintInstruction("SLA", Direct, B),   // 20 SLA B
	debugPrintInstruction("SLA", Direct, C),   // 21 SLA C
	debugPrintInstruction("SLA", Direct, D),   // 22 SLA D
	debugPrintInstruction("SLA", Direct, E),   // 23 SLA E
	debugPrintInstruction("SLA", Direct, H),   // 24 SLA H
	debugPrintInstruction("SLA", Direct, L),   // 25 SLA L
	debugPrintInstruction("SLA", Indirect, HL),// 26 SLA (HL)
	debugPrintInstruction("SLA", Direct, A),   // 27 SLA A
	debugPrintInstruction("SRA", Direct, B),   // 28 SRA B
	debugPrintInstruction("SRA", Direct, C),   // 29 SRA C
	debugPrintInstruction("SRA", Direct, D),   // 2a SRA D
	debugPrintInstruction("SRA", Direct, E),   // 2b SRA E
	debugPrintInstruction("SRA", Direct, H),   // 2c SRA H
	debugPrintInstruction("SRA", Direct, L),   // 2d SRA L
	debugPrintInstruction("SRA", Indirect, HL),// 2e SRA (HL)
	debugPrintInstruction("SRA", Direct, A),   // 2f SRA A
	debugPrintInstruction("SWAP", Direct, B),  // 30 SWAP B
	debugPrintInstruction("SWAP", Direct, C),  // 31 SWAP C
	debugPrintInstruction("SWAP", Direct, D),  // 32 SWAP D
	debugPrintInstruction("SWAP", Direct, E),  // 33 SWAP E
	debugPrintInstruction("SWAP", Direct, H),  // 34 SWAP H
	debugPrintInstruction("SWAP", Direct, L),  // 35 SWAP L
	debugPrintInstruction("SWAP", Indirect, HL), // 36 SWAP (HL)
	debugPrintInstruction("SWAP", Direct, A),  // 37 SWAP A
	debugPrintInstruction("SRL", Direct, B),   // 38 SRL B
	debugPrintInstruction("SRL", Direct, C),   // 39 SRL C
	debugPrintInstruction("SRL", Direct, D),   // 3a SRL D
	debugPrintInstruction("SRL", Direct, E),   // 3b SRL E
	debugPrintInstruction("SRL", Direct, H),   // 3c SRL H
	debugPrintInstruction("SRL", Direct, L),   // 3d SRL L
	debugPrintInstruction("SRL", Indirect, HL),// 3e SRL (HL)
	debugPrintInstruction("SRL", Direct, A),   // 3f SRL A
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, B), // 40 BIT 0,B
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, C), // 41 BIT 0,C
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, D), // 42 BIT 0,D
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, E), // 43 BIT 0,E
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, H), // 44 BIT 0,H
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, L), // 45 BIT 0,L
	debugPrintInstruction("BIT", Absolute, 0, Comma, Indirect, HL), // 46 BIT 0,(HL)
	debugPrintInstruction("BIT", Absolute, 0, Comma, Direct, A), // 47 BIT 0,A
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, B), // 48 BIT 1,B
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, C), // 49 BIT 1,C
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, D), // 4a BIT 1,D
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, E), // 4b BIT 1,E
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, H), // 4c BIT 1,H
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, L), // 4d BIT 1,L
	debugPrintInstruction("BIT", Absolute, 1, Comma, Indirect, HL), // 4e BIT 1,(HL)
	debugPrintInstruction("BIT", Absolute, 1, Comma, Direct, A), // 4f BIT 1,A
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, B), // 50 BIT 2,B
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, C), // 51 BIT 2,C
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, D), // 52 BIT 2,D
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, E), // 53 BIT 2,E
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, H), // 54 BIT 2,H
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, L), // 55 BIT 2,L
	debugPrintInstruction("BIT", Absolute, 2, Comma, Indirect, HL), // 56 BIT 2,(HL)
	debugPrintInstruction("BIT", Absolute, 2, Comma, Direct, A), // 57 BIT 2,A
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, B), // 58 BIT 3,B
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, C), // 59 BIT 3,C
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, D), // 5a BIT 3,D
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, E), // 5b BIT 3,E
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, H), // 5c BIT 3,H
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, L), // 5d BIT 3,L
	debugPrintInstruction("BIT", Absolute, 3, Comma, Indirect, HL), // 5e BIT 3,(HL)
	debugPrintInstruction("BIT", Absolute, 3, Comma, Direct, A), // 5f BIT 3,A
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, B), // 60 BIT 4,B
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, C), // 61 BIT 4,C
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, D), // 62 BIT 4,D
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, E), // 63 BIT 4,E
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, H), // 64 BIT 4,H
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, L), // 65 BIT 4,L
	debugPrintInstruction("BIT", Absolute, 4, Comma, Indirect, HL), // 66 BIT 4,(HL)
	debugPrintInstruction("BIT", Absolute, 4, Comma, Direct, A), // 67 BIT 4,A
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, B), // 68 BIT 5,B
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, C), // 69 BIT 5,C
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, D), // 6a BIT 5,D
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, E), // 6b BIT 5,E
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, H), // 6c BIT 5,H
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, L), // 6d BIT 5,L
	debugPrintInstruction("BIT", Absolute, 5, Comma, Indirect, HL), // 6e BIT 5,(HL)
	debugPrintInstruction("BIT", Absolute, 5, Comma, Direct, A), // 6f BIT 5,A
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, B), // 70 BIT 6,B
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, C), // 71 BIT 6,C
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, D), // 72 BIT 6,D
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, E), // 73 BIT 6,E
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, H), // 74 BIT 6,H
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, L), // 75 BIT 6,L
	debugPrintInstruction("BIT", Absolute, 6, Comma, Indirect, HL), // 76 BIT 6,(HL)
	debugPrintInstruction("BIT", Absolute, 6, Comma, Direct, A), // 77 BIT 6,A
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, B), // 78 BIT 7,B
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, C), // 79 BIT 7,C
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, D), // 7a BIT 7,D
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, E), // 7b BIT 7,E
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, H), // 7c BIT 7,H
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, L), // 7d BIT 7,L
	debugPrintInstruction("BIT", Absolute, 7, Comma, Indirect, HL), // 7e BIT 7,(HL)
	debugPrintInstruction("BIT", Absolute, 7, Comma, Direct, A), // 7f BIT 7,A
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, B), // 80 RES 0,B
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, C), // 81 RES 0,C
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, D), // 82 RES 0,D
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, E), // 83 RES 0,E
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, H), // 84 RES 0,H
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, L), // 85 RES 0,L
	debugPrintInstruction("RES", Absolute, 0, Comma, Indirect, HL), // 86 RES 0,(HL)
	debugPrintInstruction("RES", Absolute, 0, Comma, Direct, A), // 87 RES 0,A
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, B), // 88 RES 1,B
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, C), // 89 RES 1,C
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, D), // 8a RES 1,D
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, E), // 8b RES 1,E
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, H), // 8c RES 1,H
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, L), // 8d RES 1,L
	debugPrintInstruction("RES", Absolute, 1, Comma, Indirect, HL), // 8e RES 1,(HL)
	debugPrintInstruction("RES", Absolute, 1, Comma, Direct, A), // 8f RES 1,A
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, B), // 90 RES 2,B
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, C), // 91 RES 2,C
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, D), // 92 RES 2,D
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, E), // 93 RES 2,E
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, H), // 94 RES 2,H
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, L), // 95 RES 2,L
	debugPrintInstruction("RES", Absolute, 2, Comma, Indirect, HL), // 96 RES 2,(HL)
	debugPrintInstruction("RES", Absolute, 2, Comma, Direct, A), // 97 RES 2,A
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, B), // 98 RES 3,B
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, C), // 99 RES 3,C
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, D), // 9a RES 3,D
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, E), // 9b RES 3,E
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, H), // 9c RES 3,H
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, L), // 9d RES 3,L
	debugPrintInstruction("RES", Absolute, 3, Comma, Indirect, HL), // 9e RES 3,(HL)
	debugPrintInstruction("RES", Absolute, 3, Comma, Direct, A), // 9f RES 3,A
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, B), // a0 RES 4,B
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, C), // a1 RES 4,C
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, D), // a2 RES 4,D
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, E), // a3 RES 4,E
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, H), // a4 RES 4,H
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, L), // a5 RES 4,L
	debugPrintInstruction("RES", Absolute, 4, Comma, Indirect, HL), // a6 RES 4,(HL)
	debugPrintInstruction("RES", Absolute, 4, Comma, Direct, A), // a7 RES 4,A
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, B), // a8 RES 5,B
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, C), // a9 RES 5,C
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, D), // aa RES 5,D
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, E), // ab RES 5,E
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, H), // ac RES 5,H
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, L), // ad RES 5,L
	debugPrintInstruction("RES", Absolute, 5, Comma, Indirect, HL), // ae RES 5,(HL)
	debugPrintInstruction("RES", Absolute, 5, Comma, Direct, A), // af RES 5,A
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, B), // b0 RES 6,B
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, C), // b1 RES 6,C
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, D), // b2 RES 6,D
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, E), // b3 RES 6,E
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, H), // b4 RES 6,H
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, L), // b5 RES 6,L
	debugPrintInstruction("RES", Absolute, 6, Comma, Indirect, HL), // b6 RES 6,(HL)
	debugPrintInstruction("RES", Absolute, 6, Comma, Direct, A), // b7 RES 6,A
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, B), // b8 RES 7,B
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, C), // b9 RES 7,C
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, D), // ba RES 7,D
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, E), // bb RES 7,E
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, H), // bc RES 7,H
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, L), // bd RES 7,L
	debugPrintInstruction("RES", Absolute, 7, Comma, Indirect, HL), // be RES 7,(HL)
	debugPrintInstruction("RES", Absolute, 7, Comma, Direct, A), // bf RES 7,A
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, B), // c0 SET 0,B
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, C), // c1 SET 0,C
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, D), // c2 SET 0,D
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, E), // c3 SET 0,E
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, H), // c4 SET 0,H
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, L), // c5 SET 0,L
	debugPrintInstruction("SET", Absolute, 0, Comma, Indirect, HL), // c6 SET 0,(HL)
	debugPrintInstruction("SET", Absolute, 0, Comma, Direct, A), // c7 SET 0,A
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, B), // c8 SET 1,B
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, C), // c9 SET 1,C
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, D), // ca SET 1,D
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, E), // cb SET 1,E
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, H), // cc SET 1,H
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, L), // cd SET 1,L
	debugPrintInstruction("SET", Absolute, 1, Comma, Indirect, HL), // ce SET 1,(HL)
	debugPrintInstruction("SET", Absolute, 1, Comma, Direct, A), // cf SET 1,A
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, B), // d0 SET 2,B
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, C), // d1 SET 2,C
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, D), // d2 SET 2,D
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, E), // d3 SET 2,E
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, H), // d4 SET 2,H
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, L), // d5 SET 2,L
	debugPrintInstruction("SET", Absolute, 2, Comma, Indirect, HL), // d6 SET 2,(HL)
	debugPrintInstruction("SET", Absolute, 2, Comma, Direct, A), // d7 SET 2,A
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, B), // d8 SET 3,B
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, C), // d9 SET 3,C
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, D), // da SET 3,D
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, E), // db SET 3,E
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, H), // dc SET 3,H
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, L), // dd SET 3,L
	debugPrintInstruction("SET", Absolute, 3, Comma, Indirect, HL), // de SET 3,(HL)
	debugPrintInstruction("SET", Absolute, 3, Comma, Direct, A), // df SET 3,A
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, B), // e0 SET 4,B
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, C), // e1 SET 4,C
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, D), // e2 SET 4,D
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, E), // e3 SET 4,E
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, H), // e4 SET 4,H
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, L), // e5 SET 4,L
	debugPrintInstruction("SET", Absolute, 4, Comma, Indirect, HL), // e6 SET 4,(HL)
	debugPrintInstruction("SET", Absolute, 4, Comma, Direct, A), // e7 SET 4,A
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, B), // e8 SET 5,B
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, C), // e9 SET 5,C
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, D), // ea SET 5,D
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, E), // eb SET 5,E
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, H), // ec SET 5,H
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, L), // ed SET 5,L
	debugPrintInstruction("SET", Absolute, 5, Comma, Indirect, HL), // ee SET 5,(HL)
	debugPrintInstruction("SET", Absolute, 5, Comma, Direct, A), // ef SET 5,A
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, B), // f0 SET 6,B
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, C), // f1 SET 6,C
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, D), // f2 SET 6,D
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, E), // f3 SET 6,E
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, H), // f4 SET 6,H
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, L), // f5 SET 6,L
	debugPrintInstruction("SET", Absolute, 6, Comma, Indirect, HL), // f6 SET 6,(HL)
	debugPrintInstruction("SET", Absolute, 6, Comma, Direct, A), // f7 SET 6,A
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, B), // f8 SET 7,B
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, C), // f9 SET 7,C
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, D), // fa SET 7,D
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, E), // fb SET 7,E
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, H), // fc SET 7,H
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, L), // fd SET 7,L
	debugPrintInstruction("SET", Absolute, 7, Comma, Indirect, HL), // fe SET 7,(HL)
	debugPrintInstruction("SET", Absolute, 7, Comma, Direct, A)  // ff SET 7,A
};

void HandleCB(CPU* cpu, MMU* mmu, uint16_t addr) {
	uint8_t opcode = mmu->Read(addr + 1);
	cbhandlers[opcode](cpu, mmu, addr + 1);
}

const static Debug::InstructionPrinter handlers[] = {
	debugPrintInstruction("NOP"),                              // 00 NOP
	debugPrintInstruction("LD ", Direct, BC, Comma, HexOffset16, 1), // 01 LD  BC,d16
	debugPrintInstruction("LD ", Indirect, BC, Comma, Direct, A),    // 02 LD  (BC),A
	debugPrintInstruction("INC", Direct, BC),                  // 03 INC BC
	debugPrintInstruction("INC", Direct, B),                   // 04 INC B
	debugPrintInstruction("DEC", Direct, B),                   // 05 DEC B
	debugPrintInstruction("LD ", Direct, B, Comma, HexOffset8, 1),   // 06 LD  B,d8
	debugPrintInstruction("RLCA"),                             // 07 RLCA
	debugPrintInstruction("LD ", IndStart, HexOffset16, 1, IndFinish, Comma, Direct, SP), // 08 LD  (a16),SP
	debugPrintInstruction("ADD", Direct, HL, Comma, Direct, BC),     // 09 ADD HL,BC
	debugPrintInstruction("LD ", Direct, A, Comma, Indirect, BC),    // 0a LD  A,(BC)
	debugPrintInstruction("DEC", Direct, BC),                  // 0b DEC BC
	debugPrintInstruction("INC", Direct, C),                   // 0c INC C
	debugPrintInstruction("DEC", Direct, C),                   // 0d DEC C
	debugPrintInstruction("LD ", Direct, C, Comma, HexOffset8, 1),   // 0e LD  C,d8
	debugPrintInstruction("RRCA"),                             // 0f RRCA
	debugPrintInstruction("STOP"),                             // 10 STOP
	debugPrintInstruction("LD ", Direct, DE, Comma, HexOffset16, 1), // 11 LD  DE,d16
	debugPrintInstruction("LD ", Indirect, DE, Comma, Direct, A),    // 12 LD  (DE),A
	debugPrintInstruction("INC", Direct, DE),                  // 13 INC DE
	debugPrintInstruction("INC", Direct, D),                   // 14 INC D
	debugPrintInstruction("DEC", Direct, D),                   // 15 DEC D
	debugPrintInstruction("LD ", Direct, D, Comma, HexOffset8, 1),   // 16 LD  D,d8
	debugPrintInstruction("RLA"),                              // 17 RLA
	debugPrintInstruction("JR ", Offset8, 1),                  // 18 JR  r8
	debugPrintInstruction("ADD", Direct, HL, Comma, Direct, DE),     // 19 ADD HL,DE
	debugPrintInstruction("LD ", Direct, A, Comma, Indirect, DE),    // 1a LD  A,(DE)
	debugPrintInstruction("DEC", Direct, DE),                  // 1b DEC DE
	debugPrintInstruction("DEC", Direct, E),                   // 1c DEC E
	debugPrintInstruction("DEC", Direct, E),                   // 1d DEC E
	debugPrintInstruction("LD ", Direct, E, Comma, HexOffset8, 1),   // 1e LD  E,d8
	debugPrintInstruction("RRA"),                              // 1f RRA
	debugPrintInstruction("JR ", NZ, Comma, Offset8, 1),       // 20 JR  NZ,r8
	debugPrintInstruction("LD ", Direct, HL, Comma, HexOffset16, 1), // 21 LD  HL,d16
	debugPrintInstruction("LDI", Indirect, HL, Comma, Direct, A),    // 22 LDI (HL),A
	debugPrintInstruction("INC", Direct, HL),                  // 23 INC HL
	debugPrintInstruction("INC", Direct, H),                   // 24 INC H
	debugPrintInstruction("DEC", Direct, H),                   // 25 DEC H
	debugPrintInstruction("LD ", Direct, H, Comma, HexOffset8, 1),   // 26 LD  H,d8
	debugPrintInstruction("DAA"),                              // 27 DAA
	debugPrintInstruction("JR ", ZE, Comma, Offset8, 1),       // 28 JR  Z,r8
	debugPrintInstruction("ADD", Direct, HL, Comma, Direct, HL),     // 29 ADD HL,HL
	debugPrintInstruction("LDI", Direct, A, Comma, Indirect, HL),    // 2a LDI A,(HL)
	debugPrintInstruction("DEC", Direct, HL),                  // 2b DEC HL
	debugPrintInstruction("INC", Direct, L),                   // 2c INC L
	debugPrintInstruction("DEC", Direct, L),                   // 2d DEC L
	debugPrintInstruction("LD ", Direct, L, Comma, HexOffset8, 1),   // 2e LD  L,d8
	debugPrintInstruction("CPL"),                              // 2f CPL
	debugPrintInstruction("JR ", NC, Comma, Offset8, 1),       // 30 JR  NC,r8
	debugPrintInstruction("LD ", Direct, SP, Comma, HexOffset16, 1), // 31 LD  SP,d16
	debugPrintInstruction("LDD", Indirect, HL, Comma, Direct, A), // 32 LDD (HL),A
	debugPrintInstruction("INC", Direct, SP),                  // 33 INC SP
	debugPrintInstruction("INC", Indirect, HL),                // 34 INC (HL)
	debugPrintInstruction("DEC", Indirect, HL),                // 35 DEC (HL)
	debugPrintInstruction("LD ", Indirect, HL, Comma, Offset8, 1),// 36 LD  (HL),d8
	debugPrintInstruction("SCF"),                              // 37 SCF
	debugPrintInstruction("JR ", CA, Comma, Offset8, 1),       // 38 JR  C,r8
	debugPrintInstruction("ADD", Direct, HL, Direct, SP),      // 39 ADD HL,SP
	debugPrintInstruction("LDD", Direct, A, Comma, Indirect, HL), // 3a LDD A,(HL)
	debugPrintInstruction("DEC", Direct, SP),                  // 3b DEC SP
	debugPrintInstruction("INC", Direct, A),                   // 3c INC A
	debugPrintInstruction("DEC", Direct, A),                   // 3d DEC A
	debugPrintInstruction("LD ", Direct, A, Comma, HexOffset8, 1),// 3e LD  A,d8
	debugPrintInstruction("CCF"),                              // 3f CCF
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, B), // 40 LD B,B
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, C), // 41 LD B,C
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, D), // 42 LD B,D
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, E), // 43 LD B,E
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, H), // 44 LD B,H
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, L), // 45 LD B,L
	debugPrintInstruction("LD ", Direct, B, Comma, Indirect, HL), // 46 LD B,(HL)
	debugPrintInstruction("LD ", Direct, B, Comma, Direct, A), // 47 LD B,A
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, B), // 48 LD C,B
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, C), // 49 LD C,C
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, D), // 4a LD C,D
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, E), // 4b LD C,E
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, H), // 4c LD C,H
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, L), // 4d LD C,L
	debugPrintInstruction("LD ", Direct, C, Comma, Indirect, HL), // 4e LD C,(HL)
	debugPrintInstruction("LD ", Direct, C, Comma, Direct, A), // 4f LD C,A
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, B), // 50 LD D,B
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, C), // 51 LD D,C
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, D), // 52 LD D,D
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, E), // 53 LD D,E
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, H), // 54 LD D,H
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, L), // 55 LD D,L
	debugPrintInstruction("LD ", Direct, D, Comma, Indirect, HL), // 56 LD D,(HL)
	debugPrintInstruction("LD ", Direct, D, Comma, Direct, A), // 57 LD D,A
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, B), // 58 LD E,B
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, C), // 59 LD E,C
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, D), // 5a LD E,D
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, E), // 5b LD E,E
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, H), // 5c LD E,H
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, L), // 5d LD E,L
	debugPrintInstruction("LD ", Direct, E, Comma, Indirect, HL), // 5e LD E,(HL)
	debugPrintInstruction("LD ", Direct, E, Comma, Direct, A), // 5f LD E,A
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, B), // 60 LD H,B
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, C), // 61 LD H,C
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, D), // 62 LD H,D
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, E), // 63 LD H,E
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, H), // 64 LD H,H
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, L), // 65 LD H,L
	debugPrintInstruction("LD ", Direct, H, Comma, Indirect, HL), // 66 LD H,(HL)
	debugPrintInstruction("LD ", Direct, H, Comma, Direct, A), // 67 LD H,A
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, B), // 68 LD L,B
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, C), // 69 LD L,C
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, D), // 6a LD L,D
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, E), // 6b LD L,E
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, H), // 6c LD L,H
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, L), // 6d LD L,L
	debugPrintInstruction("LD ", Direct, L, Comma, Indirect, HL), // 6e LD L,(HL)
	debugPrintInstruction("LD ", Direct, L, Comma, Direct, A), // 6f LD L,A
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, B), // 70 LD (HL),B
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, C), // 71 LD (HL),C
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, D), // 72 LD (HL),D
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, E), // 73 LD (HL),E
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, H), // 74 LD (HL),H
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, L), // 75 LD (HL),L
	debugPrintInstruction("HALT"),                             // 76 HALT
	debugPrintInstruction("LD ", Indirect, HL, Comma, Direct, A), // 77 LD (HL),A
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, B), // 78 LD A,B
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, C), // 79 LD A,C
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, D), // 7a LD A,D
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, E), // 7b LD A,E
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, H), // 7c LD A,H
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, L), // 7d LD A,L
	debugPrintInstruction("LD ", Direct, A, Comma, Indirect, HL), // 7e LD A,(HL)
	debugPrintInstruction("LD ", Direct, A, Comma, Direct, A), // 7f LD A,A
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, B), // 80 ADD A,B
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, C), // 81 ADD A,C
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, D), // 82 ADD A,D
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, E), // 83 ADD A,E
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, H), // 84 ADD A,H
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, L), // 85 ADD A,L
	debugPrintInstruction("ADD", Direct, A, Comma, Indirect, HL), // 86 ADD A,(HL)
	debugPrintInstruction("ADD", Direct, A, Comma, Direct, A), // 87 ADD A,A
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, B), // 88 ADC A,B
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, C), // 89 ADC A,C
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, D), // 8a ADC A,D
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, E), // 8b ADC A,E
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, H), // 8c ADC A,H
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, L), // 8d ADC A,L
	debugPrintInstruction("ADC", Direct, A, Comma, Indirect, HL), // 8e ADC A,(HL)
	debugPrintInstruction("ADC", Direct, A, Comma, Direct, A), // 8f ADC A,A
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, B), // 90 SUB A,B
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, C), // 91 SUB A,C
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, D), // 92 SUB A,D
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, E), // 93 SUB A,E
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, H), // 94 SUB A,H
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, L), // 95 SUB A,L
	debugPrintInstruction("SUB", Direct, A, Comma, Indirect, HL), // 96 SUB A,(HL)
	debugPrintInstruction("SUB", Direct, A, Comma, Direct, A), // 97 SUB A,A
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, B), // 98 SBC A,B
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, C), // 99 SBC A,C
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, D), // 9a SBC A,D
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, E), // 9b SBC A,E
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, H), // 9c SBC A,H
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, L), // 9d SBC A,L
	debugPrintInstruction("SBC", Direct, A, Comma, Indirect, HL), // 9e SBC A,(HL)
	debugPrintInstruction("SBC", Direct, A, Comma, Direct, A), // 9f SBC A,A
	debugPrintInstruction("AND", Direct, A, Comma, Direct, B), // a0 AND A,B
	debugPrintInstruction("AND", Direct, A, Comma, Direct, C), // a1 AND A,C
	debugPrintInstruction("AND", Direct, A, Comma, Direct, D), // a2 AND A,D
	debugPrintInstruction("AND", Direct, A, Comma, Direct, E), // a3 AND A,E
	debugPrintInstruction("AND", Direct, A, Comma, Direct, H), // a4 AND A,H
	debugPrintInstruction("AND", Direct, A, Comma, Direct, L), // a5 AND A,L
	debugPrintInstruction("AND", Direct, A, Comma, Indirect, HL), // a6 AND A,(HL)
	debugPrintInstruction("AND", Direct, A, Comma, Direct, A), // a7 AND A,A
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, B), // a8 XOR A,B
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, C), // a9 XOR A,C
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, D), // aa XOR A,D
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, E), // ab XOR A,E
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, H), // ac XOR A,H
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, L), // ad XOR A,L
	debugPrintInstruction("XOR", Direct, A, Comma, Indirect, HL), // ae XOR A,(HL)
	debugPrintInstruction("XOR", Direct, A, Comma, Direct, A), // af XOR A,A
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, B), // b0 OR  A,B
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, C), // b1 OR  A,C
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, D), // b2 OR  A,D
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, E), // b3 OR  A,E
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, H), // b4 OR  A,H
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, L), // b5 OR  A,L
	debugPrintInstruction("OR ", Direct, A, Comma, Indirect, HL), // b6 OR  A,(HL)
	debugPrintInstruction("OR ", Direct, A, Comma, Direct, A), // b7 OR  A,A
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, B), // b8 CP  A,B
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, C), // b9 CP  A,C
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, D), // ba CP  A,D
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, E), // bb CP  A,E
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, H), // bc CP  A,H
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, L), // bd CP  A,L
	debugPrintInstruction("CP ", Direct, A, Comma, Indirect, HL), // be CP  A,(HL)
	debugPrintInstruction("CP ", Direct, A, Comma, Direct, A), // bf CP  A,A
	debugPrintInstruction("RET", NZ),                          // c0 RET NZ
	debugPrintInstruction("POP", Direct, BC),                  // c1 POP BC
	debugPrintInstruction("JP ", NZ, Comma, HexOffset16, 1),   // c2 JP  NZ,a16
	debugPrintInstruction("JP ", HexOffset16, 1),              // c3 JP  a16
	debugPrintInstruction("CALL", NZ, Comma, HexOffset16, 1),  // c4 CALL NZ,a16
	debugPrintInstruction("PUSH", Direct, BC),                 // c5 PUSH BC
	debugPrintInstruction("ADD", Direct, A, Comma, HexOffset8, 1), // c6 ADD A,d8
	debugPrintInstruction("RST", Hex8, 0x00),                  // c7 RST 00h
	debugPrintInstruction("RET", ZE),                          // c8 RET Z
	debugPrintInstruction("RET"),                              // c9 RET
	debugPrintInstruction("JP ", ZE, Comma, HexOffset16, 1),   // ca JP  Z,a16
	HandleCB,                                                  // cb [See cbhandlers]
	debugPrintInstruction("CALL", ZE, Comma, HexOffset16, 1),  // cc CALL Z,a16
	debugPrintInstruction("CALL", HexOffset16, 1),             // cd CALL a16
	debugPrintInstruction("ADC", Direct, A, Comma, HexOffset8, 1), // ce ADC A,d8
	debugPrintInstruction("RST", Hex8, 0x08),                  // cf RST 08h
	debugPrintInstruction("RET", NC),                          // d0 RET NC
	debugPrintInstruction("POP", Direct, DE),                  // d1 POP DE
	debugPrintInstruction("JP ", NZ, Comma, HexOffset16, 1),   // d2 JP  NC,a16
	debugPrintInstruction("WRONG"),                            // d3 --
	debugPrintInstruction("CALL", NC, Comma, HexOffset16, 1),  // d4 CALL NC,a16
	debugPrintInstruction("PUSH", Direct, DE),                 // d5 PUSH DE
	debugPrintInstruction("SUB", Direct, A, Comma, HexOffset8, 1), // d6 SUB A,d8
	debugPrintInstruction("RST", Hex8, 0x10),                  // d7 RST 10h
	debugPrintInstruction("RET", CA),                          // d8 RET C
	debugPrintInstruction("RETI"),                             // d9 RETI
	debugPrintInstruction("JP ", CA, Comma, HexOffset16, 1),   // da JP  C,a16
	debugPrintInstruction("WRONG"),                            // db --
	debugPrintInstruction("CALL", CA, Comma, HexOffset16, 1),  // dc CALL C,a16
	debugPrintInstruction("WRONG"),                            // dd --
	debugPrintInstruction("SBC", Direct, A, Comma, HexOffset8, 1), // de SBC A,d8
	debugPrintInstruction("RST", Hex8, 0x18),                  // df RST 18h
	debugPrintInstruction("LDH", IndStart, Hex16, 0xff00, "+", HexOffset8, 1, IndFinish, Comma, Direct, A), // e0 LDH (a8),A
	debugPrintInstruction("POP", Direct, HL),                  // e1 POP HL
	debugPrintInstruction("LD ", IndStart, Hex16, 0xff00, "+", Direct, C, IndFinish, Comma, Direct, A), // e2 LD  (C),A
	debugPrintInstruction("WRONG"),                            // e3 --
	debugPrintInstruction("WRONG"),                            // e4 --
	debugPrintInstruction("PUSH", Direct, HL),                 // e5 PUSH HL
	debugPrintInstruction("AND", Direct, A, Comma, HexOffset8, 1), // e6 AND A,d8
	debugPrintInstruction("RST", Hex8, 0x20),                  // e7 RST 20h
	debugPrintInstruction("ADD", Direct, SP, Comma, Offset8, 1),   // e8 ADD SP,r8
	debugPrintInstruction("JP ", Indirect, HL),                // e9 JP  (HL)
	debugPrintInstruction("LD ", IndStart, HexOffset16, 1, IndFinish, Comma, Direct, A), // ea LD  (a16),A
	debugPrintInstruction("WRONG"),                            // eb --
	debugPrintInstruction("WRONG"),                            // ec --
	debugPrintInstruction("WRONG"),                            // ed --
	debugPrintInstruction("XOR", Direct, A, Comma, HexOffset8, 1), // ee XOR A,d8
	debugPrintInstruction("RST", Hex8, 0x28),                  // ef RST 28h
	debugPrintInstruction("LDH", Direct, A, Comma, IndStart, Hex16, 0xff00, "+", HexOffset8, 1, IndFinish), // f0 LDH A,(a8)
	debugPrintInstruction("POP", Direct, AF),                  // f1 POP AF
	debugPrintInstruction("LD ", Direct, A, Comma, IndStart, Hex16, 0xff00, "+", Direct, C, IndFinish), // f2 LD  A,(C)
	debugPrintInstruction("DI"),                               // f3 DI
	debugPrintInstruction("WRONG"),                            // f4 --
	debugPrintInstruction("PUSH", Direct, AF),                 // f5 PUSH AF
	debugPrintInstruction("OR ", Direct, A, Comma, HexOffset8, 1), // f6 OR  A,d8
	debugPrintInstruction("RST", Hex8, 0x30),                  // f7 RES 30h
	debugPrintInstruction("LD ", Direct, HL, Comma, Direct, SP, "+", Offset8, 1), // f8 LD  HL,SP+r8
	debugPrintInstruction("LD ", Direct, SP, Comma, Direct, HL),   // f9 LD  SP,HL
	debugPrintInstruction("LD ", Direct, A, Comma, IndStart, HexOffset16, 1, IndFinish), // fa LD  A,(a16)
	debugPrintInstruction("EI"),                               // fb EI
	debugPrintInstruction("WRONG"),                            // fc --
	debugPrintInstruction("WRONG"),                            // fd --
	debugPrintInstruction("CP ", Direct, A, Comma, HexOffset8, 1), // fe CP  A,d8
	debugPrintInstruction("RST", Hex8, 0x38)                   // ff RST 38h
};

void Debugger::printInstruction(const uint16_t addr) const {
	uint8_t opcode = emulator->mmu.Read(addr);
	handlers[opcode](&(emulator->cpu), &(emulator->mmu), addr);
}

void Debugger::printRegisters() const {
	std::ios::fmtflags fmt(std::cout.flags());
	std::cout
		<< ":-----------------------------------------------------:\r\n"
		<< "| A  |Flag| B  | C  | D  | E  | H  | L  |  SP  |  PC  |\r\n"
		<< "|----+----+----+----+----+----+----+----+------+------|\r\n"
		<< std::hex
		<<  "| " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.AF.Single.A
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.AF.Single.Flags.Byte
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.BC.Single.B
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.BC.Single.C
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.DE.Single.D
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.DE.Single.E
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.HL.Single.H
		<< " | " << std::setfill('0') << std::setw(2) << (int) emulator->cpu.HL.Single.L
		<< " | " << std::setfill('0') << std::setw(4) << emulator->cpu.SP
		<< " | " << std::setfill('0') << std::setw(4) << emulator->cpu.PC << " |\r\n"
		<< ":-----------------------------------------------------:" << std::endl;
	std::cout.flags(fmt);
}

void Debugger::printStack() const {
	uint16_t sp = emulator->cpu.SP;
	bool current = true;
	int iter = 0;
	const int limit = 5;

	std::ios::fmtflags fmt(std::cout.flags());
	while (sp < 0xffff && iter < limit) {
		if (current) {
			std::cout << "--> ";
			current = false;
		} else {
			std::cout << "    ";
		}
		std::cout << std::hex << std::setfill('0') << std::setw(4) << sp << " | " << std::setfill('0') << std::setw(2) << (int) emulator->mmu.Read(sp) << std::endl;
		sp++; iter++;
	}
	std::cout.flags(fmt);
}

void Debugger::printFlags() const {
	FlagStruct* flags = &emulator->cpu.AF.Single.Flags.Values;
	std::cout
		<< ":---------------:\r\n"
		<< "| Z | N | H | C |\r\n"
		<< "|---+---+---+---|\r\n"
		<< "| " << (int) flags->Zero << " | " << (int) flags->BCD_AddSub << " | " << (int) flags->BCD_HalfCarry << " | " << (int) flags->Carry << " |\r\n"
		<< ":---------------:" << std::endl;
}
