#include "SystemInterface.h"
#include <SDL2/SDL.h>
#include "Managers/TimerMan.h"
#include "System/System.h"
namespace RTE {
	SystemInterface::SystemInterface() = default;
	SystemInterface::~SystemInterface() = default;

	double SystemInterface::GetElapsedTime() {
		return g_TimerMan.GetRealTickCount() / g_TimerMan.GetTicksPerSecond();
	}

	bool SystemInterface::LogMessage(Rml::Log::Type type, const std::string &message) {
		std::string typedMessage;
		switch (type) {
			case Rml::Log::Type::LT_INFO: {
				typedMessage = "INFO: ";
			} break;

			case Rml::Log::Type::LT_ERROR: {
				typedMessage = "ERROR: ";
			} break;
			case Rml::Log::Type::LT_ASSERT: {
				typedMessage = "ERROR: Assert failure: ";
			} break;
			case Rml::Log::Type::LT_DEBUG: {
				typedMessage = "DEBUG: ";
			} break;
			case Rml::Log::Type::LT_WARNING: {
				typedMessage = "WARNING: ";
			} break;
			default:
				typedMessage = "";
				break;
		}
		typedMessage += message;
		System::PrintToCLI(typedMessage);
		return true;
	}

	void SystemInterface::SetMouseCursor(const std::string &/*cursor_name*/) {
	}

	void SystemInterface::SetClipboardText(const std::string &text) {
		SDL_SetClipboardText(text.c_str());
	}

	void SystemInterface::GetClipboardText(std::string &text) {
		text = SDL_GetClipboardText();
	}
} // namespace RTE
