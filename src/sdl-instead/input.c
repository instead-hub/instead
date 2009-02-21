#include <SDL.h>
#include "input.h"
int input(struct inp_event *inp, int wait)
{	
	int rc;
	SDL_Event event;
	if (wait)
		rc = SDL_WaitEvent(&event);
	else
		rc = SDL_PollEvent(&event);
	if (!rc)
		return 0;
	inp->sym = NULL;	
	inp->type = 0;
	switch(event.type){
	case SDL_QUIT:
		return -1;
	case SDL_KEYDOWN:	//A key has been pressed
		inp->type = KEY_DOWN; 
		inp->code = event.key.keysym.scancode;
		inp->sym = SDL_GetKeyName(event.key.keysym.sym);
		break;
	case SDL_KEYUP:
		inp->type = KEY_UP; 
		inp->code = event.key.keysym.scancode;
		inp->sym = SDL_GetKeyName(event.key.keysym.sym);
		break;
	case SDL_MOUSEMOTION:
		inp->type = MOUSE_MOTION;
		inp->x = event.button.x;
		inp->y = event.button.y;
		break;
	case SDL_MOUSEBUTTONUP:	
		inp->type = MOUSE_UP;
		inp->x = event.button.x;
		inp->y = event.button.y;
		if (event.button.button == 4)
			inp->type = 0;
		else if (event.button.button == 5)
			inp->type = 0;
		break;
	case SDL_MOUSEBUTTONDOWN:
		inp->type = MOUSE_DOWN;
		inp->x = event.button.x;
		inp->y = event.button.y;
		if (event.button.button == 4)
			inp->type = MOUSE_WHEEL_UP;
		else if (event.button.button == 5)
			inp->type = MOUSE_WHEEL_DOWN;
		break;
	}
	return 1;
}

