#include "ROM.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <cstring>

// From https://stackoverflow.com/questions/15138353/reading-the-binary-file-into-the-vector-of-unsigned-chars
ROM ROM::FromFile(const std::string& filename) {
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

	return ROM(bytes);
}

ROM::ROM(const std::vector<uint8_t>& bytes) {
	// Get header bytes from buffer
	const int headerSize = sizeof(ROMHeader);
	uint8_t headerBytes[headerSize];
	std::copy(bytes.begin() + 0x100, bytes.begin() + 0x100 + headerSize, headerBytes);

	memcpy(&header, headerBytes, headerSize);

	// Create ROM MBC (Memory Bank Controller) from type
	switch (header.Type) {
	case ROM_ONLY:
	case ROM_RAM:
	case ROM_R_B:
		controller = new NoMBC(header.Type);
		break;
	case ROM_MBC1:
	case ROM_MBC1_RAM:
	case ROM_MBC1_R_B:
		controller = new MBC1(header.Type);
		break;
	case ROM_MBC3:
	case ROM_MBC3_RAM:
	case ROM_MBC3_R_B:
		controller = new MBC3(header.Type);
		break;
	default:
		throw std::logic_error("Unsupported MBC type");
	}

	// Load content to ROM banks
	controller->LoadROM(header, bytes);

	//TODO load from .sav to RAM

	// The title can be either 15 or 13 characters, depending on target console
	std::string title;
	if (header.GBC.colorFlag == GBSupported || header.GBC.colorFlag == GBCOnly) {
		title = std::string(header.GBC.title);
	} else {
		title = std::string(header.GBTitle);
	}

	std::cout << "Loaded ROM: " << title << std::endl;
}

ROM::~ROM() {
	delete controller;
}

void ROM::debugPrintData() const {
	std::cout << "== ROM INFO ==" << std::endl;
	// The title can be either 15 or 13 characters, depending on target console
	if (header.GBC.colorFlag == GBSupported || header.GBC.colorFlag == GBCOnly) {
		std::cout << "Title (GBC format): " << std::string(header.GBC.title) << std::endl;
		std::cout << "Manufacturer Code: " << std::string(header.GBC.manCode) << std::endl;
	} else {
		std::cout << "Title (GB format): " << std::string(header.GBTitle) << std::endl;
	}

	// GBC features support
	std::string gbFlag = "GBC not supported";
	if (header.GBC.colorFlag == GBSupported) gbFlag = "GBC features supported";
	if (header.GBC.colorFlag == GBCOnly) gbFlag = "GBC Only";
	std::cout << "GBC support: " << gbFlag << std::endl;

	// SGB features support
	std::string super = "SGB not supported";
	if (header.superFlag == SGB) super = "SGB features supported";
	std::cout << "SGB support: " << super << std::endl;

	// ROM/RAM types
	std::cout << "ROM Type: " << std::hex << header.Type << std::endl;
	std::cout << "ROM Size: " << std::hex << header.ROMSize << std::endl;
	std::cout << "RAM Size: " << std::hex << header.RAMSize << std::endl;

	// Manufacturer code(s)
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
