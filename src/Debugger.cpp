#include "Debugger.h"
#include <iostream>

using namespace Debug;

Debugger::Debugger(Emulator *_emulator, DebugOpts _opts) {
	emulator = _emulator;
	opts = _opts;
}

Debugger::~Debugger() {}

void Debugger::Run() {
	emulator->cpu.running = false;
	while (true) {
		if (!emulator->cpu.running || opts & DBG_INTERACTIVE) {
			std::cout << "(mfemu) ";
			DebugCmd cmd = getCommand();
			switch (cmd.instr) {
			case CMD_RUN:
				emulator->cpu.running = true;
				break;
			case CMD_BREAK: {
				uint16_t arg = atoi(cmd.args.front().c_str());
				setBreakpoint(arg);
				break;
			}
			case CMD_STEP:
				if (emulator->cpu.running) {
					SDL_RenderClear(emulator->renderer);
					emulator->cpu.Step();
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
			uint16_t opcode = emulator->cpu.Read(PC);
			emulator->cpu.Execute(opcode);
			if (!(opts & DBG_NOGRAPHICS))
				SDL_RenderClear(emulator->renderer);

			++emulator->cpu.PC;
		}
	}
}

// Reads a command from stdin and returns a struct { cmd, args }.
// Currently only takes 1 argument.
DebugCmd Debugger::getCommand() {
	std::string instr, arg;
	DebugCmd cmd;

	std::cin >> instr >> arg;
	if (instr == "run")
		cmd.instr = CMD_RUN;
	else if (instr == "break")
		cmd.instr = CMD_BREAK;
	else if (instr == "quit" || instr == "exit")
		cmd.instr = CMD_QUIT;
	else if (instr == "step")
		cmd.instr = CMD_STEP;
	else {
		cmd.instr = CMD_INVALID;
		return cmd;
	}

	cmd.args.push_back(arg);

	return cmd;
}

void Debugger::setBreakpoint(uint16_t addr) {
	breakPoints.insert(addr);
	std::cout << "Set breakpoint to " << std::hex << (int)addr << std::endl;
}
