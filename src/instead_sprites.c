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
#include "noise1234.h"

static LIST_HEAD(sprites);

static LIST_HEAD(fonts);

#define FN_SCALED 1

typedef struct {
	struct list_node list;
	char	*name;
	int	flags;
	fnt_t	fnt;
} _fnt_t;

typedef struct {
	struct list_node list;
	char	*name;
	img_t	img;
} _spr_t;

struct lua_pixels;

static img_t pixels_img(struct lua_pixels *hdr);

static void sprites_free(void)
{
/*	fprintf(stderr, "sprites free \n"); */
	while (!list_empty(&sprites)) {
		_spr_t *sp = list_top(&sprites, _spr_t, list);
		free(sp->name);
		cache_forget(gfx_image_cache(), sp->img);
		list_del(&sp->list);
		free(sp);
	}
	while (!list_empty(&fonts)) {
		_fnt_t *fn = list_top(&fonts, _fnt_t, list);
		fnt_free(fn->fnt);
		free(fn->name);
		list_del(&fn->list);
		free(fn);
	}
	game_pict_modify(NULL);
	cache_shrink(gfx_image_cache());
}

static _spr_t *sprite_lookup(const char *name)
{
	_spr_t *pos = NULL;
	_spr_t *sp;
	list_for_each(&sprites, pos, list) {
		sp = (_spr_t*)pos;
		if (!strcmp(name, sp->name)) {
			list_del(&sp->list);
			list_add(&sprites, &sp->list); /* move it on head */
			return sp;
		}
	}
	return NULL;
}

static _fnt_t *font_lookup(const char *name)
{
	_fnt_t *pos = NULL;
	_fnt_t *fn;
	list_for_each(&fonts, pos, list) {
		fn = (_fnt_t*)pos;
		if (!strcmp(name, fn->name)) {
			list_del(&fn->list);
			list_add(&fonts, &fn->list); /* move it on head */
			return fn;
		}
	}
	return NULL;
}

static _spr_t *sprite_new(const char *name, img_t img)
{
	_spr_t *sp;
	sp = malloc(sizeof(_spr_t));
	if (!sp)
		return NULL;
/*	INIT_LIST_HEAD(&sp->list); */
	sp->name = strdup(name);
	if (!sp->name) {
		free(sp);
		return NULL;
	}
	sp->img = img;
	if (cache_add(gfx_image_cache(), name, img)) {
		free(sp->name);
		free(sp);
		return NULL;
	}
/*	fprintf(stderr, "added: %s\n", name); */
	list_add(&sprites, &sp->list);
	return sp;
}

static _fnt_t *font_new(const char *name, fnt_t fnt)
{
	_fnt_t *fn;
	fn = malloc(sizeof(_fnt_t));
	if (!fn)
		return NULL;
/*	INIT_LIST_HEAD(&fn->list); */
	fn->name = strdup(name);
	if (!fn->name) {
		free(fn);
		return NULL;
	}
	fn->fnt = fnt;
	fn->flags = 0;
	list_add(&fonts, &fn->list);
	return fn;
}

static void sprite_name(const char *name, char *sname, int size)
{
	unsigned long h = 0;
	if (!sname || !size)
		return;
	h = hash_string(name);
	do { /* new uniq name */
		snprintf(sname, size, "spr:%lx", h);
		h ++;
	} while (sprite_lookup(sname) || cache_lookup(gfx_image_cache(), sname));
	sname[size - 1] = 0;
}

static void font_name(const char *name, char *sname, int size)
{
	unsigned long h = 0;
	if (!sname || !size)
		return;
	h = hash_string(name);
	do { /* new uniq name */
		snprintf(sname, size, "fnt:%lx", h);
		h ++;
	} while (font_lookup(sname));
	sname[size - 1] = 0;
}

static int _free_sprite(const char *key);

static int luaB_free_sprites(lua_State *L) {
	sprites_free();
	return 0;
}

static int luaB_load_sprite(lua_State *L) {
	img_t img = NULL;
	_spr_t *sp;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];
	struct lua_pixels *pixels = lua_touserdata(L, 1);
	const char *desc = luaL_optstring(L, 2, NULL);
	const char *fname = NULL;
	char pixels_name[32];

	if (!pixels)
		fname = luaL_optstring(L, 1, NULL);
	else {
		snprintf(pixels_name, sizeof(pixels_name), "pxl:%p", pixels);
		pixels_name[sizeof(pixels_name) - 1] = 0;
		fname = pixels_name;
	}

	if (!fname)
		return 0;

	if (pixels) {
		img = pixels_img(pixels);
		if (img)
			img = gfx_dup(img);
	} else {
		img = gfx_load_image((char*)fname);
		if (img)
			theme_img_scale(&img);
	}
	if (img)
		img = gfx_display_alpha(img); /*speed up */

	if (!img)
		goto err;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(fname, sname, sizeof(sname));
	} else
		key = desc;
	sp = sprite_new(key, img);
	if (!sp)
		goto err;

	lua_pushstring(L, key);
	return 1;
err:
	game_res_err_msg(fname, debug_sw);
	gfx_free_image(img);
	return 0;
}

static int luaB_load_font(lua_State *L) {
	int scaled = 0;
	fnt_t fnt = NULL;
	_fnt_t *fn;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];
	struct game_theme *t = &game_theme;

	const char *fname = luaL_optstring(L, 1, NULL);
	int sz = luaL_optnumber(L, 2, t->font_size);
	const char *desc = luaL_optstring(L, 3, NULL);
	if (!fname || sz == 0)
		return 0;
	if (sz > 0) {
		sz *= game_theme.scale;
		scaled = 1;
	} else
		sz = - sz; /* sz < 0 is unscalable */
	fnt = fnt_load((char*)fname, sz);

	if (!fnt)
		return 0;

	if (!desc || font_lookup(desc)) {
		key = sname;
		font_name(fname, sname, sizeof(sname));
	} else
		key = desc;

	fn = font_new(key, fnt);
	if (!fn)
		goto err;

	if (scaled)
		fn->flags |= FN_SCALED;

	lua_pushstring(L, key);
	return 1;
err:
	fnt_free(fnt);
	return 0;
}

