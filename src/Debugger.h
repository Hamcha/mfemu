#pragma once

#include <iostream>
#include <list>
#include <unordered_set>
#include <functional>
#include "Emulator.h"

namespace Debug {

using InstructionPrinter = std::function<void(std::ostream& out, CPU* cpu, MMU* mmu, const uint16_t addr)>;

enum DebugOpts : uint8_t {
	DBG_INTERACTIVE = 1,
	DBG_PRINTINSTR  = 1 << 1,
	DBG_NOSTART     = 1 << 2,
	DBG_TRACK       = 1 << 3
};

} // end namespace Debug

class Debugger {
private:
	Emulator *const emulator;
	uint8_t opts;
	std::unordered_set<uint16_t> breakPoints;
	bool track = false;

	void setBreakpoint(const uint16_t addr);
	void printInstruction(const uint16_t addr, std::ostream& out = std::cout) const;
	void printRegisters(std::ostream& out = std::cout) const;
	void printStack(std::ostream& out = std::cout) const;
	void printFlags(std::ostream& out = std::cout) const;
public:

	//! Max number of elements in the instruction history
	int historySize;

	explicit Debugger(Emulator *const _emulator, const uint8_t _opts);
	~Debugger();

	void Run();

	Emulator* getEmulator() const { return emulator; }
};
