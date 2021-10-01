#ifndef _RTE_MENUMANRML_
#define _RTE_MENUMANRML_
#include "Singleton.h"
#define g_RmlMenuMan RmlMenuMan::Instance()
namespace RTE {
	class SettingsMenu;
	class ModManager;
	class RmlMenuMan: public Singleton<RmlMenuMan> {
	public:
		RmlMenuMan();
		~RmlMenuMan();

		void Initialize();
	private:
		std::unique_ptr<SettingsMenu> m_SettingsMenuDataModel;
		std::unique_ptr<ModManager> m_ModManagerDataModel;
	};
}
#endif
