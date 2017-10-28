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

#if defined(S60) || defined(_WIN32_WCE) || defined(MAEMO)
int opt_fsize = 12;
#elif defined(IOS) || defined(SAILFISHOS) || defined(ANDROID) || defined(WINRT)
int opt_fsize = 5;
#else
int opt_fsize = 0;
#endif

#if defined(MAEMO)
int opt_fs = 1;
int opt_owntheme = 0;
int opt_hl = 0;
#elif defined(IOS) || defined(ANDROID) || defined(SAILFISHOS)
int opt_fs = 1;
int opt_owntheme = 1;
int opt_hl = 0;
#elif defined(_WIN32_WCE) || defined(WINRT)
int opt_fs = 1;
int opt_owntheme = 1;
int opt_hl = 0;
#else
int opt_fs = 0;
int opt_owntheme = 1;
int opt_hl = 1;
#endif
int opt_fading = 1;
#if defined(S60) || defined(MAEMO) || defined(_WIN32_WCE)
int opt_hz = 22050;
#else
int opt_hz = 44100;
#endif
int opt_vol = 127;
int opt_motion = 1;
int opt_click = 1;
int opt_music = 1;
int opt_autosave = 1;
int opt_filter = 1;
int opt_kbd = KBD_SMART;
int opt_justify = 0;
int opt_vsync = -1;
int opt_debug = -1;
int opt_resizable = -1;
int opt_hires = 1;

char *opt_game = NULL;
char *opt_theme = NULL;
char *opt_lang = NULL;
int opt_mode[2] = {-1, -1};

int parse_mode(const char *v, void *data)
{
	int w, h;
	char *eptr;
	int *p = ((int *)data);
	w = strtol(v, &eptr, 10);
	if (!w || (eptr == v) || (*eptr != 'x'))
		return -1;
	eptr ++;
	h = strtol(eptr, &eptr, 10);
	if (!h || (*eptr))
		return -1;
	p[0] = w;
	p[1] = h;
	return 0;
}

static struct parser cfg_parser[] = {
	{ "hz", parse_int, &opt_hz, 0 },
	{ "fs", parse_int, &opt_fs, 0 },
	{ "vol", parse_int, &opt_vol, 0 },
	{ "hl", parse_int, &opt_hl, 0 },
	{ "game", parse_string, &opt_game, 0 },
	{ "theme", parse_string, &opt_theme, 0 },
	{ "autosave", parse_int, &opt_autosave, 0 },
	{ "motion", parse_int, &opt_motion, 0 },
	{ "click", parse_int, &opt_click, 0 },
	{ "music", parse_int, &opt_music, 0 },
	{ "fscale", parse_int, &opt_fsize, 0 },
	{ "filter", parse_int, &opt_filter, 0 },
	{ "owntheme", parse_int, &opt_owntheme, 0 },
	{ "lang", parse_string, &opt_lang, 0 },
	{ "kbd", parse_int, &opt_kbd, 0 },
	{ "mode", parse_mode, opt_mode, 0 },
	{ "justify", parse_int, &opt_justify, 0 },
	{ "fading", parse_int, &opt_fading, 0 },
	{ "vsync", parse_int, &opt_vsync, 0 },
	{ "debug", parse_int, &opt_debug, 0 },
	{ "resizable", parse_int, &opt_resizable, 0 },
	{ "hires", parse_int, &opt_hires, 0 },
	{ NULL, NULL, NULL, 0 },
};

void cfg_init(void)
{
#if defined(IOS)
	ios_cfg_init();
#endif
}

static int cfg_parse(const char *path)
{
	return parse_ini(path, cfg_parser);
}

int cfg_load(void)
{
	char *p = game_cfg_path();
	if (!p)
		return -1;
	if (access(p, R_OK))
		return 0;
	return cfg_parse(p);
}

int cfg_save(void)
{
	int save_owntheme = (opt_owntheme == 2)?0:opt_owntheme;
	int save_autosave = (opt_autosave == 2)?1:opt_autosave;
	FILE *fp;
	char *p = game_cfg_path();
	if (!p)
		return -1;
	fp = fopen(p, "w");
	if (!fp)
		return -1;
	fprintf(fp, "fs = %d\nhl = %d\nhz = %d\nvol = %d\nautosave = %d\n\
game = %s\nfscale = %d\nmotion = %d\n\
click = %d\nmusic = %d\ntheme = %s\n\
filter = %d\nowntheme = %d\nlang = %s\nkbd = %d\nmode = %dx%d\njustify = %d\nfading = %d\nhires = %d\n",
		opt_fs, opt_hl, opt_hz, opt_vol, save_autosave,
		curgame_dir?curgame_dir:"", opt_fsize, opt_motion,
		opt_click, opt_music, curtheme_dir[THEME_GLOBAL]?curtheme_dir[THEME_GLOBAL]:DEFAULT_THEME,
		opt_filter, save_owntheme, opt_lang, opt_kbd, opt_mode[0], opt_mode[1],
		opt_justify, opt_fading, opt_hires);
	if (opt_vsync != -1)
		fprintf(fp, "vsync = %d\n", opt_vsync);
	if (opt_debug != -1)
		fprintf(fp, "debug = %d\n", opt_debug);
	if (opt_resizable != -1)
		fprintf(fp, "resizable = %d\n", opt_resizable);
	fclose(fp);
	data_sync();
	return 0;
}
