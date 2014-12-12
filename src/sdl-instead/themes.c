/*
 * Copyright 2009-2014 Peter Kosyh <p.kosyh at gmail.com>
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

char	*curtheme_dir = NULL;

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
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	rc = game_theme_load(v);
/*	if (!rc)
		game_theme_select(v); */
	setdir(cwd);
	return rc;
}
struct parser cmd_parser[] = {
	{ "scr.w", parse_int, &game_theme.w, 0 },
	{ "scr.h", parse_int, &game_theme.h, 0 },
	{ "scr.gfx.scalable", parse_int, &game_theme.gfx_scalable, 0 },
	{ "scr.col.bg", parse_color, &game_theme.bgcol, 0 },
	{ "scr.gfx.bg", parse_full_path, &game_theme.bg_name, CHANGED_BG },
	{ "scr.gfx.cursor.normal", parse_full_path, &game_theme.cursor_name, CHANGED_CURSOR },
	{ "scr.gfx.cursor.x", parse_int, &game_theme.cur_x, 0 },
	{ "scr.gfx.cursor.y", parse_int, &game_theme.cur_y, 0 },
	{ "scr.gfx.use", parse_full_path, &game_theme.use_name, CHANGED_USE }, /* compat */
	{ "scr.gfx.cursor.use", parse_full_path, &game_theme.use_name, CHANGED_USE },
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
	{ "win.scroll.mode", parse_int, &game_theme.win_scroll_mode, 0 },

	{ "win.fnt.name", parse_full_path, &game_theme.font_name, CHANGED_FONT },
	{ "win.fnt.size", parse_int, &game_theme.font_size, CHANGED_FONT },
	{ "win.fnt.height", parse_float, &game_theme.font_height, 0 },
/* compat mode directive */
	{ "win.gfx.h", parse_int, &game_theme.max_scene_h, 0 },
/* here it was */
	{ "win.gfx.up", parse_full_path, &game_theme.a_up_name, CHANGED_UP },
	{ "win.gfx.down", parse_full_path, &game_theme.a_down_name, CHANGED_DOWN },
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
	{ "inv.fnt.name", parse_full_path, &game_theme.inv_font_name, CHANGED_IFONT },
	{ "inv.fnt.size", parse_int, &game_theme.inv_font_size, CHANGED_IFONT },
	{ "inv.fnt.height", parse_float, &game_theme.inv_font_height, 0 },
	{ "inv.gfx.up", parse_full_path, &game_theme.inv_a_up_name, CHANGED_IUP },
	{ "inv.gfx.down", parse_full_path, &game_theme.inv_a_down_name, CHANGED_IDOWN },
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
	{ "menu.fnt.name", parse_full_path, &game_theme.menu_font_name, CHANGED_MFONT },
	{ "menu.fnt.size", parse_int, &game_theme.menu_font_size, CHANGED_MFONT },
	{ "menu.fnt.height", parse_float, &game_theme.menu_font_height, 0 },
	{ "menu.gfx.button", parse_full_path, &game_theme.menu_button_name, CHANGED_BUTTON },
	{ "menu.button.x", parse_int, &game_theme.menu_button_x, 0 },
	{ "menu.button.y", parse_int, &game_theme.menu_button_y, 0 },
/* compat */
	{ "menu.buttonx", parse_int, &game_theme.menu_button_x, 0 },
	{ "menu.buttony", parse_int, &game_theme.menu_button_y, 0 },

