#pragma once

#include <vector>
#include <cstdint>

#include "ROM.Data.h"

//! Single ROM Bank, holding 16KB of code/data
struct ROMBank {
	uint8_t bytes[16 * 1024];
};

//! Single RAM Bank, holding 8KB of data
struct RAMBank {
	uint8_t bytes[8 * 1024];
};

class MBC {
protected:
	std::vector<ROMBank> banks; //!< Switchable bank     (0000-7fff)
	std::vector<RAMBank> ram;   //!< Switchable RAM bank (a000-bfff)
	uint8_t romBankId = 0;      //!< Current ROM bank id
	uint8_t ramBankId = 0;      //!< Current RAM bank id

	bool hasRam = false;        //!< Enabled if the ROM has RAM
	bool hasBattery = false;    //!< Enabled if the ROM has battery

	bool ramEnabled = false;    //!< RAM Enable flag (MBC1+)

	ROMHeader header;           //!< ROM Header

public:
	virtual ~MBC() {}

	//! Read from ROM (checks for MBC, RAM etc.)
	virtual uint8_t Read(const uint16_t) const = 0;

	//! Write to MBC or special registers
	virtual void Write(const uint16_t, const uint8_t) = 0;

	//! Create the required banks and fill them with ROM data
	void LoadROM(const ROMHeader& header, const std::vector<uint8_t>& data);
};

class NoMBC : public MBC {
public:
	uint8_t Read(const uint16_t location) const override;
	void Write(const uint16_t location, const uint8_t value) override;

	NoMBC(const ROMType type);
};

class MBC1 : public MBC {
private:
	bool ramBankingEnable = false;

public:
	uint8_t Read(const uint16_t location) const override;
	void Write(const uint16_t location, const uint8_t value) override;

	MBC1(const ROMType type);
};

class MBC3: public MBC {public:
	uint8_t Read(const uint16_t location) const override;
	void Write(const uint16_t location, const uint8_t value) override;

	MBC3(const ROMType type);
};