static int luaB_text_size(lua_State *L) {
	_fnt_t *fn;
	int w = 0, h = 0;

	const char *font = luaL_optstring(L, 1, NULL);
	const char *text = luaL_optstring(L, 2, NULL);

	if (!font)
		return 0;

	fn = font_lookup(font);

	if (!fn)
		return 0;
	if (!text) {
		w = 0;
		if (fn->flags & FN_SCALED)
			h = ceil((float)fnt_height(fn->fnt) / game_theme.scale);
		else
			h = fnt_height(fn->fnt);
	} else {
		txt_size(fn->fnt, text, &w, &h);
		if (fn->flags & FN_SCALED) {
			w = ceil((float)w / game_theme.scale);
			h = ceil((float)h / game_theme.scale);
		}
	}
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

static int luaB_font_size_scaled(lua_State *L) {
	int sz = luaL_optnumber(L, 1, game_theme.font_size);
	lua_pushinteger(L, FONT_SZ(sz));
	return 1;
}

static int luaB_text_sprite(lua_State *L) {
	img_t img = NULL;
	_spr_t *sp;
	_fnt_t *fn;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *font = luaL_optstring(L, 1, NULL);
	const char *text = luaL_optstring(L, 2, NULL);
	char txtkey[32];
	const char *color = luaL_optstring(L, 3, NULL);
	int style = luaL_optnumber(L, 4, 0);
	const char *desc = luaL_optstring(L, 5, NULL);

	color_t col = { .r = game_theme.fgcol.r, .g = game_theme.fgcol.g, .b = game_theme.fgcol.b };

	if (!font)
		return 0;
	if (color)
		gfx_parse_color (color, &col);
	if (!text)
		text = "";

	fn = font_lookup(font);

	if (!fn)
		return 0;

	fnt_style(fn->fnt, style);

	img = fnt_render(fn->fnt, text, col);

	if (img)
		img = gfx_display_alpha(img); /*speed up */

	if (!img)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		strncpy(txtkey, text, sizeof(txtkey));
		txtkey[sizeof(txtkey) - 1] = 0;
		sprite_name(txtkey, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img);

	if (!sp)
		goto err;

	lua_pushstring(L, key);
	return 1;
err:
	gfx_free_image(img);
	return 0;
}

static img_t grab_sprite(const char *dst, int *xoff, int *yoff)
{
	img_t d;
	if (DIRECT_MODE && dst && !strcmp(dst, "screen")) {
		d = gfx_screen(NULL);
		*xoff = game_theme.xoff;
		*yoff = game_theme.yoff;
	} else {
		*xoff = 0;
		*yoff = 0;
		d = cache_lookup(gfx_image_cache(), dst);
	}
	return d;
}


static int luaB_sprite_size(lua_State *L) {
	img_t s = NULL;
	float v;
	int w, h;
	int xoff, yoff;
	const char *src = luaL_optstring(L, 1, NULL);
	if (!src)
		return 0;
	s = grab_sprite(src, &xoff, &yoff);
	if (!s)
		return 0;

	v = game_theme.scale;

	w = ceil ((float)(gfx_img_w(s) - xoff * 2) / v);
	h = ceil ((float)(gfx_img_h(s) - yoff * 2) / v);

	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

#define BLIT_COPY 0
#define BLIT_DRAW 1
#define BLIT_COMPOSE 2

static int luaB_blit_sprite(lua_State *L, int mode) {
	img_t s = NULL, d = NULL;
	img_t img2 = NULL;
	float v;
	struct lua_pixels *pixels = lua_touserdata(L, 1);
	const char *src = NULL;

	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	int w = luaL_optnumber(L, 4, -1);
	int h = luaL_optnumber(L, 5, -1);
	const char *dst = luaL_optstring(L, 6, NULL);
	int xx = luaL_optnumber(L, 7, 0);
	int yy = luaL_optnumber(L, 8, 0);
	int alpha = luaL_optnumber(L, 9, 255);
	int xoff = 0, yoff = 0;
	int xoff0 = 0, yoff0 = 0;

	if (!pixels)
		src = luaL_optstring(L, 1, NULL);

	if ((!src && !pixels) || !dst)
		return 0;

	if (pixels)
		s = pixels_img(pixels);
	if (!s)
		s = grab_sprite(src, &xoff0, &yoff0);

	d = grab_sprite(dst, &xoff, &yoff);

	if (!s || !d)
		return 0;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
		if (w != -1)
			w = ceil(w * v);
		if (h != -1)
			h = ceil(h * v);
		xx *= v;
		yy *= v;
	}

	if (w == -1)
		w = gfx_img_w(s) - 2 * xoff0;
	if (h == -1)
		h = gfx_img_h(s) - 2 * yoff0;

	game_pict_modify(d);

	if (alpha != 255) {
		img2 = gfx_alpha_img(s, alpha);
		if (img2)
			s = img2;
	}
	game_gfx_clip();

	switch (mode) {
	case BLIT_DRAW:
		gfx_draw_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	case BLIT_COPY:
		gfx_copy_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	case BLIT_COMPOSE:
		gfx_compose_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	default:
		break;
	}

	gfx_noclip();
	gfx_free_image(img2);
	lua_pushboolean(L, 1);
	return 1;
}


static int luaB_draw_sprite(lua_State *L)
{
	return luaB_blit_sprite(L, BLIT_DRAW);
}

static int luaB_copy_sprite(lua_State *L)
{
	return luaB_blit_sprite(L, BLIT_COPY);
}

static int luaB_compose_sprite(lua_State *L)
{
	return luaB_blit_sprite(L, BLIT_COMPOSE);
}

static int luaB_alpha_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	int alpha = luaL_optnumber(L, 2, 255);
	const char *desc = luaL_optstring(L, 3, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;

	img2 = gfx_alpha_img(s, alpha);
	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_colorkey_sprite(lua_State *L) {
	img_t s;
	color_t  col;

	const char *src = luaL_optstring(L, 1, NULL);
	const char *color = luaL_optstring(L, 2, NULL);

	if (color)
		gfx_parse_color(color, &col);

	if (!src)
		return 0;
	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;
	if (color)
		gfx_set_colorkey(s, col);
	else
		gfx_unset_colorkey(s);
	return 0;
}

static int luaB_dup_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];
	const char *src = luaL_optstring(L, 1, NULL);
	const char *desc = luaL_optstring(L, 2, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;

	img2 = gfx_dup(s);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_scale_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	float xs = luaL_optnumber(L, 2, 0);
	float ys = luaL_optnumber(L, 3, 0);
	int smooth = lua_toboolean(L, 4);
	const char *desc = luaL_optstring(L, 5, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;

	if (xs == 0)
		xs = 1.0f;

	if (ys == 0)
		ys = xs;

	img2 = gfx_scale(s, xs, ys, smooth);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}


static int luaB_rotate_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	float angle = luaL_optnumber(L, 2, 1.0f);
	int smooth = lua_toboolean(L, 3);
	const char *desc = luaL_optstring(L, 4, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;
	img2 = gfx_rotate(s, angle, smooth);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_fill_sprite(lua_State *L) {
	img_t d;
	float v;
	const char *dst = luaL_optstring(L, 1, NULL);
	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	int w = luaL_optnumber(L, 4, -1);
	int h = luaL_optnumber(L, 5, -1);
	const char *color = luaL_optstring(L, 6, NULL);
	int xoff = 0, yoff = 0;
	color_t  col = { .r = game_theme.bgcol.r, .g = game_theme.bgcol.g, .b = game_theme.bgcol.b };
	if (!dst)
		return 0;

	d = grab_sprite(dst, &xoff, &yoff);

	if (color)
		gfx_parse_color(color, &col);

	if (!d)
		return 0;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
		if (w != -1)
			w = ceil(w * v);
		if (h != -1)
			h = ceil(h * v);
	}
	if (w == -1)
		w = gfx_img_w(d) - 2 * xoff;
	if (h == -1)
		h = gfx_img_h(d) - 2 * yoff;
	game_pict_modify(d);
	game_gfx_clip();
	gfx_img_fill(d, x + xoff, y + yoff, w, h, col);
	gfx_noclip();
	lua_pushboolean(L, 1);
	return 1;
}

static int luaB_pixel_sprite(lua_State *L) {
	img_t d;
	float v;
	int rc, w, h;
	color_t  col = { .r = game_theme.bgcol.r, .g = game_theme.bgcol.g, .b = game_theme.bgcol.b, .a = 255 };
	const char *dst = luaL_optstring(L, 1, NULL);
	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	const char *color = luaL_optstring(L, 4, NULL);
	int alpha = luaL_optnumber(L, 5, 255);
	int xoff = 0, yoff = 0;

	if (!dst)
		return 0;

	d = grab_sprite(dst, &xoff, &yoff);

	if (color)
		gfx_parse_color(color, &col);

	if (!d)
		return 0;

	w = gfx_img_w(d) - 2 * xoff;
	h = gfx_img_h(d) - 2 * yoff;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
	}

	if (color) {
		if (x < 0 || y < 0 || x >= w || y >= h)
			return 0;
		game_pict_modify(d);
		col.a = alpha;
		rc = gfx_set_pixel(d, x + xoff, y + yoff, col);
	} else {
		rc = gfx_get_pixel(d, x + xoff, y + yoff, &col);
	}

	if (rc)
		return 0;

	lua_pushinteger(L, col.r);
	lua_pushinteger(L, col.g);
	lua_pushinteger(L, col.b);
	lua_pushinteger(L, col.a);
	return 4;
}

static int _free_sprite(const char *key)
{
	_spr_t *sp;
	if (!key)
		return -1;
	sp = sprite_lookup(key);

	if (!sp)
		return -1;

	cache_forget(gfx_image_cache(), sp->img);
	cache_shrink(gfx_image_cache());

	list_del(&sp->list);
	free(sp->name); free(sp);
	return 0;
}

static int luaB_free_sprite(lua_State *L) {
	const char *key = luaL_optstring(L, 1, NULL);
	if (_free_sprite(key))
		return 0;
	lua_pushboolean(L, 1);
	return 1;
}

static int luaB_free_font(lua_State *L) {
	const char *key = luaL_optstring(L, 1, NULL);
	_fnt_t *fn;
	if (!key)
		return 0;

	fn = font_lookup(key);
	if (!fn)
		return 0;

	list_del(&fn->list);
	fnt_free(fn->fnt);
	free(fn->name); free(fn);
	lua_pushboolean(L, 1);
	return 1;
}

extern int theme_setvar(char *name, char *val);
extern char *theme_getvar(const char *name);

static int luaB_theme_var(lua_State *L) {
	const char *var = luaL_optstring(L, 1, NULL);
	const char *val = luaL_optstring(L, 2, NULL);
	if (var && !val) { /* get */
		char *p = theme_getvar(var);
		if (p) {
			lua_pushstring(L, p);
			free(p);
			return 1;
		}
		return 0;
	}
	if (!val || !var)
		return 0;
/*	if (!game_own_theme)
		return 0; */
	if (!opt_owntheme)
		return 0;
	if (!strcmp(var, "scr.w") ||
		!strcmp(var, "scr.h")) /* filter resolution */
		return 0;
	if (!theme_setvar((char*)var, (char*)val)) {
		if (strcmp(var, "win.scroll.mode")) /* let change scroll mode w/o theme reload */
			game_theme_changed = 2;
	}
	return 0;
}

static int luaB_theme_name(lua_State *L) {
	char *name;
	if (game_own_theme && opt_owntheme) {
		if (game_own_theme == 2) {
			name = malloc(strlen(curtheme_dir[THEME_GAME]) + 2);
			if (!name)
				return 0;
			sprintf(name, ".%s", curtheme_dir[THEME_GAME]);
			lua_pushstring(L, name);
			free(name);
		} else {
			lua_pushstring(L, ".");
		}
	} else
		lua_pushstring(L, curtheme_dir[THEME_GLOBAL]);
	return 1;
}

static int luaB_stead_busy(lua_State *L) {
	static unsigned long busy_time = 0;
	int busy = lua_toboolean(L, 1);
	if (busy) {
		struct inp_event ev;
		int dirty = 0;
		memset(&ev, 0, sizeof(ev));
		while (input(&ev, 0) == AGAIN);
		if (ev.type == MOUSE_MOTION) {
			game_cursor(CURSOR_ON); /* to make all happy */
			dirty = 1;
		}
		if (!busy_time)
			busy_time = gfx_ticks();
		if (gfx_ticks() - busy_time >= 750 && menu_visible() != menu_wait) {
			game_menu(menu_wait);
			dirty = 1;
		}
		if (dirty)
			game_gfx_commit(0);
		return 0;
	}
	if (menu_visible() == menu_wait) {
		menu_toggle(-1);
	}
	busy_time = 0;
	return 0;
}

static int luaB_mouse_pos(lua_State *L) {
	int x = luaL_optnumber(L, 1, -1);
	int y = luaL_optnumber(L, 2, -1);
	int m;
	float v = game_theme.scale;
	if (x != -1 && y != -1) {
		x *= v;
		y *= v;
		gfx_warp_cursor(x + game_theme.xoff, y + game_theme.yoff);
		x = -1;
		y = -1;
	}
	m = gfx_cursor(&x, &y);
	x = (x - game_theme.xoff) / v;
	y = (y - game_theme.yoff) / v;
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	lua_pushinteger(L, m);
	return 3;
}

static int luaB_finger_pos(lua_State *L) {
	int x, y;
	float pressure;
	float v = game_theme.scale;
	const char *finger = luaL_optstring(L, 1, NULL);
	if (!finger)
		return 0;
	if (finger_pos(finger, &x, &y, &pressure)) /* no finger */
		return 0;
	x = (x - game_theme.xoff) / v;
	y = (y - game_theme.yoff) / v;
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	lua_pushnumber(L, pressure);
	return 3;
}

extern int mouse_filter_delay;

static int luaB_mouse_filter(lua_State *L) {
	int d = luaL_optnumber(L, 1, -1);
	int ov = mouse_filter_delay;
	if (d != -1)
		mouse_filter_delay = d;
	lua_pushinteger(L, ov);
	return 1;
}

static int luaB_mouse_show(lua_State *L) {
	int show = lua_toboolean(L, 1);
	int ov = game_cursor_show;
	if (lua_isboolean(L, 1))
		game_cursor_show = show;
	lua_pushboolean(L, ov);
	return 1;
}

static int luaB_get_ticks(lua_State *L) {
	lua_pushinteger(L, gfx_ticks());
	return 1;
}

static int luaB_get_themespath(lua_State *L) {
	char themes_path[PATH_MAX];

	if (THEMES_PATH[0] != '/') {
		strcpy(themes_path, instead_cwd());
		strcat(themes_path, "/");
	} else
		themes_path[0] = 0;
	strcat(themes_path, THEMES_PATH);
	unix_path(themes_path);
	lua_pushstring(L, themes_path);
	return 1;
}
#define PIXELS_MAGIC 0x1980
struct lua_pixels {
	int type;
	int w;
	int h;
	float scale;
	size_t size;
	img_t img;
	int dirty;
	int direct;
};

static int pixels_size(lua_State *L) {
	struct lua_pixels *hdr = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!hdr || hdr->type != PIXELS_MAGIC)
		return 0;
	lua_pushinteger(L, hdr->w);
	lua_pushinteger(L, hdr->h);
	lua_pushnumber(L, hdr->scale);
	return 3;
}

