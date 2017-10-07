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

int theme_relative = 0;

char	*curtheme_dir[2] = { NULL, NULL };
struct theme *curtheme_loading = NULL;

static int parse_win_align(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "left"))
		*i = ALIGN_LEFT;
	else if (!strcmp(v, "justify"))
		*i = ALIGN_JUSTIFY;
	else if (!strcmp(v, "center"))
		*i = ALIGN_CENTER;
	else if (!strcmp(v, "right"))
		*i = ALIGN_RIGHT;
	else
		return -1;
	return 0;
}

static int parse_gfx_mode(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "fixed"))
		*i = GFX_MODE_FIXED;	
	else if (!strcmp(v, "embedded"))
		*i = GFX_MODE_EMBEDDED;
	else if (!strncmp(v, "float", 5)) {
		*i = GFX_MODE_FLOAT;
		v += 5;
		if (!*v) /* compat */
			*i |= GFX_ALIGN_SET(ALIGN_TOP);
		while (*v) {
			if (*v != '-')
				return -1;
			v ++;
			if (!strncmp(v, "top", 3)) {
				*i |= GFX_ALIGN_SET(ALIGN_TOP);
				v += 3;
			} else if (!strncmp(v, "middle", 6)) {
				*i |= GFX_ALIGN_SET(ALIGN_MIDDLE);
				v += 6;
			} else if (!strncmp(v, "bottom", 6)) {
				*i |= GFX_ALIGN_SET(ALIGN_BOTTOM);
				v += 6;
			} else if (!strncmp(v, "left", 4)) {
				*i |= GFX_ALIGN_SET(ALIGN_LEFT);
				v += 4;
			} else if (!strncmp(v, "right", 5)) {
				*i |= GFX_ALIGN_SET(ALIGN_RIGHT);
				v += 5;
			} else if (!strncmp(v, "center", 6)) {
				*i |= GFX_ALIGN_SET(ALIGN_CENTER);
				v += 6;
			} else
				return -1;
		}
	} else if (!strcmp(v, "direct"))
		*i = GFX_MODE_DIRECT;	
	else
		return -1;
	return 0;	
}

static int out_gfx_mode(const void *v, char **out)
{
	char *o;
	char buff[256];
	int m = *((int*)v);
	switch (GFX_MODE(m)) {
	case GFX_MODE_FIXED:
		o = strdup("fixed");
		break;
	case GFX_MODE_EMBEDDED:
		o = strdup("embedded");
		break;
	case GFX_MODE_FLOAT:
		strcpy(buff, "float");
		if (GFX_ALIGN(m) != ALIGN_TOP) { /* compat */
			if (GFX_ALIGN(m) & ALIGN_TOP)
				strcat(buff,"-top");
			else if (GFX_ALIGN(m) & ALIGN_BOTTOM)
				strcat(buff,"-bottom");
			if (GFX_ALIGN(m) & ALIGN_LEFT)
				strcat(buff,"-left");
			else if (GFX_ALIGN(m) & ALIGN_RIGHT)
				strcat(buff,"-right");
		}
		o = strdup(buff);
		break;
	case GFX_MODE_DIRECT:
		o = strdup("direct");
		break;
	default:
		o = strdup("");
		break;
	}
	if (!o)
		return -1;
	*out = o;
	return 0;
}

static int parse_inv_mode(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "vertical") || !strcmp(v, "0") || !strcmp(v, "vertical-left"))
		*i = INV_MODE_VERT | INV_ALIGN_SET(ALIGN_LEFT);
	else if (!strcmp(v, "horizontal") || !strcmp(v, "1") || !strcmp(v, "horizontal-center"))
		*i = INV_MODE_HORIZ | INV_ALIGN_SET(ALIGN_CENTER);
	else if (!strcmp(v, "horizontal-left") || !strcmp(v, "1"))
		*i = INV_MODE_HORIZ | INV_ALIGN_SET(ALIGN_LEFT);
	else if (!strcmp(v, "horizontal-right") || !strcmp(v, "1"))
		*i = INV_MODE_HORIZ | INV_ALIGN_SET(ALIGN_RIGHT);
	else if (!strcmp(v, "disabled") || !strcmp(v, "-1"))
		*i = INV_MODE_DISABLED;
	else if (!strcmp(v, "vertical-right"))
		*i = INV_MODE_VERT | INV_ALIGN_SET(ALIGN_RIGHT);
	else if (!strcmp(v, "vertical-center"))
		*i = INV_MODE_VERT | INV_ALIGN_SET(ALIGN_CENTER);
	else
		return -1;
	return 0;
}

static int parse_ways_mode(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "top"))
		*i = ALIGN_TOP;
	else if (!strcmp(v, "bottom"))
		*i = ALIGN_BOTTOM;
	else
		return -1;
	return 0;
}

static int out_ways_mode(const void *v, char **out)
{
	char *o;
	int m = *((int*)v);
	o = malloc(64);
	if (!o)
		return -1;
	if (m == ALIGN_BOTTOM)
		sprintf(o, "bottom");
	else
		sprintf(o, "top");
	*out = o;
	return 0;
}

static int out_inv_mode(const void *v, char **out)
{
	char *o;
	int m = *((int*)v);
	o = malloc(64);
	if (!o)
		return -1;
	if (m == INV_MODE_DISABLED) {
		sprintf(o, "disabled");
		*out = o;
		return 0;
	}

	if ((INV_MODE(m) == INV_MODE_HORIZ))
		sprintf(o, "horizontal");
	else
		sprintf(o, "vertical");

	if ((m & INV_ALIGN_SET(ALIGN_CENTER)) == INV_ALIGN_SET(ALIGN_CENTER)) {
		strcat(o, "-center");
	} else if ((m & INV_ALIGN_SET(ALIGN_LEFT)) == INV_ALIGN_SET(ALIGN_LEFT)) {
		strcat(o, "-left");
	} else if ((m & INV_ALIGN_SET(ALIGN_RIGHT)) == INV_ALIGN_SET(ALIGN_RIGHT)) {
		strcat(o, "-right");
	}
	*out = o;
	return 0;
}

