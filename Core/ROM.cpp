#include "ROM.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <cstring>
#include <map>

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
	static const std::map<uint8_t, std::string> RomType = {
		{0x00, "ROM only"},
		{0x08, "ROM + RAM"},
		{0x09, "ROM + RAM + Battery"},
		{0x01, "ROM + MBC1"},
		{0x02, "ROM + MBC1 + RAM"},
		{0x03, "ROM + MBC1 + RAM + Battery"},
		{0x05, "ROM + MBC2"},
		{0x06, "ROM + MBC2 + RAM + Battery"},
		{0x11, "ROM + MBC3"},
		{0x12, "ROM + MBC3 + RAM"},
		{0x13, "ROM + MBC3 + RAM + Battery"},
		{0x0f, "ROM + MBC3 + Timer + Battery"},
		{0x10, "ROM + MBC3 + Timer + RAM + Battery"},
		{0x19, "ROM + MBC5"},
		{0x1a, "ROM + MBC5 + RAM"},
		{0x1b, "ROM + MBC5 + RAM + Battery"},
		{0x1c, "ROM + MBC5 + Rumble"},
		{0x1d, "ROM + MBC5 + RAM + Rumble"},
		{0x1e, "ROM + MBC5 + RAM + Rumble + Battery"},
		{0x20, "ROM + MBC6 + RAM + Battery"},
		{0x22, "ROM + MBC7 + RAM + Battery + Accelerometer"},
		{0x0b, "ROM + MMM1"},
		{0x0c, "ROM + MMM1 + RAM"},
		{0x0d, "ROM + MMM1 + RAM + Battery"},
		{0xfc, "ROM + Camera"},
		{0xfd, "ROM + TAMA5"},
		{0xfe, "ROM + HUC3"},
		{0xff, "ROM + HUC1 + RAM + Battery"}
	};

	static const std::string RomSize[] = {
		"32 kB, 2 banks",
		"64 kB, 4 banks",
		"128 kB, 8 banks",
		"256 kB, 16 banks",
		"512 kB, 32 banks",
		"1 MB, 64 banks",
		"2 MB, 128 banks",
		"4 MB, 256 banks",
		"8 MB, 512 banks"
	};

	static const std::string RamSize[] = {
		"None",
		"2 kB, 1 bank",
		"8 kB, 1 bank",
		"32 kB, 4 banks",
		"128 kB, 16 banks",
		"64 kB, 8 banks"
	};

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
	std::cout << "ROM Type: " << RomType.at(header.Type) << " (" << std::hex << (int)header.Type << ")" << std::endl;
	std::cout << "ROM Size: " << RomSize[(int)header.ROMSize] << " (" << std::hex << (int)header.ROMSize << ")" << std::endl;
	std::cout << "RAM Size: " << RamSize[(int)header.RAMSize] << " (" << std::hex << (int)header.RAMSize << ")" << std::endl;

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
