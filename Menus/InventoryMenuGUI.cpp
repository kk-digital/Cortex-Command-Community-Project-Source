#include "InventoryMenuGUI.h"

#include "FrameMan.h"
#include "UInputMan.h"
#include "MovableMan.h"
#include "PresetMan.h"

#include "Controller.h"
#include "AHuman.h"
#include "HDFirearm.h"
#include "Icon.h"

#include "AllegroBitmap.h"
#include "GUIFont.h"

#include "GUIControlManager.h"
#include "GUICollectionBox.h"
#include "GUILabel.h"
#include "GUIButton.h"
#include "GUIScrollbar.h"

namespace RTE {

	const Vector InventoryMenuGUI::c_CarouselBoxMaxSize(50, 32);
	const Vector InventoryMenuGUI::c_CarouselBoxMinSize(40, 24);
	const Vector InventoryMenuGUI::c_CarouselBoxSizeStep = (c_CarouselBoxMaxSize - c_CarouselBoxMinSize) / (c_ItemsPerRow / 2);
	const int InventoryMenuGUI::c_CarouselBoxCornerRadius = ((c_CarouselBoxMaxSize.GetFloorIntY() - c_CarouselBoxMinSize.GetFloorIntY()) / 2) - 1;

	BITMAP *InventoryMenuGUI::s_CursorBitmap = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::CarouselItemBox::GetIconsAndMass(std::vector<BITMAP*>& itemIcons, float& totalItemMass, const std::vector<std::pair<MovableObject*, MovableObject*>>* equippedItems) const {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::Clear(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int InventoryMenuGUI::Create(Controller* activityPlayerController, Actor* inventoryActor, MenuMode menuMode) { return 0; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::Destroy() {
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int InventoryMenuGUI::SetupCarouselMode() { return 0; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int InventoryMenuGUI::SetupFullOrTransferMode() { return 0; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::SetInventoryActor(Actor *newInventoryActor){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::SetEnabled(bool enable){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool InventoryMenuGUI::EnableIfNotEmpty() {
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::ClearSelectedItem() {
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::SetSelectedItem(GUIButton *selectedItemButton, MovableObject *selectedItemObject, int inventoryIndex, int equippedItemIndex, bool isBeingDragged) {
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::Update(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::Draw(BITMAP *targetBitmap, const Vector &targetPos) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateCarouselMode(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateCarouselItemBoxSizesAndPositions(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullMode(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullModeEquippedItemButtons(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullModeScrollbar(const std::deque<MovableObject *> *inventory){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullModeInventoryItemButtons(const std::deque<MovableObject *> *inventory){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullModeInformationText(const std::deque<MovableObject *> *inventory){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateFullModeNonItemButtonIconsAndHighlightWidths(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::UpdateTransferMode() {
		//TODO Make Transfer mode
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::HandleInput(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool InventoryMenuGUI::HandleMouseInput() { return false; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::HandleNonMouseInput(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Directions InventoryMenuGUI::GetNonMouseButtonControllerMovement() { return None; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIButton* InventoryMenuGUI::HandleNonMouseUpInput() { return NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIButton * InventoryMenuGUI::HandleNonMouseDownInput() { return NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIButton* InventoryMenuGUI::HandleNonMouseLeftInput() { return NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIButton* InventoryMenuGUI::HandleNonMouseRightInput() { return NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::HandleItemButtonPressOrHold(GUIButton* pressedButton, MovableObject* buttonObject, int buttonEquippedItemIndex, bool buttonHeld) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::SwapEquippedItemAndInventoryItem(int equippedItemIndex, int inventoryItemIndex) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::ReloadSelectedItem(){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::DropSelectedItem(const Vector *dropDirection){}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::DrawCarouselMode(BITMAP *targetBitmap, const Vector &drawPos) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::DrawCarouselItemBoxBackground(const CarouselItemBox &itemBoxToDraw) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::DrawCarouselItemBoxForeground(const CarouselItemBox &itemBoxToDraw, AllegroBitmap *carouselAllegroBitmap) const{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void InventoryMenuGUI::DrawFullMode(BITMAP *targetBitmap, const Vector &drawPos) const{}
}