static int parse_color(const char *v, void *data)
{
	color_t *c = (color_t *)data;
	return gfx_parse_color(v, c);
}

static int out_color(const void *v, char **out)
{
	char *o;
	color_t *c = (color_t *)v;
	o = malloc(16);
	if (!o)
		return -1;
	sprintf(o, "#%02x%02x%02x", c->r, c->g, c->b);
	*out = o;
	return 0;
}

static int parse_include(const char *v, void *data)
{
	int rc;
	char cwd[PATH_MAX];
	if (!strlowcmp(v, DEFAULT_THEME))
		return 0;
	if (curtheme_loading && curtheme_loading->type == THEME_GAME
		&& strlowcmp(v, curtheme_loading->dir)
		&& theme_lookup(v, THEME_GAME)) { /* internal theme? */
		return game_theme_load(v, THEME_GAME);
	}
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	rc = game_theme_load(v, THEME_GLOBAL);
/*	if (!rc)
		game_theme_select(v); */
	setdir(cwd);
	return rc;
}

static int theme_parse_full_path(const char *v, void *data)
{
	int rc;
	char **p = (char **)data;
	char *np;
	if (theme_relative) {
		if (!strncmp(v, "blank:", 6) || 
			!strncmp(v, "box:", 4) ||
			!strncmp(v, "spr:", 4)) /* hack for special files*/
			return parse_path(v, data);
		rc = parse_path(v, data);
		if (rc || !*p || !*p[0])
			return rc;

		if (curtheme_loading && curtheme_loading->type == THEME_GAME) {
			np = getfilepath(curtheme_loading->path, *p);
			if (!*np)
				return -1;
			free(*p); *p = np;
		}
		return 0;
	}
	return parse_full_path(v, data);
}

struct parser cmd_parser[] = {
	{ "scr.w", parse_int, &game_theme.w, 0 },
	{ "scr.h", parse_int, &game_theme.h, 0 },
	{ "scr.gfx.scalable", parse_int, &game_theme.gfx_scalable, 0 },
	{ "scr.col.bg", parse_color, &game_theme.bgcol, 0 },
	{ "scr.gfx.icon", theme_parse_full_path, &game_theme.icon_name, CHANGED_ICON },
	{ "scr.gfx.bg", theme_parse_full_path, &game_theme.bg_name, CHANGED_BG },
	{ "scr.gfx.cursor.normal", theme_parse_full_path, &game_theme.cursor_name, CHANGED_CURSOR },
	{ "scr.gfx.cursor.x", parse_int, &game_theme.cur_x, 0 },
	{ "scr.gfx.cursor.y", parse_int, &game_theme.cur_y, 0 },
	{ "scr.gfx.use", theme_parse_full_path, &game_theme.use_name, CHANGED_USE }, /* compat */
	{ "scr.gfx.cursor.use", theme_parse_full_path, &game_theme.use_name, CHANGED_USE },
	{ "scr.gfx.pad", parse_int, &game_theme.pad, 0 }, 
	{ "scr.gfx.x", parse_int, &game_theme.gfx_x, 0 },
	{ "scr.gfx.y", parse_int, &game_theme.gfx_y, 0 },
	{ "scr.gfx.w", parse_int, &game_theme.max_scene_w, 0 },
	{ "scr.gfx.h", parse_int, &game_theme.max_scene_h, 0 },
	{ "scr.gfx.mode", parse_gfx_mode, &game_theme.gfx_mode, 0 },

	{ "win.align", parse_win_align, &game_theme.win_align, 0 },
	{ "win.x", parse_int, &game_theme.win_x, 0 },
	{ "win.y", parse_int, &game_theme.win_y, 0 },
	{ "win.w", parse_int, &game_theme.win_w, 0 },
	{ "win.h", parse_int, &game_theme.win_h, 0 },
	{ "win.ways.mode", parse_ways_mode, &game_theme.ways_mode, 0 },
	{ "win.scroll.mode", parse_int, &game_theme.win_scroll_mode, 0 },

	{ "win.fnt.name", theme_parse_full_path, &game_theme.font_name, CHANGED_FONT },
	{ "win.fnt.size", parse_int, &game_theme.font_size, CHANGED_FONT },
	{ "win.fnt.height", parse_float, &game_theme.font_height, 0 },
/* compat mode directive */
	{ "win.gfx.h", parse_int, &game_theme.max_scene_h, 0 },
/* here it was */
	{ "win.gfx.up", theme_parse_full_path, &game_theme.a_up_name, CHANGED_UP },
	{ "win.gfx.down", theme_parse_full_path, &game_theme.a_down_name, CHANGED_DOWN },
	{ "win.up.x", parse_int, &game_theme.a_up_x, 0 },
	{ "win.up.y", parse_int, &game_theme.a_up_y, 0 },
	{ "win.down.x", parse_int, &game_theme.a_down_x, 0 },
	{ "win.down.y", parse_int, &game_theme.a_down_y, 0 },
	{ "win.col.fg", parse_color, &game_theme.fgcol, 0 }, 
	{ "win.col.link", parse_color, &game_theme.lcol, 0 },
	{ "win.col.alink", parse_color, &game_theme.acol, 0 }, 

	{ "inv.x", parse_int, &game_theme.inv_x, 0 },
	{ "inv.y", parse_int, &game_theme.inv_y, 0 },
	{ "inv.w", parse_int, &game_theme.inv_w, 0 },
	{ "inv.h", parse_int, &game_theme.inv_h, 0 },
	{ "inv.mode", parse_inv_mode, &game_theme.inv_mode, 0 },
	{ "inv.horiz", parse_inv_mode, &game_theme.inv_mode, 0 },

