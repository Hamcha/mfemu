#include <iostream>
#include "ROM.h"
#include "CPU.h"
#define DEBUG 1

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;
	std::string romFile = "test.gb";
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'v':
				return 0; // we already printed the version
			default:
				std::cout << "Usage: " << argv[0] << " [-h] [file.gb]" << std::endl;
				return 0;
			}
		}
		romFile = std::string(argv[i]);
	}
	ROM file = ROM::FromFile(romFile);

	// The title can be either 15 or 13 characters, depending on target console
	std::string title;
	if (file.header.colorFlag == GBSupported || file.header.colorFlag == GBCOnly) {
		title = std::string(file.header.GBCTitle);
	} else {
		title = std::string(file.header.GBTitle);
	}

	std::cout << "Loaded ROM: " << title << std::endl;
#if DEBUG
	file.debugPrintData();
#endif

	// Create CPU and load ROM into it
	CPU emulator(&file);

	std::cout << "Starting emulation..." << std::endl;

	while(emulator.running) {
		emulator.Step();
		/*if (emulator.cycles.cpu > 10000) {
			std::cout << "CPU has been running for more than 10000 cycles, halting" << std::endl;
			break;
		}*/
	}

	std::cout << "CPU Halted" << std::endl;
}
