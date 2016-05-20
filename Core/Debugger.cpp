#include "Debugger.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <bitset>
#include <tuple>
#include <deque>
#include <unordered_map>
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
	if (!_debugger->GetEmulator()->cpu.paused) {
		std::clog << "Emulation paused. Type 'run' to resume." << std::endl;
		_debugger->GetEmulator()->cpu.paused = true;
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
	CMD_ROMINFO,
	CMD_HELP,
	CMD_QUIT,
	CMD_TOGGLEBP,
	CMD_DUMP,
	CMD_COUNTERS,
	CMD_INTS,
};

struct DebugCmd {
	DebugInstr instr;
	std::list<std::string> args;
};

static DebugCmd getCommand(const char* prompt);

// { cmd_string => { command, n.args, description } }
static const std::unordered_map<std::string, std::tuple<DebugInstr, int, std::string>> debugInstructions = {
	{ "run",        std::make_tuple(CMD_RUN,       0, "Start emulation") },
	{ "print",      std::make_tuple(CMD_PRINT,     1, "Print current instruction (or any given one via argument)") },
	{ "reg",        std::make_tuple(CMD_REGISTERS, 0, "Print registers") },
	{ "stack",      std::make_tuple(CMD_STACK,     0, "Print stack") },
	{ "flags",      std::make_tuple(CMD_FLAGS,     0, "Print CPU flags") },
	{ "mem",        std::make_tuple(CMD_MEMORY,    1, "Print value at a specified memory location") },
	{ "counters",   std::make_tuple(CMD_COUNTERS,  0, "Print CPU and machine counters") },
	{ "interrupts", std::make_tuple(CMD_INTS,      0, "Print enabled interrupts") },
	{ "track",      std::make_tuple(CMD_TRACK,     0, "Toggle instruction printing") },
	{ "break",      std::make_tuple(CMD_BREAK,     1, "Set breakpoint at <addr>") },
	{ "togglebp",   std::make_tuple(CMD_TOGGLEBP,  0, "Toggle breakpoints (speedup)") },
	{ "quit",       std::make_tuple(CMD_QUIT,      0, "Quit the debugger") },
	{ "q",          std::make_tuple(CMD_QUIT,      0, "Quit the debugger") },
	{ "exit",       std::make_tuple(CMD_QUIT,      0, "Quit the debugger") },
	{ "step",       std::make_tuple(CMD_STEP,      0, "Fetch and execute a single instruction") },
	{ "cont",       std::make_tuple(CMD_CONTINUE,  0, "Resume execution") },
	{ "continue",   std::make_tuple(CMD_CONTINUE,  0, "Resume execution") },
	{ "rominfo",    std::make_tuple(CMD_ROMINFO,   0, "Print ROM information") },
	{ "help",       std::make_tuple(CMD_HELP,      0, "Print a help message") },
	{ "dump",       std::make_tuple(CMD_DUMP,      1, "Dump instruction history to specified file") },
	{ "?",          std::make_tuple(CMD_HELP,      0, "Print a help message") }
};

static std::string debuggerHelp() {
	std::stringstream stream;
	for (const auto& pair : debugInstructions) {
		stream << std::left << std::setw(10) << pair.first << ": " << std::get<2>(pair.second) << "\r\n";
	}
	return stream.str();
}

Debugger::Debugger(Emulator *const _emulator, const uint8_t _opts) 
	: emulator(_emulator)
	, opts(_opts)
	, historySize(10)
{
	track = (_opts & DBG_TRACK) > 0;
}

Debugger::~Debugger() {}

