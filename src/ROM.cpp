#include "ROM.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <cstring>

// From https://stackoverflow.com/questions/15138353/reading-the-binary-file-into-the-vector-of-unsigned-chars
ROM ROM::FromFile(const std::string filename) {
	// Make file stream
	std::ifstream file(filename, std::ios::binary);
	if (!file.good()) {
		throw std::runtime_error("Bad Stream Status: does the file exist?");
	}
	file.unsetf(std::ios::skipws);

	// Get file size
	std::streampos fileSize;
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> bytes;
	bytes.reserve(fileSize);
	bytes.insert(bytes.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());

	ROM rom(bytes);
	return rom;
}

ROM::ROM(const std::vector<uint8_t> bytes) {
	// Get header bytes from buffer
	const int headerSize = sizeof(ROMHeader);
	uint8_t headerBytes[headerSize];
	std::copy(bytes.begin() + 0x100, bytes.begin() + 0x100 + headerSize, headerBytes);

	memcpy(&header, headerBytes, headerSize);

	// Get banks from buffer
	int bankCount;
	switch (header.ROMSize) {
	case ROM_32K:  bankCount = 0; break;
	case ROM_64K:  bankCount = 4; break;
	case ROM_128K: bankCount = 8; break;
	case ROM_256K: bankCount = 16; break;
	case ROM_512K: bankCount = 32; break;
	case ROM_1M:   bankCount = header.Type == ROM_MBC1 ? 63 : 64; break;
	case ROM_2M:   bankCount = header.Type == ROM_MBC1 ? 125 : 128; break;
	case ROM_4M:   bankCount = 256; break;
	case ROM_1_1M: bankCount = 72; break;
	case ROM_1_2M: bankCount = 80; break;
	case ROM_1_5M: bankCount = 96; break;
	default:       throw std::runtime_error("Erroneous ROM Type");
	}

	// Read fixed 16k from buffer
	std::copy(bytes.begin(), bytes.begin() + 16 * 1024, fixed.bytes);

	// Read banks from buffer
	banks.reserve(bankCount);
	for (int i = 1; i < bankCount; i++) {
		ROMBank b;
		std::copy(bytes.begin() + 16 * 1024 * i, bytes.begin() + 16 * 1024 * (i + 1), b.bytes);
		banks.push_back(b);
	}

	// Setup RAM banks
	int ramcount;
	switch (header.RAMSize) {
	case RAM_NONE: ramcount = 0; break;
	case RAM_2KB: case RAM_8KB: ramcount = 1; break;
	case RAM_32KB: ramcount = 4; break;
	}
	ram.reserve(ramcount);

	//TODO load from .sav to RAM
}

ROM::~ROM() {}

void ROM::debugPrintData() {
	std::cout << "== ROM INFO ==" << std::endl;
	// The title can be either 15 or 13 characters, depending on target console
	if (header.colorFlag == GBSupported || header.colorFlag == GBCOnly) {
		std::cout << "Title (GBC format): " << std::string(header.GBCTitle) << std::endl;
		std::cout << "Manifacturer Code: " << std::string(header.manCode) << std::endl;
	} else {
		std::cout << "Title (GB format): " << std::string(header.GBTitle) << std::endl;
	}

	// GBC features support
	std::string gbFlag = "GBC not supported";
	if (header.colorFlag == GBSupported) gbFlag = "GBC features supported";
	if (header.colorFlag == GBCOnly) gbFlag = "GBC Only";
	std::cout << "GBC support: " << gbFlag << std::endl;

	// SGB features support
	std::string super = "SGB not supported";
	if (header.superFlag == SGB) super = "SGB features supported";
	std::cout << "SGB support: " << super << std::endl;

	// ROM/RAM types
	std::cout << "ROM Type: " << std::hex << header.Type << std::endl;
	std::cout << "ROM Size: " << std::hex << header.ROMSize << std::endl;
	std::cout << "RAM Size: " << std::hex << header.RAMSize << std::endl;

	// Manifacturer code(s)
	if (header.oldLicenseeCode != 0x33) {
		std::cout << "Licensee code (old): " << std::hex << (int) header.oldLicenseeCode << std::endl;
	} else {
		std::cout << "Licensee code (new): " << header.newLicenseeCode[0] << header.newLicenseeCode[1] << std::endl;
	}

	// Destination code and other infos
	std::cout << "Destination code: " << (header.destinationCode == Japanese ? "Japan Only" : "Non-Japanese") << std::endl;
	std::cout << "Mask ROM version: " << (int) header.maskROMVersion << std::endl;

	std::cout << "== END ROM INFO ==" << std::endl;
}
