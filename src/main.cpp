#include <iostream>
#include "ROM.h"
#include "CPU.h"
#define DEBUG 1

int main() {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;
	ROM file = ROM::FromFile("test.gb");

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
		if (emulator.cycles.cpu > 10000) {
			std::cout << "CPU has been running for more than 10000 cycles, halting" << std::endl;
			break;
		}
	}

	std::cout << "CPU Halted" << std::endl;
}
