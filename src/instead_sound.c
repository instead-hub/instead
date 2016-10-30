/*
 * Copyright 2009-2016 Peter Kosyh <p.kosyh at gmail.com>
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

static int luaB_is_sound(lua_State *L) {
	const char *chan = luaL_optstring(L, 1, NULL);
	int c, r;
	if (!chan)
		c = -1;
	else
		c = atoi(chan);
	r = (sound_channel(c) != NULL);
	lua_pushboolean(L, r);  /* else not a number */
	return 1;
}

static int luaB_load_sound(lua_State *L) {
	int rc;
	const char *fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	rc = sound_load(fname);
	if (rc)
		return 0;
	lua_pushstring(L, fname);
	return 1;
}

static int luaB_load_sound_mem(lua_State *L) {
	int hz = luaL_optnumber(L, 1, -1);
	int channels = luaL_optnumber(L, 2, -1);
	int len; int i;
	short *buf = NULL;
	const char *name;
	int fmt = 0;
	luaL_checktype(L, 3, LUA_TTABLE);

	if (hz < 0 || channels < 0)
		return 0;

	len = lua_objlen(L, 3);
	if (len <= 0)
		return 0;
	buf = malloc(sizeof(short) * len);
	if (!buf)
		return 0;

	lua_pushvalue(L, 3);

	for (i = 0; i < len; i++) {
		float v;
		lua_pushinteger(L, i + 1);
		lua_gettable(L, -2);

		if (!lua_isnumber(L, -1)) {
			v = 0;
		} else {
			v = (float)lua_tonumber(L, -1);
		}
		buf[i] = (short)((float)v * 16383) * 2;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	/* here we got the sample */
	if (channels == 2)
		fmt |= SND_FMT_STEREO;

	if (hz == 11025)
		fmt |= SND_FMT_11;
	else if (hz == 22050)
		fmt |= SND_FMT_22;
	else
		fmt |= SND_FMT_44;
	name = sound_load_mem(fmt, buf, len);
/*	free(buf); */
	if (!name)
		return 0;
	lua_pushstring(L, name);
	return 1;
}

static int luaB_free_sound(lua_State *L) {
	const char *fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	sound_unload(fname);
	return 0;
}

static int luaB_free_sounds(lua_State *L) {
	sounds_free();
	return 0;
}

static int luaB_panning_sound(lua_State *L) {
	int chan = luaL_optnumber(L, 1, -1);
	int left = luaL_optnumber(L, 2, 255);
	int right = luaL_optnumber(L, 3, 255);
	snd_panning(chan, left, right);
	return 0;
}

static int luaB_volume_sound(lua_State *L) {
	int vol = luaL_optnumber(L, 1, -1);
	vol = snd_volume_mus(vol);
	lua_pushnumber(L, vol);
	return 1;
}

static int luaB_channel_sound(lua_State *L) {
	const char *s;
	int ch = luaL_optnumber(L, 1, 0);
	ch = ch % SND_CHANNELS;
	s = sound_channel(ch);
	if (s) {
		lua_pushstring(L, s);
		return 1;
	}
	return 0;
}

static const luaL_Reg sound_funcs[] = {
	{"instead_sound", luaB_is_sound},
	{"instead_sound_load", luaB_load_sound},
	{"instead_sound_load_mem", luaB_load_sound_mem},
	{"instead_sound_free", luaB_free_sound},
	{"instead_sound_channel", luaB_channel_sound},
	{"instead_sound_panning", luaB_panning_sound},
	{"instead_sound_volume", luaB_volume_sound},
	{"instead_sounds_free", luaB_free_sounds},

	{NULL, NULL}
};

static int sound_inited = 0;

static int sound_done(void)
{
	if (!sound_inited)
		return 0;
	game_stop_mus(500);
	sounds_free();
	snd_done();
	sound_inited = 0;
	return 0;
}

static int sound_init(void)
{
	int rc;
	instead_api_register(sound_funcs);
	rc = instead_loadfile(dirpath(STEAD_PATH"/ext/sound.lua"));
	if (rc)
		return rc;
	if (sound_inited)
		sound_done();
	snd_init(opt_hz);
	if (!nosound_sw)
		game_change_vol(0, opt_vol);
	sound_inited = 1;
	return 0;
}

static int sound_cmd(void)
{
	game_music_player();
	game_sound_player();
	return 0;
}

static struct instead_ext ext = {
	.init = sound_init,
	.done = sound_done,
	.cmd = sound_cmd,
};

int instead_sound_init(void)
{
	return instead_extension(&ext);
}
