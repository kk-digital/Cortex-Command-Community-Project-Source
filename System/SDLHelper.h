#ifndef _SDLHELPER_
#define _SDLHELPER_
/// <summary>
/// This header includes the SDL base header and adds some very useful
/// vector operators to SDL_Rect and SDL_Point
/// </summary>

#include <SDL2/SDL.h>


/// <summary>
/// Shift the Position of an SDL_Rect by an SDL_Point
/// </summary>
SDL_Rect operator+(const SDL_Rect &lhs, const SDL_Point &rhs){
	return SDL_Rect{lhs.x+rhs.x,lhs.y+rhs.y, lhs.w, lhs.h};
}

/// <summary>
/// Shift the Position of an SDL_Rect by -SDL_Point
/// </summary>
SDL_Rect operator-(const SDL_Rect &lhs, const SDL_Point &rhs){
	return SDL_Rect{lhs.x-rhs.x, lhs.y-rhs.y, lhs.w, lhs.h};
}


/// <summary>
/// Add two SDL_Points
/// </summary>
SDL_Point operator+(const SDL_Point &lhs, const SDL_Point &rhs){
	return SDL_Point{lhs.x+rhs.x, lhs.x+rhs.y};
}

/// <summary>
/// Subtract two SDL_Points
/// </summary>
SDL_Point operator-(const SDL_Point &lhs, const SDL_Point &rhs){
	return SDL_Point{lhs.x-rhs.x, lhs.y-rhs.x};
}

#endif
