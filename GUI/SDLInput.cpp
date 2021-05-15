#include "GUI.h"
#include "SDLInput.h"

#include "FrameMan.h"
#include "UInputMan.h"

#include "Vector.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mouse.h>

namespace RTE {
	SDLInput::SDLInput(int whichPlayer, bool keyJoyMouseCursor) :
	    GUIInput(whichPlayer, keyJoyMouseCursor) {
		std::fill(m_KeyboardBuffer, m_KeyboardBuffer + KEYBOARD_BUFFER_SIZE, Released);
		std::fill(m_ScanCodeState, m_ScanCodeState + KEYBOARD_BUFFER_SIZE, Released);
	}

	// TODO: This wraps legacy code; Would need to refactor the whole GUI Lib to fix
	void SDLInput::Update() {
		std::fill(m_KeyboardBuffer, m_KeyboardBuffer + KEYBOARD_BUFFER_SIZE, 0);
		std::fill(m_ScanCodeState, m_ScanCodeState + KEYBOARD_BUFFER_SIZE, 0);

		int numkeys{0};
		const uint8_t* keyboardStatus = SDL_GetKeyboardState(&numkeys);

		for (int key{0}; key < numkeys && key < KEYBOARD_BUFFER_SIZE; key++){
			if(keyboardStatus[key])
				m_ScanCodeState[key] = Pushed;
			else
				m_ScanCodeState[key] = Released;
		}

		keyboardStatus = nullptr;

		m_Modifier = ModNone;

		if(g_UInputMan.FlagAltState())
			m_Modifier|=ModAlt;
		if(g_UInputMan.FlagCtrlState())
			m_Modifier|=ModCtrl;
		if(g_UInputMan.FlagShiftState())
			m_Modifier|=ModShift;

		uint32_t buttons = SDL_GetMouseState(&m_LastFrameMouseX, &m_LastFrameMouseY);
		if (!m_KeyJoyMouseCursor) {
			if (buttons & SDL_BUTTON_LEFT) {
				m_MouseButtonsEvents[0] = (m_MouseButtonsStates[0] == Up) ? Pushed : Repeat;
				m_MouseButtonsStates[0] = Down;
			} else {
				m_MouseButtonsEvents[0] = (m_MouseButtonsStates[0] == Down) ? Released : None;
				m_MouseButtonsStates[0] = Up;
			}
		}

		if (buttons & SDL_BUTTON_RIGHT) {
			m_MouseButtonsEvents[1] = (m_MouseButtonsStates[1] == Up) ? Pushed : Repeat;
			m_MouseButtonsStates[1] = Down;
		} else {
			m_MouseButtonsEvents[1] = (m_MouseButtonsStates[1] == Down) ? Released : None;
			m_MouseButtonsStates[1] = Up;
		}

		if (buttons & SDL_BUTTON_MIDDLE) {
			m_MouseButtonsEvents[2] = (m_MouseButtonsStates[2] == Up) ? Pushed : Repeat;
			m_MouseButtonsStates[2] = Down;
		} else {
			m_MouseButtonsEvents[2] = (m_MouseButtonsStates[2] == Down) ? Released : None;
			m_MouseButtonsStates[2] = Up;
		}
	}
} // namespace RTE