	{ "inv.col.fg", parse_color, &game_theme.icol, 0 },
	{ "inv.col.link", parse_color, &game_theme.ilcol, 0 },
	{ "inv.col.alink", parse_color, &game_theme.iacol, 0 }, 
	{ "inv.fnt.name", theme_parse_full_path, &game_theme.inv_font_name, CHANGED_IFONT },
	{ "inv.fnt.size", parse_int, &game_theme.inv_font_size, CHANGED_IFONT },
	{ "inv.fnt.height", parse_float, &game_theme.inv_font_height, 0 },
	{ "inv.gfx.up", theme_parse_full_path, &game_theme.inv_a_up_name, CHANGED_IUP },
	{ "inv.gfx.down", theme_parse_full_path, &game_theme.inv_a_down_name, CHANGED_IDOWN },
	{ "inv.up.x", parse_int, &game_theme.inv_a_up_x, 0 },
	{ "inv.up.y", parse_int, &game_theme.inv_a_up_y, 0 },
	{ "inv.down.x", parse_int, &game_theme.inv_a_down_x, 0 },
	{ "inv.down.y", parse_int, &game_theme.inv_a_down_y, 0 },

	{ "menu.col.bg", parse_color, &game_theme.menu_bg, 0 },
	{ "menu.col.fg", parse_color, &game_theme.menu_fg, 0 },
	{ "menu.col.link", parse_color, &game_theme.menu_link, 0 },
	{ "menu.col.alink", parse_color, &game_theme.menu_alink, 0 },
	{ "menu.col.alpha", parse_int, &game_theme.menu_alpha, 0 },
	{ "menu.col.border", parse_color, &game_theme.border_col, 0 },
	{ "menu.bw", parse_int, &game_theme.border_w, 0 },
	{ "menu.fnt.name", theme_parse_full_path, &game_theme.menu_font_name, CHANGED_MFONT },
	{ "menu.fnt.size", parse_int, &game_theme.menu_font_size, CHANGED_MFONT },
	{ "menu.fnt.height", parse_float, &game_theme.menu_font_height, 0 },
	{ "menu.gfx.button", theme_parse_full_path, &game_theme.menu_button_name, CHANGED_BUTTON },
	{ "menu.button.x", parse_int, &game_theme.menu_button_x, 0 },
	{ "menu.button.y", parse_int, &game_theme.menu_button_y, 0 },
/* compat */
	{ "menu.buttonx", parse_int, &game_theme.menu_button_x, 0 },
	{ "menu.buttony", parse_int, &game_theme.menu_button_y, 0 },

	{ "snd.click", theme_parse_full_path, &game_theme.click_name, CHANGED_CLICK },
	{ "include", parse_include, NULL, 0 },
	{ NULL, NULL, NULL, 0 },
};

#define TF_POSX	1
#define TF_POSY	2
#define TF_NEG	4

typedef struct {
	const char *name;
	int *val;
	int flags;
} theme_scalable_t;

static theme_scalable_t theme_scalables[] = {
	{ "scr.w", &game_theme.w, 0 },
	{ "scr.h", &game_theme.h, 0 },
	{ "scr.gfx.cursor.x", &game_theme.cur_x, 0 },
	{ "scr.gfx.cursor.y", &game_theme.cur_y, 0 },
	{ "scr.gfx.pad", &game_theme.pad, 0 }, 
	{ "scr.gfx.x", &game_theme.gfx_x, TF_POSX },
	{ "scr.gfx.y", &game_theme.gfx_y, TF_POSY },
	{ "scr.gfx.w", &game_theme.max_scene_w, TF_NEG },
	{ "scr.gfx.h", &game_theme.max_scene_h, TF_NEG },
	{ "win.x", &game_theme.win_x, TF_POSX },
	{ "win.y", &game_theme.win_y, TF_POSY },
	{ "win.w", &game_theme.win_w, 0 },
	{ "win.h", &game_theme.win_h, 0 },
	{ "win.fnt.size", &game_theme.font_size, 0 },
	{ "inv.x", &game_theme.inv_x, TF_POSX },
	{ "inv.y", &game_theme.inv_y, TF_POSY },
	{ "inv.w", &game_theme.inv_w, 0 },
	{ "inv.h", &game_theme.inv_h, 0 },
	{ "inv.fnt.size", &game_theme.inv_font_size, 0 },
	{ "menu.fnt.size", &game_theme.menu_font_size, 0 },
	{ "menu.button.x", &game_theme.menu_button_x, TF_POSX },
	{ "menu.button.y", &game_theme.menu_button_y, TF_POSY },
	{ "win.up.x", &game_theme.a_up_x, TF_POSX | TF_NEG },
	{ "win.up.y", &game_theme.a_up_y, TF_POSY | TF_NEG },
	{ "win.down.x", &game_theme.a_down_x, TF_POSX | TF_NEG },
	{ "win.down.y", &game_theme.a_down_y, TF_POSY | TF_NEG },
	{ "inv.up.x", &game_theme.inv_a_up_x, TF_POSX | TF_NEG },
	{ "inv.up.y", &game_theme.inv_a_up_y, TF_POSY | TF_NEG },
	{ "inv.down.x", &game_theme.inv_a_down_x, TF_POSX | TF_NEG },
	{ "inv.down.y", &game_theme.inv_a_down_y, TF_POSY | TF_NEG },
	{ NULL, NULL, 0 },
};
static int theme_scalables_unscaled[sizeof(theme_scalables)/sizeof(theme_scalable_t)];

