#ifndef _RTESDLINPUT_
#define _RTESDLINPUT_

#include "GUIInput.h"

namespace RTE {

	class SDLInput : public GUIInput {
	public:
		SDLInput(int whichPlayer, bool keyJoyMouseCursor= false);

		~SDLInput() = default;

		void Update() override;

	private:
		void ConvertKeyEvent(int sdlKey, int guilibKey, float elapsedS);
	};
} // namespace RTE
#endif
