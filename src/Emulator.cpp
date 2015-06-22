#include "Emulator.h"
#include <iostream>

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

Emulator::Emulator(const std::string& romfile, bool graphics /* = true */)
	: rom(ROM::FromFile(romfile)), mmu(&rom, &gpu), cpu(&mmu) {
	window = nullptr;
	renderer = nullptr;
	running = true;
	if (graphics && !initSDL()) {
		std::cout << "Emulator could not start correctly, check error above.." << std::endl;
		return;
	}
	gpu.initScreen(renderer);
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
		Step();
	}
	std::cout << "CPU Halted" << std::endl;
}

void Emulator::Step() {
	CycleCount c = cpu.Step();
	gpu.Step(c.machine);
}

void Emulator::Update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			running = false;
			break;
		}
	}
}