struct game_theme game_theme = {
	.scale = 1.0f,
	.w = 800,
	.h = 480,
	.gfx_scalable = 1,
	.bg_name = NULL,
	.bg = NULL,
	.use_name = NULL,
	.cursor_name = NULL,
	.icon = NULL,
	.icon_name = NULL,
	.use = NULL,
	.cursor = NULL,
	.cur_x = 0,
	.cur_y = 0,
	.font_name = NULL,
	.font_height = 1.0f,
	.font = NULL,
	.a_up_name = NULL,
	.a_down_name = NULL,
	.a_up = NULL,
	.a_down = NULL,
	.a_up_x = -1,
	.a_up_y = -1,
	.a_down_x = -1,
	.a_down_y = -1,
	.inv_font_name = NULL,
	.inv_font = NULL,
	.inv_font_height = 1.0f,
	.inv_a_up_name = NULL,
	.inv_a_down_name = NULL,
	.inv_a_up_x = -1,
	.inv_a_up_y = -1,
	.inv_a_down_x = -1,
	.inv_a_down_y = -1,
	.inv_a_up = NULL,
	.inv_a_down = NULL,
	.menu_font_name = NULL,
	.menu_font_height = 1.0f,
	.menu_font = NULL,
	.menu_button_name = NULL,
	.menu_button = NULL,
	.win_align = ALIGN_JUSTIFY,
	.win_scroll_mode = 2,
	.gfx_mode = GFX_MODE_EMBEDDED,
	.inv_mode = INV_MODE_VERT | INV_ALIGN_SET(ALIGN_LEFT),
	.ways_mode = ALIGN_TOP,
	.click_name = NULL,
	.click = NULL,
	.xoff = 0,
	.yoff = 0,
	.changed = 0,
};
struct	game_theme game_theme;

static void free_theme_strings(void)
{
	struct game_theme *t = &game_theme;
	FREE(t->use_name);
	FREE(t->icon_name);
	FREE(t->cursor_name);
	FREE(t->bg_name);
	FREE(t->inv_a_up_name);
	FREE(t->inv_a_down_name);
	FREE(t->a_down_name);
	FREE(t->a_up_name);
	FREE(t->font_name);
	FREE(t->inv_font_name);
	FREE(t->menu_font_name);
	FREE(t->menu_button_name);
	FREE(t->click_name);
}

int game_theme_free(void)
{
	free_theme_strings();

	if (game_theme.font)
		fnt_free(game_theme.font);
	if (game_theme.inv_font)
		fnt_free(game_theme.inv_font);
	if (game_theme.menu_font)
		fnt_free(game_theme.menu_font);

	if (game_theme.a_up)
		gfx_free_image(game_theme.a_up);
	if (game_theme.a_down)
		gfx_free_image(game_theme.a_down);
	if (game_theme.inv_a_up)
		gfx_free_image(game_theme.inv_a_up);
	if (game_theme.inv_a_down)
		gfx_free_image(game_theme.inv_a_down);

	if (game_theme.use)
		gfx_free_image(game_theme.use);
	if (game_theme.cursor) {
		gfx_free_image(game_theme.cursor);
		gfx_set_cursor(NULL, 0, 0);
	}
	if (game_theme.bg)
		gfx_free_image(game_theme.bg);

	if (game_theme.menu_button)
		gfx_free_image(game_theme.menu_button);

	if (game_theme.click) {
		sound_put(game_theme.click);
	}

	if (game_theme.icon) {
		gfx_set_icon(NULL);
		gfx_free_image(game_theme.icon);
	}

	game_theme.font = game_theme.inv_font = game_theme.menu_font = NULL;
	game_theme.a_up = game_theme.a_down = game_theme.use = NULL;
	game_theme.inv_a_up = game_theme.inv_a_down = NULL;
	game_theme.menu_button = NULL;
	game_theme.bg = NULL;
	game_theme.click = NULL;
	game_theme.cur_x = game_theme.cur_y = 0;
	game_theme.cursor = game_theme.use = NULL;
	game_theme.icon = NULL;
	return 0;
}

int theme_img_scale(img_t *p)
{
	img_t pic;
	float v = game_theme.scale;
	if (!p || !*p || v == 1.0f)
		return 0;

	if (!cache_have(gfx_image_cache(), *p))
		return 0; /* do not scale sprites! */

	pic = gfx_scale(*p, v, v, SCALABLE_THEME_SMOOTH);
	if (!pic)
		return -1;
	gfx_free_image(*p); 
	*p = pic;
	return 0;
}

static  int game_theme_scale(int w, int h)
{
	int i;
	float xs, ys, v;
	int xoff, yoff;
	struct game_theme *t = &game_theme;

	if (w < 0 || h < 0 || (w == t->w && h == t->h)) {
		t->scale = 1.0f;
		t->xoff = 0;
		t->yoff = 0;
		w = t->w;
		h = t->h;
		goto out;
	}


	xs = (float)w / (float)t->w;
	ys = (float)h / (float)t->h;
	
	v = (xs < ys)?xs:ys;

	if (!SCALABLE_THEME) {
		if (v > 1.0f) {
			int ff = 1;
			while (ff && ff <= v && ff < 0x1000)
				ff <<= 1;
			ff >>= 1;
			v = ff;
		} else {
			float f = ceil(1.0f / v);
			int ff = 1;
			while (ff && ff < f && ff < 0x1000)
				ff <<= 1;
			v = 1.0f / (float)ff;
		}
	}

	xoff = (w - t->w*v)/2;
	yoff = (h - t->h*v)/2;

	if (xoff < 0)
		xoff = 0;
	if (yoff < 0)
		yoff = 0;

	t->scale = v;
	t->xoff = xoff;
	t->yoff = yoff;
out:
	for (i = 0; theme_scalables[i].name; i++) {
		int val = *(theme_scalables[i].val);
		theme_scalables_unscaled[i] = val;
		if (val == -1 && (theme_scalables[i].flags & TF_NEG))
			continue;
		val *= t->scale;
		if (theme_scalables[i].flags & TF_POSX)
			val += t->xoff;
  		if (theme_scalables[i].flags & TF_POSY)
			val += t->yoff;
		*(theme_scalables[i].val) = val;
	}
	t->w = w;
	t->h = h;
	return 0;
}
extern int parse_relative_path;

