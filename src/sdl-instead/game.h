#ifndef __GAME_H__
#define __GAME_H__

#ifndef GAMES_PATH
#define GAMES_PATH "./games"
#endif

#define DEFAULT_THEME "default"
#ifndef THEMES_PATH
#define THEMES_PATH "./themes"
#endif

#define MAIN_FILE	"main.lua"
#define THEME_FILE	"theme.ini"

#define MENU_GAMES_MAX 8
#define MENU_THEMES_MAX 8

#define FONT_MIN_SZ	8
#define FONT_MAX_SZ	64

#define MAX_SAVE_SLOTS 6
extern char *game_local_games_path(void);
extern char *game_local_themes_path(void);
extern int cfg_load(void);
extern int cfg_save(void);
extern char *opt_game;

extern int nosound_sw;
extern int alsa_sw;
extern int fullscreen_sw;
extern int window_sw;

extern int opt_fs;
extern char *opt_theme;
extern char *curtheme;
extern int game_theme_select(const char *name);


extern int game_load_theme(const char *path);
extern int game_init(const char *game);
extern void game_done(void);
extern int game_loop(void);

extern int games_lookup(const char *path);
extern int themes_lookup(const char *path);
extern void game_err_msg(const char *s);
extern int game_error(const char *name);
#endif

