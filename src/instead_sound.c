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

static int cur_vol = 0;

int game_change_vol(int d, int val)
{
	int v = snd_volume_mus(-1);
	int pc = snd_vol_to_pcn(v);
	int opc = pc;
	if (d) {
		pc += d;
		if (pc < 0)
			pc = 0;
		if (pc > 100)
			pc = 100;
		while (snd_vol_to_pcn(v) != pc)
			v += (d<0)?-1:1;
	} else {
		v = val;
		pc = snd_vol_to_pcn(v);
	}
	if (!pc)
		v = 0;
	snd_volume_mus(v);
	if (opc && !pc) {
		game_stop_mus(0);
	}
	if (!opc && pc) {
		game_music_player();
	}
	cur_vol = snd_volume_mus(-1);
	if (!nosound_sw)
		opt_vol = cur_vol;
	return 0;
}

#define MAX_WAVS SND_CHANNELS * 2

static LIST_HEAD(sounds);
static int sounds_nr = 0;

typedef struct {
	struct list_node list;
	char	*fname;
	wav_t	wav;
	int	loaded;
	int	system;
	int	fmt;
	short	*buf;
	size_t	len;
} _snd_t;
typedef struct {
	_snd_t *snd;
	int	loop;
	int	channel;
} _snd_req_t;
static _snd_t *channels[SND_CHANNELS];
static _snd_req_t sound_reqs[SND_CHANNELS];
static void sound_play(_snd_t *sn, int chan, int loop);

void sound_play_click(void)
{
	sound_play(game_theme.click, -1, 1);
}

static void sound_callback(void *aux)
{
	_snd_req_t *r;
	int channel = *((int *)aux);
	int c = channel % SND_CHANNELS;
	free(aux);
/*	fprintf(stderr, "finished: %d\n", channel); */
	channels[c] = NULL;
	r = &sound_reqs[c];
	if (r->snd) {
		_snd_t *s = r->snd;
		r->snd = NULL;
		sound_play(s, channel, r->loop);
	} else {
		snd_halt_chan(channel, 0); /* to avoid races */
	}
}

void game_channel_finished(int channel) /* SDL callback */
{
	int *i = malloc(sizeof(channel));
	if (!i) {
		fprintf(stderr, "game_channel_finished: No memory\n");
		return;
	}
	*i = channel;
	push_user_event(sound_callback, i);
}

static int  sound_playing(_snd_t *snd)
{
	int i;
	for (i = 0; i < SND_CHANNELS; i++) {
		if (channels[i] == snd)
			return i;
		if (sound_reqs[i].snd == snd)
			return i;
	}
	return -1;
}

static const char *sound_channel(int i)
{
	_snd_t *sn;
	if (i >= SND_CHANNELS)
		i = i % SND_CHANNELS;
	if (i == -1) {
		for (i = 0; i < SND_CHANNELS; i++) {
			sn = channels[i];
			if (sn && !sn->system)
				return sn->fname;
		}
		return NULL;
	}
	sn = channels[i];
	if (!sn)
		return NULL;
	if (sn->system)
		return NULL; /* hidden system sound */
	return sn->fname;
}

static void sound_free(_snd_t *sn)
{
	if (!sn)
		return;
	list_del(&sn->list);
	sounds_nr --;
	free(sn->fname);
	snd_free_wav(sn->wav);
	if (sn->buf)
		free(sn->buf);
	free(sn);
}

static void sounds_shrink(void)
{
	_snd_t *pos, *pos2;
	_snd_t *sn;
	pos = list_top(&sounds, _snd_t, list);
/*	fprintf(stderr,"shrink try\n"); */
	while (pos && sounds_nr > MAX_WAVS) {
		sn = (_snd_t*)pos;
		if (sound_playing(sn) != -1 || sn->loaded) {
			pos = list_next(&sounds, pos, list);
			continue;
		}
		pos2 = list_next(&sounds, pos, list);
		sound_free(sn);
		pos = pos2;
/*		fprintf(stderr,"shrink by 1\n"); */
	}
}

static void sounds_free(void)
{
	int i = 0;
	_snd_t *pos, *pos2;
	_snd_t *sn;
	pos = list_top(&sounds, _snd_t, list);

	snd_halt_chan(-1, 0); /* halt sounds */
	while (pos) {
		sn = (_snd_t*)pos;
		pos2 = list_next(&sounds, pos, list);
		if (sn->system)
			sn->loaded = 1; /* ref by system only */
		else
			sound_free(sn);
		pos = pos2;
	}
	for (i = 0; i < SND_CHANNELS; i++) {
		channels[i] = NULL;
		sound_reqs[i].snd = NULL;
	}
/*	sounds_nr = 0;
	fprintf(stderr, "%d\n", sounds_nr); */
	input_uevents(); /* all callbacks */
}

