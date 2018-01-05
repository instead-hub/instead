/*
 * Copyright 2009-2017 Peter Kosyh <p.kosyh at gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "externals.h"
#include "internals.h"

#include <SDL.h>

static int m_focus = 1;
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
	memset(&event, 0, sizeof(event));
	memset(&uevent, 0, sizeof(uevent));
	uevent.type = SDL_USEREVENT;
	uevent.code = 0;
	event.type = SDL_USEREVENT;
	uevent.data1 = (void*) p;
	uevent.data2 = data;
	event.user = uevent;
	SDL_PushEvent(&event);
}

#ifdef SAILFISHOS
static SDL_FingerID finger_mouse = 0;

static void push_mouse_event(SDL_Event *sevent)
{
	SDL_Event event;
	memset(&event, 0, sizeof(event));
	if (sevent->type == SDL_FINGERDOWN) {
		finger_mouse = sevent->tfinger.fingerId;
		event.type = SDL_MOUSEBUTTONDOWN;
	} else if (sevent->type == SDL_FINGERUP) {
		finger_mouse = 0;
		event.type = SDL_MOUSEBUTTONUP;
	} else if (sevent->type == SDL_FINGERMOTION) {
		if (sevent->tfinger.fingerId != finger_mouse)
			return;
		event.type = SDL_MOUSEMOTION;
	}
	event.button.x = sevent->tfinger.x;
	event.button.y = sevent->tfinger.y;
	event.button.clicks = 1;
	event.button.button = 1;
	SDL_PushEvent(&event);
}
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
static unsigned long last_press_ms = 0;
static unsigned long last_repeat_ms = 0;
extern void gfx_finger_pos_scale(float x, float y, int *ox, int *oy);
#endif
#define INPUT_REP_DELAY_MS 500
#define INPUT_REP_INTERVAL_MS 30

#ifdef IOS
int HandleAppEvents(void *userdata, SDL_Event *event)
{
	switch (event->type) {
	case SDL_APP_LOWMEMORY:
		return 0;
	case SDL_APP_WILLENTERBACKGROUND:
		/* Prepare your app to go into the background.  Stop loops, etc.
		This gets called when the user hits the home button, or gets a call.
		*/
		return 0;
	case SDL_APP_DIDENTERBACKGROUND:
		/* This will get called if the user accepted whatever sent your app to the background.
		If the user got a phone call and canceled it, you'll instead get an SDL_APP_DIDENTERFOREGROUND event and restart your loops.
		When you get this, you have 5 seconds to save all your state or the app will be terminated.
		Your app is NOT active at this point.
		*/
		/* snd_pause(1); */
		m_minimized = 1;
		return 0;
	case SDL_APP_WILLENTERFOREGROUND:
		/* This call happens when your app is coming back to the foreground.
			Restore all your state here.
		*/
		return 0;
	case SDL_APP_DIDENTERFOREGROUND:
		/* Restart your loops here.
		Your app is interactive and getting CPU again.
		*/
		/* snd_pause(0); */
		m_minimized = 0;
		return 0;
	case SDL_APP_TERMINATING:
		cfg_save();
		game_done(0);
		snd_done();
		gfx_video_done();
		gfx_done();
		return 0;
	default:
		/* No special processing, add it to the event queue */
		return 1;
	}
}
#endif

int input_init(void)
{
#if SDL_VERSION_ATLEAST(2,0,0)
	/* SDL_EnableKeyRepeat(500, 30); */ /* TODO ? */
	last_press_ms = 0;
	last_repeat_ms = 0;
#else
	SDL_EnableKeyRepeat(INPUT_REP_DELAY_MS, INPUT_REP_INTERVAL_MS);
#endif
#ifdef IOS
	SDL_SetEventFilter(HandleAppEvents, NULL);
#endif
	return 0;
}

void input_clear(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event));
	return;
}

void input_uevents(void)
{
	SDL_Event peek;
#if SDL_VERSION_ATLEAST(1,3,0)
	while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_USEREVENT, SDL_USEREVENT) > 0) {
#else
	while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_EVENTMASK (SDL_USEREVENT)) > 0) {
#endif
		void (*p) (void*) = (void (*)(void*)) peek.user.data1;
		if (p)
			p(peek.user.data2);
	}
}

