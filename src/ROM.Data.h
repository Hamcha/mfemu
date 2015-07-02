#pragma once

#include <cstdint>

//! Gameboy Color flag, overlaps title
enum GBCFlag : uint8_t {
	GBSupported = 0x80, //!< Supports colors, but works on older models
	GBCOnly = 0xc0      //!< Game requires Gameboy Color
};

//! Super Gameboy flag
enum SGBFlag : uint8_t {
	NotSGB = 0x00, //!< GB/GBC-only game
	SGB = 0x03,    //!< SGB game
};

//! ROM Type flag
enum ROMType : uint8_t {
	ROM_ONLY = 0x00,       //!< ROM Only (32kB)
	ROM_RAM = 0x08,        //!< ROM + RAM
	ROM_R_B = 0x09,        //!< ROM + RAM + Battery
	ROM_MBC1 = 0x01,       //!< Memory Bank Controller 1 (16/4Mbit)
	ROM_MBC1_RAM = 0x02,   //!< MBC1 + RAM
	ROM_MBC1_R_B = 0x03,   //!< MBC1 + RAM + Battery
	ROM_MBC2 = 0x05,       //!< Memory Bank Controller 2 (<= 2Mbit)
	ROM_MBC2_R_B = 0x06,   //!< MBC2 + RAM + Battery
	ROM_MBC3 = 0x11,       //!< Memory Bank Controller 3 (16Mbit)
	ROM_MBC3_RAM = 0x12,   //!< MBC3 + RAM
	ROM_MBC3_R_B = 0x13,   //!< MBC3 + RAM + Battery
	ROM_MBC3_T_B = 0x0f,   //!< MBC3 + Timer + Battery
	ROM_MBC3_T_R_B = 0x10, //!< MBC3 + Timer + RAM + Battery
	ROM_MBC5 = 0x19,       //!< Memory Bank Controller 5 (64Mbit)
	ROM_MBC5_RAM = 0x1a,   //!< MBC5 + RAM
	ROM_MBC5_R_B = 0x1b,   //!< MBC5 + RAM + Battery
	ROM_MBC5_RMB = 0x1c,   //!< MBC5 + Rumble
	ROM_MBC5_R_R = 0x1d,   //!< MBC5 + Rumble + RAM
	ROM_MBC5_R_R_B = 0x1e, //!< MBC5 + Rumble + RAM + Battery
	ROM_MBC6_R_B = 0x20,   //!< MBC6 + RAM + Battery
	ROM_MBC7_R_B_A = 0x22, //!< MBC7 + RAM + Battery + Accelerometer
	ROM_MMM1 = 0x0b,       //!< no idea
	ROM_MMM1_RAM = 0x0c,   //!< MMM1 + RAM
	ROM_MMM1_R_B = 0x0d,   //!< MMM1 + RAM + Battery
	ROM_CAMERA = 0xfc,     //!< Pocket Camera
	ROM_TAMA5 = 0xfd,      //!< Bandai TAMA5
	ROM_HUC3 = 0xfe,       //!< (???)
	ROM_HUC1_R_B = 0xff    //!< Hudson Soft Infrared MBC1
};

//! ROM Size flag
enum ROMSizeType : uint8_t {
	ROM_32K = 0x00,  //!<  32kB,   2 banks
	ROM_64K = 0x01,  //!<  64kB,   4 banks
	ROM_128K = 0x02, //!< 128kB,   8 banks
	ROM_256K = 0x03, //!< 256kB,  16 banks
	ROM_512K = 0x04, //!< 512kB,  32 banks
	ROM_1M = 0x05,   //!<   1MB,  64 banks (only 63 used in MBC1)
	ROM_2M = 0x06,   //!<   2MB, 128 banks (only 125 used in MBC1)
	ROM_4M = 0x07,   //!<   4MB, 256 banks
	ROM_8M = 0x08    //!<   8MB, 512 banks
};

//! RAM Size flag
enum RAMSizeType : uint8_t {
	RAM_NONE = 0x00, //!<   No RAM
	RAM_2KB = 0x01,  //!<  2kB RAM
	RAM_8KB = 0x02,  //!<  8kB RAM (1 bank)
	RAM_32KB = 0x03, //!< 32kB RAM (4 banks of 8kB)
	RAM_64KB = 0x05, //!< 64kB RAM (8 banks of 8kB)
	RAM_128KB = 0x04 //!< 128kB RAM (16 banks of 8kB)
};

//! Destination Code
enum DestCode : uint8_t {
	Japanese = 0x00,   //!< Japanese game
	NonJapanese = 0x01 //!< Non-Japanese game
};

//! ROM Header, located in the 0100 - 014f zone
struct ROMHeader {
	uint8_t entryPoint[0x04];     //!< Entry point                    (0100 - 0103)
	uint8_t nintendoLogo[0x30];   //!< Nintendo's boot logo (checked) (0104 - 0133)
	union {
		char GBTitle[0x10];       //!< Gameboy title, 15 characters   (0134 - 0143)
		struct {                  // or
			char title[0x0b];     //!< GBC title, 11 characters       (0134 - 013e)
			char manCode[0x04];   //!< Manufacturer Code, 3 chars     (013f - 0142)
			GBCFlag colorFlag;    //!< Gameboy Color flag             (0143)
		} GBC;
	};
	char newLicenseeCode[0x02];   //!< New licensee code, 2 chars     (0144 - 0145)
	SGBFlag superFlag;            //!< Super Gameboy flag             (0146)
	ROMType Type;                 //!< ROM Controller type            (0147)
	ROMSizeType ROMSize;          //!< ROM Total Size                 (0148)
	RAMSizeType RAMSize;          //!< RAM Total Size                 (0149)
	DestCode destinationCode;     //!< Destination Code (region)      (014a)
	uint8_t oldLicenseeCode;      //!< Old licensee code, 1 byte      (014b) (watch out for 0x33)
	uint8_t maskROMVersion;       //!< Mask ROM version               (014c)
	uint8_t headerChecksum;       //!< Header checksum (verified)     (014d)
	uint8_t globalChecksum[0x02]; //!< Global ROM checksum (ignored)  (014e - 014f)
};