void Debugger::Run() {
	emulator->cpu.paused = (opts & DBG_NOSTART) == DBG_NOSTART;
	if (!emulator->cpu.paused)
		emulator->init();

	// Trap SIGINT to pause the execution
	_debugger = this;
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	setup_signal_trap();
#elif _WIN32 || _WIN64
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)InterruptHandlerProxy, TRUE);
#endif

	std::deque<uint16_t> lastInstructions(historySize * 2);
	bool skipBreakpoints = false;
	bool useBreakpoints = true;
	bool pauseAtEnd = false;

	while (emulator->running) {
		emulator->CheckUpdate();

		if (emulator->cpu.paused && opts & DBG_INTERACTIVE) {
			DebugCmd cmd = getCommand("(mfemu)");
			switch (cmd.instr) {
			case CMD_RUN:
				std::clog << "Starting emulation..." << std::endl;
				skipBreakpoints = true;
				if (!emulator->isInit)
					emulator->init();
				emulator->cpu.paused = false;
				break;
			case CMD_TOGGLEBP:
				useBreakpoints = !useBreakpoints;
				std::clog << "Breakpoints are now " << (useBreakpoints ? "enabled" : "ignored") << std::endl;
				break;
			case CMD_PRINT: {
				uint16_t arg = emulator->cpu.PC;
				if (cmd.args.front().length() > 0) {
					std::stringstream ss(cmd.args.front());
					ss >> std::hex >> arg;
				} else {
					// Print history (up to 10)
					uint8_t maxBack = 10;
					if (lastInstructions.size() < 10) {
						maxBack = (uint8_t)lastInstructions.size();
					}
					for (uint8_t cur = 0; cur < maxBack; ++cur) {
						std::cout << "   ";
						printInstruction(lastInstructions[maxBack - cur - 1]);
					}
					std::cout << " > ";
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
				uint8_t value = emulator->mmu.Read(arg);
				std::cout
					<< std::dec << (int) value
					<< " - " << std::hex <<  "0x" << std::setfill('0') << std::setw(2) << (int) value
					<< " - " << std::bitset<8>(value) << std::endl;
				std::cout.flags(fmt);
				break;
			}
			case CMD_FLAGS:
				printFlags();
				break;
			case CMD_COUNTERS:
				printCounters();
				break;
			case CMD_INTS:
				printInterrupts();
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
				emulator->cpu.paused = false;
				pauseAtEnd = true;
				skipBreakpoints = true;
				if (!emulator->cpu.running) {
					std::cout << "WARNING: CPU is halted! Use \"run\" and trigger an interrupt." << std::endl;
				}

				break;
			case CMD_CONTINUE:
				if (!emulator->cpu.paused) {
					opts &= ~DBG_INTERACTIVE;
				} else {
					std::cerr << "CPU is not running: type `run` to start emulation." << std::endl;
				}
				break;
			case CMD_ROMINFO:
				emulator->rom.debugPrintData();
				break;
			case CMD_HELP:
				std::cout << debuggerHelp();
				break;
			case CMD_QUIT:
				std::clog << "...quitting." << std::endl;
				return;
			case CMD_DUMP: {
				std::string fname = "history_dump.txt";
				if (cmd.args.front().length() > 0) {
					fname = cmd.args.front();
				}
				std::ofstream historyFile(fname, std::ios::out);
				auto it = lastInstructions.rbegin();
				while (it != lastInstructions.rend()) {
					printInstruction(*it, historyFile);
					it++;
				}
				historyFile.close();
				std::clog << "Saved history dump to " << fname << std::endl;
				break;
			}
			default:
				std::cerr << "Invalid command" << std::endl;
			}

			continue;
		}

		// Execute instructions until a breakpoint is found
		uint16_t PC = emulator->cpu.PC;
		if (useBreakpoints && !skipBreakpoints && breakPoints.find(PC) != breakPoints.end()) {
			emulator->cpu.paused = true;
			std::ios::fmtflags fmt(std::cout.flags());
			std::cout << "Breakpoint reached: " << std::hex << (int)PC << std::endl;
			std::cout.flags(fmt);
			continue;
		}

		if (track || pauseAtEnd) {
			printInstruction(emulator->cpu.PC);
		}

		// Save instruction to history
		lastInstructions.push_front(emulator->cpu.PC);
		if (lastInstructions.size() > historySize * 2) {
			lastInstructions.resize(historySize);
		}

		emulator->Step();

		// ignoreBreakpoints should only last one iteration
		if (skipBreakpoints) {
			skipBreakpoints = false;
		}

		// Re-pause CPU after single step
		if (pauseAtEnd) {
			emulator->cpu.paused = true;
			pauseAtEnd = false;
		}
	}
}

// Reads a command from stdin and returns a struct { cmd, args }.
// Currently only takes 1 argument.
static DebugCmd getCommand(const char* prompt) {
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
		cmd.instr = std::get<0>(inst_pair->second);
		for (int i = 0; i < std::get<1>(inst_pair->second); i += 1) {
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
	std::ios::fmtflags fmt(std::clog.flags());
	if (breakPoints.find(addr) != breakPoints.end()) {
		breakPoints.erase(addr);
		std::clog << "Removed breakpoint to " << std::setfill('0') << std::setw(4) << std::hex << (int)addr << std::endl;
	} else {
		breakPoints.insert(addr);
		std::clog << "Set breakpoint to " << std::setfill('0') << std::setw(4) << std::hex << (int)addr << std::endl;
	}
	std::clog.flags(fmt);
}
