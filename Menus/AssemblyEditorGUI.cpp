//////////////////////////////////////////////////////////////////////////////////////////
// File:            AssemblyEditorGUI.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Implementation file for the AssemblyEditorGUI class
// Project:         GUI Library
// Author(s):       Daniel Tabar
//                  dtabar@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "AssemblyEditorGUI.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "GameActivity.h"
#include "SceneEditor.h"
#include "UInputMan.h"

#include "Controller.h"
#include "SceneObject.h"
#include "MOSprite.h"
#include "HeldDevice.h"
#include "HDFirearm.h"
#include "TDExplosive.h"
#include "TerrainObject.h"
#include "AHuman.h"
#include "ACrab.h"
#include "SLTerrain.h"
#include "ObjectPickerGUI.h"
#include "PieMenuGUI.h"
#include "Scene.h"
#include "SettingsMan.h"

using namespace RTE;

#define MAXBRAINPATHCOST 10000
#define BLUEPRINTREVEALRATE 150
#define BLUEPRINTREVEALPAUSE 1500

BITMAP *AssemblyEditorGUI::s_pValidPathDot = 0;
BITMAP *AssemblyEditorGUI::s_pInvalidPathDot = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this AssemblyEditorGUI, effectively
//                  resetting the members of this abstraction level only.

void AssemblyEditorGUI::Clear(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the AssemblyEditorGUI object ready for use.

int AssemblyEditorGUI::Create(Controller* pController, FeatureSets featureSet, int whichModuleSpace, int nativeTechModule, float foreignCostMult) { return 0; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the AssemblyEditorGUI object.

void AssemblyEditorGUI::Destroy(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetController
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the controller used by this. The ownership of the controller is
//                  NOT transferred!

void AssemblyEditorGUI::SetController(Controller *pController){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPosOnScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets where on the screen that this GUI is being drawn to. If upper
//                  left corner, then 0, 0. This will affect the way the mouse is positioned
//                  etc.

void AssemblyEditorGUI::SetPosOnScreen(int newPosX, int newPosY){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCurrentObject
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the new Object to be held at the cursor of this Editor. Ownership
//                  IS transferred!

bool AssemblyEditorGUI::SetCurrentObject(SceneObject* pNewObject) { return false; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetModuleSpace
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets which DataModule space to be picking objects from. If -1, then
//                  let the player pick from all loaded modules.

void AssemblyEditorGUI::SetModuleSpace(int moduleSpaceID)
{}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetNativeTechModule
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets which DataModule ID should be treated as the native tech of the
//                  user of this menu.

void AssemblyEditorGUI::SetNativeTechModule(int whichModule){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetForeignCostMultiplier
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the multiplier of the cost of any foreign Tech items.

void AssemblyEditorGUI::SetForeignCostMultiplier(float newMultiplier){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TestBrainResidence
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether the resident brain is currently placed into a valid
//                  location in this scene, based on whether there is a clear path to the
//                  sky above it. This forces the editor into place brain mode with the
//                  current resident brain if the current placement is no bueno. It also
//                  removes the faulty brain from residence in the scene!

bool AssemblyEditorGUI::TestBrainResidence(bool noBrainIsOK)
{
	// Brain is fine, leave it be
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this Menu each frame

void AssemblyEditorGUI::Update(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the menu

void AssemblyEditorGUI::Draw(BITMAP *pTargetBitmap, const Vector &targetPos) const{}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdatePieMenu
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the PieMenu config based ont eh current editor state.

void AssemblyEditorGUI::UpdatePieMenu(){}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateBrainPath
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the brain path to the current resident brain, if any. If
//                  there's none, the path is cleared.

bool AssemblyEditorGUI::UpdateBrainPath() { return false; }