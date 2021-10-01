#ifndef _RTE_MODMANAGERRML_
#define _RTE_MODMANAGERRML_
#include "RmlUi/Core/DataModelHandle.h"
namespace RTE {
	class ModManager {
	public:
		ModManager();
		~ModManager();

		void Initialize(Rml::Context *context);
	private:
		Rml::DataModelHandle m_ModelHandle;
	};
}
#endif
