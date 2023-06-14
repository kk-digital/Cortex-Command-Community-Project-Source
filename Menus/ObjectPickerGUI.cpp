#include "ObjectPickerGUI.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "UInputMan.h"
#include "SettingsMan.h"

#include "GUI.h"
#include "AllegroBitmap.h"
#include "AllegroScreen.h"
#include "AllegroInput.h"
#include "GUIControlManager.h"
#include "GUICollectionBox.h"
#include "GUIListBox.h"
#include "GUILabel.h"

#include "DataModule.h"
#include "SceneObject.h"
#include "EditorActivity.h"
#include "BunkerAssembly.h"
#include "BunkerAssemblyScheme.h"

namespace RTE {

	BITMAP *ObjectPickerGUI::s_Cursor = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::Clear() {
		m_CursorPos.Reset();
		m_GUIScreen = nullptr;
		m_GUIInput = nullptr;
		m_GUIControlManager = nullptr;
		m_ParentBox = nullptr;
		m_PopupBox = nullptr;
		m_PopupText = nullptr;
		m_GroupsList = nullptr;
		m_ObjectsList = nullptr;
		m_Controller = nullptr;
		m_PickerState = PickerState::Disabled;
		m_PickerFocus = PickerFocus::GroupList;
		m_OpenCloseSpeed = 0.3F;
		m_ModuleSpaceID = -1;
		m_ShowType.clear();
		m_NativeTechModuleID = 0;
		m_ForeignCostMult = 4.0F;
		m_ShownGroupIndex = 0;
		m_SelectedGroupIndex = 0;
		m_SelectedObjectIndex = 0;
		m_PickedObject = nullptr;
		m_RepeatStartTimer.Reset();
		m_RepeatTimer.Reset();

		m_ExpandedModules.resize(g_PresetMan.GetTotalModuleCount());
		std::fill(m_ExpandedModules.begin(), m_ExpandedModules.end(), false);
		m_ExpandedModules.at(0) = true; // Base.rte is always expanded
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ObjectPickerGUI::Create(Controller *controller, int whichModuleSpace, const std::string_view &onlyOfType) {
		RTEAssert(controller, "No controller sent to ObjectPickerGUI on creation!");
		m_Controller = controller;

		if (!m_GUIScreen) { m_GUIScreen = std::make_unique<AllegroScreen>(g_FrameMan.GetBackBuffer8()); }
		if (!m_GUIInput) { m_GUIInput = std::make_unique<AllegroInput>(controller->GetPlayer()); }
		if (!m_GUIControlManager) { m_GUIControlManager = std::make_unique<GUIControlManager>(); }
		RTEAssert(m_GUIControlManager->Create(m_GUIScreen.get(), m_GUIInput.get(), "Base.rte/GUIs/Skins", "DefaultSkin.ini"), "Failed to create GUI Control Manager and load it from Base.rte/GUIs/Skins/DefaultSkin.ini");

		//m_GUIControlManager->Load("Base.rte/GUIs/ObjectPickerGUI.ini");
		m_GUIControlManager->EnableMouse(controller->IsMouseControlled());

		if (!s_Cursor) {
			ContentFile cursorFile("Base.rte/GUIs/Skins/Cursor.png");
			s_Cursor = cursorFile.GetAsBitmap();
		}
		SetModuleSpace(whichModuleSpace);
		ShowOnlyType(onlyOfType);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SetEnabled(bool enable) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SetNativeTechModule(int whichModule) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ObjectPickerGUI::SetListFocus(PickerFocus listToFocusOn) {
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ObjectPickerGUI::SelectGroupByName(const std::string_view &groupName) {
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SelectGroupByIndex(int groupIndex, bool updateObjectsList) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SelectNextOrPrevGroup(bool selectPrev) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::UpdateGroupsList() {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const SceneObject * ObjectPickerGUI::GetSelectedObject() {
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SelectObjectByIndex(int objectIndex, bool playSelectionSound) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SelectNextOrPrevObject(bool getPrev) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::AddObjectsListModuleGroup(int moduleID) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::SetObjectsListModuleGroupExpanded(int moduleID, bool expanded) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::ToggleObjectsListModuleGroupExpansion(int moduleID) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::ShowDescriptionPopupBox() {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::UpdateObjectsList(bool selectTop) {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::Update() {
		return;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ObjectPickerGUI::HandleInput() {
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ObjectPickerGUI::HandleMouseEvents() {
		int mouseX;
		int mouseY;
		m_GUIInput->GetMousePosition(&mouseX, &mouseY);
		m_CursorPos.SetXY(static_cast<float>(mouseX), static_cast<float>(mouseY));

		GUIEvent guiEvent;
		while (m_GUIControlManager->GetEvent(&guiEvent)) {
			if (guiEvent.GetType() == GUIEvent::Notification) {
				if (guiEvent.GetControl() == m_GroupsList) {
					if (guiEvent.GetMsg() == GUIListBox::MouseDown) {
						SelectGroupByIndex(m_GroupsList->GetSelectedIndex());
					} else if (guiEvent.GetMsg() == GUIListBox::MouseMove) {
						const GUIListPanel::Item *groupListItem = m_GroupsList->GetItem(m_CursorPos.GetFloorIntX(), m_CursorPos.GetFloorIntY());
						if (groupListItem && m_SelectedGroupIndex != groupListItem->m_ID) { SelectGroupByIndex(groupListItem->m_ID, false); }
					} else if (guiEvent.GetMsg() == GUIListBox::MouseEnter) {
						SetListFocus(PickerFocus::GroupList);
					} else if (guiEvent.GetMsg() == GUIListBox::MouseLeave && m_SelectedGroupIndex != m_ShownGroupIndex) {
						SelectGroupByIndex(m_ShownGroupIndex, false);
					}
				} else if (guiEvent.GetControl() == m_ObjectsList) {
					if (guiEvent.GetMsg() == GUIListBox::MouseDown) {
						m_ObjectsList->ScrollToSelected();
						if (const GUIListPanel::Item *objectListItem = m_ObjectsList->GetSelected()) {
							if (objectListItem->m_ExtraIndex >= 0) {
								ToggleObjectsListModuleGroupExpansion(objectListItem->m_ExtraIndex);
							} else if (objectListItem->m_pEntity) {
								SelectObjectByIndex(m_ObjectsList->GetSelectedIndex());
								return true;
							}
						}
					} else if (guiEvent.GetMsg() == GUIListBox::MouseMove) {
						const GUIListPanel::Item *objectListItem = m_ObjectsList->GetItem(m_CursorPos.GetFloorIntX(), m_CursorPos.GetFloorIntY());
						if (objectListItem && m_SelectedObjectIndex != objectListItem->m_ID) { SelectObjectByIndex(objectListItem->m_ID); }
					} else if (guiEvent.GetMsg() == GUIListBox::MouseEnter) {
						SetListFocus(PickerFocus::ObjectList);
					}
				} else {
					if (guiEvent.GetMsg() == GUIListBox::Click && m_CursorPos.GetFloorIntX() < m_ParentBox->GetXPos()) {
						return true;
					}
				}
			}
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::AnimateOpenClose() {
		
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ObjectPickerGUI::Draw(BITMAP *drawBitmap) const {
		AllegroScreen drawScreen(drawBitmap);
		m_GUIControlManager->Draw(&drawScreen);
		if (IsEnabled() && m_Controller->IsMouseControlled()) { draw_sprite(drawBitmap, s_Cursor, m_CursorPos.GetFloorIntX(), m_CursorPos.GetFloorIntY()); }
	}
}