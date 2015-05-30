#include "Debugger.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace Debug;

Debugger::Debugger(Emulator *_emulator, uint8_t _opts) {
	emulator = _emulator;
	opts = _opts;
}

Debugger::~Debugger() {}

void Debugger::Run() {
	emulator->cpu.running = false;
	while (true) {
		if (!emulator->cpu.running || opts & DBG_INTERACTIVE) {
			DebugCmd cmd = getCommand("(mfemu)");
			switch (cmd.instr) {
			case CMD_RUN:
				std::cout << "Starting emulation..." << std::endl;
				emulator->cpu.running = true;
				break;
			case CMD_PRINT:
				printInstruction(emulator->cpu.PC);
				break;
			case CMD_BREAK: {
				std::stringstream ss(cmd.args.front());
				uint16_t arg;
				ss >> std::hex >> arg;
				setBreakpoint(arg);
				break;
			}
			case CMD_STEP:
				if (emulator->cpu.running) {
					if (!(opts & DBG_NOGRAPHICS)) {
						SDL_RenderClear(emulator->renderer);
					}
					emulator->cpu.Step();
					printInstruction(emulator->cpu.PC);
				} else {
					std::cerr << "CPU is not running: type `run` to start emulation." << std::endl;
				}
				break;
			case CMD_CONTINUE:
				if (emulator->cpu.running) {
					opts &= ~DBG_INTERACTIVE;
				} else {
					std::cerr << "CPU is not running: type `run` to start emulation." << std::endl;
				}
				break;
			case CMD_HELP:
				std::cout 
					<< "run           Start emulation" << std::endl
					<< "print         Print current instruction" << std::endl
					<< "break <addr>  Set breakpoint at <addr>" << std::endl
					<< "step          Fetch and execute a single instruction" << std::endl
					<< "continue      Resume execution" << std::endl
					<< "help          Print a help message" << std::endl
					<< "quit          Quit the debugger" << std::endl;
				break;
			case CMD_QUIT:
				std::cerr << "...quitting." << std::endl;
				return;
			default:
				std::cerr << "Invalid command" << std::endl;
			}
		} else {
			// Execute instructions until a breakpoint is found
			uint16_t PC = emulator->cpu.PC;
			if (breakPoints.find(PC) != breakPoints.end()) {
				opts |= DBG_INTERACTIVE;
				std::cout << "Breakpoint reached: " << std::hex << (int)PC << std::endl;
				continue;
			}
			uint8_t opcode = emulator->cpu.Read(PC);
			emulator->cpu.Execute(opcode);
			if (!(opts & DBG_NOGRAPHICS))
				SDL_RenderClear(emulator->renderer);

			++emulator->cpu.PC;
		}
	}
}

// Reads a command from stdin and returns a struct { cmd, args }.
// Currently only takes 1 argument.
DebugCmd Debugger::getCommand(const char* prompt) {
	static DebugCmd latest = { CMD_INVALID };

	std::cout << prompt << " " << std::flush;

	std::string line;
	std::string instr;
	DebugCmd cmd;

	std::getline(std::cin, line);

	if ((std::cin.rdstate() & std::cin.eofbit) != 0) {
		cmd.instr = CMD_QUIT;
		return cmd;
	}

	// rtrim spaces
	line.erase(std::find_if(line.rbegin(), line.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), line.end());
	std::stringstream ss(line);

	if (line.length() < 1) {
		if (latest.instr != CMD_INVALID) {
			// repeat latest command
			return latest;
		} else {
			return getCommand(prompt);
		}
	} else {
		ss >> instr;
	}

	if (instr == "run") {
		cmd.instr = CMD_RUN;
	} else if (instr == "print") {
		cmd.instr = CMD_PRINT;
	} else if (instr == "break") {
		cmd.instr = CMD_BREAK;
		std::string arg;
		ss >> arg;
		cmd.args.push_back(arg);
	} else if (instr == "quit" || instr == "exit") {
		cmd.instr = CMD_QUIT;
	} else if (instr == "step") {
		cmd.instr = CMD_STEP;
	} else if (instr == "cont" || instr == "continue") {
		cmd.instr = CMD_CONTINUE;
	} else if (instr == "help" || instr == "?") {
		cmd.instr = CMD_HELP;
	} else {
		cmd.instr = CMD_INVALID;
	}

	latest = cmd;
	return cmd;
}

void Debugger::setBreakpoint(uint16_t addr) {
	breakPoints.insert(addr);
	std::cout << "Set breakpoint to " << std::setfill('0') << std::setw(4) << std::hex << (int)addr << std::endl;
}