#if SDL_VERSION_ATLEAST(1,3,0)
static void key_compat(struct inp_event *inp)
{
	int len = strlen(inp->sym);
	if (!strcmp(inp->sym, "pageup"))
		strcpy(inp->sym, "page up");
	else if (!strcmp(inp->sym, "pagedown"))
		strcpy(inp->sym, "page down");
	else if (!strcmp(inp->sym, "numlock"))
		strcpy(inp->sym, "num lock");
	else if (!strcmp(inp->sym, "scrolllock"))
		strcpy(inp->sym, "scroll lock");
	else if (!strcmp(inp->sym, "capslock"))
		strcpy(inp->sym, "caps lock");
	else if (len >= 8 && !strncmp(inp->sym, "keypad ", 7)) {
		inp->sym[0] = '[';
		strcpy(inp->sym + 1, inp->sym + 7);
		strcpy(inp->sym + 1 + len - 7, "]");
	}
}
#endif
#if defined(IOS) || defined(SAILFISHOS)
static unsigned long touch_stamp = 0;
static int touch_num = 0;
#endif
int finger_pos(const char *finger, int *x, int *y, float *pressure)
{
#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_TouchID tid;
	SDL_FingerID fid;
	SDL_Finger *f;
	int i, n;
	i = hex2data(finger, &fid, sizeof(fid));

	if (i != sizeof(fid) * 2 || finger[i] != ':')
		return -1;
	if (hex2data(finger + i + 1, &tid, sizeof(tid)) != sizeof(tid) * 2)
		return -1;

	n = SDL_GetNumTouchFingers(tid);
	if (n <= 0)
		return -1;
	for (i = 0; i < n; i++) {
		f = SDL_GetTouchFinger(tid, i);
		if (f->id == fid) {
			if (pressure)
				*pressure = f->pressure;
			gfx_finger_pos_scale(f->x, f->y, x, y);
			return 0;
		}
	}
	return -1;
#else
	return -1;
#endif
}
int input(struct inp_event *inp, int wait)
{
	int rc;
	SDL_Event event;
	SDL_Event peek;
	memset(&event, 0, sizeof(event));
	memset(&peek, 0, sizeof(peek));

#if !defined(__EMSCRIPTEN__)
	if (wait) {
		rc = SDL_WaitEvent(&event);
	} else
#endif
		rc = SDL_PollEvent(&event);
	if (!rc)
		return 0;

	inp->sym[0] = 0;
	inp->type = 0;
	inp->count = 1;
	switch(event.type){
#if SDL_VERSION_ATLEAST(2,0,0)
	case SDL_MULTIGESTURE:
	case SDL_FINGERMOTION:
		if (DIRECT_MODE && !game_paused())
			return AGAIN;
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, event.type, event.type) > 0)
			return AGAIN; /* to avoid flickering */
#if defined(SAILFISHOS)
		push_mouse_event(&event);
#endif
		break;
	case SDL_FINGERUP:
#ifdef IOS
		touch_num = 0;
#endif
	case SDL_FINGERDOWN:
#if defined(IOS) || defined(SAILFISHOS)
		push_mouse_event(&event);
		if (event.type == SDL_FINGERDOWN) {
			if (gfx_ticks() - touch_stamp > 100) {
				touch_num = 0;
				touch_stamp = gfx_ticks();
			}
			touch_num ++;
			if (touch_num >= 3) {
				inp->type = KEY_DOWN;
				inp->code = 0;
				strncpy(inp->sym, "escape", sizeof(inp->sym));
				break;
			}
		}
#endif
		gfx_finger_pos_scale(event.tfinger.x, event.tfinger.y, &inp->x, &inp->y);
		inp->type = (event.type == SDL_FINGERDOWN) ? FINGER_DOWN : FINGER_UP;
		data2hex(&event.tfinger.fingerId,
			sizeof(event.tfinger.fingerId),
			inp->sym);
		inp->sym[sizeof(event.tfinger.fingerId) * 2] = ':';
		data2hex(&event.tfinger.touchId,
			sizeof(event.tfinger.touchId),
			inp->sym + sizeof(event.tfinger.fingerId) * 2 + 1);
		inp->sym[sizeof(event.tfinger.fingerId) * 2 + 1 + sizeof(event.tfinger.touchId) * 2] = 0;
		break;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
/*		case SDL_WINDOWEVENT_SHOWN: */
/*		case SDL_WINDOWEVENT_RESIZED: */
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			gfx_resize(event.window.data1, event.window.data2);
			/* Fall through */
		case SDL_WINDOWEVENT_EXPOSED:
			gfx_flip();
			gfx_commit();
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
			m_minimized = (event.window.event == SDL_WINDOWEVENT_MINIMIZED && !opt_fs);
			snd_pause(!nopause_sw && m_minimized);
			break;