char *theme_getvar(char *name)
{
	int i;
	for (i = 0; theme_scalables[i].name; i ++) {
		int val;
		char buf[64];
		if (strcmp(theme_scalables[i].name, name))
			continue;
		val = theme_scalables_unscaled[i];
		sprintf(buf, "%d", val);
		return strdup(buf);
	}
	/* so, it is a string or like this */
	for (i = 0; cmd_parser[i].cmd; i++) {
		int *num;
		char *s;
		float *f;
		char buf[64];
		if (strcmp(cmd_parser[i].cmd, name))
			continue;
		if (cmd_parser[i].fn == parse_int) {
			num = (int *)cmd_parser[i].p;
			sprintf(buf, "%d", *num);
			return strdup(buf);
		} else if (cmd_parser[i].fn == theme_parse_full_path) {
			s = *((char **)cmd_parser[i].p);
			if (!s)
				return NULL;
			return strdup(s);
		} else if (cmd_parser[i].fn == parse_inv_mode) {
			if (out_inv_mode(cmd_parser[i].p, &s))
				return NULL;
			return s;
		} else if (cmd_parser[i].fn == parse_ways_mode) {
			if (out_ways_mode(cmd_parser[i].p, &s))
				return NULL;
			return s;
		} else if (cmd_parser[i].fn == parse_gfx_mode) {
			if (out_gfx_mode(cmd_parser[i].p, &s))
				return NULL;
			return s;
		} else if (cmd_parser[i].fn == parse_float) {
			f = (float*)cmd_parser[i].p;
			sprintf(buf, "%f", *f);
			return strdup(buf);
		} else if (cmd_parser[i].fn == parse_color) {
			if (out_color(cmd_parser[i].p, &s))
				return NULL;
			return s;
		} else
			return NULL;
		break;
	}
	return NULL;
}

static int theme_process_cmd(char *n, char *v, struct parser *cmd_parser)
{
	int i;
	n = strip(n);
	v = strip(v);

	if (process_cmd(n, v, cmd_parser))
		return -1;

	for (i = 0; cmd_parser[i].cmd; i++) {
		if (!strcmp(cmd_parser[i].cmd, n)) {
			game_theme.changed |= cmd_parser[i].aux;
			return 0;
		}
	}

	return -1;
}

int theme_setvar(char *name, char *val)
{
	int rc = -1;
	struct game_theme *t = &game_theme;
	theme_relative = 1;
	if (!theme_process_cmd(name, val, cmd_parser)) {
		int i;
		for (i = 0; theme_scalables[i].name; i++) {
			int val;
			if (strcmp(theme_scalables[i].name, name))
				continue;
			val = *(theme_scalables[i].val);
			theme_scalables_unscaled[i] = val;
			if (val == -1 && (theme_scalables[i].flags & TF_NEG))
				continue;
			val *= t->scale;
			if (theme_scalables[i].flags & TF_POSX)
				val += t->xoff;
	  		if (theme_scalables[i].flags & TF_POSY)
				val += t->yoff;
			*(theme_scalables[i].val) = val;
			break;
		}
		rc = 0;
	}
	theme_relative = 0;
	return rc;
}

static int theme_bg_scale(void)
{
	struct game_theme *t = &game_theme;
	if (t->bg) {
		img_t screen, pic;
		int xoff = (t->w - gfx_img_w(t->bg))/2;
		int yoff = (t->h - gfx_img_h(t->bg))/2;
		if (xoff < 0)
			xoff = 0;
		if (yoff < 0)
			yoff = 0;
		if (t->scale != 1.0f || xoff || yoff) {
			pic = gfx_new(t->w, t->h);
			if (!pic)
				return -1;
			screen = gfx_screen(pic);
			gfx_img_fill(pic, 0, 0, t->w, t->h, gfx_col(0,0,0));
			gfx_draw(t->bg, xoff, yoff);
			gfx_screen(screen);
			gfx_free_image(t->bg);
			t->bg = pic;
		}
	}	
	return 0;
}

int game_theme_optimize(void)
{
/* todo: check errors */
	struct game_theme *t = &game_theme;

	if (t->bg && cache_have(gfx_image_cache(), t->bg)) {
		t->bg = gfx_display_alpha(t->bg);
		gfx_unset_alpha(t->bg);
	}
	if (t->a_up && cache_have(gfx_image_cache(), t->a_up))
		t->a_up = gfx_display_alpha(t->a_up);
	if (t->a_down && cache_have(gfx_image_cache(), t->a_down))
		t->a_down = gfx_display_alpha(t->a_down);
	if (t->inv_a_up && cache_have(gfx_image_cache(), t->inv_a_up))
		t->inv_a_up = gfx_display_alpha(t->inv_a_up);
	if (t->inv_a_down && cache_have(gfx_image_cache(), t->inv_a_down))
		t->inv_a_down = gfx_display_alpha(t->inv_a_down);
	if (t->use && cache_have(gfx_image_cache(), t->use))
		t->use = gfx_display_alpha(t->use);
	if (t->cursor && cache_have(gfx_image_cache(), t->cursor)) {
		t->cursor = gfx_display_alpha(t->cursor);
		gfx_set_cursor(t->cursor, t->cur_x, t->cur_y);
	}
	if (t->menu_button && cache_have(gfx_image_cache(), t->menu_button))
		t->menu_button = gfx_display_alpha(t->menu_button);
	return 0;
}

