#include "externals.h"
#include "internals.h"

#if defined(ANDROID) || defined(S60) || defined(_WIN32_WCE) || defined(MAEMO)
int opt_fsize = 12;
#else
int opt_fsize = 0;
#endif
#if defined(MAEMO) || defined(ANDROID)
int opt_fs = 1;
int opt_owntheme = 0;
int opt_hl = 0;
#else
	#ifndef _WIN32_WCE
	int opt_fs = 0;
	int opt_owntheme = 1;
	int opt_hl = 1;
	#else
	int opt_fs = 1;
	int opt_owntheme = 1;
	int opt_hl = 0;
	#endif
#endif
int opt_fading = 1;
int opt_hz = 22050;
int opt_vol = 127;
int opt_motion = 1;
int opt_click = 1;
int opt_music = 1;
int opt_autosave = 1;
int opt_filter = 1;
int opt_kbd = KBD_SMART;
int opt_justify = 0;

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
	{ NULL, NULL, NULL, 0 },
};

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
filter = %d\nowntheme = %d\nlang = %s\nkbd = %d\nmode = %dx%d\njustify = %d\nfading = %d", 
		opt_fs, opt_hl, opt_hz, opt_vol, save_autosave, 
		curgame_dir?curgame_dir:"", opt_fsize, opt_motion, 
		opt_click, opt_music, curtheme_dir?curtheme_dir:DEFAULT_THEME, 
		opt_filter, save_owntheme, opt_lang, opt_kbd, opt_mode[0], opt_mode[1], 
		opt_justify, opt_fading);
	fclose(fp);
	return 0;
}
