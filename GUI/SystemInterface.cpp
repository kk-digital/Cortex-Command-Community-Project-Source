#include "SystemInterface.h"
#include <SDL2/SDL.h>
#include "Managers/TimerMan.h"

namespace RTE{
	SystemInterface::SystemInterface() = default;
	SystemInterface::~SystemInterface() = default;

	double SystemInterface::GetElapsedTime() {
		return g_TimerMan.GetRealTickCount() / g_TimerMan.GetTicksPerSecond();
	}

	bool SystemInterface::LogMessage(Rml::Log::Type type, const std::string &message) {
		return true;
	}

	void SystemInterface::SetMouseCursor(const std::string &cursor_name) {
	}

	void SystemInterface::SetClipboardText(const std::string &text) {
		SDL_SetClipboardText(text.c_str());
	}

	void SystemInterface::GetClipboardText(std::string &text) {
		text = SDL_GetClipboardText();
	}
}
