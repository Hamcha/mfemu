#pragma once

#include <list>
#include <set>
#include "Emulator.h"

namespace Debug {

enum DebugOpts : uint8_t {
	DBG_INTERACTIVE = 1,
	DBG_NOGRAPHICS  = 1 << 1,
};

enum DebugInstr {
	CMD_INVALID,
	CMD_RUN,
	CMD_STEP,
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

	Debug::DebugCmd getCommand(const char* prompt);
	void setBreakpoint(uint16_t addr);
public:
	Debugger(Emulator *_emulator, uint8_t _opts);
	~Debugger();

	void Run();
};