static int game_theme_update_data(void)
{
	struct game_theme *t = &game_theme;
	const char *res = NULL;
	int idf = idf_only(instead_idf(), 0);
	if (t->font_name && (t->changed & CHANGED_FONT)) {
		fnt_free(t->font);
		if (!(t->font = fnt_load(t->font_name, FONT_SZ(t->font_size)))) {
			res = t->font_name;
			goto err;
		}
	}

	if (t->inv_font_name && (t->changed & CHANGED_IFONT)) {
		fnt_free(t->inv_font);
		if (!(t->inv_font = fnt_load(t->inv_font_name, FONT_SZ(t->inv_font_size)))) {
			res = t->inv_font_name;
			goto err;
		}
	}

	if (t->menu_font_name && (t->changed & CHANGED_MFONT)) {
		int m = FONT_SZ(t->inv_font_size);
		if (MAX_MENU_LINES * m * game_theme.menu_font_height > game_theme.h)
			m = game_theme.h / MAX_MENU_LINES / game_theme.menu_font_height;
		else if (m < t->menu_font_size)
			m = t->menu_font_size;
/*		fprintf(stderr, "%d %d > %d? %d", (int)FONT_SZ(t->inv_font_size), (int)FONT_SZ(t->inv_font_size) * MAX_MENU_LINES, game_theme.h, m); */
		fnt_free(t->menu_font);
		if (!(t->menu_font = fnt_load(t->menu_font_name, m))) { /* do not scale menu!!! */
			res = t->menu_font_name;
			goto err;
		}
	}

	if (t->a_up_name && (t->changed & CHANGED_UP)) {
		gfx_free_image(t->a_up);
		if (!(t->a_up = gfx_load_image(t->a_up_name))) {
			res = t->a_up_name;
			goto err;
		}
		if (theme_img_scale(&t->a_up))
			goto err;
	}

	if (t->a_down_name && (t->changed & CHANGED_DOWN)) {
		gfx_free_image(t->a_down);
		if (!(t->a_down = gfx_load_image(t->a_down_name))) {
			res = t->a_down_name;
			goto err;
		}
		if (theme_img_scale(&t->a_down))
			goto err;
	}

	if (t->inv_a_up_name && (t->changed & CHANGED_IUP)) {
		gfx_free_image(t->inv_a_up);
		if (!(t->inv_a_up = gfx_load_image(t->inv_a_up_name))) {
			res = t->inv_a_up_name;
			goto err;
		}
		if (theme_img_scale(&t->inv_a_up))
			goto err;
	}

	if (t->inv_a_down_name && (t->changed & CHANGED_IDOWN)) {
		gfx_free_image(t->inv_a_down);
		if (!(t->inv_a_down = gfx_load_image(t->inv_a_down_name))) {
			res = t->inv_a_down_name;
			goto err;
		}
		if (theme_img_scale(&t->inv_a_down))
			goto err;
	}

	if (t->bg_name && (t->changed & CHANGED_BG)) {
		gfx_free_image(t->bg);
		t->bg = NULL;
		if (t->bg_name[0] && !(t->bg = gfx_load_image(t->bg_name))) {
			res = t->bg_name;
			goto skip; /* not fatal */
		}
		if (theme_img_scale(&t->bg))
			goto err;
		if (theme_bg_scale())
			goto err;
	}
skip:
	if (t->icon_name && (t->changed & CHANGED_ICON)) {
		if (t->icon)
			gfx_free_image(t->icon);
		t->icon = gfx_load_image(t->icon_name);
	}

	if (t->use_name && (t->changed & CHANGED_USE)) {
		gfx_free_image(t->use);	
		if (!(t->use = gfx_load_image(t->use_name))) {
			res = t->use_name;
			goto err;
		}
		if (theme_img_scale(&t->use))
			goto err;
	}

	if (t->cursor_name && (t->changed & CHANGED_CURSOR)) {
		gfx_free_image(t->cursor);
		if (!(t->cursor = gfx_load_image(t->cursor_name))) {
			res = t->cursor_name;
			goto err;
		}
		if (theme_img_scale(&t->cursor))
			goto err;
		gfx_set_cursor(t->cursor, t->cur_x, t->cur_y);
	}
	
	if (t->menu_button_name && (t->changed & CHANGED_BUTTON)) {
		gfx_free_image(t->menu_button);
		if (!(t->menu_button = gfx_load_image(t->menu_button_name))) {
			res = t->menu_button_name;
			goto err;
		}
		if (theme_img_scale(&t->menu_button))
			goto err;
	}
	
	if (t->click_name && (t->changed & CHANGED_CLICK)) {
		sound_put(t->click);
		t->click = sound_get(t->click_name);
	}

/*	free_theme_strings(); */ /* todo, font */
	t->changed = 0;
	if (!t->cursor || !t->use || !t->inv_a_up || !t->inv_a_down || !t->a_down || !t->a_up ||
		!t->font || !t->inv_font || !t->menu_font || !t->menu_button) {
		fprintf(stderr,"Can't init theme. Not all required elements are defined.\n");
		goto err;
	}
	idf_only(instead_idf(), idf);
	return 0;
err:
	idf_only(instead_idf(), idf);
	t->changed = 0;
	game_res_err_msg(res, 1);
	return -1;
}

int game_theme_update(void)
{
	game_release_theme();
	if (game_theme_update_data()) {
		fprintf(stderr, "Can not update theme!\n");
		game_error();
		return -1;
	}

	if (game_apply_theme()) {
		fprintf(stderr, "Can not apply theme!\n");
		game_error();
		return -1;
	}
	return 0;
}

#if defined(ANDROID) || defined(IOS) || defined(WINRT)
extern void rotate_landscape(void);
extern void rotate_portrait(void);
extern void unlock_rotation(void);
#endif