#define PXL_BLEND_COPY 1
#define PXL_BLEND_BLEND 2
static inline void blend(unsigned char *s, unsigned char *d)
{
	unsigned int r, g, b, a;
	unsigned int sa = s[3];
	unsigned int da = d[3];
	a = sa + (da * (255 - sa) >> 8);
	r = ((unsigned int)s[0] * sa >> 8) +
		((unsigned int)d[0] * da * (255 - sa) >> 16);
	g = ((unsigned int)s[1] * sa >> 8) +
		((unsigned int)d[1] * da * (255 - sa) >> 16);
	b = ((unsigned int)s[2] * sa >> 8) +
		((unsigned int)d[2] * da * (255 - sa) >> 16);
	d[0] = r; d[1] = g; d[2] = b; d[3] = a;
}

static inline void draw(unsigned char *s, unsigned char *d)
{
	unsigned int r, g, b, a;
	unsigned int sa = s[3];
	a = 255;
	r = ((unsigned int)s[0] * sa >> 8) +
		((unsigned int)d[0] * (255 - sa) >> 8);
	g = ((unsigned int)s[1] * sa >> 8) +
		((unsigned int)d[1] * (255 - sa) >> 8);
	b = ((unsigned int)s[2] * sa >> 8) +
		((unsigned int)d[2] * (255 - sa) >> 8);
	d[0] = r; d[1] = g; d[2] = b; d[3] = a;
}
static inline void pixel(unsigned char *s, unsigned char *d)
{
	unsigned char a_src = s[3];
	unsigned char a_dst = d[3];
	if (a_src == 255 || a_dst == 0) {
		memcpy(d, s, 4);
	} else if (a_dst == 255) {
		draw(s, d);
	} else if (a_src == 0) {
		/* nothing to do */
	} else {
		blend(s, d);
	}
}
static inline void line0(struct lua_pixels *hdr, int x1, int y1, int dx, int dy, int xd, unsigned char *col)
{
	int dy2 = dy * 2;
	int dyx2 = dy2 - dx * 2;
	int err = dy2 - dx;
	unsigned char *ptr = NULL;
	int w = hdr->w; int h = hdr->h;

	int ly = w * 4;
	int lx = xd * 4;

	while ((x1 < 0 || y1 < 0 || x1 >= w) && dx --) {
		if (err >= 0) {
			y1 ++;
			err += dyx2;
		} else {
			err += dy2;
		}
		x1 += xd;
	}
	if (dx < 0)
		return;
	ptr = (unsigned char*)(hdr + 1);
	ptr += (y1 * w + x1) << 2;

	pixel(col, ptr);
	while (dx --) {
		if (err >= 0) {
			y1 ++;
			if (y1 >= h)
				break;
			ptr += ly;
			err += dyx2;
		} else {
			err += dy2;
		}
		x1 += xd;
		if (x1 >= w || x1 < 0)
			break;
		ptr += lx;
		pixel(col, ptr);
	}
	return;
}

static inline void line1(struct lua_pixels *hdr, int x1, int y1, int dx, int dy, int xd, unsigned char *col)
{
	int dx2 = dx * 2;
	int dxy2 = dx2 - dy * 2;
	int err = dx2 - dy;
	int w = hdr->w; int h = hdr->h;
	unsigned char *ptr = NULL;
	int ly = w * 4;
	int lx = xd * 4;

	while ((x1 < 0 || y1 < 0 || x1 >= w) && dy --) {
		if (err >= 0) {
		        x1 += xd;
			err += dxy2;
		} else {
			err += dx2;
		}
		y1 ++;
	}
	if (dy < 0)
		return;

	ptr = (unsigned char*)(hdr + 1);
	ptr += (y1 * w + x1) << 2;

	pixel(col, ptr);

	while (dy --) {
		if (err >= 0) {
			x1 += xd;
			if (x1 < 0 || x1 >= w)
				break;
			ptr += lx;
			err += dxy2;
		} else {
			err += dx2;
		}
		y1 ++;
		if (y1 >= h)
			break;
		ptr += ly;
		pixel(col, ptr);
	}
	return;
}

