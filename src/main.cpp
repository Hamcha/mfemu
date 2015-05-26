#include <iostream>
#include "Emulator.h"
#include "Debugger.h"

enum MainFlags {
	F_DEFAULT,
	F_ROMINFO,
	F_DEBUG
};

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;

	std::string romFile("test.gb");
	MainFlags flags(F_DEFAULT);

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
				std::cout << "Usage: " << argv[0] << " [-hivd] [file.gb]" << std::endl;
				return 0;
			}
		}
		romFile = std::string(argv[i]);
	}

	if (flags == F_ROMINFO) {
		ROM rom = ROM::FromFile(romFile);
		rom.debugPrintData();
		return 0;
	}

	if (flags == F_DEBUG) {
		Emulator emulator(romFile, false);
		Debugger debugger(&emulator, Debug::DBG_NOGRAPHICS);	
		debugger.Run();
	} else {
		// Create CPU and load ROM into it
		Emulator emulator(romFile);
		std::cout << "Starting emulation..." << std::endl;
		emulator.Run();
	}
}
