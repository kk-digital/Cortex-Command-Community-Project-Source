#ifndef _RTE_EVENTHANDLERSETTINGSRML_
#define _RTE_EVENTHANDLERSETTINGSRML_
#include "EventHandler.h"

namespace RTE {
	class EventHandlerSettings : public EventHandler {
	public:
		EventHandlerSettings();
		~EventHandlerSettings();

		void ProcessEvent(const Rml::Event &event, const std::string &value) override;
	};
} // namespace RTE
#endif
