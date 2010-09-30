#include "externals.h"
#include "internals.h"

int opt_fsize = 0;
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
int opt_hz = 22050;
int opt_vol = 127;
int opt_motion = 1;
int opt_click = 1;
int opt_music = 1;
int opt_autosave = 1;
int opt_filter = 1;
int opt_kbd = KBD_SMART;

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
	{ "hz", parse_int, &opt_hz },
	{ "fs", parse_int, &opt_fs },
	{ "vol", parse_int, &opt_vol }, 
	{ "hl", parse_int, &opt_hl },
	{ "game", parse_string, &opt_game },
	{ "theme", parse_string, &opt_theme }, 
	{ "autosave", parse_int, &opt_autosave },
	{ "motion", parse_int, &opt_motion }, 
	{ "click", parse_int, &opt_click }, 
	{ "music", parse_int, &opt_music }, 
	{ "fscale", parse_int, &opt_fsize },
	{ "filter", parse_int, &opt_filter },
	{ "owntheme", parse_int, &opt_owntheme },
	{ "lang", parse_string, &opt_lang },
	{ "kbd", parse_int, &opt_kbd },
	{ "mode", parse_mode, opt_mode },
	{ NULL, },
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
filter = %d\nowntheme = %d\nlang = %s\nkbd = %d\nmode = %dx%d", 
		opt_fs, opt_hl, opt_hz, opt_vol, opt_autosave, 
		curgame_dir?curgame_dir:"", opt_fsize, opt_motion, 
		opt_click, opt_music, curtheme_dir?curtheme_dir:DEFAULT_THEME, 
		opt_filter, opt_owntheme, opt_lang, opt_kbd, opt_mode[0], opt_mode[1]);
	fclose(fp);
	return 0;
}
