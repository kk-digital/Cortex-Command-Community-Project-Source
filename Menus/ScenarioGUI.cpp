#include "ScenarioGUI.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "UInputMan.h"
#include "SettingsMan.h"
#include "ConsoleMan.h"

#include "GameActivity.h"
#include "Entity.h"
#include "Scene.h"

#include "GUI.h"
#include "AllegroBitmap.h"
#include "AllegroScreen.h"
#include "AllegroInput.h"
#include "GUICollectionBox.h"
#include "GUIComboBox.h"
#include "GUIButton.h"
#include "GUILabel.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::Clear() {
		m_UpdateResult = ScenarioMenuUpdateResult::NoEvent;

		m_ScenarioActivities.clear();
		m_SelectedActivity = nullptr;

		m_ActivityScenes = nullptr;
		m_SelectedScene = nullptr;
		m_HoveredScene = nullptr;

		m_PlanetCenter.Reset();
		m_PlanetRadius = 0;
		m_LineToSitePoints.clear();

		m_DraggedBox = nullptr;
		m_PrevMousePos.Reset();

		m_BlinkTimer.Reset();
		m_ScenePreviewAnimTimer.Reset();

		m_DefaultScenePreview.Reset();
		m_DrawDefaultScenePreview = true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::Create(AllegroScreen *guiScreen, AllegroInput *guiInput){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::CreateActivityInfoBox(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::CreateSceneInfoBox(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::SetEnabled(const Vector &center, float radius){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::SetDraggedBox(int mouseX, int mouseY) {
		//GUICollectionBox *hoveredBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControlUnderPoint(mouseX, mouseY, m_RootBox, 1));
		//const GUIControl *hoveredControl = m_GUIControlManager->GetControlUnderPoint(mouseX, mouseY, hoveredBox, 1);
		//bool nonDragControl = (dynamic_cast<const GUIButton *>(hoveredControl) || dynamic_cast<const GUIComboBox *>(hoveredControl));
		//if (hoveredBox && !nonDragControl && !m_DraggedBox && !m_ActivitySelectComboBox->IsDropped()) { m_DraggedBox = hoveredBox; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::SetSelectedActivity(const Activity *newSelectedActivity){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::SetSelectedScene(Scene *newSelectedScene){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::DragBox(int mouseX, int mouseY){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::FetchActivitiesAndScenesLists(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::AdjustSitePointOffsetsOnPlanet(const std::vector<Scene *> &sceneList) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::CalculateLinesToSitePoint(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ScenarioGUI::ScenarioMenuUpdateResult ScenarioGUI::Update() {
		m_UpdateResult = ScenarioMenuUpdateResult::NoEvent;

		if (g_ConsoleMan.IsEnabled() && !g_ConsoleMan.IsReadOnly()) {
			return m_UpdateResult;
		}
		if (g_UInputMan.KeyPressed(KEY_ESC)) {
			g_GUISound.BackButtonPressSound()->Play();
			if (m_ActivityConfigBox->IsEnabled()) {
				m_ActivityConfigBox->SetEnabled(false);
			} else {
				return ScenarioMenuUpdateResult::BackToMain;
			}
		}

		int mousePosX;
		int mousePosY;
		//m_GUIControlManager->GetManager()->GetInputController()->GetMousePosition(&mousePosX, &mousePosY);
		//m_GUIControlManager->Update();

		if (!m_ActivityConfigBox->IsEnabled()) {
			m_RootBox->SetVisible(true);
			m_ActivityConfigBoxRootBox->SetVisible(false);

			UpdateHoveredSitePointLabel(mousePosX, mousePosY);
			HandleInputEvents(mousePosX, mousePosY);

			if (m_SceneInfoBox->GetVisible()) {
				m_StartActivityConfigButton->SetText(m_BlinkTimer.AlternateReal(333) ? "Test Start Here" : "> Test0Start Here <");

				if (m_DrawDefaultScenePreview && m_ScenePreviewAnimTimer.GetElapsedRealTimeMS() > m_DefaultScenePreview.GetSpriteAnimDuration() / m_DefaultScenePreview.GetFrameCount()) {
					m_DefaultScenePreview.SetNextFrame();
					m_ScenePreviewAnimTimer.Reset();
				}
			}
			if (m_ResumeButton->GetVisible()) { 
				//m_GUIControlManager->GetManager()->SetFocus((m_BlinkTimer.AlternateReal(500)) ? m_ResumeButton : nullptr); 
			}
		} else {
			m_RootBox->SetVisible(false);
			m_ActivityConfigBoxRootBox->SetVisible(true);

			if (m_ActivityConfigBox->Update(mousePosX, mousePosY)) { m_UpdateResult = ScenarioMenuUpdateResult::ActivityStarted; }
		}
		return m_UpdateResult;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::UpdateHoveredSitePointLabel(int mouseX, int mouseY){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::HandleInputEvents(int mouseX, int mouseY){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::Draw() const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::DrawSitePoints(BITMAP *drawBitmap) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScenarioGUI::DrawLinesToSitePoint(BITMAP *drawBitmap) const{}
}