#if defined(SAILFISHOS)
		case SDL_WINDOWEVENT_FOCUS_LOST:
			snd_pause(!nopause_sw);
			while (1) { /* pause */
				SDL_WaitEvent(&event);
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
					snd_pause(0);
					break;
				}
				if (event.type == SDL_QUIT) {
					game_running = 0;
					return -1;
				}
			}
			break;
#endif
		case SDL_WINDOWEVENT_ENTER:
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			m_focus = 1;
			if (opt_fs)
				mouse_cursor(0);
			break;
		case SDL_WINDOWEVENT_LEAVE:
			m_focus = 0;
			if (opt_fs)
				mouse_cursor(1); /* is it hack?*/
			break;
		default:
			break;
		}
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0)
			return AGAIN; /* to avoid flickering */
		return 0;
#else
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
#endif
	case SDL_USEREVENT: {
		void (*p) (void*) = (void (*)(void*))event.user.data1;
		if (!p) /* idle cycles */
			return 0;
		p(event.user.data2);
		return AGAIN;
		}
	case SDL_QUIT:
		game_running = 0;
		return -1;
	case SDL_KEYDOWN:	/* A key has been pressed */
#if SDL_VERSION_ATLEAST(2,0,0)
		if (event.key.repeat) {
			if (DIRECT_MODE && !game_paused()) /* do not send key repeats */
				return AGAIN;
			if (gfx_ticks() - last_press_ms < INPUT_REP_DELAY_MS)
				return AGAIN;
			if ((gfx_ticks() - last_repeat_ms) < INPUT_REP_INTERVAL_MS)
				return AGAIN;
			last_repeat_ms = gfx_ticks();
		} else {
			last_press_ms = gfx_ticks();
			last_repeat_ms = gfx_ticks();
		}
#endif
		inp->type = KEY_DOWN;
		inp->code = event.key.keysym.scancode;
#if SDL_VERSION_ATLEAST(1,3,0)
		strncpy(inp->sym, SDL_GetScancodeName(inp->code), sizeof(inp->sym));
#else
		strncpy(inp->sym, SDL_GetKeyName(event.key.keysym.sym), sizeof(inp->sym));
#endif
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
#if SDL_VERSION_ATLEAST(1,3,0)
		key_compat(inp);
#endif
#if SDL_VERSION_ATLEAST(1,3,0) /* strange bug in some SDL2 env, with up/down events storm */
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYUP) > 0) {
			if (peek.key.keysym.scancode == event.key.keysym.scancode &&
				peek.key.repeat == 0)
				return AGAIN;
		}
#endif
		break;
	case SDL_KEYUP:
		inp->type = KEY_UP;
		inp->code = event.key.keysym.scancode;
#if SDL_VERSION_ATLEAST(1,3,0)
		strncpy(inp->sym, SDL_GetScancodeName(inp->code), sizeof(inp->sym));
#else
		strncpy(inp->sym, SDL_GetKeyName(event.key.keysym.sym), sizeof(inp->sym));
#endif
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
#if SDL_VERSION_ATLEAST(1,3,0)
		key_compat(inp);
#endif
#if SDL_VERSION_ATLEAST(1,3,0) /* strange bug in some SDL2 env, with up/down events storm */
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYUP) > 0) {
			if (event.key.keysym.scancode == peek.key.keysym.scancode &&
				peek.key.repeat == 0)
				return AGAIN;
		}
#endif
		break;
	case SDL_MOUSEMOTION:
		m_focus = 1; /* ahhh */
		if (DIRECT_MODE && !game_paused())
			return AGAIN;
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
#if SDL_VERSION_ATLEAST(2,0,0)
	case SDL_MOUSEWHEEL:
		if (DIRECT_MODE && !game_paused())
			return AGAIN;

		inp->type = (event.wheel.y > 0) ? MOUSE_WHEEL_UP : MOUSE_WHEEL_DOWN;

		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_MOUSEWHEEL, SDL_MOUSEWHEEL) > 0) {
			if (!((event.wheel.y > 0 &&
				inp->type == MOUSE_WHEEL_UP) ||
			    (event.wheel.y < 0 &&
			    	inp->type == MOUSE_WHEEL_DOWN)))
				break;
			inp->count ++;
		}
		break;
#endif
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
	default:
		break;
	}
	return 1;
}
