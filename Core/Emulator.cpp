#include "Emulator.h"
#include <iostream>
#include <sstream>

Emulator::Emulator(const std::string& romfile, const EmulatorFlags emuflags)
	: rom(ROM::FromFile(romfile)), mmu(&rom, &gpu, &input), cpu(&mmu) {
	window = nullptr;
	renderer = nullptr;
	running = true;
	flags = emuflags;
	frameCycles = 0;
	titleFpsCount = 0;
}

Emulator::~Emulator() {
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}

bool Emulator::init() {
	// Initialize SDL
	if (!initSDL()) {
		std::cout << "Emulator could not start correctly, check error above.." << std::endl;
		return false;
	}
	// Initialize GPU
	gpu.InitScreen(renderer);
	if (!flags.useBootrom) {
		fakeBootrom();
	}
	return isInit = true;
}

bool Emulator::initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("mfemu", 100, 100, WIDTH * flags.scale, HEIGHT * flags.scale, SDL_WINDOW_SHOWN);
	if (window == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr){
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	return true;
}

void Emulator::Run() {
	if (!init())
		return;

	while (running) {
		CheckUpdate();
		Step();
	}
	std::cout << "CPU Halted" << std::endl;
}

void Emulator::Step() {
	const CycleCount c = cpu.Step();
	frameCycles += c.machine;
	mmu.UpdateTimers(c);
	gpu.Step(c.machine);

	if (mmu.interruptsEnabled) {
		checkInterrupts();
	}
}

void Emulator::checkInterrupts() {
	if (gpu.didVblank) {
		mmu.SetInterrupt(IntLCDVblank);
		gpu.didVblank = false;
	}

	if (gpu.didLCDInterrupt) {
		mmu.SetInterrupt(IntLCDControl);
		gpu.didLCDInterrupt = false;
	}

	if (input.buttonPressed) {
		mmu.SetInterrupt(IntInput);
		input.buttonPressed = false;
	}

	cpu.HandleInterrupts();
}

void Emulator::CheckUpdate() {
	// Only check once every frame
	if (frameCycles >= 70224) {
		Update();
		frameCycles = 0;
	}
}

void Emulator::Update() {
	// Update window title once every 10 frames
	if (++titleFpsCount == 10) {
		titleFpsCount = 0;
		std::stringstream winTitleStream;
		winTitleStream << rom.header.GBC.title << " (" << int(gpu.percent) << "%)";
		SDL_SetWindowTitle(window, winTitleStream.str().c_str());
	}

	// Get system events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYAXISMOTION:
			input.HandleInputEvent(event);
			break;
		}
	}
}

void Emulator::fakeBootrom() {
	// Setup stack
	cpu.SP = 0xfffe;

	// Zero the VRAM the fast(tm) way
	memset(gpu.VRAM[0].bytes, 0, 8 * 1024);

	// Turn off Bootrom
	mmu.Write(0xff50, 1);

	// Put PC at the end (0x0100)
	cpu.PC = 0x100;
}
