#include <iostream>
#include "Emulator.h"
#include "Debugger.h"

enum MainFlags : uint8_t {
	F_DEFAULT      = 1,
	F_ROMINFO      = 1 << 1,
	F_DEBUG        = 1 << 2
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
			default:
				std::cout << "Usage: " << argv[0] << " [-hivd] <file.gb>" << std::endl
					<< "\t-h: get this help" << std::endl
					<< "\t-v: print version and exit" << std::endl
					<< "\t-i: print ROM info and exit" << std::endl
					<< "\t-d: run the debugger on the given rom" << std::endl;
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
		Debugger debugger(&emulator, debugger_flags);
		debugger.Run();
	} else {
		// Create CPU and load ROM into it
		Emulator emulator(romFile);
		std::cout << "Starting emulation..." << std::endl;
		emulator.Run();
	}

	return 0;
}
