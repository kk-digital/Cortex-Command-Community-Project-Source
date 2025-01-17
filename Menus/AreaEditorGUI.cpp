//////////////////////////////////////////////////////////////////////////////////////////
// File:            AreaEditorGUI.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Implementation file for the AreaEditorGUI class
// Project:         GUI Library
// Author(s):       Daniel Tabar
//                  dtabar@datArealms.com
//                  http://www.datArealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "AreaEditorGUI.h"
#include "GUISound.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "UInputMan.h"

#include "Controller.h"
#include "MOSprite.h"
#include "HeldDevice.h"
#include "AHuman.h"
#include "ACrab.h"
#include "SLTerrain.h"
#include "AreaPickerGUI.h"
#include "PieMenuGUI.h"
#include "Scene.h"

using namespace RTE;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this AreaEditorGUI, effectively
//                  resetting the members of this abstraction level only.

void AreaEditorGUI::Clear()
{
    m_pController = 0;
    m_FullFeatured = false;
    m_EditMade = false;
    m_EditorGUIMode = PICKINGAREA;
    m_PreviousMode = PREADDMOVEBOX;
    m_BlinkTimer.Reset();
    m_BlinkMode = NOBLINK;
    m_RepeatStartTimer.Reset();
    m_RepeatTimer.Reset();
    m_pPieMenu = 0;
    m_ActivatedPieSliceType = PieSlice::PieSliceIndex::PSI_NONE;
    m_pPicker = 0;
    m_GridSnapping = true;
    m_CursorPos.Reset();
    m_CursorOffset.Reset();
    m_CursorInAir = true;
    m_pCurrentArea = 0;
    m_EditedBox.Reset();
    m_pBoxToBlink = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the AreaEditorGUI Area ready for use.

int AreaEditorGUI::Create(Controller* pController, bool fullFeatured, int whichModuleSpace) { return 0; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the AreaEditorGUI Area.

void AreaEditorGUI::Destroy(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetController
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the controller used by this. The ownership of the controller is
//                  NOT transferred!

void AreaEditorGUI::SetController(Controller *pController){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPosOnScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets where on the screen that this GUI is being drawn to. If upper
//                  left corner, then 0, 0. This will affect the way the mouse is positioned
//                  etc.

void AreaEditorGUI::SetPosOnScreen(int newPosX, int newPosY)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCurrentArea
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the currently selected Area of this Editor. Ownership IS NOT
//                  transferred!

void AreaEditorGUI::SetCurrentArea(Scene::Area *pArea){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdatePickerList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the list that the GUI's Area picker has, from the current
//                  scene state.

void AreaEditorGUI::UpdatePickerList(string selectAreaName){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this Menu each frame

void AreaEditorGUI::Update(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the menu

void AreaEditorGUI::Draw(BITMAP *pTargetBitmap, const Vector &targetPos) const{}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdatePieMenu
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the PieMenu config based ont eh current editor state.

void AreaEditorGUI::UpdatePieMenu(){}