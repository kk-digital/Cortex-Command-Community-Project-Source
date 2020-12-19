#ifndef _GUIRECTH_
#define _GUIRECTH_
/// <summary>
/// The GUIRect structure defines a rectangle by the coordinates of its
/// upper-left and lower-right corners.
/// </summary>
struct GUIRect {
	long left;
	long top;
	long right;
	long bottom;
};

/// <summary>
/// Sets the bounds of a GUIRect.
/// </summary>
/// <param name="pRect">Pointer to the GUIRect.</param>
/// <param name="left">Position of top left corner on X axis.</param>
/// <param name="top">Position of top left corner on Y axis.</param>
/// <param name="right">Position of bottom right corner on X axis.</param>
/// <param name="bottom">Position of bottom right corner on Y axis.</param>
void SetRect(GUIRect *pRect, int left, int top, int right, int bottom);
#endif
