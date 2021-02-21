#ifndef _GUI_
#define _GUI_

#pragma region Rectangle Structure
/// <summary>
/// The GUIRect structure defines a rectangle by the coordinates of its upper-left and lower-right corners as well as width and height.
/// </summary>
struct GUIRect { int x; int y; int w; int h;};

/// <summary>
/// Sets the bounds of a GUIRect.
/// </summary>
/// <param name="pRect">Pointer to the GUIRect.</param>
/// <param name="x">Position of top left corner on X axis.</param>
/// <param name="y">Position of top left corner on Y axis.</param>
/// <param name="width">Width of the Rectangle<./param>
/// <param name="height">Height of the Rectangle.</param>
inline void SetRect(GUIRect *rect, int x, int y, int width, int height){
	rect->x = x; rect->y = y; rect->w = width; rect->h = height;
}
#pragma endregion

#include "RTETools.h"

#include "GUI/Interface.h"
#include "GUI/GUIProperties.h"
#include "GUI/GUIInput.h"
#include "GUI/GUIFont.h"
#include "GUI/GUISkin.h"
#include "GUI/GUIPanel.h"
#include "GUI/GUIManager.h"
#include "GUI/GUIUtil.h"
#include "GUI/GUIControl.h"
#include "GUI/GUIEvent.h"
#include "GUI/GUIControlFactory.h"
#include "GUI/GUIControlManager.h"
#include "GUI/GUISound.h"

// TODO: find better way to do this probably through RTETools
#if defined(__linux__)
#include <strings.h>
#define stricmp strcasecmp
#endif

#endif
