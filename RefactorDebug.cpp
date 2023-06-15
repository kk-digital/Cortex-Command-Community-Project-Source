#include "RefactorDebug.h"

#include "MenuMan.h"
#include "ConsoleMan.h"
#include "SettingsMan.h"
#include "PresetMan.h"
#include "UInputMan.h"
#include "PerformanceMan.h"
#include "MetaMan.h"
#include "NetworkServer.h"
#include "Scene.h"
#include "ActivityMan.h"
#include "SceneMan.h"
using namespace RTE;

namespace RTE {
	void RefactorDebug::StartGameWithoutGui(string DefaultScenario_type, string DefaultScenario_preset) {
		bool LoadDefaultScenario = true;
		g_SceneMan.SetSceneToLoad("Ketanot Hills");

		const Activity* pActivityPreset = dynamic_cast<const Activity*>(g_PresetMan.GetEntityPreset(DefaultScenario_type, DefaultScenario_preset)->Clone());
		Activity* pActivity = dynamic_cast<Activity*>(pActivityPreset->Clone());
		GameActivity* pTestGame = dynamic_cast<GameActivity*>(pActivity);
		RTEAssert(pTestGame, "Couldn't find the \"RefactorDebug Preset\" GAScripted Activity! Has it been defined?");
		pTestGame->ClearPlayers(false);

		// Add PlayerOne and isHuman is true
		pTestGame->AddPlayer(Players::PlayerOne, true, Activity::Teams::TeamOne, 0);

		// Add PlayerTwo and isHuman is false(CPU)
		pTestGame->AddPlayer(Players::PlayerTwo, false, Activity::Teams::TeamOne, 0);

		//pTestGame->SetTeamOfPlayer(0, 0);
		//pTestGame->SetCPUTeam(1);
		pTestGame->SetStartingGold(10000);
		pTestGame->SetFogOfWarEnabled(false);
		pTestGame->SetDifficulty(Activity::DifficultySetting::MediumDifficulty);

		//g_SceneMan.SetSceneToLoad("Editor Scene");
		g_ActivityMan.SetStartActivity(pTestGame);
		g_ActivityMan.SetRestartActivity();
	}
}