int game_theme_init(void)
{
	int w  = opt_mode[0];
	int h  = opt_mode[1];

	game_cursor_show = 1;

	if (opt_fs && opt_hires && !gfx_get_max_mode(&w, &h, MODE_ANY)) {
#if defined(IOS) || defined(ANDROID) || defined(WINRT)
		if ((game_theme.w > game_theme.h && w < h) ||
			(game_theme.w < game_theme.h && w > h)) { /* rotated */
			if (gfx_get_max_mode(&w, &h, (game_theme.w > game_theme.h)?MODE_H:MODE_V)) {
				gfx_get_max_mode(&w, &h, MODE_ANY); /* fallback to any mode */
			}
		}
		if (game_theme.w > game_theme.h)
			rotate_landscape();
		else if (game_theme.w < game_theme.h)
			rotate_portrait();
		else
			unlock_rotation();
#endif
	}
#if defined(ANDROID) || defined(IOS) || defined(WINRT)
	else {
		unlock_rotation();
	}
#endif

	if (w == -1) { /* as theme */
#if !defined(IOS) /* IOS always hardware accelerated */
		if (gfx_get_max_mode(&w, &h, MODE_ANY) || (game_theme.w <= w && game_theme.h <= h)) {
			w = opt_mode[0];
			h = opt_mode[1];
		}
#endif
	}

	game_theme_scale(w, h);

	if (gfx_set_mode(game_theme.w, game_theme.h, opt_fs)) {
		opt_mode[0] = opt_mode[1] = -1; opt_fs = 0; /* safe options */
		return -1;
	}
	if (game_theme_update_data()) {
		fprintf(stderr, "Can not init theme!\n");
		game_theme_free();
		game_theme_select(DEFAULT_THEME);
		return -1;
	}
	gfx_bg(game_theme.bgcol);
	if (!DIRECT_MODE)
		game_clear(0, 0, game_theme.w, game_theme.h);
	gfx_flip();
	gfx_commit();
	return 0;
}

static int theme_parse_idf(idf_t idf, const char *path)
{
	idff_t idff = NULL;
	if (idf)
		idff = idf_open(idf, path);

	if (idff) {
		int rc = parse_idff(idff, path, cmd_parser);
		idf_close(idff);
		if (rc)
			fprintf(stderr, "Theme parsed with errors!\n");
		return rc;
	}
	if (parse_ini(dirpath(path), cmd_parser)) {
		fprintf(stderr, "Theme parsed with errors!\n");
/*		game_theme_free(); */
		return -1;
	}

	return 0;
}

int theme_load(const char *name)
{
	idf_t idf = NULL;

	if (theme_relative)
		idf = instead_idf();

	if (theme_parse_idf(idf, name))
		return 0; /* no theme loaded if error in parsing */
	return 0;
}

int theme_load_idf(idf_t idf, const char *name)
{
	if (theme_parse_idf(idf, name))
		return 0; /* no theme loaded if error in parsing */
	return 0;
}

struct	theme *themes = NULL;
int themes_nr = 0;

static int is_theme_idf(idf_t idf, const char *path, const char *n)
{
	int rc = 0;
	char *p = getpath(path, n);
	char *pp;
	if (!p)
		return 0;
	pp = malloc(strlen(p) + strlen(THEME_FILE) + 1);
	if (pp) {
		strcpy(pp, p);
		strcat(pp, THEME_FILE);
		if (idf)
			rc = !idf_access(idf, pp);
		else
			rc = !access(pp, R_OK);
		free(pp);
	}
	free(p);
	return rc;
}

static int is_theme(const char *path, const char *n)
{
	if (!n)
		return 0;
	if (!strcmp("..", n) || !strcmp(".", n))
		return 0;
	return is_theme_idf(NULL, path, n);
}

static char *theme_name(const char *path, const char *d_name)
{
	char *l;
	char *p = getfilepath(path, THEME_FILE);
	if (!p)
		goto err;
	l = lookup_lang_tag(p, "Name", ";", opt_lang);
	free(p);
	if (l) 
		return l;
err:
	return strdup(d_name);
}

static char *theme_name_idf(idf_t idf, const char *path, const char *d_name)
{
	char *l;	
	char *p = getfilepath(path, THEME_FILE);
	if (!p)
		goto err;
	l = lookup_lang_tag_idf(idf, p, "Name", ";", opt_lang);
	free(p);
	if (l) 
		return l;
err:
	return strdup(d_name);
}

static int cmp_theme(const void *p1, const void *p2)
{
	const struct theme *t1 = (const struct theme*)p1;
	const struct theme *t2 = (const struct theme*)p2;
	if (t1->type != t2->type)
		return t1->type - t2->type;
	return strcmp(t1->name, t2->name);
}

static void themes_sort()
{
	qsort(themes, themes_nr, sizeof(struct theme), cmp_theme);
}

struct theme *theme_lookup(const char *name, int type);

int themes_count(int type)
{
	int rc = 0;
	int i;
	for (i = 0; i < themes_nr; i++)
		rc = rc + (themes[i].type == type);
	return rc;
}

int themes_max(int *type)
{
	int n;
	int count = themes_count(THEME_GLOBAL);

	if (type)
		*type = THEME_GLOBAL;

	if ((n = themes_count(THEME_GAME)) > 0 && opt_owntheme) {
		count = n;
		if (type)
			*type = THEME_GAME;
	}

	return count;
}

void themes_drop(int type)
{
	int new_size;
	struct theme *new_themes = NULL;
	int rc, i, k = 0;
	rc  = themes_count(type);
	if (!rc)
		return;
	new_size = (themes_nr - rc) * sizeof(struct theme);
	if (new_size)
		new_themes = malloc(new_size);

	if (!new_themes) {
		fprintf(stderr, "Fatal: can't alloc memory.\n");
		return;
	}

	for (i = 0; i < themes_nr; i ++) {
		if (themes[i].type == type) {
			free(themes[i].path);
			free(themes[i].dir);
			free(themes[i].name);
		} else {
			char *p = curtheme_dir[themes[i].type];
			new_themes[k++] = themes[i];
			if (p && !strlowcmp(p, themes[i].dir))
				curtheme_dir[themes[i].type] = p;
		}
	}
	themes_nr = k;
	curtheme_dir[type] = NULL;

	free(themes);
	themes = new_themes;
}

