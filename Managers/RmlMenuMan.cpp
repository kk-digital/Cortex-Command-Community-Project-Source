#include "RmlMenuMan.h"
#include "RmlUIMan.h"
#include "Menus/rmlModManager.h"
#include "Menus/rmlSettingsMenu.h"

namespace RTE {
	RmlMenuMan::RmlMenuMan() {
		m_ModManagerDataModel = std::make_unique<ModManager>();
		m_SettingsMenuDataModel = std::make_unique<SettingsMenu>();
	}
	RmlMenuMan::~RmlMenuMan() {}

	void RmlMenuMan::Initialize() {
		RTEAssert(g_RmlUIMan.GetContext(), "Trying to load menus before a context was initialized!");

		m_ModManagerDataModel->Initialize(g_RmlUIMan.GetContext());

		m_SettingsMenuDataModel->Initialize(g_RmlUIMan.GetContext());

		// g_RmlUIMan.LoadDocument("TitleBackground");
		g_RmlUIMan.LoadDocument("Example");
	}
}
