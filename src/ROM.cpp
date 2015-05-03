#include "ROM.h"
#include <fstream>

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
	// Move bytes to internal buffer
	raw = bytes;

	// Get header bytes from buffer
	const int headerSize = sizeof(ROMHeader);
	uint8_t headerBytes[headerSize];

	for (int i = 0; i < headerSize; i++) {
		headerBytes[i] = raw[0x100 + i];
	}

	memcpy(&header, headerBytes, headerSize);
}

ROM::~ROM() {
}