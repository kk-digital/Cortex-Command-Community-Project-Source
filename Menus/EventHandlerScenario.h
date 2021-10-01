#ifndef _RTE_HANDLERSCENARIORML_
#define _RTE_HANDLERSCENARIORML_
#include "EventHandler.h"
namespace RTE {
	class EventHandlerScenario : public EventHandler {
	public:
		EventHandlerScenario();
		~EventHandlerScenario();

		void ProcessEvent(const Rml::Event &event, const std::string &value) override;
	};
} // namespace RTE
#endif
