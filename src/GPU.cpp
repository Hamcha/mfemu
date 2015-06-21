#include "GPU.h"

void GPU::Step(int cycles) {
	cycleCount += cycles;

	switch (mode) {
		// Hblank
	case 0:
		break;
		// Vblank (lasts 10 lines)
	case 1:
		break;
		// OAM read
	case 2:
		break;
		// VRAM read
	case 3:
		break;
	}
}