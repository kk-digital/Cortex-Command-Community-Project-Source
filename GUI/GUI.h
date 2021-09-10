#ifndef _GUI_
#define _GUI_

#ifndef GUI_STANDALONE
#define GUIAssert(expression, description) RTEAssert(expression, description)
#define GUIAbort(description) RTEAbort(description)
#else
#define GUIAssert(expression, description) assert(expression)
#define GUIAbort(description) abort()
#endif

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

#ifndef GUI_STANDALONE
#include "RTETools.h"
#endif

#include "GUIInterface.h"
#include "GUIProperties.h"
#include "GUIInput.h"
#include "GUIFont.h"
#include "GUISkin.h"
#include "GUIPanel.h"
#include "GUIManager.h"
#include "GUIUtil.h"
#include "GUIControl.h"
#include "GUIEvent.h"
#include "GUIControlFactory.h"
#include "GUIControlManager.h"

#ifndef GUI_STANDALONE
#include "GUISound.h"
#endif

// TODO: find better way to do this probably through RTETools
#if defined(__linux__)
#include <strings.h>
#define stricmp strcasecmp
#endif

#endif
