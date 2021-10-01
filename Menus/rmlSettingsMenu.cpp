#include "rmlSettingsMenu.h"
#include "EventHandlerSettings.h"
#include "RmlUi/Core.h"
#include "RmlUIMan.h"
#include "SettingsMan.h"

namespace RTE {
	SettingsMenu::SettingsMenu() {

	}

	SettingsMenu::~SettingsMenu() {}

	void SettingsMenu::Initialize(Rml::Context *context) {
		Rml::DataModelConstructor modelConstructor = context->CreateDataModel("settings");
		g_RmlUIMan.RegisterEventHandler("Settings", &m_EventHandler);
		m_ModelHandle = modelConstructor.GetModelHandle();
	}
}
