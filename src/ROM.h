#pragma once

#include <cstdint>

// Located on ROM - Addr 0143
// NOTE: This might be part of the title, treat carefully
enum GBCFlag : uint8_t {
	GBSupported = 0x80, //!< Supports colors, but works on older models
	GBCOnly = 0xc0      //!< Game requires Gameboy Color
};

// Located on ROM - Addr 0146
enum SGBFlag : uint8_t {
	NotSGB = 0x00, //!< GB/GBC-only game
	SGB = 0x03,    //!< SGB game
};

// Located on ROM - Addr 0147
enum ROMType : uint8_t {
	ROM_ONLY = 0x00,       //!< ROM Only (32kB)
	ROM_RAM = 0x08,        //!< ROM + RAM
	ROM_R_B = 0x09,        //!< ROM + RAM + Battery
	ROM_MBC1 = 0x01,       //!< Memory Bank Controller 1 (16/4Mbit)
	ROM_MBC1_RAM = 0x02,   //!< MBC1 + RAM
	ROM_MBC1_R_B = 0x03,   //!< MBC1 + RAM + Battery
	ROM_MBC2 = 0x04,       //!< Memory Bank Controller 2 (<= 2Mbit)
	ROM_MBC2_BAT = 0x05,   //!< MBC2 + Battery
	ROM_MBC3 = 0x11,       //!< Memory Bank Controller 3 (16Mbit)
	ROM_MBC3_RAM = 0x12,   //!< MBC3 + RAM
	ROM_MBC3_R_B = 0x13,   //!< MBC3 + RAM + Battery
	ROM_MBC3_T_B = 0x0f,   //!< MBC3 + Timer + Battery
	ROM_MBC3_T_R_B = 0x10, //!< MBC3 + Timer + RAM + Battery
	ROM_MBC4 = 0x15,       //!< Memory Bank Controller 4 (???)
	ROM_MBC4_RAM = 0x16,   //!< MBC4 + RAM
	ROM_MBC4_R_B = 0x17,   //!< MBC4 + RAM + Battery
	ROM_MBC5 = 0x19,       //!< Memory Bank Controller 5 (64Mbit)
	ROM_MBC5_RAM = 0x1A,   //!< MBC5 + RAM
	ROM_MBC5_R_B = 0x1B,   //!< MBC5 + RAM + Battery
	ROM_MMM1 = 0x0b,       //!< no idea
	ROM_MMM1_RAM = 0x0c,   //!< MMM1 + RAM
	ROM_MMM1_R_B = 0x0d,   //!< MMM1 + RAM + Battery
	ROM_CAMERA = 0xfc,     //!< Pocket Camera
	ROM_TAMA5 = 0xfd,      //!< Bandai TAMA5
	ROM_HUC3 = 0xfe,       //!< (???)
	ROM_HUC1_R_B = 0xff    //!< Hudson Soft Infrared MBC1
};

// Located on ROM - Addr 0148
enum ROMSize : uint8_t {
	ROM_32K = 0x00,  //!<  32kB,  no banks
	ROM_64K = 0x01,  //!<  64kB,   4 banks
	ROM_128K = 0x02, //!< 128kB,   8 banks
	ROM_256K = 0x03, //!< 256kB,  16 banks
	ROM_512K = 0x04, //!< 512kB,  32 banks
	ROM_1M = 0x05,   //!<   1MB,  64 banks (only 63 used in MBC1)
	ROM_2M = 0x06,   //!<   2MB, 128 banks (only 125 used in MBC1)
	ROM_4M = 0x07,   //!<   4MB, 256 banks
	ROM_1_1M = 0x52, //!< 1.1MB,  72 banks
	ROM_1_2M = 0x53, //!< 1.2MB,  80 banks
	ROM_1_5M = 0x54  //!< 1.5MB,  96 banks
};

// Located on ROM - Addr 0149
enum RAMSize : uint8_t {
	RAM_NONE = 0x00, //!<   No RAM
	RAM_2KB = 0x01,  //!<  2kB RAM
	RAM_8KB = 0x02,  //!<  8kB RAM
	RAM_32KB = 0x03, //!< 32kB RAM (4 banks of 8kB)
};

// Located on ROM - Addr 014a
enum DestCode : uint8_t {
	Japanese = 0x00,   //!< Japanese game
	NonJapanese = 0x01 //!< Non-Japanese game
};

struct ROMHeader {
	uint8_t entryPoint[0x04];     // 0100 - 0103
	uint8_t nintendoLogo[0x30];   // 0104 - 0133
	union {
		char GBTitle[0x10];       // 0134 - 0143
		struct {
			char GBCTitle[0x0b];  // 0134 - 013e
			char manCode[0x04];   // 013f - 0142
			GBCFlag GBCFlag;      // 0143
		};
	};
	char newLicenseeCode[0x02];   // 0144 - 0145
	SGBFlag SGBFlag;              // 0146
	ROMType ROMType;              // 0147
	ROMSize ROMSize;              // 0148
	RAMSize RAMSize;              // 0149
	DestCode destinationCode;     // 014a
	uint8_t oldLicenseeCode;      // 014b (watch out for 0x33)
	uint8_t maskROMVersion;       // 014c
	uint8_t headerChecksum;       // 014d        (to verify)
	uint8_t globalChecksum[0x02]; // 014e - 014f (not verified)
};

struct ROMFile {
	uint8_t   start[0x100];
	ROMHeader header;
};