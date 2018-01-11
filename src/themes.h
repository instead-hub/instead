/*
 * Copyright 2009-2018 Peter Kosyh <p.kosyh at gmail.com>
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

#ifndef __THEMES_INCLUDED_H
#define __THEMES_INCLUDED_H

#include "graphics.h"
#include "sound.h"

#define DEFAULT_THEME "default"
#ifndef THEMES_PATH
#define THEMES_PATH "./themes"
#endif

#define THEME_FILE	"theme.ini"

#define THEME_GLOBAL	0
#define THEME_GAME	1

struct game_theme {
	float	scale;
	int 	w;
	int 	h;
	int	gfx_scalable;
	color_t	bgcol;
	char	*bg_name;
	img_t	bg;
	char	*use_name;
	char	*cursor_name;
	int	cur_x;
	int	cur_y;
	img_t	use;
	img_t	cursor;
	int 	pad;

	int	win_align;
	int	win_x;
	int	win_y;
	int	win_w;
	int	win_h;
	int	win_scroll_mode;

	char	*font_name;
	int	font_size;
	float	font_height;
	fnt_t	font;

	int	gfx_x;
	int	gfx_y;
	int	max_scene_w;
	int 	max_scene_h;

	char	*a_up_name;
	char	*a_down_name;
	img_t	a_up;
	img_t	a_down;
	int	a_up_x; int	a_up_y;
	int	a_down_x; int	a_down_y;

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
	float	inv_font_height;
	fnt_t	inv_font;
	
	char	*inv_a_up_name;
	char	*inv_a_down_name;
	img_t	inv_a_up;
	img_t	inv_a_down;

	int	inv_a_up_x; int	inv_a_up_y;
	int	inv_a_down_x; int	inv_a_down_y;
	
/*	int	lstyle;
	int	ilstyle; */

	color_t menu_bg;
	color_t menu_fg;
	color_t border_col;
	color_t menu_link;
	color_t menu_alink;
	int 	menu_alpha;
	int 	border_w;
	char	*menu_font_name;
	int	menu_font_size;
	float	menu_font_height;
	fnt_t	menu_font;
	
	char	*menu_button_name;
	img_t	menu_button;
	int	menu_button_x;
	int 	menu_button_y;
	int	gfx_mode;
	int	inv_mode;
	int	ways_mode;
	char	*click_name;
	void	*click;
	char	*icon_name;
	img_t	icon;
	int xoff;
	int yoff;
	int	changed;
};

#define CHANGED_FONT	1
#define CHANGED_IFONT	2
#define CHANGED_MFONT	4
#define CHANGED_BG	8
#define CHANGED_WIN	0x10
#define CHANGED_INV	0x20
#define CHANGED_CLICK	0x40
#define CHANGED_CURSOR	0x80
#define CHANGED_USE	0x100
#define CHANGED_UP	0x200
#define CHANGED_DOWN	0x400
#define CHANGED_IUP	0x800
#define CHANGED_IDOWN	0x1000
#define CHANGED_BUTTON	0x2000
#define CHANGED_ICON	0x4000
#define CHANGED_ALL 0xffff
struct theme {
	char *path;
	char *name;
	char *dir;
	int  type;
	int  idf;
};

extern	struct	theme *themes;
extern	int	themes_nr;
extern	char	*curtheme_dir[];
extern	char	*curgametheme_dir;

extern struct 	game_theme game_theme;
extern struct	game_theme game_theme_unscaled;

extern int 	game_default_theme(void);
extern int 	game_theme_select(const char *name);

extern int	themes_lookup(const char *path, int type);
extern int	themes_lookup_idf(idf_t idf, const char *path, int type);

extern struct theme *theme_lookup(const char *name, int type);
extern int 	themes_rename(void);
extern int	themes_count(int type);
extern int	themes_max(int *type);
extern void	themes_drop(int type);

extern int 	game_theme_load(const char *name, int type);
extern int	game_theme_init(void);
extern int 	game_theme_free(void);
extern int	game_theme_optimize(void);
extern int	game_theme_update(void);

extern int 	theme_load(const char *name);
extern int 	theme_load_idf(idf_t idf, const char *name);
extern int 	theme_img_scale(img_t *p);
extern int	theme_scalable_mode(int w, int h);

extern int theme_relative;

#define GFX_MODE_FLOAT 0
#define GFX_MODE_FIXED 1
#define GFX_MODE_EMBEDDED 2
#define GFX_MODE_DIRECT 3
#define GFX_MODE(v) ((v)&0xff)
#define GFX_ALIGN(v) ((v)>>8)
#define GFX_ALIGN_SET(v) ((v)<<8)

#define INV_MODE_DISABLED -1
#define INV_MODE_VERT 0
#define INV_MODE_HORIZ 1
#define INV_MODE(v) ((v)&0xff)
#define INV_ALIGN(v) ((v)>>8)
#define INV_ALIGN_SET(v) ((v)<<8)

#define WAYS_BOTTOM (game_theme.ways_mode == ALIGN_BOTTOM)
#define DIRECT_MODE (game_theme.gfx_mode == GFX_MODE_DIRECT)

#define SCALABLE_FONT (!(game_theme.gfx_scalable & 4))
#define SCALABLE_THEME (game_theme.gfx_scalable & 3)
#define SCALABLE_THEME_SMOOTH (game_theme.gfx_scalable & 1)

#endif
