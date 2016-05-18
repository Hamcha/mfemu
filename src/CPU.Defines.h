#pragma once

#include "CPU.h"
#include <functional>

using CPUHandler = std::function<CycleCount(CPU* cpu, MMU* mmu)>;

enum RID {
	A, B, C, D, E, H, L
};
enum PID {
	AF, BC, DE, HL, SP, PC
};
enum JumpCondition {
	NO, NZ, ZE, NC, CA
};
enum RotationType {
	ThC, Shf, Rep, Rot
};