static _snd_t *sound_find(const char *fname)
{
	_snd_t *pos = NULL;
	_snd_t *sn;
	if (!fname)
		return NULL;
	list_for_each(&sounds, pos, list) {
		sn = (_snd_t*)pos;
		if (!strcmp(fname, sn->fname)) {
			list_del(&sn->list);
			list_add(&sounds, &sn->list); /* move it on head */
			return sn;
		}
	}
	return NULL;
}

static int sound_find_channel(void)
{
	int i;
	for (i = 0; i < SND_CHANNELS; i ++) {
		if (!channels[i] && !sound_reqs[i].snd)
			return i;
	}
	return -1;
}

static void sound_play(_snd_t *sn, int chan, int loop)
{
	int c;
	if (!sn)
		return;
	if (chan == -1) {
		c = sound_find_channel();
		if (c == -1)
			return; /* all channels are busy */
	} else
		c = chan;
	if (channels[c]) {
		sound_reqs[c].snd = sn;
		sound_reqs[c].loop = loop;
		sound_reqs[c].channel = chan;
		snd_halt_chan(chan, 0); /* work in callback */
		input_uevents(); /* all callbacks */
		return;
	}
	c = snd_play(sn->wav, c, loop - 1);
/*	fprintf(stderr, "added: %d\n", c); */
	if (c == -1)
		return;
	channels[c] = sn;
}

static _snd_t *sound_add(const char *fname, int fmt, short *buf, int len)
{
	wav_t w;
	_snd_t *sn;
	if (!fname || !*fname)
		return NULL;
	sn = malloc(sizeof(_snd_t));
	if (!sn)
		return NULL;
	memset(sn, 0, sizeof(*sn));
/*	LIST_HEAD_INIT(&sn->list); */
	sn->fname = strdup(fname);
	sn->loaded = 0;
	sn->system = 0;
	sn->buf = buf;
	sn->len = len;
	sn->fmt = fmt;
	if (!sn->fname) {
		free(sn);
		return NULL;
	}
	if (buf)
		w = snd_load_mem(fmt, buf, len);
	else
		w = snd_load_wav(fname);
	if (!w) {
		if (snd_enabled())
			game_res_err_msg(fname, debug_sw);
		goto err;
	}
	sn->wav = w;

	sounds_shrink();

	list_add(&sounds, &sn->list);
	sounds_nr ++;
	return sn;
err:
	free(sn->fname);
	free(sn);
	return NULL;
}

static void sounds_reload(void)
{
	_snd_t *pos = NULL;
	_snd_t *sn;
	int i;
	snd_halt_chan(-1, 0); /* stop all sound */
	list_for_each(&sounds, pos, list) {
		sn = (_snd_t*)pos;
		snd_free_wav(sn->wav);
		if (sn->buf)
			sn->wav = snd_load_mem(sn->fmt, sn->buf, sn->len);
		else
			sn->wav = snd_load_wav(sn->fname);
	}
	for (i = 0; i < SND_CHANNELS; i++) {
		channels[i] = NULL;
		sound_reqs[i].snd = NULL;
	}
	input_uevents(); /* all callbacks */
}

static void *_sound_get(const char *fname, int fmt, short *buff, size_t len)
{
	_snd_t *sn = NULL;
	if (fname) {
		sn = sound_find(fname);
		if (sn) {
			sn->loaded ++; /* to pin */
			return sn;
		}
		sn = sound_add(fname, fmt, buff, len);
	} else if (buff) {
		char *name = malloc(64);
		if (!name)
			return NULL;
		snprintf(name, 64, "snd:%p", buff); name[64 - 1] = 0;
		sn = sound_add(name, fmt, buff, len);
		if (!sn)
			free(name);
		else {
			snprintf(name, 64, "snd:%p", sn); name[64 - 1] = 0;
			free(sn->fname);
			sn->fname = name;
		}
	}
	if (!sn)
		return NULL;
	sn->loaded = 1;
	return sn;
}

static void _sound_put(void *s)
{
	_snd_t *sn = (_snd_t *)s;
	if (!sn || !sn->loaded)
		return;
	if (!sn->system || sn->loaded > 1)
		sn->loaded --;
	if (!sn->loaded && sound_playing(sn) == -1)
		sound_free(sn);
	return;
}

void *sound_get(const char *fname)
{
	_snd_t *sn = _sound_get(fname, 0, NULL, 0);
	if (!sn)
		return NULL;
	sn->system = 1;
	return sn;
}

void sound_put(void *s)
{
	_snd_t *sn = (_snd_t *)s;
	if (!sn)
		return;
	sn->system = 0;
	_sound_put(sn);
}

static int sound_load(const char *fname)
{
	_snd_t *sn = _sound_get(fname, 0, NULL, 0);
	if (!sn)
		return -1;
	return 0;
}

