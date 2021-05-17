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

		int numkeys{0};
		const uint8_t* keyboardStatus = SDL_GetKeyboardState(&numkeys);

		for (int key{0}; key < numkeys && key < KEYBOARD_BUFFER_SIZE; key++){
			if (keyboardStatus[key]) {
				if (m_ScanCodeState[key] == Pushed || m_ScanCodeState[key] == Repeat) {
					m_ScanCodeState[key] = Repeat;
					uint8_t keyname = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key)); // TODO: srsly do not do it this way, just wrap UInputMan for GUI thatll do
					m_KeyboardBuffer[keyname] = Repeat;
				} else {
					m_ScanCodeState[key] = Pushed;
					uint8_t keyname = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key));
					m_KeyboardBuffer[keyname] = Pushed;
				}
			} else {
				m_ScanCodeState[key] = Released;
				uint8_t keyname = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key));
				m_KeyboardBuffer[keyname] = Released;
			}
		}

		keyboardStatus = nullptr;

		m_Modifier = ModNone;

		if(g_UInputMan.FlagAltState())
			m_Modifier|=ModAlt;
		if(g_UInputMan.FlagCtrlState())
			m_Modifier|=ModCtrl;
		if(g_UInputMan.FlagShiftState())
			m_Modifier|=ModShift;

		uint32_t buttons = SDL_GetMouseState(&m_MouseX, &m_MouseY);
		m_LastFrameMouseX = m_MouseX;
		m_LastFrameMouseY = m_MouseY;
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

    if (m_KeyJoyMouseCursor)
    {
        // // Direction change
        // Vector joyKeyDirectional = g_UInputMan.GetMenuDirectional() * 5;

        // // See how much to accelerate the joystick input based on how long the stick has been pushed around
        // if (joyKeyDirectional.GetMagnitude() < 0.95)
        //     m_pCursorAccelTimer->Reset();

        // float acceleration = 0.25 + MIN(m_pCursorAccelTimer->GetElapsedRealTimeS(), 0.5) * 20;

        // // Manipulate the mouse position with the joysticks or keys
        // mouse_x += joyKeyDirectional.m_X * mouseDenominator * elapsedS * 15.0f * acceleration;
        // mouse_y += joyKeyDirectional.m_Y * mouseDenominator * elapsedS * 15.0f * acceleration;
        // // Prevent mouse from flying out of the screen
        // mouse_x = MAX(0, mouse_x);
        // mouse_y = MAX(0, mouse_y);
        // // Pull in a bit so cursor doesn't completely disappear
        // mouse_x = MIN((g_FrameMan.GetResX() * mouseDenominator) - 3, mouse_x);
        // mouse_y = MIN((g_FrameMan.GetResY() * mouseDenominator) - 3, mouse_y);

		// Button states/presses, Primary - ACTUALLY make either button work, we don't have use for secondary in menus
		if (g_UInputMan.MenuButtonHeld(UInputMan::MENU_EITHER)) {
			m_MouseButtonsStates[0] = Down;
			m_MouseButtonsEvents[0] = Repeat;
		}
		if (g_UInputMan.MenuButtonPressed(UInputMan::MENU_EITHER)) {
			m_MouseButtonsStates[0] = Down;
			m_MouseButtonsEvents[0] = Pushed;
		} else if (g_UInputMan.MenuButtonReleased(UInputMan::MENU_EITHER)) {
			m_MouseButtonsStates[0] = Up;
			m_MouseButtonsEvents[0] = Released;
		} else if (m_MouseButtonsEvents[0] == Released) {
			m_MouseButtonsStates[0] = Up;
			m_MouseButtonsEvents[0] = None;
		}
	}
	}

	void ConvertKeyEvent(int sdlKey, int guilibKey, float elapsedS){

	}
} // namespace RTE
