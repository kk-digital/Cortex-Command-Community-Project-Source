#ifndef _RTE_SETTINGSMENURML_
#define _RTE_SETTINGSMENURML_
#include "RmlUi/Core/DataModelHandle.h"
#include "EventHandlerSettings.h"

namespace RTE {
	class SettingsMenu {
	public:
		SettingsMenu();
		~SettingsMenu();

		void Initialize(Rml::Context *context);

	private:
		EventHandlerSettings m_EventHandler;
		Rml::DataModelHandle m_ModelHandle;
	};
}
#endif
