#pragma once

#include <SDL.h>
#include <string>

#include "ROM.h"
#include "MMU.h"
#include "CPU.h"
#include "GPU.h"
#include "Input.h"

/*! \brief Game boy Emulator
 *
 *  "God" class that manages the execution and interaction
 *  of all the submodules (ROM/CPU/GPU/MMU/etc.)
 */
class Emulator {
	friend class Debugger;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	uint64_t frameCycles;
	uint64_t titleFpsCount;

	bool initSDL();
	void checkInterrupts();
	void fakeBootrom();
public:
	ROM rom;      //!< ROM file
	GPU gpu;      //!< LCD driver
	MMU mmu;      //!< Memory management unit
	CPU cpu;      //!< CPU

	Input input;  //!< Input manager

	bool running; //!< Is the emulator running?

	/*! \brief Create a GB emulator
	 *
	 *  Creates an instance of the mfemu emulator with
	 *  a path to the ROM to load.
	 *
	 *  \param romfile Path to the ROM to load
	 *  \param useBootrom Use the Game Boy boot rom
	 */
	explicit Emulator(
		const std::string& romfile,
		const bool useBootrom = true
	);

	~Emulator();

	/*! \brief Run the emulator
	 *
	 *  Runs the emulator in a "blocking" way.
	 */
	void Run();

	/*! \brief Execute a single step
	 *
	 *  Executes a single step, useful for running
	 *  the emulator on an external loop (ie. Debugger)
	 */
	void Step();

	/*! \brief Check for window update
	 *
	 *  Checks if the window should be updated (title / fps count)
	 */
	void CheckUpdate();

	/*! \brief Update the window
	 *
	 *  Updates the window title and internal timers (ie. fps count)
	 */
	void Update();
};
