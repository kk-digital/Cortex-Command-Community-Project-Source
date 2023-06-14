//////////////////////////////////////////////////////////////////////////////////////////
// File:            BuyMenuGUI.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Implementation file for the BuyMenuGUI class
// Project:         GUI Library
// Author(s):       Daniel Tabar
//                  dtabar@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "BuyMenuGUI.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "UInputMan.h"
#include "MetaMan.h"
#include "SettingsMan.h"

#include "GUI.h"
#include "AllegroBitmap.h"
#include "AllegroScreen.h"
#include "AllegroInput.h"
#include "GUIControlManager.h"
#include "GUICollectionBox.h"
#include "GUITab.h"
#include "GUIListBox.h"
#include "GUITextBox.h"
#include "GUIButton.h"
#include "GUILabel.h"

#include "DataModule.h"
#include "Controller.h"
#include "SceneObject.h"
#include "MovableObject.h"
#include "MOSprite.h"
#include "HeldDevice.h"
#include "AHuman.h"
#include "ACraft.h"

using namespace RTE;

BITMAP *RTE::BuyMenuGUI::s_pCursor = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this BuyMenuGUI, effectively
//                  resetting the members of this abstraction level only.

void BuyMenuGUI::Clear()
{
    m_pController = 0;
    m_pGUIScreen = 0;
    m_pGUIInput = 0;
    m_pGUIController = 0;
    m_MenuEnabled = DISABLED;
    m_MenuFocus = OK;
    m_FocusChange = false;
    m_MenuCategory = CRAFT;
    m_MenuSpeed = 8.0;
    m_ListItemIndex = 0;
    m_LastHoveredMouseIndex = 0;
    m_BlinkTimer.Reset();
    m_BlinkMode = NOBLINK;
    m_MenuTimer.Reset();
    m_RepeatStartTimer.Reset();
    m_RepeatTimer.Reset();
    m_pParentBox = 0;
    m_pPopupBox = 0;
    m_pPopupText = 0;
    m_pLogo = 0;
    for (int i = 0; i < CATEGORYCOUNT; ++i)
    {
        m_pCategoryTabs[i] = 0;
        m_CategoryItemIndex[i] = 0;
    }
    m_MetaPlayer = Players::NoPlayer;
    m_NativeTechModule = 0;
    m_ForeignCostMult = 4.0;
    int moduleCount = g_PresetMan.GetTotalModuleCount();
    m_aExpandedModules = new bool[moduleCount];
    for (int i = 0; i < moduleCount; ++i)
        m_aExpandedModules[i] = i == 0 ? true : false;
    m_pShopList = 0;
    m_pCartList = 0;
    m_pCraftBox = 0;

	m_pCraftCollectionBox = 0;
	m_pCraftNameLabel = 0;
	m_pCraftPriceLabel = 0;
	m_pCraftPassengersCaptionLabel = 0;
	m_pCraftPassengersLabel = 0;
	m_pCraftMassCaptionLabel = 0;
	m_pCraftMassLabel = 0;

    m_pSelectedCraft = 0;
    m_pCostLabel = 0;
    m_pBuyButton = 0;
    m_pSaveButton = 0;
    m_pClearButton = 0;
    m_Loadouts.clear();
    m_PurchaseMade = false;
    m_CursorPos.Reset();

	m_EnforceMaxPassengersConstraint = true;
	m_EnforceMaxMassConstraint = true;

	m_OnlyShowOwnedItems = false;
	m_AllowedItems.clear();
	m_AlwaysAllowedItems.clear();
	m_OwnedItems.clear();

    m_SelectingEquipment = false;
    m_LastVisitedEquipmentTab = GUNS;
    m_LastVisitedMainTab = BODIES;
    m_LastEquipmentScrollPosition = -1;
    m_LastMainScrollPosition = -1;
    m_FirstMainTab = CRAFT;
    m_LastMainTab = SETS;
    m_FirstEquipmentTab = TOOLS;
    m_LastEquipmentTab = SHIELDS;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the BuyMenuGUI object ready for use.

int BuyMenuGUI::Create(Controller* pController) { return 0; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the BuyMenuGUI object.

void BuyMenuGUI::Destroy()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:			SetHeaderImage
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the header image to the one specified in path

void BuyMenuGUI::SetHeaderImage(string path)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:			SetLogoImage
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the logo image to the one specified in path

void BuyMenuGUI::SetLogoImage(string path)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:			ClearCartList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clear the cart out of items selected for purchase

void BuyMenuGUI::ClearCartList()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  LoadAllLoadoutsFromFile
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads or re-loads all the loadout presets from the appropriate files
//                  on disk. This will first clear out all current loadout presets!

bool BuyMenuGUI::LoadAllLoadoutsFromFile()
{
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SaveAllLoadoutsToFile
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves all the loadouts to appropriate file on disk. Does NOT save
//                  any named presets which will be loaded from the standard preset
//                  loadouts first anyway.

bool BuyMenuGUI::SaveAllLoadoutsToFile()
{
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Enables or disables the menu. This will animate it in and out of view.

void BuyMenuGUI::SetEnabled(bool enable)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPosOnScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets where on the screen that this GUI is being drawn to. If upper
//                  left corner, then 0, 0. This will affect the way the mouse is positioned
//                  etc.

void BuyMenuGUI::SetPosOnScreen(int newPosX, int newPosY)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMetaPlayer
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets which MetaPlayer uses this menu, if any.

void BuyMenuGUI::SetMetaPlayer(int metaPlayer)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetNativeTechModule
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets which DataModule ID should be treated as the native tech of the
//                  user of this menu.

void BuyMenuGUI::SetNativeTechModule(int whichModule)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetModuleExpanded
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets whether a data module shown in the item menu should be expanded
//                  or not.

void BuyMenuGUI::SetModuleExpanded(int whichModule, bool expanded)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetOrderList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return the list of things currently in the purchase order list box.

bool BuyMenuGUI::GetOrderList(list<const SceneObject *> &listToFill)
{
    return true;
}

bool BuyMenuGUI::CommitPurchase(string presetName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTotalOrderCost
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return teh total cost of everything listed in the order box.

float BuyMenuGUI::GetTotalOrderCost()
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


float BuyMenuGUI::GetTotalOrderMass() const {
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float BuyMenuGUI::GetCraftMass() {
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTotalOrderPassengers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return the total number of passengers in the order box.
// Arguments:       None.
// Return value:    The total number of passengers.

int BuyMenuGUI::GetTotalOrderPassengers() const {
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::EnableEquipmentSelection(bool enabled) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::RefreshTabDisabledStates() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this Menu each frame

void BuyMenuGUI::Update() {}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the menu

void BuyMenuGUI::Draw(BITMAP *drawBitmap) const
{
    AllegroScreen drawScreen(drawBitmap);
    m_pGUIController->Draw(&drawScreen);

    // Draw the cursor on top of everything 
    if (IsEnabled() && m_pController->IsMouseControlled())
//        m_pGUIController->DrawMouse();
        draw_sprite(drawBitmap, s_pCursor, m_CursorPos.GetFloorIntX(), m_CursorPos.GetFloorIntY());
}

void BuyMenuGUI::CategoryChange(bool focusOnCategoryTabs)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  SaveCurrentLoadout
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the current loadout into a Set.

void BuyMenuGUI::SaveCurrentLoadout()
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  DeployLoadout
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads the loadout set into the cart, replacing whatever's there now.

bool BuyMenuGUI::DeployLoadout(int index)
{
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  AddObjectsToItemList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds all objects of a specific type already defined in PresetMan
//                  to the current shop/item list. They will be grouped into the different
//                  data modules they were read from.

void BuyMenuGUI::AddObjectsToItemList(vector<list<Entity *> > &moduleList, string type, string group)
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddPresetsToItemList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds all loadout presets' representations to the item GUI list.

void BuyMenuGUI::AddPresetsToItemList()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::UpdateTotalCostLabel(int whichTeam) {
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::UpdateTotalMassLabel(const ACraft* pCraft, GUILabel* pLabel) const {
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::UpdateTotalPassengersLabel(const ACraft* pCraft, GUILabel* pLabel) const {
	if (!pLabel) {
		return;
	}

	std::string display;
	if (pCraft && pCraft->GetMaxInventoryMass() != 0 && pCraft->GetMaxPassengers() != 0) {
		display = std::to_string(GetTotalOrderPassengers());
		if (pCraft->GetMaxPassengers() > 0) {
			display += " / " + std::to_string(pCraft->GetMaxPassengers());
		}
	} else {
		display = "NO SPACE";
	}

	pLabel->SetText(display);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BuyMenuGUI::TryPurchase()
{
}

