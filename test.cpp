#include <SDL2/SDL.h>
#include "Managers/FrameMan.h"

using namespace RTE;

static bool quit = false;
int quit_handler(void* quit, SDL_Event* event){
	if((*event).type==SDL_QUIT){
		*static_cast<int*>(quit) = true;
	}
	return 1;
}

int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_AddEventWatch(quit_handler, &quit);
	g_FrameMan.Initialize();
	SDL_Event e;
	while(!quit){
		SDL_PollEvent(&e);
	}
	g_FrameMan.Destroy();
	SDL_Quit();
}