int themes_lookup_idf(idf_t idf, const char *path, int type)
{
	char *p;
	idff_t idf_dir;
	int n = 0, i = 0;
	struct theme *new_themes;
	char *idf_de;
	if (!idf)
		return -1;
	idf_dir = idf_opendir(idf, path);
	if (!idf_dir)
		return -1;
	while ((idf_de = idf_readdir(idf_dir))) {
		if (theme_lookup(idf_de, type))
			continue;
		if (!is_theme_idf(idf, path, idf_de))
			continue;
		n ++;
	}
	if (!n)
		goto out;
	idf_closedir(idf_dir); idf_dir = idf_opendir(idf, path);
	if (!idf_dir)
		return -1;
	new_themes = realloc(themes, sizeof(struct theme) * (n + themes_nr));
	if (!new_themes) {
		idf_closedir(idf_dir);
		return -1;
	}
	themes = new_themes;
	while ((idf_de = idf_readdir(idf_dir)) && i < n) {
		if (theme_lookup(idf_de, type))
			continue;
		if (!is_theme_idf(idf, path, idf_de))
			continue;
		p = getpath(path, idf_de);
		themes[themes_nr].path = p;
		themes[themes_nr].dir = strdup(idf_de);
		themes[themes_nr].name = theme_name_idf(idf, p, idf_de);
		themes[themes_nr].type = type;
		themes[themes_nr].idf = 1;
		themes_nr ++;
		i ++;
	}
out:
	idf_closedir(idf_dir);
	themes_sort();
	return 0;
}

int themes_lookup(const char *path, int type)
{
	char *p;
	int n = 0, i = 0;
	DIR *d;
	struct dirent *de;
	struct theme *new_themes;

	if (!path)
		return 0;

	d = opendir(path);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (theme_lookup(de->d_name, type))
			continue;
		if (!is_theme(path, de->d_name))
			continue;
		n ++;
	}
	if (!n)
		goto out;
	closedir(d); d = opendir(path);
	if (!d)
		return -1;
	new_themes = realloc(themes, sizeof(struct theme) * (n + themes_nr));
	if (!new_themes) {
		closedir(d);
		return -1;
	}
	themes = new_themes;
	while ((de = readdir(d)) && i < n) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (theme_lookup(de->d_name, type))
			continue;
		if (!is_theme(path, de->d_name))
			continue;
		p = getpath(path, de->d_name);
		themes[themes_nr].path = p;
		themes[themes_nr].dir = strdup(de->d_name);
		themes[themes_nr].name = theme_name(p, de->d_name);
		themes[themes_nr].type = type;
		themes[themes_nr].idf = 0;
		themes_nr ++;
		i ++;
	}
out:
	closedir(d);
	themes_sort();
	return 0;
}

int themes_rename(void)
{
	int i;
	char cwd[PATH_MAX];
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	for (i = 0; i < themes_nr; i++) {
		FREE(themes[i].name);
		themes[i].name = theme_name(dirpath(themes[i].path), themes[i].dir);
	}
	setdir(cwd);
	return 0;
}

struct theme *theme_lookup(const char *name, int type)
{
	int i;
	if (!name || !*name) {
		if (themes_nr == 1 && themes[0].type == type) 
			return &themes[0];
		return NULL;
	}
	for (i = 0; i<themes_nr; i ++) {
		if (!strlowcmp(themes[i].dir, name) && themes[i].type == type) {
			return &themes[i];
		}
	}
	return NULL;
}

int game_theme_load(const char *name, int type)
{
	struct theme *otheme = curtheme_loading;
	struct theme *theme;
	struct game *game;
	char cwd[PATH_MAX];
	int rc = -1;
	int rel = theme_relative;

	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);

	if (type == THEME_GLOBAL) {
		theme_relative = 0;
	} else {
		game = game_lookup(curgame_dir);
		if (!game)
			return -1;
		if (!game->idf)
			setdir(game->path);
		theme_relative = 1;
	}

	theme = theme_lookup(name, type);
	if (!theme)
		goto err;

	if (theme->idf) /* cwd is always game_cwd */
		strcpy(cwd, idf_getdir(instead_idf()));

	curtheme_loading = theme;

	if (theme->idf) {
		if (idf_setdir(instead_idf(), theme->path))
			goto err;
		if (theme_load_idf(instead_idf(), THEME_FILE))
			goto err;
	} else {
		if (setdir(theme->path))
			goto err;
		if (theme_load(THEME_FILE))
			goto err;
	}
	rc = 0;
err:
	curtheme_loading = otheme;

	if (theme && theme->idf)
		idf_setdir(instead_idf(), cwd);
	else
		setdir(cwd);

	theme_relative = rel;
	return rc;
}

int game_theme_select(const char *name)
{
	struct theme *theme;
	theme = theme_lookup(name, THEME_GAME);
	if (theme) {
		curtheme_dir[THEME_GAME] = theme->dir;
		game_cfg_save();
		return 0;
	}
	theme = theme_lookup(name, THEME_GLOBAL);
	if (!theme)
		return -1;
	curtheme_dir[THEME_GLOBAL] = theme->dir;
	return 0;
}

int game_default_theme(void)
{
	int rc;
	rc = game_theme_load(DEFAULT_THEME, THEME_GLOBAL);
	if (rc)
		return rc;
	if (opt_owntheme && themes_count(THEME_GAME) > 0)
		rc = game_theme_load(DEFAULT_THEME, THEME_GAME);
	return rc;
}