static void lineAA(struct lua_pixels *src, int x0, int y0, int x1, int y1,
		 int r, int g, int b, int a)
{
	int dx, dy, err, e2, sx;
	int w, h;
	int syp, sxp, ed;
	unsigned char *ptr;
	unsigned char col[4];
	col[0] = r; col[1] = g; col[2] = b; col[3] = a;
	if (y0 > y1) {
		int tmp;
		tmp = x0; x0 = x1; x1 = tmp;
		tmp = y0; y0 = y1; y1 = tmp;
	}
	w = src->w; h = src->h;
	if (y1 < 0 || y0 >= h)
		return;
	if (x0 < x1) {
		sx = 1;
		if (x0 >= w || x1 < 0)
			return;
	} else {
		sx = -1;
		if (x1 >= w || x0 < 0)
			return;
	}
	sxp = sx * 4;
	syp = w * 4;

	dx =  abs(x1 - x0);
	dy = y1 - y0;

	err = dx - dy;
	ed = dx + dy == 0 ? 1: sqrt((float)dx * dx + (float)dy * dy);

	while (y0 < 0 || x0 < 0 || x0 >= w) {
		e2 = err;
		if (2 * e2 >= -dx) {
			if (x0 == x1)
				break;
			err -= dy;
			x0 += sx;
		}
		if (2 * e2 <= dy) {
			if (y0 == y1)
				break;
			err += dx;
			y0 ++;
		}
	}

	if (y0 < 0 || x0 < 0 || x0 >= w)
		return;

	ptr = (unsigned char*)(src + 1);
	ptr += (y0 * w + x0) << 2;

	while (1) {
		unsigned char *optr = ptr;
		col[3] = a - a * abs(err - dx + dy) / ed;
		pixel(col, ptr);
		e2 = err;
		if (2 * e2 >= -dx) {
			if (x0 == x1)
				break;
			if (e2 + dy < ed) {
				col[3] = a - a * (e2 + dy) / ed;
				pixel(col, ptr + syp);
			}
			err -= dy;
			x0 += sx;
			if (x0 < 0 || x0 >= w)
				break;
			ptr += sxp;
		}
		if (2 * e2 <= dy) {
			if (y0 == y1)
				break;
			if (dx - e2 < ed) {
				col[3] = a - a * (dx - e2) / ed;
				pixel(col, optr + sxp);
			}
			err += dx;
			y0 ++;
			if (y0 >= h)
				break;
			ptr += syp;
		}
	}
	src->dirty = 1;
}

static void line(struct lua_pixels *src, int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
	int dx, dy, tmp;
	unsigned char col[4];
	if (y1 > y2) {
		tmp = y1; y1 = y2; y2 = tmp;
		tmp = x1; x1 = x2; x2 = tmp;
	}
	col[0] = r; col[1] = g; col[2] = b; col[3] = a;
	if (y1 >= src->h)
		return;
	if (y2 < 0)
		return;
	if (x1 < x2) {
		if (x2 < 0)
			return;
		if (x1 >= src->w)
			return;
	} else {
		if (x1 < 0)
			return;
		if (x2 >= src->w)
			return;
	}
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx > 0) {
		if (dx > dy) {
			line0(src, x1, y1, dx, dy, 1, col);
		} else {
			line1(src, x1, y1, dx, dy, 1, col);
		}
	} else {
		dx = -dx;
		if (dx > dy) {
			line0(src, x1, y1, dx, dy, -1, col);
		} else {
			line1(src, x1, y1, dx, dy, -1, col);
		}
	}
	src->dirty = 1;
}

static int _pixels_blend(struct lua_pixels *src, int x, int y, int w, int h,
			struct lua_pixels *dst, int xx, int yy, int mode)

{
	unsigned char *ptr1, *ptr2;
	int cy, cx, srcw, dstw;

	if (!w)
		w = src->w;
	if (!h)
		h = src->h;

	if (x < 0 || x + w > src->w)
		return 0;

	if (y < 0 || y + h > src->h)
		return 0;

	if (w <= 0 || h <= 0)
		return 0;

	if (xx < 0) {
		w += xx;
		x -= xx;
		xx = 0;
	}
	if (yy < 0) {
		h += yy;
		y -= yy;
		yy = 0;
	}
	if (w <= 0 || h <= 0)
		return 0;

	if (xx >= dst->w || yy >= dst->h)
		return 0;

	if (xx + w > dst->w)
		w = dst->w - xx;
	if (yy + h > dst->h)
		h = dst->h - yy;

	ptr1 = (unsigned char *)(src + 1);
	ptr2 = (unsigned char *)(dst + 1);
	ptr1 += (y * src->w + x) << 2;
	ptr2 += (yy * dst->w + xx) << 2;
	srcw = src->w * 4; dstw = dst->w * 4;
	dst->dirty = 1;
	for (cy = 0; cy < h; cy ++) {
		if (mode == PXL_BLEND_COPY)
			memcpy(ptr2, ptr1, w << 2);
		else {
			unsigned char *p2 = ptr2;
			unsigned char *p1 = ptr1;
			for (cx = 0; cx < w; cx ++) {
				pixel(p1, p2);
				p1 += 4;
				p2 += 4;
			}
		}
		ptr2 += dstw;
		ptr1 += srcw;
	}
	return 0;
}

static int pixels_copy(lua_State *L) {
	int x = 0, y = 0, w = 0, h = 0, xx = 0, yy = 0;
	struct lua_pixels *src, *dst;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	dst = (struct lua_pixels*)lua_touserdata(L, 2);
	if (!dst) {
		x = luaL_optnumber(L, 2, 0);
		y = luaL_optnumber(L, 3, 0);
		w = luaL_optnumber(L, 4, 0);
		h = luaL_optnumber(L, 5, 0);
		dst = (struct lua_pixels*)lua_touserdata(L, 6);
		xx = luaL_optnumber(L, 7, 0);
		yy = luaL_optnumber(L, 8, 0);
	} else {
		xx = luaL_optnumber(L, 3, 0);
		yy = luaL_optnumber(L, 4, 0);
	}
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	if (!dst || dst->type != PIXELS_MAGIC)
		return 0;
	return _pixels_blend(src, x, y, w, h, dst, xx, yy, PXL_BLEND_COPY);
}

static int pixels_blend(lua_State *L) {
	int x = 0, y = 0, w = 0, h = 0, xx = 0, yy = 0;
	struct lua_pixels *src, *dst;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	dst = (struct lua_pixels*)lua_touserdata(L, 2);
	if (!dst) {
		x = luaL_optnumber(L, 2, 0);
		y = luaL_optnumber(L, 3, 0);
		w = luaL_optnumber(L, 4, 0);
		h = luaL_optnumber(L, 5, 0);
		dst = (struct lua_pixels*)lua_touserdata(L, 6);
		xx = luaL_optnumber(L, 7, 0);
		yy = luaL_optnumber(L, 8, 0);
	} else {
		xx = luaL_optnumber(L, 3, 0);
		yy = luaL_optnumber(L, 4, 0);
	}
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	if (!dst || dst->type != PIXELS_MAGIC)
		return 0;
	return _pixels_blend(src, x, y, w, h, dst, xx, yy, PXL_BLEND_BLEND);
}

static void _fill(struct lua_pixels *src, int x, int y, int w, int h,
		  int r, int g, int b, int a, int mode) {
	unsigned char col[4];
	unsigned char *ptr1;
	int cy, cx;
	if (!src || src->type != PIXELS_MAGIC)
		return;
	col[0] = r; col[1] = g; col[2] = b; col[3] = a;
	if (!w)
		w = src->w;
	if (!h)
		h = src->h;

	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}

	if (w <= 0 || h <= 0 || x >= src->w || y >= src->h)
		return;

	if (x + w > src->w)
		w = src->w - x;
	if (y + h > src->h)
		h = src->h - y;

	ptr1 = (unsigned char *)(src + 1);
	ptr1 += (y * src->w + x) << 2;
	src->dirty = 1;
	for (cy = 0; cy < h; cy ++) {
		unsigned char *p1 = ptr1;
		for (cx = 0; cx < w; cx ++) {
			if (mode == PXL_BLEND_COPY)
				memcpy(p1, col, 4);
			else
				pixel(col, p1);
			p1 += 4;
		}
		ptr1 += (src->w * 4);
	}
	return;
}

