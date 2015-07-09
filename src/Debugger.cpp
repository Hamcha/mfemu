#include "Debugger.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <utility>
#include <map>
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
#include <unistd.h>
#include <csignal>
#elif _WIN32 || _WIN64
#include <windows.h>
#endif

using namespace Debug;

static Debugger* _debugger = nullptr;

// While the debugger is running, trap the SIGINT to pause the emulation.
static void interrupt_handler(int s) {
	if (_debugger == nullptr) return;
	if (_debugger->getEmulator()->cpu.running) {
		std::clog << "Emulation paused. Type 'run' to resume." << std::endl;
		_debugger->getEmulator()->cpu.running = false;
		return;
	}
	exit(s);
}

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
static void setup_signal_trap() {
	struct sigaction sigintHandler;
	sigintHandler.sa_handler = interrupt_handler;
	sigemptyset(&sigintHandler.sa_mask);
	sigintHandler.sa_flags = 0;
	sigaction(SIGINT, &sigintHandler, NULL);
}
#elif _WIN32 || _WIN64
static BOOL InterruptHandlerProxy(DWORD ctrlType) {
	switch (ctrlType) {
	case CTRL_C_EVENT:
		interrupt_handler(0);
		return TRUE;
	}
	return FALSE;
}
#endif

// { cmd_string => { command, n.args } }
static const std::map<std::string, std::pair<DebugInstr, int>> debugInstructions = {
	{ "run",      std::make_pair(CMD_RUN,       0) },
	{ "print",    std::make_pair(CMD_PRINT,     1) },
	{ "reg",      std::make_pair(CMD_REGISTERS, 0) },
	{ "stack",    std::make_pair(CMD_STACK,     0) },
	{ "flags",    std::make_pair(CMD_FLAGS,     0) },
	{ "mem",      std::make_pair(CMD_MEMORY,    1) },
	{ "track",    std::make_pair(CMD_TRACK,     0) },
	{ "break",    std::make_pair(CMD_BREAK,     1) },
	{ "quit",     std::make_pair(CMD_QUIT,      0) },
	{ "exit",     std::make_pair(CMD_QUIT,      0) },
	{ "step",     std::make_pair(CMD_STEP,      0) },
	{ "cont",     std::make_pair(CMD_CONTINUE,  0) },
	{ "continue", std::make_pair(CMD_CONTINUE,  0) },
	{ "rominfo",  std::make_pair(CMD_ROMINFO,   0) },
	{ "help",     std::make_pair(CMD_HELP,      0) },
	{ "?",        std::make_pair(CMD_HELP,      0) }
};

Debugger::Debugger(Emulator *const _emulator, const uint8_t _opts) 
	: emulator(_emulator), opts(_opts) {}

Debugger::~Debugger() {}

void Debugger::Run() {
	emulator->cpu.running = (opts & DBG_NOSTART) != DBG_NOSTART;

	// Trap SIGINT to pause the execution
	_debugger = this;
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	setup_signal_trap();
#elif _WIN32 || _WIN64
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)InterruptHandlerProxy, TRUE);
#endif

	while (emulator->running) {
		emulator->CheckUpdate();

		if (!emulator->cpu.running && opts & DBG_INTERACTIVE) {
			DebugCmd cmd = getCommand("(mfemu)");
			switch (cmd.instr) {
			case CMD_RUN:
				std::clog << "Starting emulation..." << std::endl;
				ignoreBreakpoints = true;
				emulator->cpu.running = true;
				break;
			case CMD_PRINT: {
				uint16_t arg = emulator->cpu.PC;
				if (cmd.args.front().length() > 0) {
					std::stringstream ss(cmd.args.front());
					ss >> std::hex >> arg;
				}
				printInstruction(arg);
				break;
			}
			case CMD_REGISTERS:
				printRegisters();
				break;
			case CMD_STACK:
				printStack();
				break;
			case CMD_MEMORY: {
				std::stringstream ss(cmd.args.front());
				uint16_t arg;
				ss >> std::hex >> arg;
				std::ios::fmtflags fmt(std::cout.flags());
				std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) emulator->mmu.Read(arg) << std::endl;
				std::cout.flags(fmt);
				break;
			}
			case CMD_FLAGS:
				printFlags();
				break;
			case CMD_TRACK:
				track = !track;
				std::cout << "Tracking has been " << (track ? "ENABLED" : "DISABLED") << std::endl;
				break;
			case CMD_BREAK: {
				std::stringstream ss(cmd.args.front());
				uint16_t arg;
				ss >> std::hex >> arg;
				setBreakpoint(arg);
				break;
			}
			case CMD_STEP:
				emulator->cpu.Step();
				printInstruction(emulator->cpu.PC);
				break;
			case CMD_CONTINUE:
				if (emulator->cpu.running) {
					opts &= ~DBG_INTERACTIVE;
				} else {
					std::cerr << "CPU is not running: type `run` to start emulation." << std::endl;
				}
				break;
			case CMD_ROMINFO:
				emulator->rom.debugPrintData();
				break;
			case CMD_HELP:
				std::cout 
					<< "run           Start emulation\r\n"
					<< "print         Print current instruction (or any given one via argument)\r\n"
					<< "mem           Print value at a specified memory location\r\n"
					<< "reg           Print registers\r\n"
					<< "stack         Print stack\r\n"
					<< "break <addr>  Set breakpoint at <addr>\r\n"
					<< "step          Fetch and execute a single instruction\r\n"
					<< "continue      Resume execution\r\n"
					<< "track         Toggle instruction printing\r\n"
					<< "help          Print a help message\r\n"
					<< "quit          Quit the debugger" << std::endl;
				break;
			case CMD_QUIT:
				std::clog << "...quitting." << std::endl;
				return;
			default:
				std::cerr << "Invalid command" << std::endl;
			}

			continue;
		}

		// Execute instructions until a breakpoint is found
		uint16_t PC = emulator->cpu.PC;
		if (!ignoreBreakpoints && breakPoints.find(PC) != breakPoints.end()) {
			emulator->cpu.running = false;
			std::ios::fmtflags fmt(std::cout.flags());
			std::cout << "Breakpoint reached: " << std::hex << (int)PC << std::endl;
			std::cout.flags(fmt);
			continue;
		}

		if (track) {
			printInstruction(emulator->cpu.PC);
		}
		emulator->Step();

		// ignoraBreakpoints should only last one iteration
		if (ignoreBreakpoints) {
			ignoreBreakpoints = false;
		}
	}
}

// Reads a command from stdin and returns a struct { cmd, args }.
// Currently only takes 1 argument.
DebugCmd Debugger::getCommand(const char* prompt) const {
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

	auto inst_pair = debugInstructions.find(instr);
	if (inst_pair != debugInstructions.end()) {
		cmd.instr = inst_pair->second.first;
		for (int i = 0; i < inst_pair->second.second; i += 1) {
			std::string arg;
			ss >> arg;
			cmd.args.push_back(arg);
		}
	} else {
		cmd.instr = CMD_INVALID;
	}

	latest = cmd;
	return cmd;
}

void Debugger::setBreakpoint(const uint16_t addr) {
	breakPoints.insert(addr);
	std::ios::fmtflags fmt(std::clog.flags());
	std::clog << "Set breakpoint to " << std::setfill('0') << std::setw(4) << std::hex << (int)addr << std::endl;
	std::clog.flags(fmt);
}
