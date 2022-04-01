#include "DebugMan.h"

#include "FrameMan.h"
#include "PerformanceMan.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GL/glew.h"

#include "ActivityMan.h"
#include "GameActivity.h"
#include "Entity.h"
#include "Scene.h"
#include "PresetMan.h"

namespace RTE {
	DebugMan::DebugMan() = default;
	DebugMan::~DebugMan() = default;

	bool DebugMan::Initialize() {
		m_ShowDemoWindow = true;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForOpenGL(g_FrameMan.GetWindow(), g_FrameMan.GetContext());
		ImGui_ImplOpenGL3_Init("#version 330 core");

		/* Scenario select */
		m_CurrentActivity = -1;
		m_CurrentScene = -1;
		m_StartingActivity = false;
		m_GameDifficulty = Activity::DifficultySetting::MediumDifficulty;
		m_Gold = 3000.f;
		return true;
	}

	void DebugMan::Destroy() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void DebugMan::Update() {
	}

	void DebugMan::UpdateImGuiInput(SDL_Event *event) {
		ImGui_ImplSDL2_ProcessEvent(event);
	}

	void DebugMan::Draw() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		g_PerformanceMan.Draw();

		if (!g_ActivityMan.IsInActivity()) {
			ScenarioSelect();
		}



		if (m_ShowDemoWindow) {
			ImGui::ShowDemoWindow(&m_ShowDemoWindow);
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void DebugMan::ScenarioSelect() {
		if (m_Activites.empty()) {
			std::list<Entity *> presetList;
			g_PresetMan.GetAllOfType(presetList, "Scene");
			std::vector<Scene *> loadedScenes;
			for (Entity *preset: presetList) {
				Scene *scene = dynamic_cast<Scene *>(preset);
				if (scene && !scene->GetLocation().IsZero() && !scene->IsMetagameInternal()) {
					loadedScenes.emplace_back(scene);
				}
			}
			presetList.clear();
			g_PresetMan.GetAllOfType(presetList, "Activity");
			for (Entity *presetEntity: presetList) {
				if (GameActivity *presetActivity = dynamic_cast<GameActivity *>(presetEntity)) {
					std::pair<Activity *, std::vector<Scene *>> activityAndCompatibleScenes(presetActivity, std::vector<Scene *>());
					for (Scene *filteredScene: loadedScenes) {
						if (presetActivity->SceneIsCompatible(filteredScene)) {
							activityAndCompatibleScenes.second.emplace_back(filteredScene);
						}
					}
					m_Scenes.insert(activityAndCompatibleScenes);
					m_Activites.emplace_back(presetActivity);
				}
			}
			m_CurrentActivity = 0;
		}
		{
			ImGui::Begin("Scenario Select");

			const std::string &currentActivity = m_Activites.at(m_CurrentActivity)->GetPresetName();
			if (ImGui::BeginCombo("Activity", currentActivity.c_str())) {
				for (size_t n = 0; n < m_Activites.size(); ++n) {
					bool isSelected = (m_CurrentActivity == n);
					if (ImGui::Selectable(m_Activites.at(n)->GetPresetName().c_str(), isSelected)) {
						m_CurrentActivity = n;
						m_CurrentScene = -1;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginListBox("Scenario")) {
				std::vector<Scene *> &scenes = m_Scenes.at(m_Activites.at(m_CurrentActivity));
				for (size_t n = 0; n < m_Scenes.at(m_Activites.at(m_CurrentActivity)).size(); ++n) {
					bool selected = (m_CurrentScene == n);
					if (ImGui::Selectable(scenes.at(n)->GetPresetName().c_str(), selected)) {
						m_CurrentScene = n;
					}

					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}

			ImGui::SliderInt("Difficulty", &m_GameDifficulty, Activity::DifficultySetting::MinDifficulty, Activity::DifficultySetting::MaxDifficulty);
			ImGui::SliderInt("Starting Gold", &m_Gold, 0, 10000);

			if (ImGui::Button("Start")) {
				m_StartingActivity = true;
				GameActivity *gameActivity = dynamic_cast<GameActivity *>(m_Activites.at(m_CurrentActivity)->Clone());

				gameActivity->SetDifficulty(m_GameDifficulty);
				gameActivity->SetStartingGold(m_Gold);

				gameActivity->SetRequireClearPathToOrbit(false);
				gameActivity->SetFogOfWarEnabled(false);
				Scene *selectedScene = m_Scenes.at(m_Activites.at(m_CurrentActivity)).at(m_CurrentScene);
				constexpr bool deployUnits = true;
				g_SceneMan.SetSceneToLoad(selectedScene, true, deployUnits);

				gameActivity->ClearPlayers(false);
				gameActivity->SetCPUTeam();
				gameActivity->AddPlayer(Players::PlayerOne, true, Activity::Teams::TeamOne, 0);

				gameActivity->SetTeamTech(Activity::Teams::TeamOne, "-Random-");

				g_LuaMan.FileCloseAll();
				g_ActivityMan.SetStartActivity(gameActivity);
			}

			ImGui::End();
		}
	}

	bool DebugMan::IsStartActivity() {
		if (m_StartingActivity) {
			m_StartingActivity = false;
			g_ActivityMan.SetRestartActivity();
			return true;
		}
		return false;
	}
} // namespace RTE
