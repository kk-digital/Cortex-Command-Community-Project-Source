#ifndef _RTESDLINPUT_
#define _RTESDLINPUT_

#include "GUIInput.h"
#include <SDL2/SDL.h>

namespace RTE {

	class SDLInput : public GUIInput {
	public:
		SDLInput(int whichPlayer, bool keyJoyMouseCursor= false);

		~SDLInput();

		void Update() override;

	private:
		void ConvertKeyEvent(int sdlKey, int guilibKey, float elapsedS);

		bool CreateTimer();

		float GetTime();
	};
} // namespace RTE
#endif
