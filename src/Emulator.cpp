#include "Emulator.h"
#include <iostream>
#include <sstream>

bool Emulator::initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("mfemu", 100, 100, 160, 144, SDL_WINDOW_SHOWN);
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

Emulator::Emulator(const std::string& romfile, const bool graphics /* = true */)
	: rom(ROM::FromFile(romfile)), mmu(&rom, &gpu, &input), cpu(&mmu) {
	window = nullptr;
	renderer = nullptr;
	running = true;
	frameCycles = 0;
	if (graphics && !initSDL()) {
		std::cout << "Emulator could not start correctly, check error above.." << std::endl;
		return;
	}
	gpu.InitScreen(renderer);
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

void Emulator::Run() {
	while (running) {
		CheckUpdate();
		Step();
	}
	std::cout << "CPU Halted" << std::endl;
}

void Emulator::Step() {
	CycleCount c = cpu.Step();
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
	// Update window title
	std::stringstream winTitleStream;
	winTitleStream << rom.header.GBC.title << " (" << int(gpu.percent) << "%)";
	SDL_SetWindowTitle(window, winTitleStream.str().c_str());

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
