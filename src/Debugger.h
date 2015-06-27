#pragma once

#include <list>
#include <set>
#include <functional>
#include "Emulator.h"

namespace Debug {

typedef std::function<void(CPU* cpu, MMU* mmu, const uint16_t addr)> InstructionPrinter;

enum DebugOpts : uint8_t {
	DBG_INTERACTIVE = 1,
	DBG_NOGRAPHICS  = 1 << 1,
	DBG_PRINTINSTR  = 1 << 2,
	DBG_NOSTART     = 1 << 3
};

enum DebugInstr {
	CMD_INVALID,
	CMD_RUN,
	CMD_PRINT,
	CMD_REGISTERS,
	CMD_STACK,
	CMD_TRACK,
	CMD_STEP,
	CMD_MEMORY,
	CMD_FLAGS,
	CMD_BREAK,
	CMD_CONTINUE,
	CMD_HELP,
	CMD_QUIT
};

struct DebugCmd {
	DebugInstr instr;
	std::list<std::string> args;
};

} // end namespace Debug

class Debugger {
private:
	Emulator *emulator;
	uint8_t opts;
	std::set<uint16_t> breakPoints;
	bool track;

	Debug::DebugCmd getCommand(const char* prompt);
	void setBreakpoint(const uint16_t addr);
	void printInstruction(const uint16_t addr);
	void printRegisters();
	void printStack();
	void printFlags();
public:
	Debugger(Emulator *_emulator, const uint8_t _opts);
	~Debugger();

	void Run();

	Emulator* getEmulator() const { return emulator; }
};
