#include <iostream>
#include "Emulator.h"
#include "Debugger.h"

enum MainFlags : uint8_t {
	F_DEFAULT      = 1,
	F_ROMINFO      = 1 << 1,
	F_DEBUG        = 1 << 2,
	F_NOSTART      = 1 << 3
};

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;

	std::string romFile("test.gb");
	uint8_t flags = F_DEFAULT;

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
				flags |= F_DEBUG;
				break;
			case 'n':
				flags |= F_NOSTART;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-hvidn] <file.gb>\r\n"
					<< "\t-h: get this help\r\n"
					<< "\t-v: print version and exit\r\n"
					<< "\t-i: print ROM info and exit\r\n"
					<< "\t-d: run the debugger on the given rom\r\n"
					<< "\t-n: don't start the emulation right away" << std::endl;
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

	Emulator emulator(romFile);

	if (flags & F_DEBUG) {
		uint8_t debugger_flags = Debug::DBG_INTERACTIVE;
		if (flags & F_NOSTART) {
			debugger_flags |= Debug::DBG_NOSTART;
		}
		Debugger debugger(&emulator, debugger_flags);
		debugger.Run();
	} else {
		// Create CPU and load ROM into it
		std::cout << "Starting emulation..." << std::endl;
		emulator.Run();
	}

	return 0;
}
