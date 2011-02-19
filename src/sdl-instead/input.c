#include "externals.h"
#include "internals.h"

#include <SDL.h>

static int m_focus = 0;
static int m_minimized = 0;

int minimized(void)
{
	if (nopause_sw)
		return 0;
	return m_minimized;
}

int mouse_focus(void)
{
	return m_focus;
}

int mouse_cursor(int on)
{
	if (on)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
	return 0;
}

void push_user_event(void (*p) (void*), void *data)
{
	SDL_Event event;
	SDL_UserEvent uevent;
	uevent.type = SDL_USEREVENT;
	uevent.code = 0;
	event.type = SDL_USEREVENT;
	uevent.data1 = p;
	uevent.data2 = data;
	event.user = uevent;
	SDL_PushEvent(&event);
}

int input_init(void)
{
	SDL_EnableKeyRepeat(500, 30);
	m_focus = !!(SDL_GetAppState() & SDL_APPMOUSEFOCUS);
	return 0;
}

void input_clear(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event));
	return;
}

int input(struct inp_event *inp, int wait)
{	
	int rc;
	SDL_Event event;
	SDL_Event peek;
	memset(&event, 0, sizeof(event));
	memset(&peek, 0, sizeof(peek));
	if (wait) {
		rc = SDL_WaitEvent(&event);
	} else
		rc = SDL_PollEvent(&event);
	if (!rc)
		return 0;
	inp->sym[0] = 0;
	inp->type = 0;
	inp->count = 1;
	switch(event.type){
	case SDL_ACTIVEEVENT:
		if (event.active.state & SDL_APPACTIVE) {
			m_minimized = !event.active.gain;
			snd_pause(!nopause_sw && m_minimized);
		}
		if (event.active.state & (SDL_APPMOUSEFOCUS | SDL_APPINPUTFOCUS)) {
			if (event.active.gain) {
				m_focus = 1;
				if (opt_fs)
					mouse_cursor(0);
			} else if (event.active.state & SDL_APPMOUSEFOCUS) {
				m_focus = 0;
				if (opt_fs)
					mouse_cursor(1); /* is it hack?*/
			}
		}
#if SDL_VERSION_ATLEAST(1,3,0)
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_ACTIVEEVENT, SDL_ACTIVEEVENT) > 0)
#else
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_EVENTMASK(SDL_ACTIVEEVENT)) > 0)
#endif
			return AGAIN; /* to avoid flickering */
		return 0;
	case SDL_USEREVENT: {
		void (*p) (void*) = event.user.data1;
		p(event.user.data2);
		return AGAIN;
		}
	case SDL_QUIT:
		game_running = 0;
		return -1;
	case SDL_KEYDOWN:	//A key has been pressed
		inp->type = KEY_DOWN; 
		inp->code = event.key.keysym.scancode;
		strncpy(inp->sym, SDL_GetKeyName(event.key.keysym.sym), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
		break;
	case SDL_KEYUP:
		inp->type = KEY_UP; 
		inp->code = event.key.keysym.scancode;
		strncpy(inp->sym, SDL_GetKeyName(event.key.keysym.sym), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
		break;
	case SDL_MOUSEMOTION:
		m_focus = 1; /* ahhh */
		inp->type = MOUSE_MOTION;
		inp->x = event.button.x;
		inp->y = event.button.y;
#if SDL_VERSION_ATLEAST(1,3,0)
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION) > 0) {
#else
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_EVENTMASK (SDL_MOUSEMOTION)) > 0) {
#endif
			inp->x = peek.button.x;
			inp->y = peek.button.y;
		}
		break;
	case SDL_MOUSEBUTTONUP:	
		inp->type = MOUSE_UP;
		inp->x = event.button.x;
		inp->y = event.button.y;
		inp->code = event.button.button;
		if (event.button.button == 4)
			inp->type = 0;
		else if (event.button.button == 5)
			inp->type = 0;	
		break;
	case SDL_MOUSEBUTTONDOWN:
		m_focus = 1; /* ahhh */
		inp->type = MOUSE_DOWN;
		inp->x = event.button.x;
		inp->y = event.button.y;
		inp->code = event.button.button;
		if (event.button.button == 4)
			inp->type = MOUSE_WHEEL_UP;
		else if (event.button.button == 5)
			inp->type = MOUSE_WHEEL_DOWN;
#if SDL_VERSION_ATLEAST(1,3,0)
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONDOWN) > 0) {
#else
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_EVENTMASK (SDL_MOUSEBUTTONDOWN)) > 0) {
#endif
			if (!((event.button.button == 4 &&
				inp->type == MOUSE_WHEEL_UP) ||
			    (event.button.button == 5 &&
			    	inp->type == MOUSE_WHEEL_DOWN)))
				break;
			inp->count ++;
		}

		break;
	}
	return 1;
}