	{ "snd.click", parse_full_path, &game_theme.click_name, CHANGED_CLICK },
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
	game_theme.font = game_theme.inv_font = game_theme.menu_font = NULL;
	game_theme.a_up = game_theme.a_down = game_theme.use = NULL;
	game_theme.inv_a_up = game_theme.inv_a_down = NULL;
	game_theme.menu_button = NULL;
	game_theme.bg = NULL;
	game_theme.click = NULL;
	game_theme.cur_x = game_theme.cur_y = 0;
	game_theme.cursor = game_theme.use = NULL;
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
		} else if (cmd_parser[i].fn == parse_full_path) {
			s = *((char **)cmd_parser[i].p);
			if (!s)
				return NULL;
			return strdup(s);
		} else if (cmd_parser[i].fn == parse_inv_mode) {
			if (out_inv_mode(cmd_parser[i].p, &s))
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
	int idf = idf_only(game_idf, 0);
	if (t->font_name && (t->changed & CHANGED_FONT)) {
		fnt_free(t->font);
		if (!(t->font = fnt_load(t->font_name, FONT_SZ(t->font_size))))
			goto err;
	}
	
	if (t->inv_font_name && (t->changed & CHANGED_IFONT)) {
		fnt_free(t->inv_font);
		if (!(t->inv_font = fnt_load(t->inv_font_name, FONT_SZ(t->inv_font_size))))
			goto err;
	}

	if (t->menu_font_name && (t->changed & CHANGED_MFONT)) {
		int m = FONT_SZ(t->inv_font_size);
		if (MAX_MENU_LINES * m * game_theme.menu_font_height > game_theme.h)
			m = game_theme.h / MAX_MENU_LINES / game_theme.menu_font_height;
		else if (m < t->menu_font_size)
			m = t->menu_font_size;
/*		fprintf(stderr, "%d %d > %d? %d", (int)FONT_SZ(t->inv_font_size), (int)FONT_SZ(t->inv_font_size) * MAX_MENU_LINES, game_theme.h, m); */
		fnt_free(t->menu_font);
		if (!(t->menu_font = fnt_load(t->menu_font_name, m))) /* do not scale menu!!! */
			goto err;
	}

	if (t->a_up_name && (t->changed & CHANGED_UP)) {
		gfx_free_image(t->a_up);
		if (!(t->a_up = gfx_load_image(t->a_up_name)))
			goto err;
		if (theme_img_scale(&t->a_up))
			goto err;
	}
	
	if (t->a_down_name && (t->changed & CHANGED_DOWN)) {
		gfx_free_image(t->a_down);
		if (!(t->a_down = gfx_load_image(t->a_down_name)))
			goto err;
		if (theme_img_scale(&t->a_down))
			goto err;
	}

	if (t->inv_a_up_name && (t->changed & CHANGED_IUP)) {
		gfx_free_image(t->inv_a_up);
		if (!(t->inv_a_up = gfx_load_image(t->inv_a_up_name)))
			goto err;
		if (theme_img_scale(&t->inv_a_up))
			goto err;
	}

	if (t->inv_a_down_name && (t->changed & CHANGED_IDOWN)) {
		gfx_free_image(t->inv_a_down);
		if (!(t->inv_a_down = gfx_load_image(t->inv_a_down_name)))
			goto err;
		if (theme_img_scale(&t->inv_a_down))
			goto err;
	}

	if (t->bg_name && (t->changed & CHANGED_BG)) {
		gfx_free_image(t->bg);
		t->bg = NULL;
		if (t->bg_name[0] && !(t->bg = gfx_load_image(t->bg_name)))
			goto skip; /* not fatal */
		if (theme_img_scale(&t->bg))
			goto err;
		if (theme_bg_scale())
			goto err;
	}
skip:
	if (t->use_name && (t->changed & CHANGED_USE)) {
		gfx_free_image(t->use);	
		if (!(t->use = gfx_load_image(t->use_name)))
			goto err;
		if (theme_img_scale(&t->use))
			goto err;
	}

	if (t->cursor_name && (t->changed & CHANGED_CURSOR)) {
		gfx_free_image(t->cursor);	
		if (!(t->cursor = gfx_load_image(t->cursor_name)))
			goto err;
		if (theme_img_scale(&t->cursor))
			goto err;
		gfx_set_cursor(t->cursor, t->cur_x, t->cur_y);
	}
	
	if (t->menu_button_name && (t->changed & CHANGED_BUTTON)) {
		gfx_free_image(t->menu_button);
		if (!(t->menu_button = gfx_load_image(t->menu_button_name)))
			goto err;
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
	idf_only(game_idf, idf);
	return 0;
err:
	idf_only(game_idf, idf);
	t->changed = 0;
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

int game_theme_init(void)
{
	color_t col = { .r = 0, .g = 0, .b = 0, .a = 0 };
	int w  = opt_mode[0];
	int h  = opt_mode[1];

	if (w == -1) { /* as theme */
#ifndef IOS /* IOS always hardware accelerated */
		if (gfx_get_max_mode(&w, &h) || (game_theme.w <= w && game_theme.h <= h)) {
			w = opt_mode[0];
			h = opt_mode[1];
		}
#endif
	}
#if 0 /* not useful */
	 else if (opt_fs) { /* selected and full screen */
		if (gfx_get_max_mode(&w, &h) || (opt_mode[0] <= w && opt_mode[1] <= h)) {
			w = opt_mode[0];
			h = opt_mode[1];
		}
	}
#endif
#if 0
	if (!SCALABLE_THEME && (w < game_theme.w || h < game_theme.h)) { /* no scalable? TODO: message? */
		w = game_theme.w;
		h = game_theme.h;
	}
#endif
	game_theme_scale(w, h);

	if (gfx_set_mode(game_theme.w, game_theme.h, opt_fs)) {
		opt_mode[0] = opt_mode[1] = -1; opt_fs = 0; /* safe options */
		return -1;
	}
	gfx_fill(0, 0, game_theme.w, game_theme.h, col);
	if (game_theme_update_data()) {
		fprintf(stderr, "Can not init theme!\n");
		game_theme_free();
		game_theme_select(DEFAULT_THEME);
		return -1;
	}
	return 0;
}

static int theme_parse(const char *path)
{
	idff_t idf = NULL;

	if (theme_relative)
		idf = idf_open(game_idf, path);

	if (idf) {
		int rc = parse_idff(idf, path, cmd_parser);
		idf_close(idf);
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
	if (theme_parse(name))
		return 0; /* no theme loaded if error in parsing */
	return 0;
}

struct	theme *themes = NULL;
int	themes_nr = 0;

static int is_theme(const char *path, const char *n)
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
		if (!access(pp, R_OK))
			rc = 1;
		free(pp);
	}
	free(p);
	return rc;
}

static char *theme_name(const char *path, const char *d_name)
{
	char *l;	
	char *p = getfilepath(path, THEME_FILE);
	if (!p)
		goto err;
	l = lookup_lang_tag(p, "Name", ";");
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
	return strcmp(t1->name, t2->name);
}

static void themes_sort()
{
	qsort(themes, themes_nr, sizeof(struct theme), cmp_theme);
}

static struct theme *theme_lookup(const char *name);

int themes_lookup(const char *path)
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
		if (theme_lookup(de->d_name))
			continue;
		if (!is_theme(path, de->d_name))
			continue;
		n ++;
	}
		
	rewinddir(d);
	if (!n)
		goto out;
	new_themes = realloc(themes, sizeof(struct theme) * (n + themes_nr));
	if (!new_themes) {
		closedir(d);
		return -1;
	}
	themes = new_themes;
	while ((de = readdir(d)) && i < n) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (theme_lookup(de->d_name))
			continue;
		if (!is_theme(path, de->d_name))
			continue;
		p = getpath(path, de->d_name);
		themes[themes_nr].path = p;
		themes[themes_nr].dir = strdup(de->d_name);
		themes[themes_nr].name = theme_name(p, de->d_name);
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

static struct theme *theme_lookup(const char *name)
{
	int i;
	if (!name || !*name) {
		if (themes_nr == 1) 
			return &themes[0];
		return NULL;
	}
	for (i = 0; i<themes_nr; i ++) {
		if (!strlowcmp(themes[i].dir, name)) {
			return &themes[i];
		}
	}
	return NULL;
}

int game_theme_load(const char *name)
{
	struct theme *theme;
	char cwd[PATH_MAX];
	int rc = -1;
	int rel = theme_relative;
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	theme = theme_lookup(name);
	theme_relative = 0;
	if (!theme || setdir(theme->path) || theme_load(THEME_FILE))
		goto err;
	rc = 0;
err:
	setdir(cwd);
	theme_relative = rel;
	return rc;
}

int game_theme_select(const char *name)
{
	struct theme *theme;
	theme = theme_lookup(name);
	if (!theme)
		return -1;
	curtheme_dir = theme->dir;
	return 0;
}

int game_default_theme(void)
{
	return game_theme_load(DEFAULT_THEME);
}
