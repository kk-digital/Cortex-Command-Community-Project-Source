#ifndef _REFACTORDEBUG_
#define _REFACTORDEBUG_

namespace RTE
{
	class RefactorDebug {
	public:
		string DefaultScenario_type = "GAScripted";
		string DefaultScenario_preset = "Default Preset";
		bool LoadDefaultScenario = true;
		void StartGameWithoutGui(string DefaultScenario_type, string DefaultScenario_preset);
	};
}
#endif // File