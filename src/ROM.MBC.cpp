#include "ROM.h"
#include <stdexcept>

uint8_t ROM::Read(const uint16_t location) const {
    // Fixed bank
    if (location < 0x4000) {
        return fixed.bytes[location];
    }
    // Variable bank
    if (location < 0x8000) {
        return banks[romBankId].bytes[location - 0x4000];
    }
    // Not applicable (used by VRAM)
    if (location < 0xa000) {
        throw std::logic_error("Trying to access VRAM in ROM");
        return 0;
    }
    // External RAM (on cartridge)
    if (location < 0xc000) {
        ram[ramBankId].bytes[location - 0xa000];
    }

    throw std::logic_error("Trying to access non-ROM memory in ROM");
    return 0;
}

void ROM::Write(const uint16_t location, const uint8_t value) {
    // ROM (probably MBC registers)
    if (location < 0x8000) {
        //TODO Handle MBC Registers
        return;
    }
    // Not applicable (used by VRAM)
    if (location < 0xa000) {
        throw std::logic_error("Trying to write to VRAM in ROM");
        return;
    }
    // External RAM (on cartridge)
    if (location < 0xc000) {
        ram[ramBankId].bytes[location - 0xa000] = value;
        return;
    }

    throw std::logic_error("Trying to write non-ROM in ROM");
}
