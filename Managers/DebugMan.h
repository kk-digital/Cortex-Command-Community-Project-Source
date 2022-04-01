#ifndef _RTEDEBUGMANH_
#define _RTEDEBUGMANH_
#include "Singleton.h"

#define g_DebugMan RTE::DebugMan::Instance()

extern "C" {
	union SDL_Event;
}

namespace RTE{
	class Scene;
	class GameActivity;
	class Activity;
	class DebugMan: public Singleton<DebugMan>{
	public:
		DebugMan();
		virtual ~DebugMan();
		bool Initialize();
		void Destroy();
		void Update();
		void UpdateImGuiInput(SDL_Event* event);
		void Draw();
		bool IsStartActivity();

	private:
		void ActorSelect();
		void BuyMenu();

		bool m_ShowDemoWindow;

		/// Scenario Selection ///
		void ScenarioSelect();

		std::map<Activity*, std::vector<Scene*>> m_Scenes;
		std::vector<GameActivity *> m_Activites;
		int m_CurrentActivity;
		int m_CurrentScene;
		int m_GameDifficulty;
		int m_Gold;
		bool m_StartingActivity;


		/// Debug Menu ///


		/// Debug Info ///
		void DebugInfo();
	};
}
#endif
