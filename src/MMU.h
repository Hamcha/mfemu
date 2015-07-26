#pragma once

#include "ROM.h"
#include "GPU.h"

struct CycleCount {
	uint64_t machine, cpu;

	CycleCount(const uint64_t m, const uint64_t c) {
		machine = m; cpu = c;
	}
	void add(const uint64_t m, const uint64_t c) {
		machine += m; cpu += c;
	}
	void add(const CycleCount c) {
		add(c.machine, c.cpu);
	}
};

struct WRAMBank {
	uint8_t bytes[4 * 1024];
};

struct ZRAMBank {
	uint8_t bytes[128];
};

/*! \brief Timer clock
 *
 *  All available settings for the controllable timer's clock speed
 */
enum TimerClock : uint8_t {
	ClockDiv1024 = 0, //!< One tick every 1024 CPU clocks
	ClockDiv16 = 1,   //!< One tick every 16   CPU clocks
	ClockDiv64 = 2,   //!< One tick every 64   CPU clocks
	ClockDiv256 = 3   //!< One tick every 256  CPU clocks
};

/*! \brief Timer control register
 *
 *  Register that controls how the controllable timer works, if it's enabled
 *  and how quickly it increases.
 */
union TimerControl {
	uint8_t raw;
	struct Values {
		TimerClock clock : 2; //!< Controllable timer's clock
		uint8_t  enabled : 1; //!< Start/Stops the timer
	} values;
};

/*! \brief Memory management unit
 *
 *  MMU manages memory and access to it, it keeps track of the timers and
 *  parses 16 bit memory locations to where they are (ROM/RAM/VRAM/etc).
 */
class MMU {
private:
	ROM* rom;                        //!< ROM instance

	WRAMBank WRAM;                   //!< Internal RAM (bank 0)
	std::vector<WRAMBank> WRAMbanks; //!< Internal RAM extra banks
	uint8_t WRAMbankId = 0;          //!< Internal RAM currnet bank id

	ZRAMBank ZRAM;                   //!< Zero page RAM (128 bytes)

	uint8_t dividerRest;             //!< Extra cycles for the divider timer
	uint16_t counterRest;            //!< Extra cycles for the controllable timer

	uint8_t readIO(const uint16_t location);
	void writeIO(const uint16_t location, const uint8_t value);

public:
	GPU* gpu;                  //!< GPU instance (for accessing VRAM)

	bool usingBootstrap;       //!< Redirects 0x000-0x100 to the Bootstrap ROM

	uint8_t divider,           //!< Divider timer
		timerCounter,          //!< Controllable timer
		timerModulo;           //!< Timer modulo (increases with each timer overflow)

	TimerControl timerControl; //!< Timer control register

	MMU(ROM* romData, GPU* _gpu);

	/*! \brief Reads from memory
	 *
	 *  Issues a read from memory given a 16 bit address, based on the address
	 *  the MMU will fetch it from either the ROM, the RAM, VRAM or IO registers
	 *
	 *  \param location 16 bit memory address of memory to locate
	 *  \return Value in memory (8 bit)
	 */
	uint8_t Read(const uint16_t location);

	/*! \brief Writes to memory
	 *
	 *  Issues a write to memory given a 16 bit address and a 8 bit value to write,
	 *  the MMU will write it to either ROM (MBC registers), RAM (ROM's or native)
	 *  VRAM or the IO registers.
	 *
	 *  \param location 16 bit memory address to write to
	 *  \param value 8 bit value to write
	 */
	void Write(const uint16_t location, const uint8_t value);

	/*! \brief Updates internal timers
	 *
	 *  Updates the internal timers (Divider and Controller timer) based on the
	 *  delta cycles provided (only cpu cycles are used)
	 *
	 *  \param delta Cycles since the last call
	 */
	void UpdateTimers(CycleCount delta);
};