#ifndef __THEMES_INCLUDED_H
#define __THEMES_INCLUDED_H

#include "graphics.h"
#include "sound.h"

#define DEFAULT_THEME "default"
#ifndef THEMES_PATH
#define THEMES_PATH "./themes"
#endif

#define THEME_FILE	"theme.ini"

struct game_theme {
	int 	w;
	int 	h;
	color_t	bgcol;
	char	*bg_name;
	img_t	bg;
	char	*use_name;
	img_t	use;
	int 	pad;
	
	int 	win_x;
	int	win_y;
	int	win_w;
	int	win_h;

	char	*font_name;
	int	font_size;
	fnt_t	font;

	int	gfx_x;
	int	gfx_y;
	int	max_scene_w;
	int 	max_scene_h;

	char	*a_up_name;
	char	*a_down_name;
	img_t	a_up;
	img_t	a_down;

	color_t fgcol;
	color_t lcol;
	color_t acol;

	int	inv_x;
	int 	inv_y;
	int 	inv_w;
	int 	inv_h;

	color_t icol;
	color_t ilcol;
	color_t iacol;
	char	*inv_font_name;
	int	inv_font_size;
	fnt_t	inv_font;
	
	char	*inv_a_up_name;
	char	*inv_a_down_name;
	img_t	inv_a_up;
	img_t	inv_a_down;
	
//	int	lstyle;
//	int	ilstyle;

	color_t menu_bg;
	color_t menu_fg;
	color_t border_col;
	color_t menu_link;
	color_t menu_alink;
	int 	menu_alpha;
	int 	border_w;
	char	*menu_font_name;
	int	menu_font_size;
	fnt_t	menu_font;
	
	char	*menu_button_name;
	img_t	menu_button;
	int	menu_button_x;
	int 	menu_button_y;
	int	gfx_mode;
	int	inv_mode;
	char	*click_name;
	void	*click;
};

struct theme {
	char *path;
	char *name;
	char *dir;
};

extern	struct	theme *themes;
extern 	int	themes_nr;
extern 	char	*curtheme_dir;

extern struct 	game_theme game_theme;
extern int 	game_default_theme(void);
extern int 	game_theme_select(const char *name);

extern int 	themes_lookup(const char *path);
extern int 	game_theme_load(const char *name);
extern int 	game_theme_free(void);
extern int 	theme_load(const char *name);
extern char 	*game_local_themes_path(void);

#define GFX_MODE_FLOAT 0
#define GFX_MODE_FIXED 1
#define GFX_MODE_EMBEDDED 2

#define INV_MODE_VERT 0
#define INV_MODE_HORIZ 1

#endif