static char *sound_load_mem(int fmt, short *buff, size_t len)
{
	_snd_t *sn = _sound_get(NULL, fmt, buff, len);
	if (!sn)
		return NULL;
	return sn->fname;
}

static void sound_unload(const char *fname)
{
	_snd_t *sn;
	sn = sound_find(fname);
	_sound_put(sn);
	return;
}

static int _play_combined_snd(char *filename, int chan, int loop)
{
	char *str;
	char *p, *ep;
	_snd_t *sn;
	p = str = strdup(filename);
	if (!str)
		return -1;

	p = strip(p);
	while (*p) {
		int c = chan, l = loop;
		int at = 0;
		ep = p + strcspn(p, ";@");

		if (*ep == '@') {
			at = 1;
			*ep = 0; ep ++;
			if (sscanf(ep, "%d,%d", &c, &l) > 1)
				at ++;
			ep += strcspn(ep, ";");
			if (*ep)
				ep ++;
		} else if (*ep == ';') {
			*ep = 0; ep ++;
		} else if (*ep) {
			goto err;
		}
		p = strip(p);
		sn = sound_find(p);
		if (!sn)
			sn = sound_add(p, 0, NULL, 0);
		if (sn)
			sound_play(sn, c, l);
		else if (at || c != -1) { /* if @ or specific channel */
			snd_halt_chan(c, (at == 2)?l:500);
		}
		p = ep;
	}
	free(str);
	return 0;
err:
	free(str);
	return -1;
}

static void game_sound_player(void)
{
	char		*snd;
	int		chan = -1;
	int		loop = 1;

	struct instead_args args[] = {
			{ .val = "nil", .type = INSTEAD_NIL },
			{ .val = "-1", .type = INSTEAD_NUM },
			{ .val = NULL }
	};

	if (!snd_volume_mus(-1))
		return;

	instead_lock();
	instead_function("instead.get_sound", NULL);

	loop = instead_iretval(2);
	chan = instead_iretval(1);
	snd = instead_retval(0);
	instead_clear();
	if (!snd) {
		if (chan != -1) {
			/* halt channel */
			snd_halt_chan(chan, 500);
			instead_function("instead.set_sound", args); instead_clear();
		}
		instead_unlock();
		return;
	}
	instead_function("instead.set_sound", args); instead_clear();
	instead_unlock();
	unix_path(snd);
	_play_combined_snd(snd, chan, loop);
	free(snd);
}

static	char *last_music = NULL;

static void free_last_music(void)
{
	if (last_music)
		free(last_music);
	last_music = NULL;
}

int game_change_hz(int hz)
{
	if (!hz)
		return -1;
#ifndef __EMSCRIPTEN__
	snd_close();
	free_last_music();
	snd_open(hz);
	snd_volume_mus(cur_vol);
	sounds_reload();
	game_music_player();
#endif
	opt_hz = snd_hz();
	return 0;
}

void game_stop_mus(int ms)
{
	snd_stop_mus(ms);
	free_last_music();
}

static void finish_music(void *data)
{
	int rc;
	if (!curgame_dir)
		return;
	instead_lock();
	instead_function("instead.finish_music", NULL);
	rc = instead_bretval(0);
	instead_clear();
	instead_unlock();
	if (rc)
		free_last_music();
	snd_volume_mus(cur_vol); /* reset volume */
}

void game_music_finished(void) /* SDL callback */
{
	push_user_event(&finish_music, NULL);
}

void game_music_player(void)
{
	int	loop;
	char		*mus;

	int cf_out = 0;
	int cf_in = 0;

	if (!snd_volume_mus(-1))
		return;
	if (!opt_music || !curgame_dir)
		return;
	instead_lock();
	instead_function("instead.get_music", NULL);
	mus = instead_retval(0);
	loop = instead_iretval(1);
	unix_path(mus);
	instead_clear();

	instead_function("instead.get_music_fading", NULL);
	cf_out = instead_iretval(0);
	cf_in = instead_iretval(1);
	instead_clear();
	instead_unlock();
	if (mus && loop == -1) { /* disabled, 0 - forever, 1-n - loops */
		free(mus);
		mus = NULL;
	}

	if (loop == 0)
		loop = -1;

	if (cf_out == 0)
		cf_out = 500;
	else if (cf_out < 0)
		cf_out = 0;

	if (cf_in < 0)
		cf_in = 0;

	if (!mus) {
		if (last_music) {
			game_stop_mus(cf_out);
		}
	} else if (!last_music || strcmp(last_music, mus)) {
		game_stop_mus(cf_out);
		last_music = mus;
		if (snd_play_mus(mus, cf_in, loop) < 0)
			game_res_err_msg(mus, debug_sw);
	} else
		free(mus);
}

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
#define SND_F2S(v) ((short)((float)(v) * 16383.0) * 2)

