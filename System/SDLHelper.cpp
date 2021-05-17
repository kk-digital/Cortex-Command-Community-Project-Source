#include "SDLHelper.h"

/// <summary>
/// Shift the Position of an SDL_Rect by an SDL_Point
/// </summary>
SDL_Rect operator+(const SDL_Rect &lhs, const SDL_Point &rhs) {
	return SDL_Rect{lhs.x + rhs.x, lhs.y + rhs.y, lhs.w, lhs.h};
}

SDL_Rect operator*(const SDL_Rect &lhs, const int rhs) {
	return SDL_Rect{lhs.x, lhs.y, lhs.w * rhs, lhs.h * rhs};
}

SDL_Rect operator*(const SDL_Rect &lhs, const float rhs) {
	return lhs * static_cast<int>(rhs);
}

SDL_Rect operator*(const SDL_Rect &lhs, const double rhs) {
	return lhs * static_cast<int>(rhs);
}

/// <summary>
/// Shift the Position of an SDL_Rect by -SDL_Point
/// </summary>
SDL_Rect operator-(const SDL_Rect &lhs, const SDL_Point &rhs) {
	return SDL_Rect{lhs.x - rhs.x, lhs.y - rhs.y, lhs.w, lhs.h};
}

/// <summary>
/// Add two SDL_Points
/// </summary>
SDL_Point operator+(const SDL_Point &lhs, const SDL_Point &rhs) {
	return SDL_Point{lhs.x + rhs.x, lhs.x + rhs.y};
}

/// <summary>
/// Subtract two SDL_Points
/// </summary>
SDL_Point operator-(const SDL_Point &lhs, const SDL_Point &rhs) {
	return SDL_Point{lhs.x - rhs.x, lhs.y - rhs.x};
}
