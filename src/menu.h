#ifndef __MENU_H_INCLUDED
#define __MENU_H_INCLUDED

#if defined(IOS) || defined(SAILFISHOS) || defined(ANDROID) || defined(WINRT)
#define MENU_GAMES_MAX 5
#define MENU_THEMES_MAX 5
#define MENU_PER_PAGER 5
#else
#define MENU_GAMES_MAX 8
#define MENU_THEMES_MAX 8
#define MENU_PER_PAGER 7
#endif

#define FONT_MIN_SZ	-5
#define FONT_MAX_SZ	30

#define MAX_SAVE_SLOTS 6

#define LANG_DEF	"en"
#define MAX_MENU_LINES 16

extern int cur_menu;
extern int top_menu;

extern char *game_menu_gen(void);
extern int game_menu_act(const char *a);
extern void custom_theme_warn(void);
extern int menu_langs_lookup(const char *path);
extern int menu_lang_select(const char *name);
extern int games_menu_maxw(void);
extern int themes_menu_maxw(void);

struct lang {
	char *path;
	char *name;
	char *file;
};
extern struct	lang *langs;
extern int	langs_nr;

enum {
	menu_main = 1,
	menu_about, 
	menu_about_instead, 
	menu_settings,
	menu_quit,
	menu_askquit,
	menu_saved, 
	menu_games, 
	menu_themes, 
	menu_own_theme,
	menu_custom_theme,
	menu_load, 
	menu_save, 
	menu_error, 
	menu_warning,
	menu_remove,
	menu_wait,
	menu_max,
};


#endif
