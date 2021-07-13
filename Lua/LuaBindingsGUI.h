#ifndef _RTELUABINDGUIS_
#define _RTELUABINDGUIS_

#include "LuaMacros.h"

#include "GameActivity.h"
#include "GUIBanner.h"
#include "BuyMenuGUI.h"
#include "SceneEditorGUI.h"

namespace RTE {

	/// <summary>
	/// 
	/// </summary>
	struct GUILuaBindings {

		LuaBindingRegisterFunctionForType(GUIBanner) {
			return luabind::class_<GUIBanner>("GUIBanner")
				.enum_("AnimMode")[
					luabind::value("BLINKING", GUIBanner::AnimMode::BLINKING),
					luabind::value("FLYBYLEFTWARD", GUIBanner::AnimMode::FLYBYLEFTWARD),
					luabind::value("FLYBYRIGHTWARD", GUIBanner::AnimMode::FLYBYRIGHTWARD),
					luabind::value("ANIMMODECOUNT", GUIBanner::AnimMode::ANIMMODECOUNT)
				]
				.enum_("AnimState")[
					luabind::value("NOTSTARTED", GUIBanner::AnimState::NOTSTARTED),
					luabind::value("SHOWING", GUIBanner::AnimState::SHOWING),
					luabind::value("SHOW", GUIBanner::AnimState::SHOW),
					luabind::value("HIDING", GUIBanner::AnimState::HIDING),
					luabind::value("OVER", GUIBanner::AnimState::OVER),
					luabind::value("ANIMSTATECOUNT", GUIBanner::AnimState::ANIMSTATECOUNT)
				]
				.enum_("BannerColor")[
					luabind::value("RED", GameActivity::BannerColor::RED),
					luabind::value("YELLOW", GameActivity::BannerColor::YELLOW)
				]
				.property("BannerText", &GUIBanner::GetBannerText)
				.property("AnimState", &GUIBanner::GetAnimState)
				.def("IsVisible", &GUIBanner::IsVisible)
				.property("Kerning", &GUIBanner::GetKerning, &GUIBanner::SetKerning)
				.def("ShowText", &GUIBanner::ShowText)
				.def("HideText", &GUIBanner::HideText)
				.def("ClearText", &GUIBanner::ClearText);
		}

		LuaBindingRegisterFunctionForType(BuyMenuGUI) {
			return luabind::class_<BuyMenuGUI>("BuyMenuGUI")
				.def("SetMetaPlayer", &BuyMenuGUI::SetMetaPlayer)
				.def("SetNativeTechModule", &BuyMenuGUI::SetNativeTechModule)
				.def("SetForeignCostMultiplier", &BuyMenuGUI::SetForeignCostMultiplier)
				.def("SetModuleExpanded", &BuyMenuGUI::SetModuleExpanded)
				.def("LoadAllLoadoutsFromFile", &BuyMenuGUI::LoadAllLoadoutsFromFile)
				.def("AddAllowedItem", &BuyMenuGUI::AddAllowedItem)
				.def("RemoveAllowedItem", &BuyMenuGUI::RemoveAllowedItem)
				.def("ClearAllowedItems", &BuyMenuGUI::ClearAllowedItems)
				.def("AddAlwaysAllowedItem", &BuyMenuGUI::AddAlwaysAllowedItem)
				.def("RemoveAlwaysAllowedItem", &BuyMenuGUI::RemoveAlwaysAllowedItem)
				.def("ClearAlwaysAllowedItems", &BuyMenuGUI::ClearAlwaysAllowedItems)
				.def("AddProhibitedItem", &BuyMenuGUI::AddProhibitedItem)
				.def("RemoveProhibitedItem", &BuyMenuGUI::RemoveProhibitedItem)
				.def("ClearProhibitedItems", &BuyMenuGUI::ClearProhibitedItems)
				.def("ForceRefresh", &BuyMenuGUI::ForceRefresh)
				.def("SetOwnedItemsAmount", &BuyMenuGUI::SetOwnedItemsAmount)
				.def("GetOwnedItemsAmount", &BuyMenuGUI::GetOwnedItemsAmount)
				.def("SetHeaderImage", &BuyMenuGUI::SetHeaderImage)
				.def("SetLogoImage", &BuyMenuGUI::SetLogoImage)
				.def("ClearCartList", &BuyMenuGUI::ClearCartList)
				.def("LoadDefaultLoadoutToCart", &BuyMenuGUI::LoadDefaultLoadoutToCart)
				.property("ShowOnlyOwnedItems", &BuyMenuGUI::GetOnlyShowOwnedItems, &BuyMenuGUI::SetOnlyShowOwnedItems)
				.property("EnforceMaxPassengersConstraint", &BuyMenuGUI::EnforceMaxPassengersConstraint, &BuyMenuGUI::SetEnforceMaxPassengersConstraint)
				.property("EnforceMaxMassConstraint", &BuyMenuGUI::EnforceMaxMassConstraint, &BuyMenuGUI::SetEnforceMaxMassConstraint);
		}

		LuaBindingRegisterFunctionForType(SceneEditorGUI) {
			return luabind::class_<SceneEditorGUI>("SceneEditorGUI")
				.enum_("EditorGUIMode")[
					luabind::value("INACTIVE", SceneEditorGUI::EditorGUIMode::INACTIVE),
					luabind::value("PICKINGOBJECT", SceneEditorGUI::EditorGUIMode::PICKINGOBJECT),
					luabind::value("ADDINGOBJECT", SceneEditorGUI::EditorGUIMode::ADDINGOBJECT),
					luabind::value("INSTALLINGBRAIN", SceneEditorGUI::EditorGUIMode::INSTALLINGBRAIN),
					luabind::value("PLACINGOBJECT", SceneEditorGUI::EditorGUIMode::PLACINGOBJECT),
					luabind::value("MOVINGOBJECT", SceneEditorGUI::EditorGUIMode::MOVINGOBJECT),
					luabind::value("DELETINGOBJECT", SceneEditorGUI::EditorGUIMode::DELETINGOBJECT),
					luabind::value("PLACEINFRONT", SceneEditorGUI::EditorGUIMode::PLACEINFRONT),
					luabind::value("PLACEBEHIND", SceneEditorGUI::EditorGUIMode::PLACEBEHIND),
					luabind::value("DONEEDITING", SceneEditorGUI::EditorGUIMode::DONEEDITING),
					luabind::value("EDITORGUIMODECOUNT", SceneEditorGUI::EditorGUIMode::EDITORGUIMODECOUNT)
				]
				.def("SetCursorPos", &SceneEditorGUI::SetCursorPos)
				.property("EditorMode", &SceneEditorGUI::GetEditorGUIMode, &SceneEditorGUI::SetEditorGUIMode)
				.def("GetCurrentObject", &SceneEditorGUI::GetCurrentObject)
				.def("SetCurrentObject", &SceneEditorGUI::SetCurrentObject)
				.def("SetModuleSpace", &SceneEditorGUI::SetModuleSpace)
				.def("SetNativeTechModule", &SceneEditorGUI::SetNativeTechModule)
				.def("SetForeignCostMultiplier", &SceneEditorGUI::SetForeignCostMultiplier)
				.def("TestBrainResidence", &SceneEditorGUI::TestBrainResidence);
		}
	};
}
#endif