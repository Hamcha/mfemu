#include <iostream>
#include "Emulator.h"

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;
	std::string romFile = "test.gb";
	for (int i = 1; i < argc; i += 1) {
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

	// Create CPU and load ROM into it
	Emulator emulator(romFile);

	std::cout << "Starting emulation..." << std::endl;

	emulator.Run();
}
