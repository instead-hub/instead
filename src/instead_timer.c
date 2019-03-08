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

/* the Lua interpreter */
static gtimer_t instead_timer = NULL_TIMER;
static int volatile instead_timer_nr = 0;

static int instead_fn(int interval, void *p);

static void instead_timer_do(void *data)
{
	char *p;
	instead_timer_nr = 0;
	instead_lock();
	if (game_paused() || !curgame_dir) {
		instead_unlock();
		return;
	}
	if (instead_function("stead.timer", NULL)) {
		instead_clear();
		instead_unlock();
		return;
	}
	p = instead_retval(0); instead_clear();
	instead_unlock();
	if (!p)
		return;
	game_cmd(p, 0); free(p);
	game_cursor(CURSOR_ON);
}

static int instead_fn(int interval, void *p)
{
	if (instead_timer_nr > 0) {
		return interval; /* framedrop */
	}
	instead_timer_nr ++;
	push_user_event(instead_timer_do, NULL);
	return interval;
}

static int luaB_set_timer(lua_State *L) {
	const char *delay = luaL_optstring(L, 1, NULL);
	int d;
	gfx_del_timer(instead_timer);
	instead_timer = NULL_TIMER;
	if (!delay)
		d = 0;
	else	
		d = atoi(delay);
	if (!d)
		return 0;
	instead_timer_nr = 0;
	instead_timer = gfx_add_timer(d, instead_fn, NULL);
	return 0;
}

static const luaL_Reg timer_funcs[] = {
	{"instead_timer", luaB_set_timer},
	{NULL, NULL}
};

static int timer_done(void)
{
	gfx_del_timer(instead_timer);
	instead_timer = NULL_TIMER;
	return 0;
}

static int timer_init(void)
{
	char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/timer.lua");
	instead_api_register(timer_funcs);
	return instead_loadfile(dirpath(path));
}

static struct instead_ext ext = {
	.err = timer_done,
	.done = timer_done, /* to avoid loop errors */
	.init = timer_init,
};
int instead_timer_init(void)
{
	return instead_extension(&ext);
}
