#pragma once

#include <map>
#include "SDL.h"

//! Input data structure
struct InputData {
	uint8_t
		Start,  //!< Start button
		Select, //!< Select button
		B,      //!< B button
		A,      //!< A button
		Down,   //!< Down  (D-pad South)
		Up,     //!< Up    (D-pad North)
		Left,   //!< Left  (D-pad West) 
		Right;  //!< Right (D-pad East)

	uint8_t 
		b5,     //!< Bit 5 / P15 output line (Down/Up/Left/Right)
		b4;     //!< Bit 4 / P14 output line (Start/Select/B/A)

	/*! \brief Gets the value for FF00
	 *
	 *  Gets the proper value to output when FF00 is being read.
	 *  Emulates the P14 and P15 output lines.
	 *
	 *  \return FF00 emulated value (first 6 bits)
	 */
	uint8_t GetRegister() {
		uint8_t data = b4 << 4 | b5 << 5;
		if (b4 == 0) {
			data |= (Down << 3) | (Up << 2) | (Left << 1) | Right;
		}
		if (b5 == 0) {
			data |= (Start << 3) | (Select << 2) | (B << 1) | A;
		}

		return data;
	}

	/*! \brief Sets a value to the input registers
	 *
	 *  Called when writing to FF00, ignores every bit except for b4 and b5, which
	 *  are used to turn on (0) or off (1) the output lines P14 and P15
	 *
	 *  \param data 8bit value to extract b4 and b5 from
	 */
	void SetRegister(uint8_t data) {
		b4 = data >> 4 & 1;
		b5 = data >> 5 & 1;
	}
};

enum Button {
	ButtonUp,
	ButtonDown,
	ButtonLeft,
	ButtonRight,
	ButtonA,
	ButtonB,
	ButtonStart,
	ButtonSelect
};

class Input {
private:
	std::map<SDL_Keycode, Button> keyboardBindings;

public:
	InputData data;     //!< Input data for reading
	bool buttonPressed; //!< Has a button been pressed? (interrupt check)

	/*! \brief Handles input events
	 *
	 * Handles every input event related to input and updates the input data variable
	 * 
	 * \param event The SDL event to parse and handle according to the configured bindings
	 */
	void HandleInputEvent(SDL_Event event);

	Input();
};
