#include "Input.h"

void setButton(InputData* data, Button button, uint8_t value);

Input::Input() {
	// Hardcoded bindings for now, change later
	keyboardBindings[SDL_SCANCODE_Z] = ButtonB;
	keyboardBindings[SDL_SCANCODE_X] = ButtonA;
	keyboardBindings[SDL_SCANCODE_RETURN] = ButtonStart;
	keyboardBindings[SDL_SCANCODE_BACKSPACE] = ButtonSelect;
	keyboardBindings[SDL_SCANCODE_UP] = ButtonUp;
	keyboardBindings[SDL_SCANCODE_DOWN] = ButtonDown;
	keyboardBindings[SDL_SCANCODE_LEFT] = ButtonLeft;
	keyboardBindings[SDL_SCANCODE_RIGHT] = ButtonRight;

	// Set all buttons to "not pressed" (1)
	data.A = data.B = data.Down = data.Up = data.Left = data.Right = data.Start = data.Select = 1;

	// Set interrupt trigger to false
	buttonPressed = false;
}

void Input::HandleInputEvent(SDL_Event event) {
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		auto iter = keyboardBindings.find(event.key.keysym.scancode);
		if (iter != keyboardBindings.end()) {
			bool pressed = event.key.state == SDL_PRESSED;
			setButton(&data, iter->second, pressed ? 0 : 1);

			// Enable interrupt if button pressed
			buttonPressed = pressed;
		}
		break;
	}
}

void setButton(InputData* data, Button button, uint8_t value) {
	switch (button) {
	case ButtonA:
		data->A = value; break;
	case ButtonB:
		data->B = value; break;
	case ButtonUp:
		data->Up = value; break;
	case ButtonDown:
		data->Down = value; break;
	case ButtonLeft:
		data->Left = value; break;
	case ButtonRight:
		data->Right = value; break;
	case ButtonStart:
		data->Start = value; break;
	case ButtonSelect:
		data->Select = value; break;
	}
}