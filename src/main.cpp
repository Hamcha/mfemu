#include <iostream>
#include <cstring>
#include "Emulator.h"
#include "Debugger.h"

enum MainFlags : uint8_t {
	F_DEFAULT = 1,
	F_ROMINFO = 1 << 1,
	F_DEBUG   = 1 << 2,
	F_NOSTART = 1 << 3,
	F_TRACK   = 1 << 4
};

int main(int argc, char **argv) {
	std::cout << "mfemu v." << VERSION << " rev." << COMMIT << std::endl << std::endl;

	std::string romFile("test.gb");
	uint8_t flags = F_DEFAULT;

	EmulatorFlags emulatorFlags;
	int queueSize = 10;

	for (uint16_t i = 1; i < argc; i += 1) {
		if (argv[i][0] == '-') {
			uint16_t j = 1, len = strlen(argv[i]);
			do {
				switch (argv[i][j]) {
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
					flags |= F_NOSTART | F_DEBUG;
					break;
				case 't':
					flags |= F_TRACK;
					break;
				case 'b':
					emulatorFlags.useBootrom = false;
					break;
				case 's': {
					int scale = atoi(argv[i + 1]);
					if (scale < 1) {
						std::cout << "Invalid scale value provided (not an integer or less than 1)" << std::endl;
						return 1;
					}
					emulatorFlags.scale = scale;
					i += 1;
					break;
				}
				case 'q': {
					int size = atoi(argv[i + 1]);
					if (size < 1) {
						std::cout << "Invalid size value provided (not an integer or less than 1)" << std::endl;
						return 1;
					}
					queueSize = size;
					i += 1;
					break;
				}
				default:
					std::cout << "Usage: " << argv[0] << " [flags] <file.gb>\r\n"
						<< "\r\nOptions are listed below:\r\n"
						<< "\t-h   : get this help\r\n"
						<< "\t-v   : print version and exit\r\n"
						<< "\t-i   : print ROM info and exit\r\n"
						<< "\t-d   : run the debugger on the given rom\r\n"
						<< "\t-t   : start with code printing enabled (requires -d)\r\n"
						<< "\t-n   : don't start the emulation right away (implies -d)\r\n"
						<< "\t-s X : scale window X times the Game Boy resolution\r\n"
						<< "\t-q X : save up to X elements in the instruction history (required -d)\r\n"
						<< "\t-b   : skip the DMG boot rom [experimental]\r\n" << std::endl;
					return 0;
				}
			} while (++j < len);
		} else {
			romFile = std::string(argv[i]);
		}
	}

	if (flags & F_ROMINFO) {
		ROM rom = ROM::FromFile(romFile);
		rom.debugPrintData();
		return 0;
	}

	// Check if using debugger flags without the debugger
	if (!(flags & F_DEBUG) && (flags & F_TRACK)) {
		std::cout << "[WARNING] Using debugger flags without the debugger, they will be ignored\r\n";
	}

	Emulator emulator(romFile, emulatorFlags);

	if (flags & F_DEBUG) {
		uint8_t debuggerFlags = Debug::DBG_INTERACTIVE;
		if (flags & F_NOSTART) {
			debuggerFlags |= Debug::DBG_NOSTART;
		}
		if (flags & F_TRACK) {
			debuggerFlags |= Debug::DBG_TRACK;
		}
		Debugger debugger(&emulator, debuggerFlags);
		debugger.historySize = queueSize;
		debugger.Run();
	} else {
		// Create CPU and load ROM into it
		std::cout << "Starting emulation..." << std::endl;
		emulator.Run();
	}

	return 0;
}
