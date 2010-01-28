#ifndef __MENU_H_INCLUDED
#define __MENU_H_INCLUDED

#define MENU_GAMES_MAX 8
#define MENU_THEMES_MAX 8
#define MENU_PER_PAGER 7

#define FONT_MIN_SZ	8
#define FONT_MAX_SZ	64

#define MAX_SAVE_SLOTS 6

#define LANG_DEF	"en"

extern int cur_menu;
extern char *game_menu_gen(void);
extern int game_menu_act(const char *a);
extern void custom_theme_warn(void);
extern int menu_langs_lookup(const char *path);
extern int menu_lang_select(const char *name);

struct lang {
	char *path;
	char *name;
	char *file;
};
extern struct	lang *langs;
extern int	langs_nr;

enum {
	menu_main = 0,
	menu_about, 
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
};


#endif
