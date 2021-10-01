#ifndef _RTE_EVENTHANDLERRML_
#define _RTE_EVENTHANDLERRML_
#include "RmlUi/Core/Types.h"
namespace RTE {
	class EventHandler {
	public:
		virtual ~EventHandler() = default;
		virtual void ProcessEvent(const Rml::Event &event, const std::string &value) = 0;
	};
} // namespace RTE
#endif