static inline int orient2d(int ax, int ay, int bx, int by, int cx, int cy)
{
	return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

static inline int min3(int a, int b, int c)
{
	if (a < b) {
		if (a < c)
			return a;
		return c;
	} else {
		if (b < c)
			return b;
		return c;
	}
}

static inline int max3(int a, int b, int c)
{
	if (a > b) {
		if (a > c)
			return a;
		return c;
	} else {
		if (b > c)
			return b;
		return c;
	}
}

static void triangle(struct lua_pixels *src, int x0, int y0, int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
	int A01 = y0 - y1, B01 = x1 - x0;
	int A12 = y1 - y2, B12 = x2 - x1;
	int A20 = y2 - y0, B20 = x0 - x2;

	int minx = min3(x0, x1, x2);
	int miny = min3(y0, y1, y2);
	int maxx = max3(x0, x1, x2);
	int maxy = max3(y0, y1, y2);

	int w0_row = orient2d(x1, y1, x2, y2, minx, miny);
	int w1_row = orient2d(x2, y2, x0, y0, minx, miny);
	int w2_row = orient2d(x0, y0, x1, y1, minx, miny);

	int y, x, w, h;
	int yd;
	unsigned char col[4];
	unsigned char *ptr;
	w = src->w; h = src->h;
	yd = 4 * w;
	col[0] = r; col[1] = b; col[2] = g; col[3] = a;

	if (minx >= w || miny >= h)
		return;
	if (minx < 0)
		minx = 0;
	if (miny < 0)
		miny = 0;
	if (maxy >= h)
		maxy = h - 1;
	if (maxx >= w)
		maxx = w - 1;
	ptr = (unsigned char *)(src + 1) + miny * yd + 4 * minx;

	src->dirty = 1;

	for (y = miny; y <= maxy; y ++) {
		int w0 = w0_row;
	        int w1 = w1_row;
		int w2 = w2_row;
		unsigned char *p = ptr;
		for (x = minx; x <= maxx; x++) {
			if ((w0 | w1 | w2) >= 0)
				pixel(col, p);
			p += 4;
			w0 += A12;
			w1 += A20;
			w2 += A01;
		}
		w0_row += B12;
		w1_row += B20;
		w2_row += B01;
		ptr += yd;
	}
}
static void fill_circle(struct lua_pixels *src, int xc, int yc, int radius, int r, int g, int b, int a)
{
	int r2 = radius * radius;
	int x, y, x1, x2, y1, y2;
	unsigned char col[4] = { r, g, b, a };
	int w = src->w, h = src->h;
	unsigned char *ptr;

	if (xc + radius < 0 || yc + radius < 0)
		return;
	if (xc - radius >= w || yc - radius >= h)
		return;

	if (radius <= 0)
		return;

	ptr = (unsigned char *)(src + 1);
	src->dirty = 1;
	ptr += (w * yc + xc) << 2;

	if (radius == 1) {
		pixel(col, ptr);
		return;
	}
	y1 = -radius; y2 = radius;
	x1 = -radius; x2 = radius;
	if (yc - radius < 0)
		y1 = -yc;
	if (xc - radius < 0)
		x1 = -xc;
	if (xc + radius >= w)
		x2 = w - xc - 1;
	if (yc + radius >= h)
		y2 = h - yc - 1;
	for (y = y1; y <= y2; y ++) {
		unsigned char *ptrl = ptr + ((y * w + x1) << 2);
		for (x = x1; x <= x2; x++) {
			if (x*x + y*y < r2 - 1)
				pixel(col, ptrl);
			ptrl += 4;
		}
	}
}

static void circle(struct lua_pixels *src, int xc, int yc, int rr, int r, int g, int b, int a)
{
	int x = -rr, y = 0, err = 2 - 2 * rr;
	unsigned char *ptr = (unsigned char *)(src + 1);
	unsigned char col[4] = { r, g, b, a };
	int w = src->w, h = src->h;

	if (rr <= 0)
		return;
	if (xc + rr < 0 || yc + rr < 0)
		return;
	if (xc - rr >= w || yc - rr >= h)
		return;
	src->dirty = 1;
	ptr += (w * yc + xc) * 4;
	if (xc - rr >= 0 && xc + rr < w &&
	    yc - rr >=0 && yc + rr < h) {
		do {
			int xmy = (x - y * w) * 4;
			int yax = (y + x * w) * 4;
			pixel(col, ptr - xmy);
			pixel(col, ptr - yax);
			pixel(col, ptr + xmy);
			pixel(col, ptr + yax);

			rr = err;
			if (rr <= y)
				err += ++y * 2 + 1;
			if (rr > x || err > y)
				err += ++x * 2 + 1;
		} while (x < 0);
		return;
	}
	/* slow */
	do {
		int xmy = (x - y * w) * 4;
		int yax = (y + x * w) * 4;
		if (((xc - x) | (w - xc + x - 1) |
		    (yc + y) | (h - yc - y - 1)) >= 0)
			pixel(col, ptr - xmy);
		if (((xc - y) | (w - xc + y - 1) |
		     (yc - x) | (h - yc + x - 1)) >= 0)
			pixel(col, ptr - yax);
		if (((xc + x) | (w - xc - x - 1) |
		     (yc - y) | (h - yc + y - 1)) >= 0)
			pixel(col, ptr + xmy);
		if (((xc + y) | (w - xc - y - 1) |
		      (yc + x) | (h - yc - x - 1)) >= 0)
			pixel(col, ptr + yax);
		rr = err;
		if (rr <= y)
			err += ++y * 2 + 1;
		if (rr > x || err > y)
			err += ++x * 2 + 1;
	} while (x < 0);

}
static void circleAA(struct lua_pixels *src, int xc, int yc, int rr, int r, int g, int b, int a)
{
	int p1, p2, p3, p4;
	int x = -rr, y = 0, x2, e2, err = 2 - 2 * rr;
	unsigned char *ptr = (unsigned char *)(src + 1);
	unsigned char col[4] = { r, g, b, a };
	int w = src->w, h = src->h;
	if (rr <= 0)
		return;
	if (xc + rr < 0 || yc + rr < 0)
		return;
	if (xc - rr >= w || yc - rr >= h)
		return;
	src->dirty = 1;
	rr = 1 - err;
	ptr += (w * yc + xc) * 4;
	do {
		int i = 255 * abs(err - 2 *(x + y)-2) / rr;
		int xmy = (x - y * w) * 4;
		int yax = (y + x * w) * 4;
		col[3] = ((255 - i) * a) >> 8;
		p1 = 0; p2 = 0; p3 = 0; p4 = 0;
		if (((xc - x) | (w - xc + x - 1) |
		     (yc + y) | (h - yc - y - 1)) >= 0) {
			pixel(col, ptr - xmy);
			p1 = 1;
		}
		if (((xc - y) | (w - xc + y - 1) |
		     (yc - x) | (h - yc + x - 1)) >= 0) {
			pixel(col, ptr - yax);
			p2 = 1;
		}
		if (((xc + x) | (w - xc - x - 1) |
		     (yc - y) | (h - yc + y - 1)) >= 0) {
			pixel(col, ptr + xmy);
			p3 = 1;
		}
		if (((xc + y) | (w - xc - y - 1) |
		     (yc + x) | (h - yc - x - 1)) >= 0) {
			pixel(col, ptr + yax);
			p4 = 1;
		}
		e2 = err;
		x2 = x;
		if (err + y > 0) {
			i = 255 * (err - 2 * x - 1) / rr;
			if (i < 256) {
				col[3] = ((255 - i) * a) >> 8;
				if (p1 && yc + y + 1 < h)
					pixel(col, ptr - xmy + w * 4);
				if (p2 && xc - y - 1 >= 0)
					pixel(col, ptr - yax - 4);
				if (p3 && yc - y - 1 >= 0)
					pixel(col, ptr + xmy - w * 4);
				if (p4 && xc + y < w)
					pixel(col, ptr + yax + 4);
			}
			err += ++x * 2 + 1;
		}
		if (e2 + x <= 0) {
			i = 255 * (2 * y + 3 - e2) / rr;
			if (i < 256) {
				col[3] = ((255 - i) * a) >> 8;
				if (p1 && xc - x2 - 1 >= 0)
					pixel(col, ptr - xmy - 4);
				if (p2 && yc - x2 - 1 >= 0)
					pixel(col, ptr - yax - w * 4);
				if (p3 && xc + x2 + 1 < w)
					pixel(col, ptr + xmy + 4);
				if (p4 && yc + x2 + 1 < h)
					pixel(col, ptr + yax + w * 4);
			}
			err += ++y * 2 + 1;
		}
	} while (x < 0);
}

static int pixels_fill(lua_State *L) {
	int x = 0, y = 0, w = 0, h = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	b = luaL_optnumber(L, 8, -1);
	if (b < 0) {
		r = luaL_optnumber(L, 2, 0);
		g = luaL_optnumber(L, 3, 0);
		b = luaL_optnumber(L, 4, 0);
		a = luaL_optnumber(L, 5, 255);
	} else {
		x = luaL_optnumber(L, 2, 0);
		y = luaL_optnumber(L, 3, 0);
		w = luaL_optnumber(L, 4, 0);
		h = luaL_optnumber(L, 5, 0);
		r = luaL_optnumber(L, 6, 0);
		g = luaL_optnumber(L, 7, 0);
		b = luaL_optnumber(L, 8, 0);
		a = luaL_optnumber(L, 9, 255);
	}
	_fill(src, x, y, w, h, r, g, b, a, PXL_BLEND_BLEND);
	return 0;
}

static int pixels_clear(lua_State *L) {
	int x = 0, y = 0, w = 0, h = 0, r = 0, g = 0, b = 0, a = 0;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	b = luaL_optnumber(L, 8, -1);
	if (b < 0) {
		r = luaL_optnumber(L, 2, 0);
		g = luaL_optnumber(L, 3, 0);
		b = luaL_optnumber(L, 4, 0);
		a = luaL_optnumber(L, 5, 0);
	} else {
		x = luaL_optnumber(L, 2, 0);
		y = luaL_optnumber(L, 3, 0);
		w = luaL_optnumber(L, 4, 0);
		h = luaL_optnumber(L, 5, 0);
		r = luaL_optnumber(L, 6, 0);
		g = luaL_optnumber(L, 7, 0);
		b = luaL_optnumber(L, 8, 0);
		a = luaL_optnumber(L, 9, 0);
	}
	_fill(src, x, y, w, h, r, g, b, a, PXL_BLEND_COPY);
	return 0;
}


static int pixels_triangle(lua_State *L) {
	int x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, r = 0, g = 0, b = 0, a = 0;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	x0 = luaL_optnumber(L, 2, 0);
	y0 = luaL_optnumber(L, 3, 0);
	x1 = luaL_optnumber(L, 4, 0);
	y1 = luaL_optnumber(L, 5, 0);
	x2 = luaL_optnumber(L, 6, 0);
	y2 = luaL_optnumber(L, 7, 0);
	r = luaL_optnumber(L, 8, 0);
	g = luaL_optnumber(L, 9, 0);
	b = luaL_optnumber(L, 10, 0);
	a = luaL_optnumber(L, 11, 255);
	triangle(src, x0, y0, x1, y1, x2, y2, r, g, b, a);
	return 0;
}

static int pixels_line(lua_State *L) {
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	x1 = luaL_optnumber(L, 2, 0);
	y1 = luaL_optnumber(L, 3, 0);
	x2 = luaL_optnumber(L, 4, 0);
	y2 = luaL_optnumber(L, 5, 0);
	r = luaL_optnumber(L, 6, 0);
	g = luaL_optnumber(L, 7, 0);
	b = luaL_optnumber(L, 8, 0);
	a = luaL_optnumber(L, 9, 255);
	line(src, x1, y1, x2, y2, r, g, b, a);
	return 0;
}

static int pixels_lineAA(lua_State *L) {
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	x1 = luaL_optnumber(L, 2, 0);
	y1 = luaL_optnumber(L, 3, 0);
	x2 = luaL_optnumber(L, 4, 0);
	y2 = luaL_optnumber(L, 5, 0);
	r = luaL_optnumber(L, 6, 0);
	g = luaL_optnumber(L, 7, 0);
	b = luaL_optnumber(L, 8, 0);
	a = luaL_optnumber(L, 9, 255);
	lineAA(src, x1, y1, x2, y2, r, g, b, a);
	return 0;
}
static int pixels_circle(lua_State *L) {
	int xc = 0, yc = 0, rr = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	xc = luaL_optnumber(L, 2, 0);
	yc = luaL_optnumber(L, 3, 0);
	rr = luaL_optnumber(L, 4, 0);
	r = luaL_optnumber(L, 5, 0);
	g = luaL_optnumber(L, 6, 0);
	b = luaL_optnumber(L, 7, 0);
	a = luaL_optnumber(L, 8, 255);
	circle(src, xc, yc, rr, r, g, b, a);
	return 0;
}
static int pixels_circleAA(lua_State *L) {
	int xc = 0, yc = 0, rr = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	xc = luaL_optnumber(L, 2, 0);
	yc = luaL_optnumber(L, 3, 0);
	rr = luaL_optnumber(L, 4, 0);
	r = luaL_optnumber(L, 5, 0);
	g = luaL_optnumber(L, 6, 0);
	b = luaL_optnumber(L, 7, 0);
	a = luaL_optnumber(L, 8, 255);
	circleAA(src, xc, yc, rr, r, g, b, a);
	return 0;
}
static int pixels_fill_circle(lua_State *L) {
	int xc = 0, yc = 0, rr = 0, r = 0, g = 0, b = 0, a = 255;
	struct lua_pixels *src;
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	xc = luaL_optnumber(L, 2, 0);
	yc = luaL_optnumber(L, 3, 0);
	rr = luaL_optnumber(L, 4, 0);
	r = luaL_optnumber(L, 5, 0);
	g = luaL_optnumber(L, 6, 0);
	b = luaL_optnumber(L, 7, 0);
	a = luaL_optnumber(L, 8, 255);
	fill_circle(src, xc, yc, rr, r, g, b, a);
	return 0;
}
struct lua_point {
	int x;
	int y;
	int nodex;
};

/*
   http://alienryderflex.com/polygon_fill/
   public-domain code by Darel Rex Finley, 2007
*/

static void fill_poly(struct lua_pixels *src, struct lua_point *v, int nr, unsigned char *col)
{
	unsigned char *ptr = (unsigned char *)(src + 1), *ptr1;
	int y, x, xmin, xmax, ymin, ymax, swap, w;
	int nodes = 0, j, i;
	xmin = v[0].x; xmax = v[0].x;
	ymin = v[0].y; ymax = v[0].y;

	for (i = 0; i < nr; i++) {
		if (v[i].x < xmin)
			xmin = v[i].x;
		if (v[i].x > xmax)
			xmax = v[i].x;
		if (v[i].y < ymin)
			ymin = v[i].y;
		if (v[i].y > ymax)
			ymax = v[i].y;
	}
	if (ymin < 0)
		ymin = 0;
	if (xmin < 0)
		xmin = 0;
	if (xmax >= src->w)
		xmax = src->w;
	if (ymax >= src->h)
		ymax = src->h;
	ptr += (ymin * src->w) << 2;
	for (y = ymin; y < ymax; y ++) {
		nodes = 0; j = nr - 1;
		for (i = 0; i < nr; i++) {
			if ((v[i].y < y && v[j].y >= y) ||
			    (v[j].y < y && v[i].y >= y)) {
				v[nodes ++].nodex = v[i].x + ((y - v[i].y) * (v[j].x - v[i].x)) /
					(v[j].y - v[i].y);
			}
			j = i;
		}
		if (nodes < 2)
			goto skip;
		i = 0;
		while (i < nodes - 1) { /* sort */
			if (v[i].nodex > v[i + 1].nodex) {
				swap = v[i].nodex;
				v[i].nodex = v[i + 1].nodex;
				v[i + 1].nodex = swap;
				if (i)
					i --;
			} else {
				i ++;
			}
		}
		for (i = 0; i < nodes; i += 2) {
			if (v[i].nodex >= xmax)
				break;
			if (v[i + 1].nodex > xmin) {
				if (v[i].nodex < xmin)
					v[i].nodex = xmin;
				if (v[i + 1].nodex > xmax)
					v[i + 1].nodex = xmax;
				// hline
				src->dirty = 1;
				w = (v[i + 1].nodex - v[i].nodex);
				ptr1 = ptr + v[i].nodex * 4;
				for (x = 0; x < w; x ++) {
					pixel(col, ptr1);
					ptr1 += 4;
				}
			}
		}
	skip:
		ptr += src->w * 4;
	}
}

static int pixels_fill_poly(lua_State *L) {
	int nr, i;
	struct lua_pixels *src;
	struct lua_point *v;
	unsigned char col[4];
	src = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	luaL_checktype(L, 2, LUA_TTABLE);
#if LUA_VERSION_NUM >= 502
	nr = lua_rawlen(L, 2);
#else
	nr = lua_objlen(L, 2);
#endif
	if (nr < 6)
		return 0;
	col[0] = luaL_optnumber(L, 3, 0);
	col[1] = luaL_optnumber(L, 4, 0);
	col[2] = luaL_optnumber(L, 5, 0);
	col[3] = luaL_optnumber(L, 6, 255);

	nr /= 2;
	v = malloc(sizeof(*v) * nr);
	if (!v)
		return 0;
	lua_pushvalue(L, 2);
	for (i = 0; i < nr; i++) {
		lua_pushinteger(L, (i * 2) + 1);
		lua_gettable(L, -2);
		v[i].x = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pushinteger(L, (i * 2) + 2);
		lua_gettable(L, -2);
		v[i].y = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	fill_poly(src, v, nr, col);
	free(v);
	return 0;
}

static int pixels_value(lua_State *L) {
	struct lua_pixels *hdr = (struct lua_pixels*)lua_touserdata(L, 1);
	int x = luaL_optnumber(L, 2, -1);
	int y = luaL_optnumber(L, 3, -1);
	int r = luaL_optnumber(L, 4, -1);
	int g = 0, b = 0, a = 0;
	unsigned char *ptr;
	if (r != -1) {
		g = luaL_optnumber(L, 5, 0);
		b = luaL_optnumber(L, 6, 0);
		a = luaL_optnumber(L, 7, 255);
	}
	if (x < 0 || y < 0)
		return 0;

	if (!hdr || hdr->type != PIXELS_MAGIC)
		return 0;

	if (x >= hdr->w || y >= hdr->h)
		return 0;

	ptr = (unsigned char*)(hdr + 1);
	ptr += ((y * hdr->w + x) << 2);
	if (r == -1) {
		lua_pushinteger(L, *(ptr ++));
		lua_pushinteger(L, *(ptr ++));
		lua_pushinteger(L, *(ptr ++));
		lua_pushinteger(L, *ptr);
		return 4;
	}
	hdr->dirty = 1;
	*(ptr ++) = r;
	*(ptr ++) = g;
	*(ptr ++) = b;
	*(ptr) = a;
	return 0;
}

static int pixels_pixel(lua_State *L) {
	struct lua_pixels *hdr = (struct lua_pixels*)lua_touserdata(L, 1);
	int x = luaL_optnumber(L, 2, -1);
	int y = luaL_optnumber(L, 3, -1);
	int r = luaL_optnumber(L, 4, -1);
	int g, b, a;
	unsigned char col[4];
	unsigned char *ptr;
	if (r == -1)
		return 0;

	g = luaL_optnumber(L, 5, 0);
	b = luaL_optnumber(L, 6, 0);
	a = luaL_optnumber(L, 7, 255);

	if (x < 0 || y < 0)
		return 0;

	if (!hdr || hdr->type != PIXELS_MAGIC)
		return 0;

	if (x >= hdr->w || y >= hdr->h)
		return 0;
	hdr->dirty = 1;
	ptr = (unsigned char*)(hdr + 1);
	ptr += ((y * hdr->w + x) << 2);
	col[0] = r; col[1] = g; col[2] = b; col[3] = a;
	pixel(col, ptr);
	return 0;
}

static img_t pixels_img(struct lua_pixels *hdr) {
	int w, h, ww, hh, xx, yy, dx, dy;
	unsigned char *ptr, *optr = NULL;
	unsigned char *p;
	img_t img;
	if (!hdr)
		return NULL;
	if (hdr->type != PIXELS_MAGIC)
		return NULL;
	img = hdr->img;
	if (!img)
		return NULL;

	if (hdr->direct || !hdr->dirty)
		return img;
	hdr->dirty = 0;

	ptr = (unsigned char*)(hdr + 1);
	ww = gfx_img_w(img);
	hh = gfx_img_h(img);
	w = hdr->w;
	h = hdr->h;


	p = gfx_get_pixels(img);

	if (!p)
		return NULL;

	dy = 0;

	for (yy = 0; yy < hh; yy++) {
		unsigned char *ptrl = ptr;

		dx = 0;

		if (optr) {
			memcpy(p, optr, ww * 4);
			p += ww * 4;
		} else {
			optr = p;
			for (xx = 0; xx < ww; xx++) {
				memcpy(p, ptrl, 4); p += 4;
				dx += w;
				while (dx >= ww) {
					dx -= ww;
					ptrl += 4;
				}
			}
		}
		dy += h;
		while (dy >= hh) {
			dy -= hh;
			ptr += (w << 2);
			optr = NULL;
		}
	}
	gfx_put_pixels(img);
	return img;
}

static int pixels_destroy(lua_State *L) {
	struct lua_pixels *hdr = (struct lua_pixels*)lua_touserdata(L, 1);
	if (!hdr || hdr->type != PIXELS_MAGIC)
		return 0;

	if (hdr->img)
		gfx_free_image(hdr->img);
	return 0;
}

static int pixels_new(lua_State *L, int w, int h, float scale, img_t src) {
	int ww, hh, direct = 0;
	img_t img2 = NULL, img;
	size_t size;
	float v = game_theme.scale;
	struct lua_pixels *hdr;

	if (src) {
		w = gfx_img_w(src);
		h = gfx_img_h(src);
		img2 = gfx_new_rgba(w, h);
		if (!img2)
			return 0;
		gfx_copy_from(src, 0, 0, w, h, img2, 0, 0);
	}
	if (w <=0 || h <= 0)
		return 0;
	ww = w; hh = h;
	if (v != 1.0f) {
		ww = ceil((float)w * v);
		hh = ceil((float)h * v);
	}
	ww = ceil((float)ww * scale);
	hh = ceil((float)hh * scale);
	size = w * h * 4;
	hdr = lua_newuserdata(L, sizeof(*hdr) + size);
	if (!hdr) {
		if (img2)
			gfx_free_image(img2);
		return 0;
	}

	hdr->type = PIXELS_MAGIC;
	hdr->img = NULL;
	hdr->w = w;
	hdr->h = h;
	hdr->scale = scale;
	hdr->size = size;
	hdr->dirty = 0;

	if (ww == w && hh == h) { /* direct map */
		direct = 1;
		img = gfx_new_from(ww, hh, (unsigned char*)(hdr + 1));
	} else {
		img = gfx_new_rgba(ww, hh);
	}
	hdr->direct = direct;
	if (!img) {
		fprintf(stderr, "Error: no free memory\n");
		memset(hdr, 0, sizeof(*hdr) + size);
		if (img2)
			gfx_free_image(img2);
		return 1;
	}

	hdr->img = img;
	if (img2) {
		unsigned char *ptr = gfx_get_pixels(img2);
		if (ptr) {
			memcpy(hdr + 1, ptr, size);
			gfx_put_pixels(img2);
		}
		gfx_free_image(img2);
	} else {
		memset(hdr + 1, 0, size);
	}
	hdr->dirty = 1;
	luaL_getmetatable(L, "pixels metatable");
	lua_setmetatable(L, -2);
	return 1;

}
static int luaB_pixels_sprite(lua_State *L) {
	const char *fname;
	int w, h, rc;
	float scale;
	img_t img = NULL;

	if (!lua_isnumber(L, 1)) {
		fname = luaL_optstring(L, 1, NULL);
		if (!fname)
			return 0;
		img = gfx_load_image((char*)fname);
		if (!img)
			return 0;
//		if (!cache_have(gfx_image_cache(), img))
//			v = 1.0f; /* do not scale sprites! */
		w = 0; h = 0;
		scale = luaL_optnumber(L, 2, 1.0f);
	} else {
		w = luaL_optnumber(L, 1, -1);
		h = luaL_optnumber(L, 2, -1);
		scale = luaL_optnumber(L, 3, 1.0f);
	}
	rc = pixels_new(L, w, h, scale, img);
	if (img)
		gfx_free_image(img);
	return rc;
}

static int pixels_scale(lua_State *L) {
	img_t img, img2;
	int rc;
	struct lua_pixels *src = (struct lua_pixels*)lua_touserdata(L, 1);;
	float xs = luaL_optnumber(L, 2, 0);
	float ys = luaL_optnumber(L, 3, 0);
	int smooth = lua_toboolean(L, 4);

	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	if (ys == 0)
		ys = xs;
	img = gfx_new_from(src->w, src->h, (unsigned char*)(src + 1));
	if (!img)
		return 0;
	img2 = gfx_scale(img, xs, ys, smooth);
	gfx_free_image(img);
	rc = pixels_new(L, 0, 0, src->scale, img2);
	gfx_free_image(img2);
	return rc;
}

static int pixels_rotate(lua_State *L) {
	img_t img, img2;
	int rc;
	struct lua_pixels *src = (struct lua_pixels*)lua_touserdata(L, 1);;
	float angle = luaL_optnumber(L, 2, 0);
	int smooth = lua_toboolean(L, 3);

	if (!src || src->type != PIXELS_MAGIC)
		return 0;
	img = gfx_new_from(src->w, src->h, (unsigned char*)(src + 1));
	if (!img)
		return 0;
	img2 = gfx_rotate(img, angle, smooth);
	gfx_free_image(img);
	rc = pixels_new(L, 0, 0, src->scale, img2);
	gfx_free_image(img2);
	return rc;
}

/*
** Creates pixels metatable.
*/
static int pixels_create_meta (lua_State *L) {
	luaL_newmetatable (L, "pixels metatable");
	lua_pushstring (L, "__index");
	lua_newtable(L);
	lua_pushstring (L, "val");
	lua_pushcfunction (L, pixels_value);
	lua_settable(L, -3);
	lua_pushstring (L, "pixel");
	lua_pushcfunction (L, pixels_pixel);
	lua_settable(L, -3);
	lua_pushstring (L, "size");
	lua_pushcfunction (L, pixels_size);
	lua_settable(L, -3);
	lua_pushstring(L, "copy");
	lua_pushcfunction (L, pixels_copy);
	lua_settable(L, -3);
	lua_pushstring(L, "blend");
	lua_pushcfunction (L, pixels_blend);
	lua_settable(L, -3);
	lua_pushstring(L, "clear");
	lua_pushcfunction (L, pixels_clear);
	lua_settable(L, -3);
	lua_pushstring(L, "fill");
	lua_pushcfunction (L, pixels_fill);
	lua_settable(L, -3);
	lua_pushstring(L, "line");
	lua_pushcfunction (L, pixels_line);
	lua_settable(L, -3);
	lua_pushstring(L, "lineAA");
	lua_pushcfunction (L, pixels_lineAA);
	lua_settable(L, -3);
	lua_pushstring(L, "circle");
	lua_pushcfunction (L, pixels_circle);
	lua_settable(L, -3);
	lua_pushstring(L, "circleAA");
	lua_pushcfunction (L, pixels_circleAA);
	lua_settable(L, -3);
	lua_pushstring(L, "fill_circle");
	lua_pushcfunction (L, pixels_fill_circle);
	lua_settable(L, -3);
	lua_pushstring(L, "fill_triangle");
	lua_pushcfunction (L, pixels_triangle);
	lua_settable(L, -3);
	lua_pushstring(L, "fill_poly");
	lua_pushcfunction (L, pixels_fill_poly);
	lua_settable(L, -3);
	lua_pushstring(L, "new_scaled");
	lua_pushcfunction (L, pixels_scale);
	lua_settable(L, -3);
	lua_pushstring(L, "new_rotated");
	lua_pushcfunction (L, pixels_rotate);
	lua_settable(L, -3);
	lua_settable(L, -3);
	lua_pushstring (L, "__gc");
	lua_pushcfunction (L, pixels_destroy);
	lua_settable (L, -3);
	return 0;
}

static int luaB_noise1(lua_State *L) {
	float r;
	int px;
	float x = luaL_optnumber(L, 1, 0);
	if (lua_isnumber(L, 2)) {
		px = luaL_optnumber(L, 2, 0);
		r = pnoise1(x, px);
	} else {
		r = noise1(x);
	}
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_noise2(lua_State *L) {
	float r;
	int px; int py;
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	if (lua_isnumber(L, 3)) {
		px = luaL_optnumber(L, 3, 0);
		py = luaL_optnumber(L, 4, 0);
		r = pnoise2(x, y, px, py);
	} else {
		r = noise2(x, y);
	}
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_noise3(lua_State *L) {
	float r;
	int px; int py; int pz;
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	float z = luaL_optnumber(L, 3, 0);
	if (lua_isnumber(L, 4)) {
		px = luaL_optnumber(L, 4, 0);
		py = luaL_optnumber(L, 5, 0);
		pz = luaL_optnumber(L, 6, 0);
		r = pnoise3(x, y, z, px, py, pz);
	} else {
		r = noise3(x, y, z);
	}
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_noise4(lua_State *L) {
	float r;
	int px; int py; int pz; int pw;
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	float z = luaL_optnumber(L, 3, 0);
	float w = luaL_optnumber(L, 4, 0);
	if (lua_isnumber(L, 5)) {
		px = luaL_optnumber(L, 5, 0);
		py = luaL_optnumber(L, 6, 0);
		pz = luaL_optnumber(L, 7, 0);
		pw = luaL_optnumber(L, 8, 0);
		r = pnoise4(x, y, z, w, px, py, pz, pw);
	} else {
		r = noise4(x, y, z, w);
	}
	lua_pushnumber(L, r);
	return 1;
}

static const luaL_Reg sprites_funcs[] = {
	{"instead_font_load", luaB_load_font},
	{"instead_font_free", luaB_free_font},
	{"instead_font_scaled_size", luaB_font_size_scaled},
	{"instead_sprite_load", luaB_load_sprite},
	{"instead_sprite_text", luaB_text_sprite},
	{"instead_sprite_free", luaB_free_sprite},
	{"instead_sprites_free", luaB_free_sprites},
	{"instead_sprite_draw", luaB_draw_sprite},
	{"instead_sprite_copy", luaB_copy_sprite},
	{"instead_sprite_compose", luaB_compose_sprite},
	{"instead_sprite_fill", luaB_fill_sprite},
	{"instead_sprite_dup", luaB_dup_sprite},
	{"instead_sprite_alpha", luaB_alpha_sprite},
	{"instead_sprite_colorkey", luaB_colorkey_sprite},
	{"instead_sprite_size", luaB_sprite_size},
	{"instead_sprite_scale", luaB_scale_sprite},
	{"instead_sprite_rotate", luaB_rotate_sprite},
	{"instead_sprite_text_size", luaB_text_size},
	{"instead_sprite_pixel", luaB_pixel_sprite},
	{"instead_sprite_pixels", luaB_pixels_sprite},
	{"instead_theme_var", luaB_theme_var},
	{"instead_theme_name", luaB_theme_name},
	{"instead_ticks", luaB_get_ticks},
	{"instead_busy", luaB_stead_busy},
	{"instead_mouse_pos", luaB_mouse_pos},
	{"instead_mouse_filter", luaB_mouse_filter},
	{"instead_mouse_show", luaB_mouse_show},
	{"instead_finger_pos", luaB_finger_pos},
	{"instead_themespath", luaB_get_themespath},
	{"instead_noise1", luaB_noise1},
	{"instead_noise2", luaB_noise2},
	{"instead_noise3", luaB_noise3},
	{"instead_noise4", luaB_noise4},
	{NULL, NULL}
};

static int sprites_done(void)
{
	sprites_free();
	return 0;
}

static int sprites_init(void)
{
	char path[PATH_MAX];
	if (pixels_create_meta(instead_lua()))
		return -1;
	instead_api_register(sprites_funcs);
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/sprites.lua");
	return instead_loadfile(dirpath(path));
}

static struct instead_ext ext = {
	.init = sprites_init,
	.done = sprites_done,
};

int instead_sprites_init(void)
{
	return instead_extension(&ext);
}
