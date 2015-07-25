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

enum TimerClock : uint8_t {
	ClockDiv1024 = 0, //! One tick every 1024 CPU clocks
	ClockDiv16   = 1, //! One tick every 16   CPU clocks
	ClockDiv64   = 2, //! One tick every 64   CPU clocks
	ClockDiv256  = 3  //! One tick every 256  CPU clocks
};

union TimerControl {
	uint8_t raw;
	struct Values {
		TimerClock clock : 2;
		uint8_t  enabled : 1;
	} values;
};

class MMU {
private:
	ROM* rom;

	WRAMBank WRAM;
	std::vector<WRAMBank> WRAMbanks;
	uint8_t WRAMbankId = 0;

	ZRAMBank ZRAM;

	uint8_t readIO(const uint16_t location);
	void writeIO(const uint16_t location, const uint8_t value);

public:
	GPU* gpu;
	bool usingBootstrap; //! Is the bootstrap ROM enabled?

	// Timers
	uint8_t divider, timerCounter, timerModulo;
	TimerControl timerControl;
	uint8_t dividerRest;
	uint16_t counterRest;

	MMU(ROM* romData, GPU* _gpu);

	uint8_t Read(const uint16_t location);
	void Write(const uint16_t location, const uint8_t value);

	void UpdateTimers(CycleCount delta);
};