#pragma once

class GPU {
private:
	int mode;
	int cycleCount;
	int line;

public:
	void Step(int cycles);
};