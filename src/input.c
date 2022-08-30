/*
 * Copyright 2009-2022 Peter Kosyh <p.kosyh at gmail.com>
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
	if (nocursor_sw)
		return 0;
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

int system_clipboard(const char *text, char **buf)
{
	if (buf) {
		if (!SDL_HasClipboardText())
			return -1;
		*buf = SDL_GetClipboardText();
		if (*buf == NULL)
			return -1;
		return 0;
	}
	if (!text)
		return -1;
	return SDL_SetClipboardText(text);
}

#ifdef SAILFISHOS
static SDL_FingerID finger_mouse = 0;

static void touch_mouse_event(SDL_Event *sevent)
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

static unsigned long last_press_ms = 0;
static unsigned long last_repeat_ms = 0;
extern void gfx_finger_pos_scale(float x, float y, int *ox, int *oy, int norm);

#define INPUT_REP_DELAY_MS 500
#define INPUT_REP_INTERVAL_MS 30

#define GAMEPAD_TICKS 100

#if defined(IOS) || defined(ANDROID)
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
		if (opt_autosave && curgame_dir) /* autosave the game */
			game_save(0);
		cfg_save();
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
		game_flip();
		game_gfx_commit(0);
		return 0;
	case SDL_APP_TERMINATING:
#if !defined(ANDROID)
		cfg_save();
		game_done(0);
		snd_done();
		gfx_video_done();
		gfx_done();
#endif
		return 0;
	default:
		/* No special processing, add it to the event queue */
		return 1;
	}
}
#endif

static SDL_GameController *gamepad = NULL;
static int deadzone = 8192;

const float MOUSE_SHIFT_MIN = 1.0;
const float MOUSE_SHIFT_MAX = 8.0;

static int gamepad_deadzone(SDL_GameController *g)
{
#if SDL_VERSION_ATLEAST(2,0,12)
	switch (SDL_GameControllerGetType(g))
	{
	case SDL_CONTROLLER_TYPE_UNKNOWN:
	case SDL_CONTROLLER_TYPE_XBOXONE:
	case SDL_CONTROLLER_TYPE_XBOX360:
	case SDL_CONTROLLER_TYPE_PS3:
		return 10000;
	case SDL_CONTROLLER_TYPE_PS4:
	case SDL_CONTROLLER_TYPE_PS5:
		return 4096;
	case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
		return 8192; // Actual dead-zone should be closer to 10% of full-scale, but we use this to account for variances in 3rd-party controllers
	case SDL_CONTROLLER_TYPE_VIRTUAL:
		return 8192;
	default:
		return 8192;
	}
#else
	return 8192;
#endif
}

static int gamepad_mouse_shift(int axis_value) {
	float range;
	if (abs(axis_value) < deadzone)
		return 0;
	range = 32768.0 - deadzone;
	if (axis_value > deadzone)
		return MOUSE_SHIFT_MIN + (MOUSE_SHIFT_MAX - MOUSE_SHIFT_MIN) * (axis_value - deadzone) / range;
	else
		return -MOUSE_SHIFT_MIN + (MOUSE_SHIFT_MAX - MOUSE_SHIFT_MIN) * (axis_value + deadzone) / range;
}

static void gamepad_init(void)
{
	int i;
	static char gamepad_cfg[PATH_MAX] = "";
	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
		fprintf(stderr, "Couldn't initialize GameController subsystem: %s\n", SDL_GetError());
		return;
	}
	for (i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			gamepad = SDL_GameControllerOpen(i);
			if (gamepad) {
				fprintf(stderr, "Found gamepad: %s\n",
					SDL_GameControllerName(gamepad));
				deadzone = gamepad_deadzone(gamepad);
				break;
			} else {
				fprintf(stderr, "Could not open gamepad %i: %s\n", i, SDL_GetError());
			}
		}
	}
	snprintf(gamepad_cfg, sizeof(gamepad_cfg) - 1, "%s/gamecontrollerdb.txt", appdir());
	SDL_GameControllerAddMappingsFromFile(gamepad_cfg);
}

static void gamepad_done(void)
{
	if(gamepad)
		SDL_GameControllerClose(gamepad);
	if(SDL_WasInit(SDL_INIT_GAMECONTROLLER))
		SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}
