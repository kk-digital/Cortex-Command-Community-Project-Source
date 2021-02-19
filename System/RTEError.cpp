#include <SDL2/SDL.h>

#include "RTEError.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	extern void ShowMessageBox(const std::string &message) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RTE Aborted! (x_x)", message.c_str(), NULL); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	extern bool RTEAbortFunc(const std::string &description, const char *file, int line) {
    // TODO: Implement for SDL
    /*
		// Save out the screen bitmap, after making a copy of it, faster sometimes
		if (screen) {
			BITMAP *abortScreenBuffer = create_bitmap(screen->w, screen->h);
			blit(screen, abortScreenBuffer, 0, 0, 0, 0, screen->w, screen->h);
			PALETTE palette;
			get_palette(palette);
			save_bmp("AbortScreen.bmp", abortScreenBuffer, palette);
			destroy_bitmap(abortScreenBuffer);
		}
		// Ditch the video mode so the message box appears without problems
		if (screen != nullptr) { set_gfx_mode(GFX_TEXT, 0, 0, 0, 0); }
		set_window_title("RTE Aborted! (x_x)");

		std::string abortMessage;

#ifndef RELEASE_BUILD
		// Show message box with explanation
		abortMessage = "Runtime Error in file " + file + ", line " + std::to_string(line) + ", because:\n\n" + description + "\n\nThe last frame has been dumped to 'AbortScreen.bmp'";
#else
		// Shortened and less confusing one. users have no use of knowing which source file and where.
		abortMessage = description + "\n\nThe last frame has been dumped to 'AbortScreen.bmp'";
#endif
		ShowMessageBox(abortMessage);

		// True so that the debugbreak code is run and the debugger goes there.
    */
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	extern bool RTEAssertFunc(bool expression, const char *description, const char *file, int line, bool &alwaysIgnore) {
		if (!expression) {
			// TODO: Make this display a box in the game asking whether to ignore or abort. For now, always abort.
			RTEAbortFunc(description, __FILE__, __LINE__);

			// True so that the debugbreak code is run and the debugger goes there.
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	extern bool RTEAssertFunc(bool expression, std::string description, const char *file, int line, bool &alwaysIgnore) {
		return RTEAssertFunc(expression, description.c_str(), file, line, alwaysIgnore);
	}
}
