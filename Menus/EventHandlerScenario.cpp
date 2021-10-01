#include "EventHandlerScenario.h"
#include "RmlUi/Core.h"

namespace RTE {
	EventHandlerScenario::EventHandlerScenario() {}

	EventHandlerScenario::~EventHandlerScenario() {}

	void EventHandlerScenario::ProcessEvent(const Rml::Event &event, const std::string &value) {
		Rml::StringList commands;
		Rml::StringUtilities::ExpandString(commands, value, ';');
		if(commands.empty()) {
			return;
		}
	}
}