static int gamepad_stamp = 0;
static int gamepad_mouse_event(SDL_Event *ev)
{
	int rc = 0;
	int axis_x = 0, axis_y = 0;
	SDL_Event event;

	memset(&event, 0, sizeof(event));
	gfx_cursor(&event.button.x, &event.button.y);
	event.type = SDL_MOUSEMOTION;
	event.button.clicks = 1;
	event.button.button = 1;

	if (ev->type == SDL_CONTROLLERBUTTONDOWN) {
		if (ev->cbutton.button != SDL_CONTROLLER_BUTTON_LEFTSTICK &&
			ev->cbutton.button != SDL_CONTROLLER_BUTTON_RIGHTSTICK)
			return 0;
		event.type = SDL_MOUSEBUTTONDOWN;
		rc = 1;
	} else if (ev->type == SDL_CONTROLLERBUTTONUP) {
		if (ev->cbutton.button != SDL_CONTROLLER_BUTTON_LEFTSTICK &&
			ev->cbutton.button != SDL_CONTROLLER_BUTTON_RIGHTSTICK)
			return 0;
		event.type = SDL_MOUSEBUTTONUP;
		rc = 1;
	} else if (ev->type == SDL_CONTROLLERAXISMOTION) {
		rc = 1;
	}

	if (gamepad) { /* poll mode */
		axis_x = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTX);
		axis_y = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTY);
		if (abs(axis_x) <= deadzone && abs(axis_y) <= deadzone) {
			axis_x = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX);
			axis_y = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY);
		}
		if (abs(axis_x) > deadzone || abs(axis_y) > deadzone) {
			rc = 1;
//			if (gfx_ticks() - gamepad_stamp <= GAMEPAD_TICKS) {
//				axis_x = 0; axis_y = 0;
//			} else
//				gamepad_stamp = gfx_ticks();
			gfx_cursor(&event.button.x, &event.button.y);
			event.button.x += gamepad_mouse_shift(axis_x);
			event.button.y += gamepad_mouse_shift(axis_y);
			gfx_warp_cursor(event.button.x, event.button.y);
		}
	}

	if (rc)
		SDL_PushEvent(&event);
	return rc;
}

static const char *gamepad_map(int button)
{
	switch(button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		return "up";
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		return "down";
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		return "left";
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		return "right";
	case SDL_CONTROLLER_BUTTON_A:
		return "return";
	case SDL_CONTROLLER_BUTTON_B:
		return "space";
	case SDL_CONTROLLER_BUTTON_X:
		return "tab";
	case SDL_CONTROLLER_BUTTON_START:
		return "escape";
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return "page up";
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return "page down";
	}
	return "";
}

int input_init(void)
{
	gamepad_init();
	/* SDL_EnableKeyRepeat(500, 30); */ /* TODO ? */
	last_press_ms = 0;
	last_repeat_ms = 0;

#if defined(IOS) || defined(ANDROID)
	SDL_SetEventFilter(HandleAppEvents, NULL);
#endif
	return 0;
}

void input_done(void)
{
	gamepad_done();
}

void input_clear(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event));
	return;
}

void input_uevents(void)
{
	char *g = curgame_dir;
	SDL_Event peek;
	curgame_dir = NULL;
	while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_USEREVENT, SDL_USEREVENT) > 0) {
		void (*p) (void*) = (void (*)(void*)) peek.user.data1;
		if (p)
			p(peek.user.data2);
	}
	curgame_dir = g;
}

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

