#include <iostream>
#include "Emulator.h"
#include "Debugger.h"

enum MainFlags : uint8_t {
	F_DEFAULT      = 1,
	F_ROMINFO      = 1 << 1,
	F_DEBUG        = 1 << 2,
	F_DEBUGVERBOSE = 1 << 3
};

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;

	std::string romFile("test.gb");
	uint8_t flags(F_DEFAULT);

	for (int i = 1; i < argc; i += 1) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'v':
				return 0; // we already printed the version
			case 'i':
				// only print ROM information and exit
				flags = F_ROMINFO;
				break;
			case 'd':
				flags = F_DEBUG;
				break;
			case 'D':
				flags = F_DEBUG | F_DEBUGVERBOSE;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-hivd] <file.gb>" << std::endl;
				std::cout << "\t-h: get this help" << std::endl;
				std::cout << "\t-v: print version and exit" << std::endl;
				std::cout << "\t-i: print ROM info and exit" << std::endl;
				std::cout << "\t-d: run the debugger on the given rom" << std::endl;
				std::cout << "\t-D: run the debugger in verbose mode (implies -d)" << std::endl;
				return 0;
			}
		} else {
			romFile = std::string(argv[i]);
		}
	}

	if (flags & F_ROMINFO) {
		ROM rom = ROM::FromFile(romFile);
		rom.debugPrintData();
		return 0;
	}

	if (flags & F_DEBUG) {
		Emulator emulator(romFile, false);
		uint8_t debugger_flags = Debug::DBG_NOGRAPHICS;
		if (flags & F_DEBUGVERBOSE)
			debugger_flags |= Debug::DBG_PRINTINSTR;
		Debugger debugger(&emulator, debugger_flags);
		debugger.Run();
	} else {
		// Create CPU and load ROM into it
		Emulator emulator(romFile);
		std::cout << "Starting emulation..." << std::endl;
		emulator.Run();
	}
}
