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
}

void Input::HandleInputEvent(SDL_Event event) {
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		auto iter = keyboardBindings.find(event.key.keysym.scancode);
		if (iter != keyboardBindings.end()) {
			setButton(&data, iter->second, event.key.state == SDL_PRESSED ? 1 : 0);
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