#if defined(IOS) || defined(SAILFISHOS)
static unsigned long touch_stamp = 0;
static int touch_num = 0;
#endif
int finger_pos(const char *finger, int *x, int *y, float *pressure)
{
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
			gfx_finger_pos_scale(f->x, f->y, x, y, 1);
			return 0;
		}
	}
	return -1;
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

	if (gamepad_mouse_event(&event))
		return AGAIN;

	inp->sym[0] = 0;
	inp->type = 0;
	inp->count = 1;
	switch(event.type){
	case SDL_TEXTINPUT:
		inp->type = KEY_TEXT;
		strncpy(inp->sym, event.text.text, sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		break;
	case SDL_MULTIGESTURE:
	case SDL_FINGERMOTION:
		if (DIRECT_MODE && !game_paused())
			return AGAIN;
		if (SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, event.type, event.type) > 0)
			return AGAIN; /* to avoid flickering */
#if defined(SAILFISHOS)
		touch_mouse_event(&event);
#endif
		break;
	case SDL_FINGERUP:
#ifdef IOS
		touch_num = 0;
#endif
	case SDL_FINGERDOWN:
#if defined(SAILFISHOS)
		touch_mouse_event(&event);
#endif
#if defined(IOS) || defined(SAILFISHOS)
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
#if SDL_VERSION_ATLEAST(2,0,7) /* broken. normalized by event watcher */
		gfx_finger_pos_scale(event.tfinger.x, event.tfinger.y, &inp->x, &inp->y, 0);
#else
		gfx_finger_pos_scale(event.tfinger.x, event.tfinger.y, &inp->x, &inp->y, 1);
#endif
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
	case SDL_CONTROLLERBUTTONDOWN:
		inp->type = KEY_DOWN;
		inp->code = event.cbutton.button;
		strncpy(inp->sym, gamepad_map(event.cbutton.button), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		if (!inp->sym[0])
			return AGAIN;
		break;
	case SDL_CONTROLLERBUTTONUP:
		inp->type = KEY_UP;
		inp->code = event.cbutton.button;
		strncpy(inp->sym, gamepad_map(event.cbutton.button), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		if (!inp->sym[0])
			return AGAIN;
		break;
	case SDL_WINDOWEVENT:
		switch (event.window.event) {
/*		case SDL_WINDOWEVENT_SHOWN: */
		case SDL_WINDOWEVENT_RESIZED: /* Android send this on screen rotate */
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			gfx_resize(event.window.data1, event.window.data2);
			/* Fall through */
		case SDL_WINDOWEVENT_EXPOSED:
			if (m_minimized) { /* broken WM? no RESTORE msg? */
				m_minimized = 0;
				snd_pause(0);
			}
			game_flip();
			game_gfx_commit(0);
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
	case SDL_USEREVENT: {
		void (*p) (void*) = (void (*)(void*))event.user.data1;
		if (!p) /* idle cycles */
			return 1;
		p(event.user.data2);
		return AGAIN;
		}
	case SDL_QUIT:
		game_running = 0;
		return -1;
	case SDL_KEYDOWN:	/* A key has been pressed */
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
		inp->type = KEY_DOWN;
		inp->code = event.key.keysym.scancode;
		strncpy(inp->sym, SDL_GetScancodeName(inp->code), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
		key_compat(inp);
		if (DIRECT_MODE && SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYUP) > 0) {
			if (peek.key.keysym.scancode == event.key.keysym.scancode &&
				peek.key.repeat == 0)
				return AGAIN;
		}
		break;
	case SDL_KEYUP:
		inp->type = KEY_UP;
		inp->code = event.key.keysym.scancode;
		strncpy(inp->sym, SDL_GetScancodeName(inp->code), sizeof(inp->sym));
		inp->sym[sizeof(inp->sym) - 1] = 0;
		tolow(inp->sym);
		key_compat(inp);
		if (DIRECT_MODE && SDL_PeepEvents(&peek, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYUP) > 0) {
			if (event.key.keysym.scancode == peek.key.keysym.scancode &&
				peek.key.repeat == 0)
				return AGAIN;
		}
		break;
	case SDL_MOUSEMOTION:
		m_focus = 1; /* ahhh */
		if (DIRECT_MODE && !game_paused())
			return AGAIN;
		inp->type = MOUSE_MOTION;
		inp->x = event.button.x;
		inp->y = event.button.y;
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION) > 0) {
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
	case SDL_MOUSEWHEEL:
		if (!game_grab_events && DIRECT_MODE && !game_paused())
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
		while (SDL_PeepEvents(&peek, 1, SDL_GETEVENT, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONDOWN) > 0) {
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

extern void gfx_real_size(int *ww, int *hh);

int input_text(int start)
{
	SDL_Rect rect;
	int w, h;
	if (start == -1)
		return SDL_IsTextInputActive();
	if (start) {
		gfx_real_size(&w, &h);
		rect.x = w / 2; rect.y = h - 1;
		rect.w = 1; rect.h = 1;
		SDL_SetTextInputRect(&rect);
		SDL_StartTextInput();
	} else
		SDL_StopTextInput();
	return 0;
}