static int luaB_load_sound_mem(lua_State *L) {
	int hz = luaL_optinteger(L, 1, -1);
	int channels = luaL_optinteger(L, 2, -1);
	int len; int i;
	short *buf = NULL;
	const char *name;
	int fmt = 0;
	luaL_checktype(L, 3, LUA_TTABLE);

	if (hz < 0 || channels < 0)
		return 0;
#if LUA_VERSION_NUM >= 502
	len = lua_rawlen(L, 3);
#else
	len = lua_objlen(L, 3);
#endif
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
		buf[i] = SND_F2S(v);
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
	int chan = luaL_optinteger(L, 1, -1);
	int left = luaL_optnumber(L, 2, 255);
	int right = luaL_optnumber(L, 3, 255);
	snd_panning(chan, left, right);
	return 0;
}

static int luaB_volume_sound(lua_State *L) {
	int vol = luaL_optnumber(L, 1, -1);
	vol = snd_volume_mus(vol);
	lua_pushinteger(L, vol);
	return 1;
}

static int luaB_channel_sound(lua_State *L) {
	const char *s;
	int ch = luaL_optinteger(L, 1, 0);
	ch = ch % SND_CHANNELS;
	s = sound_channel(ch);
	if (s) {
		lua_pushstring(L, s);
		return 1;
	}
	return 0;
}
static int callback_ref = 0;
static int sound_inited = 0;

#define SOUND_MAGIC 0x2004
struct lua_sound {
	int type;
	short *buf;
	int len;
};
static void mus_callback(void *udata, unsigned char *stream, int len)
{
	lua_State *L = (lua_State *) udata;
	struct lua_sound *hdr;
	if (!callback_ref)
		return;
	instead_lock();
	lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
	lua_pushinteger(L, snd_hz());
	lua_pushinteger(L, len >> 1);
	hdr = lua_newuserdata(L, sizeof(*hdr));
	if (!hdr)
		goto err;
	hdr->type = SOUND_MAGIC;
	hdr->len = len >> 1; /* 16bits */
	hdr->buf = (short *)stream;
	luaL_getmetatable(L, "soundbuffer metatable");
	lua_setmetatable(L, -2);
	if (instead_pcall(L, 3)) { /* on any error */
	err:
		snd_mus_callback(NULL, NULL);
		luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
	}
	instead_clear();
	instead_unlock();
	return;
}

static int luaB_music_callback(lua_State *L) {
	if (!sound_inited)
		return 0;
	if (lua_isfunction(L, 1)) {
		game_stop_mus(0);
		callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		snd_mus_callback(mus_callback, L);
	} else {
		snd_mus_callback(NULL, NULL);
		if (callback_ref)
			luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
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
	{"instead_music_callback", luaB_music_callback},
	{NULL, NULL}
};


static int sound_done(void)
{
	if (!sound_inited)
		return 0;
	if (callback_ref) {
		snd_mus_callback(NULL, NULL);
		luaL_unref(instead_lua(), LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
	}
	game_stop_mus(0);
	sounds_free();
	snd_close();
	sound_inited = 0;
	return 0;
}
/*
static int sound_size(lua_State *L) {
	struct lua_sound *hdr = (struct lua_sound*)lua_touserdata(L, 1);
	if (!hdr || hdr->type != SOUND_MAGIC)
		return 0;
	lua_pushnumber(L, hdr->len);
	return 1;
}
*/
static int sound_value(lua_State *L) {
	struct lua_sound *hdr = (struct lua_sound*)lua_touserdata(L, 1);
	int pos = luaL_optinteger(L, 2, -1);
	float v = luaL_optnumber(L, 3, 0.0f);
	if (pos <= 0)
		return 0;
	if (pos > hdr->len)
		return 0;
	pos --;
	if (lua_isnoneornil(L, 3)) {
		lua_pushinteger(L, hdr->buf[pos]);
		return 1;
	}
	hdr->buf[pos] = SND_F2S(v);
	return 0;
}

/*
** Creates chunk metatable.
*/
static int chunk_create_meta (lua_State *L) {
	luaL_newmetatable (L, "soundbuffer metatable");
	lua_pushstring (L, "__index");
	lua_pushcfunction(L, sound_value);
	lua_settable(L, -3);

	lua_pushstring (L, "__newindex");
	lua_pushcfunction(L, sound_value);
	lua_settable(L, -3);
/*
	lua_pushstring (L, "size");
	lua_pushcfunction (L, sound_size);
	lua_settable(L, -3);
*/
	return 0;
}

static int sound_init(void)
{
	int rc;
	char path[PATH_MAX];

	instead_api_register(sound_funcs);
	chunk_create_meta(instead_lua());

	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/sound.lua");

	rc = instead_loadfile(dirpath(path));
	if (rc)
		return rc;
	snd_open(opt_hz);
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
