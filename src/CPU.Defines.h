#pragma once

#include "CPU.h"
#include <functional>

typedef std::function<CycleCount(CPU* cpu, MMU* mmu)> CPUHandler;

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