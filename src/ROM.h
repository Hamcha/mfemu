#pragma once

#include <string>
#include <vector>

#include "ROM.Data.h"
#include "MBC.h"

//! ROM class
//! Loads and allows access to a ROM data and RAM banks
class ROM {
public:
	ROMHeader header; //!< ROM Header, extracted from the opened ROM
	MBC* controller;  //!< ROM Controller, used for IO access

	//! Load ROM from file
	static ROM FromFile(const std::string& filename);

	//! Load ROM from memory
	explicit ROM(const std::vector<uint8_t>& bytes);

	//! Print ROM data (for debugging)
	void debugPrintData() const;

	~ROM();
};
