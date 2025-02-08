/*
 * Copyright 2009-2025 Peter Kosyh <p.kosyh at gmail.com>
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

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#define PIXEL_FORMAT (SDL_PIXELFORMAT_RGBA32)
#define PIXEL_FORMAT_DETAILS SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32)

#ifdef _USE_HARFBUZZ
#include <hb.h>
#include <glib.h>
#endif

#include <SDL3/SDL_mutex.h>
#include "SDL3_rotozoom.h"
#include "SDL_gfxBlitFunc.h"
#include "SDL_gif.h"

#define IMG_ANIM 1
struct _img_t {
	SDL_Surface *s;
/*	SDL_Texture *t; */
	int flags;
	void	*aux;
};
#define Surf(p) (((p)?(((img_t)(p))->s):NULL))

#ifndef SDL_malloc
	#define SDL_malloc malloc
#endif

#ifndef SDL_free
	#define SDL_free free
#endif

static img_t screen = NULL;
static cache_t images = NULL;

static struct {
	const char *name;
	unsigned long val;
} cnames[] = {
	{"aliceblue", 0xf0f8ff},
	{"antiquewhite", 0xfaebd7},
	{"aqua", 0x00ffff},
	{"aquamarine", 0x7fffd4},
	{"azure", 0xf0ffff},
	{"beige", 0xf5f5dc},
	{"bisque", 0xffe4c4},
	{"black", 0x000000},
	{"blanchedalmond", 0xffebcd},
	{"blue", 0x0000ff},
	{"blueviolet", 0x8a2be2},
	{"brown", 0xa52a2a},
	{"burlywood", 0xdeb887},
	{"cadetblue", 0x5f9ea0},
	{"chartreuse", 0x7fff00},
	{"chocolate", 0xd2691e},
	{"coral", 0xff7f50},
	{"cornflowerblue", 0x6495ed},
	{"cornsilk", 0xfff8dc},
	{"crimson", 0xdc143c},
	{"cyan", 0x00ffff},
	{"darkblue", 0x00008b},
	{"darkcyan", 0x008b8b},
	{"darkgoldenrod", 0xb8860b},
	{"darkgray", 0xa9a9a9},
	{"darkgrey", 0xa9a9a9},
	{"darkgreen", 0x006400},
	{"darkkhaki", 0xbdb76b},
	{"darkmagenta", 0x8b008b},
	{"darkolivegreen", 0x556b2f},
	{"darkorange", 0xff8c00},
	{"darkorchid", 0x9932cc},
	{"darkred", 0x8b0000},
	{"darksalmon", 0xe9967a},
	{"darkseagreen", 0x8fbc8f},
	{"darkslateblue", 0x483d8b},
	{"darkslategray", 0x2f4f4f},
	{"darkslategrey", 0x2f4f4f},
	{"darkturquoise", 0x00ced1},
	{"darkviolet", 0x9400d3},
	{"deeppink", 0xff1493},
	{"deepskyblue", 0x00bfff},
	{"dimgray", 0x696969},
	{"dimgrey", 0x696969},
	{"dodgerblue", 0x1e90ff},
	{"feldspar", 0xd19275},
	{"firebrick", 0xb22222},
	{"floralwhite", 0xfffaf0},
	{"forestgreen", 0x228b22},
	{"fuchsia", 0xff00ff},
	{"gainsboro", 0xdcdcdc},
	{"ghostwhite", 0xf8f8ff},
	{"gold", 0xffd700},
	{"goldenrod", 0xdaa520},
	{"gray", 0x808080},
	{"grey", 0x808080},
	{"green", 0x008000},
	{"greenyellow", 0xadff2f},
	{"honeydew", 0xf0fff0},
	{"hotpink", 0xff69b4},
	{"indianred", 0xcd5c5c},
	{"indigo", 0x4b0082},
	{"ivory", 0xfffff0},
	{"khaki", 0xf0e68c},
	{"lavender", 0xe6e6fa},
	{"lavenderblush", 0xfff0f5},
	{"lawngreen", 0x7cfc00},
	{"lemonchiffon", 0xfffacd},
	{"lightblue", 0xadd8e6},
	{"lightcoral", 0xf08080},
	{"lightcyan", 0xe0ffff},
	{"lightgoldenrodyellow", 0xfafad2},
	{"lightgray", 0xd3d3d3},
	{"lightgrey", 0xd3d3d3},
	{"lightgreen", 0x90ee90},
	{"lightpink", 0xffb6c1},
	{"lightsalmon", 0xffa07a},
	{"lightseagreen", 0x20b2aa},
	{"lightskyblue", 0x87cefa},
	{"lightslateblue", 0x8470ff},
	{"lightslategray", 0x778899},
	{"lightslategrey", 0x778899},
	{"lightsteelblue", 0xb0c4de},
	{"lightyellow", 0xffffe0},
	{"lime", 0x00ff00},
	{"limegreen", 0x32cd32},
	{"linen", 0xfaf0e6},
	{"magenta", 0xff00ff},
	{"maroon", 0x800000},
	{"mediumaquamarine", 0x66cdaa},
	{"mediumblue", 0x0000cd},
	{"mediumorchid", 0xba55d3},
	{"mediumpurple", 0x9370d8},
	{"mediumseagreen", 0x3cb371},
	{"mediumslateblue", 0x7b68ee},
	{"mediumspringgreen", 0x00fa9a},
	{"mediumturquoise", 0x48d1cc},
	{"mediumvioletred", 0xc71585},
	{"midnightblue", 0x191970},
	{"mintcream", 0xf5fffa},
	{"mistyrose", 0xffe4e1},
	{"moccasin", 0xffe4b5},
	{"navajowhite", 0xffdead},
	{"navy", 0x000080},
	{"oldlace", 0xfdf5e6},
	{"olive", 0x808000},
	{"olivedrab", 0x6b8e23},
	{"orange", 0xffa500},
	{"orangered", 0xff4500},
	{"orchid", 0xda70d6},
	{"palegoldenrod", 0xeee8aa},
	{"palegreen", 0x98fb98},
	{"paleturquoise", 0xafeeee},
	{"palevioletred", 0xd87093},
	{"papayawhip", 0xffefd5},
	{"peachpuff", 0xffdab9},
	{"peru", 0xcd853f},
	{"pink", 0xffc0cb},
	{"plum", 0xdda0dd},
	{"powderblue", 0xb0e0e6},
	{"purple", 0x800080},
	{"red", 0xff0000},
	{"rosybrown", 0xbc8f8f},
	{"royalblue", 0x4169e1},
	{"saddlebrown", 0x8b4513},
	{"salmon", 0xfa8072},
	{"sandybrown", 0xf4a460},
	{"seagreen", 0x2e8b57},
	{"seashell", 0xfff5ee},
	{"sienna", 0xa0522d},
	{"silver", 0xc0c0c0},
	{"skyblue", 0x87ceeb},
	{"slateblue", 0x6a5acd},
	{"slategray", 0x708090},
	{"slategrey", 0x708090},
	{"snow", 0xfffafa},
	{"springgreen", 0x00ff7f},
	{"steelblue", 0x4682b4},
	{"tan", 0xd2b48c},
	{"teal", 0x008080},
	{"thistle", 0xd8bfd8},
	{"tomato", 0xff6347},
	{"turquoise", 0x40e0d0},
	{"violet", 0xee82ee},
	{"violetred", 0xd02090},
	{"wheat", 0xf5deb3},
	{"white", 0xffffff},
	{"whitesmoke", 0xf5f5f5},
	{"yellow", 0xffff00},
	{"yellowgreen", 0x9acd32},
	{NULL, 0x0},
};

color_t gfx_col(int r, int g, int b)
{
	color_t col;
	col.r = r;
	col.g = g;
	col.b = b;
	col.a = 0;
	return col;
}

int gfx_parse_color (
	const char *spec,
	color_t *def)
{
	int n, i;
	int r, g, b;
	char c;

        if (!spec)
		return -1;
	spec += strspn(spec, " \t");
	n = strlen (spec);

	if (*spec == '#') {
	    /*
	     * RGB
	     */
	    spec++;
	    n--;
	    if (n != 3 && n != 6 && n != 9 && n != 12)
		return -1;
	    n /= 3;
	    g = b = 0;
	    do {
		r = g;
		g = b;
		b = 0;
		for (i = n; --i >= 0; ) {
		    c = *spec++;
		    b <<= 4;
		    if (c >= '0' && c <= '9')
			b |= c - '0';
		    else if (c >= 'A' && c <= 'F')
			b |= c - ('A' - 10);
		    else if (c >= 'a' && c <= 'f')
			b |= c - ('a' - 10);
		    else return (0);
		}
	    } while (*spec != '\0');
	    if (def) {
		    def->r = r;
		    def->g = g;
		    def->b = b;
	    }
	    return 0;
	}

	for (i=0; cnames[i].name; i++) {
		if (!strcmp(cnames[i].name, spec)) {
			if (def) {
				def->r = (cnames[i].val & 0xff0000) >> 16;
				def->g = (cnames[i].val & 0x00ff00) >> 8;
				def->b = (cnames[i].val & 0x0000ff);
			}
			return 0;
		}
	}
	return -1;
}

struct _anim_t;

struct anspawn {
	SDL_Rect clip;
	img_t	bg;
	int x;
	int y;
};

#define ANSPAWN_BLOCK 8
struct _anim_t {
	struct _anim_t *next;
	struct _anim_t *prev;
	int	cur_frame;
	int	nr_frames;
	int	loop;
	int 	drawn;
	int	active;
	unsigned long	delay;
	int	spawn_nr;
	struct	anspawn *spawn;
	Animation_t *anim;
};

typedef struct _anim_t *anim_t;

static int anim_spawn(anim_t ag, int x, int y, int w, int h)
{
	int nr;
	SDL_Rect clip;
	SDL_GetSurfaceClipRect(Surf(screen), &clip);
	/* gfx_free_image(ag->bg); */
	if (!ag->spawn && !(ag->spawn = malloc(ANSPAWN_BLOCK * sizeof(struct anspawn))))
		return -1;
	nr = ag->spawn_nr + 1;
	if (!(nr % ANSPAWN_BLOCK)) { /* grow */
		void *p = realloc(ag->spawn, ANSPAWN_BLOCK * sizeof(struct anspawn) *
						((nr / ANSPAWN_BLOCK) + 1));
		if (!p)
			return -1;
		ag->spawn = p;
	}
	ag->spawn[ag->spawn_nr].x = x;
	ag->spawn[ag->spawn_nr].y = y;
	ag->spawn[ag->spawn_nr].clip = clip;
	ag->spawn[ag->spawn_nr].bg = gfx_grab_screen(x, y, w, h);
	ag->spawn_nr = nr;
	return 0;
}

static anim_t anim_list = NULL;

static int anim_drawn_nr = 0;

static anim_t anim_find(anim_t g)
{
	anim_t p;
	for (p = anim_list; p; p = p->next) {
		if (p == g)
			return p;
	}
	return NULL;
}

static void anim_disposal(anim_t g)
{
	SDL_Rect dest;
	SDL_Rect clip;
	int i = 0;
	SDL_Surface	*img = NULL;

	dest.x = 0;
	dest.y = 0;
	dest.w = g->anim->w;
	dest.h = g->anim->h;

	SDL_GetSurfaceClipRect(Surf(screen), &clip);

	for (i = 0; i < g->spawn_nr; i++) {
		SDL_Rect dst;
		SDL_SetSurfaceClipRect(Surf(screen), &g->spawn[i].clip);
		dst = dest;
		dst.x += g->spawn[i].x;
		dst.y += g->spawn[i].y;
		img = Surf(g->spawn[i].bg);
		if (img) {
			dst.w = img->w;
			dst.h = img->h;
			/* draw bg */
			SDL_BlitSurface(img, NULL, Surf(screen), &dst);
		}
	}
	SDL_SetSurfaceClipRect(Surf(screen), &clip);
}

static void anim_frame(anim_t g)
{
	int i;
	SDL_Rect dest;
	SDL_Rect clip;
	SDL_Surface *frame;
	frame = g->anim->frames[g->cur_frame];

	SDL_GetSurfaceClipRect(Surf(screen), &clip);

	dest.w = g->anim->w;
	dest.h = g->anim->h;

	for (i = 0; i < g->spawn_nr; i++) {
		dest.x = g->spawn[i].x;
		dest.y = g->spawn[i].y;
		SDL_SetSurfaceClipRect(Surf(screen), &g->spawn[i].clip);
		SDL_BlitSurface(frame, NULL, Surf(screen), &dest);
	}
	if (!g->active) /* initial draw */
		g->delay = timer_counter;
	SDL_SetSurfaceClipRect(Surf(screen), &clip);
}

static anim_t is_anim(img_t img)
{
	if (img && (img->flags & IMG_ANIM))
		return (anim_t)(img->aux);
	return NULL;
}

static anim_t anim_add(anim_t g)
{
	anim_t p;
	p = anim_find(g);
	if (p)
		return p;
	if (!anim_list)	{
		anim_list = g;
		g->next = NULL;
		g->prev = NULL;
		return g;
	}
	for (p = anim_list; p && p->next; p = p->next);
	p->next = g;
	g->next = NULL;
	g->prev = p;
	return g;
}

static anim_t anim_del(anim_t g)
{
	if (g->prev == NULL)
		anim_list = g->next;
	else
		g->prev->next = g->next;
	if (g->next)
		g->next->prev = g->prev;
	return g;
}

static void anim_free_spawn(anim_t g)
{
	int i;
	for (i = 0; i < g->spawn_nr; i++)
		gfx_free_image(g->spawn[i].bg);
	if (g->spawn) {
		free(g->spawn);
		g->spawn = NULL;
		g->spawn_nr = 0;
	}
}

static void anim_free(anim_t g)
{
	FreeAnimation(g->anim);
	anim_free_spawn(g);
	free(g);
}

static void gfx_free_img(img_t p)
{
	if (!p)
		return;
	SDL_DestroySurface(Surf(p));
	/* todo texture */
	SDL_free(p);
}

void gfx_free_image(img_t p)
{
	anim_t ag;
	if (!p)
		return;
	if (!cache_forget(images, p))
		return; /* cached sprite */
	if ((ag = is_anim(p))) {
		if (ag->drawn)
			anim_drawn_nr --;
		anim_del(ag);
		anim_free(ag);
		return;
	}
	gfx_free_img(p);
}

void gfx_cache_free_image(void *p)
{
	gfx_free_image((img_t)p);
}

int	gfx_img_w(img_t pixmap)
{
	if (!pixmap)
		return 0;
	return Surf(pixmap)->w;
}

int	gfx_img_h(img_t pixmap)
{
	if (!pixmap)
		return 0;
	return Surf(pixmap)->h;
}


void gfx_getclip(int *x, int *y, int *w, int *h)
{
	SDL_Rect clip;
	if (!screen)
		return;
	SDL_GetSurfaceClipRect(Surf(screen), &clip);
	if (x)
		*x = clip.x;
	if (y)
		*y = clip.y;
	if (w)
		*w = clip.w;
	if (h)
		*h = clip.h;
}

void gfx_img_noclip(img_t img)
{
	SDL_SetSurfaceClipRect(Surf(img), NULL);
}

void gfx_noclip(void)
{
	gfx_img_noclip(screen);
}

void gfx_img_clip(img_t img, int x, int y, int w, int h)
{
	SDL_Rect src;
	src.x = x;
	src.y = y;
	src.w = w;
	src.h = h;
	SDL_SetSurfaceClipRect(Surf(img), &src);
}

void gfx_clip(int x, int y, int w, int h)
{
	gfx_img_clip(screen, x, y, w, h);
}

#define GFX_IMG(v) gfx_new_img(v, 0, NULL, 0)
#define GFX_IMG_REL(v) gfx_new_img(v, 0, NULL, 1)

static img_t	gfx_new_img(SDL_Surface *s, int fl, void *data, int release)
{
	img_t i;
	if (!s)
		return NULL;
	i = SDL_malloc(sizeof(struct _img_t));
	if (i) {
		i->s = s;
/*		i->t = NULL; */
		i->flags = fl;
		i->aux = data;
	} else if (release && s) {
		SDL_DestroySurface(s);
	}
	return i;
}

img_t   gfx_new_rgba(int w, int h)
{
	SDL_Surface *dst;
	dst = SDL_CreateSurface(w, h, PIXEL_FORMAT);
	if (dst)
		SDL_SetSurfaceBlendMode(dst, SDL_BLENDMODE_BLEND);
	if (dst)
		return GFX_IMG_REL(dst);
	return NULL;
}

img_t gfx_dup(img_t src)
{
	SDL_Surface *dst;
	if (!src)
		return NULL;
	dst = SDL_ConvertSurface(Surf(src), PIXEL_FORMAT);
	if (!dst)
		return NULL;
	return GFX_IMG_REL(dst);
}

img_t   gfx_new_from(int w, int h, unsigned char *pixels)
{
	SDL_Surface *dst = SDL_CreateSurfaceFrom(w, h, PIXEL_FORMAT, pixels, w*4);
	if (dst)
		SDL_SetSurfaceBlendMode(dst, SDL_BLENDMODE_BLEND);
	if (dst)
		return GFX_IMG_REL(dst);
	return NULL;
}

img_t 	gfx_new(int w, int h)
{
	SDL_Surface *dst= SDL_CreateSurface(w, h, PIXEL_FORMAT);
	if (dst)
		SDL_SetSurfaceBlendMode(dst, SDL_BLENDMODE_NONE);
	if (dst)
		return GFX_IMG_REL(dst);
	return NULL;
}

void	gfx_img_fill(img_t img, int x, int y, int w, int h, color_t col)
{
	SDL_Rect dest;
	if (!img)
		return;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	SDL_FillSurfaceRect(Surf(img), &dest, SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, col.r, col.g, col.b));
}

void	gfx_fill(int x, int y, int w, int h, color_t col)
{
	gfx_img_fill(screen, x, y, w, h, col);
}

img_t	gfx_screen(img_t nscreen)
{
	img_t img;
	if (nscreen) {
		img = screen;
		screen = nscreen;
		return img;
	}
	return screen;
}

img_t	gfx_grab_screen(int x, int y, int w, int h)
{
	SDL_Rect dst, src;
	SDL_Surface *s;
	int a;
	img_t img;
	if (!screen)
		return NULL;
	s = SDL_CreateSurface(w, h, PIXEL_FORMAT);
	if (!s)
		return NULL;
	src.x = x; src.y = y;
	src.w = w; src.h = h;
	dst.x = 0; dst.y = 0;
	dst.w = w; dst.h = h;
/*	SDL_SetSurfaceBlendMode(screen, SDL_BLENDMODE_NONE);
	SDL_SetSurfaceBlendMode(img, SDL_BLENDMODE_NONE); */
	a = gfx_unset_alpha(screen);
	SDL_BlitSurface(Surf(screen), &src, s, &dst);
	gfx_set_alpha(screen, a);
	img = GFX_IMG_REL(s);
	if (!img)
		return NULL;
	gfx_unset_alpha(img);
	return img;
}

img_t gfx_display_alpha(img_t src)
{
	img_t dst;
	if (is_anim(src)) /* already optimized */
		return src;
	dst = gfx_dup(src); /* always to rgba */
	SDL_SetSurfaceBlendMode(Surf(dst), SDL_BLENDMODE_BLEND);
	gfx_free_image(src);
	return dst;
}

int gfx_get_pixel(img_t src, int x, int y,  color_t *color)
{
	Uint8 r, g, b, a;
	Uint32 col = 0;
	Uint8 *ptr;
	int	bpp = 4;
	SDL_Surface *img = Surf(src);
	if (!img)
		return -1;

	if (x >= img->w || y >= img->h || x < 0 || y < 0)
		return -1;

	if (!SDL_LockSurface(img))
		return -1;

	ptr = (Uint8*)img->pixels;
	ptr += img->w * y * bpp;
	ptr += x * bpp;

	memcpy(&col, ptr, bpp);

	SDL_UnlockSurface(img);
	if (color)
		SDL_GetRGBA(col, PIXEL_FORMAT_DETAILS, NULL, &r, &g, &b, &a);

	if (color) {
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = a;
	}
	return 0;
}

int gfx_set_pixel(img_t src, int x, int y,  color_t color)
{
	int bpp = 4;
	Uint32 col;
	Uint8 *ptr;
	SDL_Surface *img = Surf(src);
	if (!img)
		return -1;

	if (x >= img->w || y >= img->h || x < 0 || y < 0)
		return -1;

	if (!SDL_LockSurface(img))
		return -1;

	ptr = (Uint8*)img->pixels;
	ptr += img->w * y * bpp;
	ptr += x * bpp;
	col = SDL_MapRGBA(PIXEL_FORMAT_DETAILS, NULL, color.r, color.g, color.b, color.a);
	memcpy(ptr, &col, bpp);

	SDL_UnlockSurface(img);
	return 0;
}

void gfx_put_pixels(img_t src)
{
	SDL_Surface *img = Surf(src);
	SDL_UnlockSurface(img);
	return;
}

unsigned char *gfx_get_pixels(img_t src)
{
	Uint8 *ptr;
	SDL_Surface *img = Surf(src);

	if (!img)
		return NULL;

	if (!SDL_LockSurface(img))
		return NULL;

	ptr = (unsigned char*)img->pixels;
	return ptr;
}

img_t gfx_alpha_img(img_t src, int alpha)
{
	Uint8 *ptr;
	Uint32 col;
	int size;
	int bpp = 4;

	img_t img = NULL;
	if (!src)
		return NULL;
	img = gfx_new(Surf(src)->w, Surf(src)->h);

	if (!img)
		return NULL;

	gfx_set_alpha(img, SDL_ALPHA_OPAQUE);

	if (SDL_LockSurface(Surf(img))) {
		int w = Surf(img)->w;
		ptr = (Uint8*)(Surf(img)->pixels);
		size = Surf(img)->w * Surf(img)->h;
		while (size --) {
			Uint8 r, g, b, a;
			memcpy(&col, ptr, bpp);
			SDL_GetRGBA(col, PIXEL_FORMAT_DETAILS, NULL, &r, &g, &b, &a);
			col = SDL_MapRGBA(PIXEL_FORMAT_DETAILS, NULL, r, g, b, a * alpha /  SDL_ALPHA_OPAQUE);
			memcpy(ptr, &col, bpp);
			ptr += bpp;
			w --;
			if (!w) {
				w = Surf(img)->w;
				ptr += Surf(img)->pitch;
				ptr -= w * bpp;
			}
		}
		SDL_UnlockSurface(Surf(img));
	}
	return img;
}

void gfx_set_colorkey(img_t src, color_t col)
{
	Uint32 c;
	SDL_Surface *s = Surf(src);
	if (!s)
		return;
	c = SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, col.r, col.g, col.b);
	SDL_SetSurfaceColorKey(s, true, c);
}

void gfx_unset_colorkey(img_t src)
{
	SDL_SetSurfaceColorKey(Surf(src), false, 0);
}

void	gfx_set_alpha(img_t src, int alpha)
{
	if (alpha < 0)
		return;
/*	if (Surf(src)->format->Amask)
		alpha = SDL_ALPHA_OPAQUE; */
	SDL_SetSurfaceAlphaMod(Surf(src), alpha);
/*		SDL_SetSurfaceBlendMode((SDL_Surface *)src, SDL_BLENDMODE_NONE);
	else */
	SDL_SetSurfaceBlendMode(Surf(src), SDL_BLENDMODE_BLEND);
}

int	gfx_unset_alpha(img_t src)
{
	int alpha = -1;
	SDL_BlendMode  blendMode;
	Uint8	sdl_alpha = SDL_ALPHA_OPAQUE;
	alpha = SDL_GetSurfaceBlendMode(Surf(src), &blendMode);
	if (blendMode == SDL_BLENDMODE_BLEND) {
		SDL_GetSurfaceAlphaMod(Surf(src), &sdl_alpha);
		alpha = sdl_alpha;
	}
	SDL_SetSurfaceAlphaMod(Surf(src), SDL_ALPHA_OPAQUE);
	SDL_SetSurfaceBlendMode(Surf(src), SDL_BLENDMODE_NONE);
	return alpha;
}

static img_t img_pad(char *fname)
{
	int l,r,t,b, rc;
	img_t img, img2;
	SDL_Rect to;
	char *p = fname;
	p += strcspn(p, ",");
	if (*p != ',')
		return NULL;
	p ++;
	rc = sscanf(fname, "%d %d %d %d,", &t, &r, &b, &l);
	if (rc == 1) {
		r = b = l = t;
	} else if (rc == 2) {
		b = t; l = r;
	} else if (rc == 3) {
		l = r;
	} else if (rc == 4) {
		;
	} else
		return NULL;
	img = gfx_load_image(p);
	if (!img)
		return NULL;
	img2 = gfx_new(gfx_img_w(img) + l + r, gfx_img_h(img) + t + b);
	if (!img2) {
		gfx_free_image(img);
		return NULL;
	} else {
		img_t img = gfx_alpha_img(img2, 0);
		if (img) {
			gfx_free_image(img2);
			img2 = img;
		}
	}
	to.x = l;
	to.y = t;
	SDL_gfxBlitRGBA(Surf(img), NULL, Surf(img2), &to);
	gfx_free_image(img);
	return img2;
}

static img_t _gfx_load_combined_image(char *filename);

/* blank:WxH */
static img_t _gfx_load_special_image(char *f, int combined)
{
	int alpha = 0;
	int blank = 0;
	char *filename;
	img_t img, img2;
	char *pc = NULL, *pt = NULL;
	int wh[2] = { 0, 0 };
	if (!f)
		return NULL;

	if (!(f = filename = strdup(f)))
		return NULL;

	if (!strncmp(filename, "blank:", 6)) {
		filename += 6;
		blank = 1;
	} else if (!strncmp(filename, "spr:", 4) && !combined) {
/*		filename += 4; */
		img2 = cache_get(images, filename);
/*		fprintf(stderr, "get:%s %p\n", filename, img2); */
		goto out;
	} else if (!strncmp(filename, "box:", 4)) {
		filename += 4;
		alpha =  SDL_ALPHA_OPAQUE;
	} else if (!strncmp(filename, "pad:", 4)) {
		filename += 4;
		img2 = img_pad(filename);
		goto out;
	} else if (!strncmp(filename, "comb:", 5)) {
		filename += 5;
		img2 = _gfx_load_combined_image(filename);
		goto out;
	} else
		goto err;

	if (strchr(filename, ';'))
		goto err; /* combined */

	if (blank)
		goto skip;
	pc = filename + strcspn(filename, ",");
	if (*pc == ',') {
		*pc = 0;
		pc ++;
		pt = pc + strcspn(pc, ",");
		if (*pt == ',') {
			*pt = 0;
			pt ++;
		} else
			pt = NULL;
	} else
		pc = NULL;
skip:
	if (parse_mode(filename, wh))
		goto err;
	if (wh[0] <= 0 || wh[1] <= 0)
		goto err;
	img = gfx_new(wh[0], wh[1]);
	if (!img)
		goto err;

	if (pc) {
		color_t col = { .r = 255, .g = 255, .b = 255 };
		gfx_parse_color(pc, &col);
		gfx_img_fill(img, 0, 0, wh[0], wh[1], col);
	}
	if (pt)
		alpha = atoi(pt);

	img2 = gfx_alpha_img(img, alpha);
	gfx_free_image(img);
out:
	free(f);
	return img2;
err:
	free(f);
	return NULL;
}

cache_t gfx_image_cache(void)
{
	return images;
}

static anim_t anim_new(Animation_t *anim)
{
	anim_t ag = malloc(sizeof(struct _anim_t));
	if (!ag)
		return NULL;
	memset(ag, 0, sizeof(struct _anim_t));
	ag->anim = anim;
	ag->nr_frames = anim->count;
	ag->loop = anim->loop;
	return ag;
}

static anim_t anim_clone(anim_t ag)
{
	int i;
	anim_t nag = malloc(sizeof(struct _anim_t));
	if (!nag)
		return NULL;
	memcpy(nag, ag, sizeof(struct _anim_t));
	nag->cur_frame = 0;
	nag->drawn = 0;
	nag->active = 0;
	nag->delay = 0;
	nag->spawn_nr = 0;
	nag->spawn = NULL;
	nag->anim = malloc(sizeof(Animation_t));
	if (!nag->anim)
		goto err;
	memcpy(nag->anim, ag->anim, sizeof(Animation_t));
	nag->anim->delays = malloc(sizeof(int) * nag->nr_frames);
	nag->anim->frames = malloc(sizeof(SDL_Surface *) * nag->nr_frames);
	if (!nag->anim->delays || !nag->anim->frames)
		goto err;
	for (i = 0; i < nag->nr_frames; i ++) {
		nag->anim->frames[i] = ag->anim->frames[i];
		nag->anim->delays[i] = ag->anim->delays[i];
	}
	return nag;
err:
	if (nag->anim) {
		if (nag->anim->delays)
			free(nag->anim->delays);
		if (nag->anim->frames)
			free(nag->anim->frames);
		free(nag->anim);
	}
	free(nag);
	return NULL;
}

static img_t _gfx_load_image(char *filename, int combined)
{
	SDL_IOStream *rw;
	img_t img;
	Animation_t *anim = NULL;
	filename = strip(filename);
	img = _gfx_load_special_image(filename, combined);
	if (img)
		return img;
	if (strstr(filename,".gif") || strstr(filename,".GIF")) /* only agif now */
		anim = GIF_LoadAnim(filename);
	if (anim) { /* animation logic */
		anim_t ag = anim_new(anim);
		if (!ag)
			return NULL;
		anim_add(ag);
		img = gfx_new_img(ag->anim->frames[0], IMG_ANIM, ag, 0);
		if (!img) {
			anim_del(ag);
			anim_free(ag);
		}
		return img;
	}
	rw = RWFromIdf(instead_idf(), filename);

	if (!rw || !(img = GFX_IMG_REL(IMG_Load_IO(rw, 1))))
		return NULL;

	return gfx_display_alpha(img);
}

/* x.png;a.png@1,2;b.png@3,4 */
static img_t _gfx_load_combined_image(char *filename)
{
	char *str;
	char *p, *ep;
	img_t	base = NULL, img = NULL;
	p = str = strdup(filename);
	if (!str)
		return NULL;
	ep = p + strcspn(p, ";");
	if (*ep != ';')
		goto err; /* first image is a base image */
	*ep = 0;

	base = _gfx_load_image(strip(p), 1);
	if (!base)
		goto err;
	p = ep + 1;
	while (*p) {
		int x = 0, y = 0, c = 0;
		SDL_Rect to;
		ep = p + strcspn(p, ";@");
		if (*ep == '@') {
			*ep = 0; ep ++;
			if (*ep == 'c') {
				c = 1;
				ep ++;
			}
			sscanf(ep, "%d,%d", &x, &y);
			ep += strcspn(ep, ";");
			if (*ep)
				ep ++;
		} else if (*ep == ';') {
			*ep = 0; ep ++;
		} else if (*ep) {
			goto err;
		}
		img = _gfx_load_image(strip(p), 1);
		if (!img)
			goto err;
		to.x = x; to.y = y;
		if (c) {
			to.x -= gfx_img_w(img) / 2;
			to.y -= gfx_img_h(img) / 2;
		}
		to.w = to.h = 0;
		SDL_gfxBlitRGBA(Surf(img), NULL, Surf(base), &to);
		gfx_free_image(img);
		p = ep;
	}
	free(str);
	return base;
err:
	gfx_free_image(base);
	free(str);
	return NULL;
}

img_t gfx_load_image(char *filename)
{
	img_t img = NULL;
	if (!filename)
		return NULL;
/*	if (!access(filename, R_OK)) */
		img = _gfx_load_image(filename, 0);
	if (!img)
		img = _gfx_load_combined_image(filename);
	return img;
}

void gfx_draw_bg(img_t p, int x, int y, int width, int height)
{
	int a;
	SDL_Surface *pixbuf = Surf(p);
	SDL_Rect dest, src;
	if (!p)
		return;
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = x;
	dest.y = y;
	dest.w = width;
	dest.h = height;
	a = gfx_unset_alpha(p);
	SDL_BlitSurface(pixbuf, &src, Surf(screen), &dest);
	gfx_set_alpha(p, a);
}

void gfx_draw_from_alpha(img_t s, int x, int y, int w, int h, img_t d, int xx, int yy, int alpha)
{
	SDL_BlendMode  blendMode;
	Uint8	sdl_alpha = SDL_ALPHA_OPAQUE;
	SDL_GetSurfaceBlendMode(Surf(s), &blendMode);
	SDL_GetSurfaceAlphaMod(Surf(s), &sdl_alpha);

	SDL_SetSurfaceAlphaMod(Surf(s), alpha);
	SDL_SetSurfaceBlendMode(Surf(s), SDL_BLENDMODE_BLEND);

	gfx_draw_from(s, x, y, w, h, d, xx, yy);
	SDL_SetSurfaceBlendMode(Surf(s), blendMode);
	SDL_SetSurfaceAlphaMod(Surf(s), sdl_alpha);
}

void gfx_draw_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy)
{
	SDL_Surface *pixbuf = Surf(p);
	SDL_Surface *scr = Surf(to);
	SDL_Rect dest, src;
	if (!p)
		return;
	if (!scr)
		scr = Surf(screen);
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = xx;
	dest.y = yy;
	dest.w = width;
	dest.h = height;
	SDL_BlitSurface(pixbuf, &src, scr, &dest);
}

void gfx_compose_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy)
{
	SDL_Surface *pixbuf = Surf(p);
	SDL_Surface *scr = Surf(to);
	SDL_Rect dest, src;
	if (!scr)
		scr = Surf(screen);
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = xx;
	dest.y = yy;
	dest.w = width;
	dest.h = height;
	SDL_gfxBlitRGBA(pixbuf, &src, scr, &dest);
}

void gfx_copy(img_t p, int x, int y)
{
	int a;
	SDL_Surface *pixbuf = Surf(p);
	SDL_Rect dest;
	if (!p)
		return;
	dest.x = x;
	dest.y = y;
	dest.w = pixbuf->w;
	dest.h = pixbuf->h;
	a = gfx_unset_alpha(p);
	SDL_BlitSurface(pixbuf, NULL, Surf(screen), &dest);
	gfx_set_alpha(p, a);
}

void gfx_copy_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy)
{
	int a;
	SDL_Surface *pixbuf = Surf(p);
	SDL_Surface *scr = Surf(to);
	SDL_Rect dest, src;
	if (!p)
		return;
	if (!scr)
		scr = Surf(screen);
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = xx;
	dest.y = yy;
	dest.w = width;
	dest.h = height;
	a = gfx_unset_alpha(p);
	SDL_BlitSurface(pixbuf, &src, scr, &dest);
	gfx_set_alpha(p, a);
}

void gfx_draw(img_t p, int x, int y)
{
	anim_t ag;
	SDL_Surface *pixbuf = Surf(p);
	SDL_Rect dest;
	if (!p)
		return;
	dest.x = x;
	dest.y = y;
	dest.w = pixbuf->w;
	dest.h = pixbuf->h;
	if (!DIRECT_MODE) /* no anim in direct mode */
		ag = is_anim(p);
	else
		ag = NULL;
	if (ag) {
		anim_spawn(ag, x, y, dest.w, dest.h);
		if (!ag->drawn)
			anim_drawn_nr ++;
		anim_frame(ag);
		ag->drawn = 1;
		ag->active = 1;
		return;
	}
	SDL_BlitSurface(pixbuf, NULL, Surf(screen), &dest);
}

void gfx_stop_anim(img_t p)
{
	anim_t ag;
	ag = is_anim(p);
	if (ag)
		ag->active = 0;
}

void gfx_dispose_anim(img_t p)
{
	anim_t ag;
	ag = is_anim(p);
	if (ag) {
		if (ag->drawn)
			anim_drawn_nr --;
		ag->drawn = 0;
		anim_free_spawn(ag);
	}
}

void gfx_start_anim(img_t p)
{
	anim_t ag;
	ag = is_anim(p);
	if (ag)
		ag->active = 1;
}

int gfx_frame_anim(img_t img)
{
	anim_t ag;
	ag = is_anim(img);

	if (!ag)
		return 0;

	if (!ag->drawn || !ag->active)
		return 0;
	if (ag->loop == -1)
		return 0;

	if ((int)(timer_counter - ag->delay) < (ag->anim->delays[ag->cur_frame] / HZ))
		return 0;

	if (ag->cur_frame != ag->nr_frames - 1 || ag->loop > 1 || !ag->loop)
		anim_disposal(ag);

	ag->cur_frame ++;

	if (ag->cur_frame >= ag->nr_frames) {
		if (!ag->loop || ag->loop > 1)
			ag->cur_frame = 0;
		else
			ag->cur_frame --; /* last one */
		if (ag->loop) {
			ag->loop --;
			if (!ag->loop)
				ag->loop = -1; /* disabled */
		}

	}
	if (ag->loop != -1) {
		anim_frame(ag);
		ag->delay = timer_counter;
	}
	return 1;
}

int gfx_is_drawn_anims(void)
{
	return anim_drawn_nr;
}

void gfx_update_anim(img_t img, update_fn update)
{
	int i = 0;
	anim_t ag;
	ag = is_anim(img);
	if (!ag)
		return;
	if (!ag->drawn || !ag->active)
		return;
	for (i = 0; i < ag->spawn_nr; i++) {
		update(ag->spawn[i].x, ag->spawn[i].y,
			gfx_img_w(img), gfx_img_h(img));
	}
}

void gfx_draw_wh(img_t p, int x, int y, int w, int h)
{
	SDL_Surface *pixbuf = Surf(p);
	SDL_Rect dest, src;
	src.x = 0;
	src.y = 0;
	src.w = w;
	src.h = h;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	SDL_BlitSurface(pixbuf, &src, Surf(screen), &dest);
}
static SDL_Color bgcol = { .r = 0, .g = 0, .b = 0 };
static SDL_Color brdcol = { .r = 0, .g = 0, .b = 0 };
static SDL_Rect brd = { .x = 0, .y = 0, .w = -1, .h = -1 };

void gfx_bg(int x, int y, int w, int h, color_t col, color_t bcol)
{
	brd.x = x;
	brd.y = y;
	brd.w = w;
	brd.h = h;
	bgcol.r = col.r;
	bgcol.g = col.g;
	bgcol.b = col.b;
	brdcol.r = bcol.r;
	brdcol.g = bcol.g;
	brdcol.b = bcol.b;
}

void gfx_clear(int x, int y, int w, int h)
{
	int dx, dy;
	SDL_Rect dest;
	SDL_Surface *s = Surf(screen);
	if (!s)
		return;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	if (x < brd.x || y < brd.y || x + w >= brd.x + brd.w || y + h >= brd.y + brd.h) {
		SDL_FillSurfaceRect(s, &dest, SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, brdcol.r, brdcol.g, brdcol.b));
		dx = brd.x - x;
		dy = brd.y - y;
		if (dx > 0) {
			dest.x += dx;
			dest.w -= dx;
		}
		if (dy > 0) {
			dest.y += dy;
			dest.h -= dy;
		}
		dx = (brd.x + brd.w) - (dest.x + dest.w);
		dy = (brd.y + brd.h) - (dest.y + dest.h);
		if (dx < 0)
			dest.w += dx;
		if (dy < 0)
			dest.h += dy;
		if (dest.w > 0 && dest.h > 0)
			SDL_FillSurfaceRect(s, &dest, SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, bgcol.r, bgcol.g, bgcol.b));
	} else
		SDL_FillSurfaceRect(s, &dest, SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, bgcol.r, bgcol.g, bgcol.b));
}

int gfx_width = -1;
int gfx_height = -1;
int gfx_fs = -1;

static SDL_Rect** vid_modes = NULL;
static SDL_Rect m640x480 = { .w = 640, .h = 480 };
static SDL_Rect m800x480 = { .w = 800, .h = 480 };
static SDL_Rect m800x600 = { .w = 800, .h = 600 };
static SDL_Rect m1024x768 = { .w = 1024, .h = 768 };
static SDL_Rect m1280x800 = { .w = 1280, .h = 800 };

static SDL_Rect* std_modes[] = { &m640x480, &m800x480, &m800x600, &m1024x768, &m1280x800, NULL };

static SDL_DisplayID SDL_CurrentDisplay = 0;

static int SelectVideoDisplay()
{
	int i = 0, disp_nr;
	const char *variable = SDL_getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
	if (!variable)
		variable = SDL_getenv("SDL_VIDEO_FULLSCREEN_HEAD");
	if (variable)
		i = SDL_atoi(variable);

	SDL_DisplayID *disp_list = SDL_GetDisplays(&disp_nr);
	if (!disp_list)
		return -1;
	if (i >= disp_nr)
		i = 0;
	SDL_CurrentDisplay = disp_list[i];
	SDL_free(disp_list);
	return 0;
}

static SDL_Rect **SDL_ListModes(const SDL_PixelFormat * format, Uint32 flags)
{
	const SDL_DisplayMode *disp_mode;
	int i, nmodes;
	int disp_modes_nr = 0;
	SDL_DisplayMode **disp_modes;
	SDL_Rect **modes;
	SDL_Rect **new_modes;
	int bpp;

	SelectVideoDisplay();

	disp_mode = SDL_GetDesktopDisplayMode(SDL_CurrentDisplay);
	bpp = SDL_BITSPERPIXEL(disp_mode->format);

	nmodes = 0;
	modes = NULL;

	disp_modes = SDL_GetFullscreenDisplayModes(SDL_CurrentDisplay, &disp_modes_nr);
	for (i = 0; i < disp_modes_nr; ++i) {
		SDL_DisplayMode *mode = disp_modes[i];

		if (!mode->w || !mode->h)
			continue;

		if ((unsigned int)bpp < SDL_BITSPERPIXEL(mode->format))
			continue;

		if (mode->w > disp_mode->w || mode->h > disp_mode->h) {  /* skip large modes */
			if (mode->w > disp_mode->h || mode->h > disp_mode->w) /* landscape ? */
				continue;
		}

		if (nmodes > 0 && modes[nmodes - 1]->w == mode->w
			&& modes[nmodes - 1]->h == mode->h)
			continue;

		new_modes = SDL_realloc(modes, (nmodes + 2) * sizeof(*modes));
		if (!new_modes)
			goto out;
		modes = new_modes;
		modes[nmodes] = (SDL_Rect *) SDL_malloc(sizeof(SDL_Rect));
		if (!modes[nmodes])
			goto out;
		modes[nmodes]->x = 0;
		modes[nmodes]->y = 0;
		modes[nmodes]->w = mode->w;
		modes[nmodes]->h = mode->h;
		++nmodes;
	}
	SDL_free(disp_modes);
	if (!modes) /* no modes found */
		return (SDL_Rect **) (-1);
	if (modes) {
		modes[nmodes] = NULL;
	}
	return modes;
out:
	for (i = 0; i < nmodes; i++)
		SDL_free(modes[i]);
	SDL_free(modes);
	return NULL;
}

extern char *modes_sw;

static int gfx_parse_modes(void)
{
	const char *p = modes_sw;
	int nr = 0;
	int mode[2];
	int i = 0;

	if (!modes_sw)
		return 0;

	while (*p) {
		p += strcspn(p, ",");
		if (*p)
			p ++;
		nr ++;
	}
	if (!nr)
		return 0;
	vid_modes = SDL_malloc(sizeof(SDL_Rect *) * (nr + 1)); /* array of pointers */
	if (!vid_modes)
		return 0;
	p = modes_sw;
	nr = 0;
	while (*p) {
		char m[64];
		SDL_Rect *r;
		size_t s;
		s = strcspn(p, ",");
		memset(m, 0, sizeof(m));
		memcpy(m, p, (s > sizeof(m))? sizeof(m): s);
		m[sizeof(m) - 1] = 0;
		if (parse_mode(m, &mode))
			break;
		r = SDL_malloc(sizeof(SDL_Rect));
		if (!r)
			goto err;
		vid_modes[nr ++] = r;
		r->w = mode[0];
		r->h = mode[1];
		fprintf(stderr, "Available mode: %dx%d\n", r->w, r->h);
		p += strcspn(p, ",");
		if (*p)
			p ++;
	}
	vid_modes[nr] = NULL;
	return nr;
err:
	for (i = 0; i < nr; i++) {
		SDL_free(vid_modes[i]);
	}
	SDL_free(vid_modes);
	vid_modes = NULL;
	return 0;
}

int gfx_modes(void)
{
	int i = 0;
	SDL_Rect** modes;
	if ((i = gfx_parse_modes()))
		return i;
	modes = SDL_ListModes(NULL, 0);
	if (modes == (SDL_Rect**)0)/* no modes */
		return 0;
	if (modes == (SDL_Rect**)-1) {
		vid_modes = std_modes;
		return 5;
	}
	for (i = 0; modes[i]; ++i);
	vid_modes = modes;
	return i;
}

int gfx_get_mode(int n, int *w, int *h)
{
	if (!vid_modes)
		gfx_modes();

	if (!vid_modes || !vid_modes[n])
		return -1;
	if (w)
		*w = vid_modes[n]->w;
	if (h)
		*h = vid_modes[n]->h;
	return 0;
}

int gfx_prev_mode(int *w, int *h)
{
	int ww, hh, i = 0;

	if (!w || !h)
		return -1;


	while ((*w != -1 && *h != -1) &&
		!gfx_get_mode(i, &ww, &hh)) {
		if (ww == *w && hh == *h)
			break;
		i ++;
	}

	if (*w == -1 || *h == -1)
		i = gfx_modes();

	if (!i)
		return -1;
	i --;
	if (gfx_get_mode(i, &ww, &hh))
		return -1;
	*w = ww; *h = hh;
	return 0;
}

int gfx_next_mode(int *w, int *h)
{
	int ww, hh, i = 0;

	if (!w || !h)
		return -1;

	while ((*w != -1 && *h != -1) &&
		!gfx_get_mode(i, &ww, &hh)) {
		i ++;
		if (ww == *w && hh == *h)
			break;
	}

	if (gfx_get_mode(i, &ww, &hh))
		return -1;
	*w = ww; *h = hh;
	return 0;
}
#if defined(ANDROID) || defined(IOS)
static int current_gfx_w = - 1;
static int current_gfx_h = - 1;
#endif

#if defined(ANDROID)
extern void get_screen_size(int *w, int *h);
#endif

int gfx_get_max_mode(int *w, int *h, int o)
{
	int ww = 0, hh = 0;
	int i = 0;
	const SDL_DisplayMode *desktop_mode;
	#if defined(ANDROID)
	if (o == MODE_ANY) {
		get_screen_size(w, h);
		return 0;
	}
	#elif defined(IOS)
	if (o == MODE_ANY && current_gfx_w != -1) {
		*w = current_gfx_w;
		*h = current_gfx_h;
		return 0;
	}
	#endif
	#ifdef _USE_SWROTATE
	if ((desktop_mode = SDL_GetDesktopDisplayMode(SDL_CurrentDisplay))) {
		if ((o == MODE_H && desktop_mode->w < desktop_mode->h) ||
		    (o == MODE_V && desktop_mode->w > desktop_mode->h)) {
			*w = desktop_mode->h;
			*h = desktop_mode->w;
		} else {
			*w = desktop_mode->w;
			*h = desktop_mode->h;
		}
		return 0;
	}
	#endif
	if (o == MODE_ANY && (desktop_mode = SDL_GetDesktopDisplayMode(SDL_CurrentDisplay))) {
		*w = desktop_mode->w;
		*h = desktop_mode->h;
		return 0;
	}

	*w = 0;
	*h = 0;

	if (!vid_modes)
		gfx_modes();

	if (!vid_modes)
		return -1;

	while (!gfx_get_mode(i, &ww, &hh)) {
		if ((ww * hh >= (*w) * (*h))) {
			if (o == MODE_ANY || (o == MODE_H && ww >= hh) ||
				(o == MODE_V && hh > ww)) {
				*w = ww;
				*h = hh;
			}
		}
		i ++;
	}
	if (*w == 0 || *h == 0) /* no suitable mode */
		return -1;
	return 0;
}

int gfx_check_mode(int w, int h)
{
#if defined(IOS) || defined(ANDROID)
	return 0;
#else
	int ww = 0, hh = 0;
	int i = 0;
	if (!vid_modes)
		gfx_modes();

	if (!vid_modes)
		return -1;

	while (!gfx_get_mode(i, &ww, &hh)) {
		if (ww == w && hh == h)
			return 0;
		i ++;
	}
	return -1;
#endif
}

static SDL_Surface *icon = NULL;
extern int software_sw;
extern int glhack_sw;

#ifdef _USE_SWROTATE
static int gfx_flip_rotate = 0;
#endif
SDL_Window *SDL_VideoWindow = NULL;
static SDL_Texture *SDL_VideoTexture = NULL;
static SDL_Surface *SDL_VideoSurface = NULL;
static SDL_Renderer *Renderer = NULL;

static void GetEnvironmentWindowPosition(int w, int h, int *x, int *y)
{
	const char *window = SDL_getenv("SDL_VIDEO_WINDOW_POS");
	const char *center = SDL_getenv("SDL_VIDEO_CENTERED");
	if (window) {
		if (SDL_sscanf(window, "%d,%d", x, y) == 2)
			return;

		if (SDL_strcmp(window, "center") == 0)
			center = window;
	}
	if (center) {
		const SDL_DisplayMode *mode;
		if ((mode = SDL_GetDesktopDisplayMode(SDL_CurrentDisplay))) {
			*x = (mode->w - w) / 2;
			*y = (mode->h - h) / 2;
		}
	}
}

static int mouse_x = -1;
static int mouse_y = -1;

static bool mouse_watcher(void *userdata, SDL_Event *event)
{
#ifdef _USE_SWROTATE
	if (gfx_flip_rotate) {
		switch (event->type) {
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			mouse_y = gfx_height - event->button.x;
			mouse_x = event->button.y;
			event->button.x = mouse_x;
			event->button.y = mouse_y;
			break;
		case SDL_MOUSEMOTION:
			mouse_y = gfx_height - event->motion.x;
			mouse_x = event->motion.y;
			event->motion.x = mouse_x;
			event->motion.y = mouse_y;
			break;
		case SDL_FINGERMOTION:
		case SDL_FINGERUP:
		case SDL_FINGERDOWN:
#ifdef SAILFISHOS /* sailfish has broken touch events */
			mouse_x = event->tfinger.y;
			mouse_y = gfx_height - event->tfinger.x;
#endif
			break;

		default:
			break;
		}
		return 0;
	}
#endif
	switch (event->type) {
	case SDL_EVENT_MOUSE_BUTTON_UP:
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		mouse_x = event->button.x;
		mouse_y = event->button.y;
		break;
	case SDL_EVENT_MOUSE_MOTION:
		mouse_x = event->motion.x;
		mouse_y = event->motion.y;
		break;
	default:
		break;
	}
	return 0;
}

void gfx_real_size(int *ww, int *hh)
{
	int w, h;
	SDL_GetWindowSize(SDL_VideoWindow, &w, &h);
	if (ww)
		*ww = w;
	if (hh)
		*hh = h;
}

void gfx_finger_pos_scale(float x, float y, int *ox, int *oy, int norm)
{
	int xx = 0, yy = 0;
#ifndef SAILFISHOS
	int w, h;
	float sx, sy;
	SDL_Rect rect;

	if (!norm) { /* do not normalize? */
		w = gfx_width;
		h = gfx_height;
		sx = 1.0f;
		sy = 1.0f;
		rect.x = 0;
		rect.y = 0;
	} else {
		SDL_GetWindowSize(SDL_VideoWindow, &w, &h);
		SDL_GetRenderViewport(Renderer, &rect);
		SDL_GetRenderScale(Renderer, &sx, &sy);
	}

	if (sx != 0) {
		x = x * w;
		xx = x / sx - rect.x;
	}
	if (sy != 0) {
		y = y * h;
		yy = y / sy - rect.y;
	}
#else
	xx = (int)x; /* broken touch in SFOS */
	yy = (int)y;
#endif
#ifdef _USE_SWROTATE
	if (gfx_flip_rotate) {
		if (ox)
			*ox = yy;
		if (oy)
			*oy = gfx_height - xx;
	} else {
#endif
		if (ox)
			*ox = xx;
		if (oy)
			*oy = yy;
#ifdef _USE_SWROTATE
	}
#endif
}

#ifdef _USE_SWROTATE
void rotate_landscape(void)
{
	SDL_DisplayMode desktop_mode;
	SDL_GetDesktopDisplayMode(SDL_CurrentDisplay, &desktop_mode);
	gfx_flip_rotate = (desktop_mode.w < desktop_mode.h);
#ifdef SAILFISHOS
	SDL_SetHint(SDL_HINT_QTWAYLAND_CONTENT_ORIENTATION, "landscape");
#endif
}

void rotate_portrait(void)
{
	SDL_DisplayMode desktop_mode;
	SDL_GetDesktopDisplayMode(SDL_CurrentDisplay, &desktop_mode);
	gfx_flip_rotate = (desktop_mode.w > desktop_mode.h);
#ifdef SAILFISHOS
	SDL_SetHint(SDL_HINT_QTWAYLAND_CONTENT_ORIENTATION, "portrait");
#endif
}

void unlock_rotation(void)
{
	gfx_flip_rotate = 0;
#ifdef SAILFISHOS
	SDL_SetHint(SDL_HINT_QTWAYLAND_CONTENT_ORIENTATION, "primary");
#endif
}
#endif

int gfx_set_mode(int w, int h, int fs)
{
	int i;
	int vsync = 1;
	int window_x = SDL_WINDOWPOS_UNDEFINED;
	int window_y = SDL_WINDOWPOS_UNDEFINED;
	int win_w;
	int win_h; int sw_fallback = 0;
	int max_mode_w = 0;
	int max_mode_h = 0;

	const SDL_DisplayMode *desktop_mode;

	char title[4096];
	char *t;

	strcpy(title, "INSTEAD - " );
	strcat(title, VERSION );
	win_w = w * scale_sw; win_h = h * scale_sw;
	gfx_get_max_mode(&max_mode_w, &max_mode_h, MODE_ANY); /* get current window size */
#if defined(IOS) || defined(ANDROID) || defined(SAILFISHOS) || defined(WINRT)
	fs = 1; /* always fs for mobiles */
#endif
	if (fs && !software_sw) {
		win_w = max_mode_w;
		win_h = max_mode_h;
	}
	if (gfx_width == w && gfx_height == h && gfx_fs == fs) {
		game_reset_name();
#if defined(ANDROID)
		if (SDL_VideoWindow) /* see gfx_set_mode call from input.c */
#else
		if (SDL_VideoWindow && !fs)
#endif
			SDL_SetWindowSize(SDL_VideoWindow, win_w, win_h);
		goto done; /* already done */
	}
	SelectVideoDisplay();
	desktop_mode = SDL_GetDesktopDisplayMode(SDL_CurrentDisplay);

	if (vid_modes && vid_modes != std_modes) {
		for (i = 0; vid_modes[i]; i++)
			SDL_free(vid_modes[i]);
		SDL_free(vid_modes);
	}
	vid_modes = NULL;

	if (screen)
		gfx_free_image(screen);

/*	if (SDL_VideoTexture)
		SDL_DestroyTexture(SDL_VideoTexture); */

	if (Renderer)
		SDL_DestroyRenderer(Renderer);

	screen = NULL;
	Renderer = NULL;
	SDL_VideoTexture = NULL;

	if (SDL_VideoWindow) {
		SDL_GetWindowPosition(SDL_VideoWindow, &window_x, &window_y);
		SDL_DestroyWindow(SDL_VideoWindow);
		SDL_VideoWindow = NULL;
		if ((gfx_fs == 1 && !fs) || (window_x == 0 || window_y == 0)) { /* return from fullscreen */
			window_x = SDL_WINDOWPOS_CENTERED;
			window_y = SDL_WINDOWPOS_CENTERED;
		}
	} else
		GetEnvironmentWindowPosition(win_w, win_h, &window_x, &window_y);

	if (desktop_mode->w <= win_w || fs)
		window_x = 0;
	if (desktop_mode->h <= win_h || fs)
		window_y = 0;
	t = game_reset_name();
	if (!t)
		t = title;
	if (glhack_sw) {
		fprintf(stderr, "glhack: %d\n", glhack_sw);
		/* fix for hackish samsung devices */
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, (glhack_sw / 100) % 10);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, (glhack_sw / 10) % 10);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, glhack_sw % 10);
	}
#if defined(IOS) || defined(ANDROID) || defined(SAILFISHOS) || defined(WINRT)
	SDL_VideoWindow = SDL_CreateWindow(t, win_w, win_h,
			SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE
#if defined(ANDROID)
			| SDL_WINDOW_FULLSCREEN_DESKTOP
#endif
			);
	if (!SDL_VideoWindow) {
		fprintf(stderr, "Fallback to software window.\n");
		SDL_VideoWindow = SDL_CreateWindow(t, win_w, win_h,
			SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
	}
#else
	if (!software_sw) /* try to using scale */
		SDL_VideoWindow = SDL_CreateWindow(t, win_w, win_h,
			((fs)?SDL_WINDOW_FULLSCREEN:(resizable_sw?SDL_WINDOW_RESIZABLE:0)) | SDL_WINDOW_OPENGL);
	if (!SDL_VideoWindow) { /* try simple window */
		fprintf(stderr, "Fallback to software window.\n");
		win_w = w; win_h = h;
		SDL_VideoWindow = SDL_CreateWindow(t, win_w, win_h, ((fs)?SDL_WINDOW_FULLSCREEN:0));
	}
#endif
	if (SDL_VideoWindow == NULL) {
		fprintf(stderr, "Unable to create %dx%d window: %s\n", win_w, win_h, SDL_GetError());
		return -1;
	}
	if (icon)
		SDL_SetWindowIcon(SDL_VideoWindow, icon);
// TODO
//	if (SDL_SetWindowDisplayMode(SDL_VideoWindow, (fs)?NULL:&desktop_mode) < 0) {
//		fprintf(stderr, "Unable to set display mode: %s\n", SDL_GetError());
//		/* return -1; */
//	}
	if (!vsync_sw)
		vsync = 0;
retry:
	if (software_sw || !(Renderer = SDL_CreateRenderer(SDL_VideoWindow, NULL))) {
		fprintf(stderr, "Fallback to software renderer.\n");
		sw_fallback = 1;
		if (!(Renderer = SDL_CreateRenderer(SDL_VideoWindow, SDL_SOFTWARE_RENDERER))) {
			fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
			return -1;
		}
	}
	if (vsync)
		SDL_SetRenderVSync(Renderer, 1);
	SDL_VideoTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, w, h);
	if (!SDL_VideoTexture) {
		fprintf(stderr, "Unable to create texture: %s\n", SDL_GetError());
		if (!sw_fallback) { /* one more chance */
			SDL_DestroyRenderer(Renderer);
			software_sw = 1;
			goto retry;
		}
		return -1;
	}
	SDL_VideoSurface = SDL_CreateSurface(w, h, PIXEL_FORMAT);

	if (!SDL_VideoSurface) {
		fprintf(stderr, "Unable to create screen surface: %s\n", SDL_GetError());
		return -1;
	}
    /* Set a default screen palette */
#if 0
	if (SDL_VideoSurface->format->palette) {
/*		SDL_VideoSurface->flags |= SDL_HWPALETTE;
		SDL_DitherColors(SDL_VideoSurface->format->palette->colors,
			SDL_VideoSurface->format->BitsPerPixel);
		SDL_AddPaletteWatch(SDL_VideoSurface->format->palette,
			SDL_VideoPaletteChanged, SDL_VideoSurface); */
		SDL_SetPaletteColors(SDL_VideoSurface->format->palette,
			SDL_VideoSurface->format->palette->colors, 0,
			SDL_VideoSurface->format->palette->ncolors);
	}
#endif
	if (!nocursor_sw)
		SDL_HideCursor();

	gfx_fs = fs;
	gfx_width = w;
	gfx_height = h;
	screen = GFX_IMG_REL(SDL_VideoSurface);
	if (!screen) {
		fprintf(stderr, "Can't alloc screen!\n");
		return -1;
	}

#ifdef _USE_SWROTATE
	if (gfx_flip_rotate)
		SDL_SetRenderLogicalSize(Renderer, h, w);
	else
#endif
		SDL_SetRenderLogicalPresentation(Renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_RemoveEventWatch(mouse_watcher, NULL);
	SDL_AddEventWatch(mouse_watcher, NULL);
	fprintf(stderr, "Video mode: %dx%d (%s)\n", Surf(screen)->w, Surf(screen)->h, SDL_GetRendererName(Renderer));
done:
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderClear(Renderer);
	SDL_RenderPresent(Renderer);
	SDL_FillSurfaceRect(SDL_VideoSurface, NULL, SDL_MapRGB(PIXEL_FORMAT_DETAILS, NULL, 0, 0, 0));
	return 0;
}

int gfx_video_init(void)
{
	if (TTF_Init()) {
		fprintf(stderr, "Can't init TTF subsystem.\n");
		return -1;
	}
#ifndef ICON_PATH
#define ICON_PATH "./icon"
#endif
	icon = IMG_Load( ICON_PATH"/sdl_instead_1.png" );
	return 0;
}

static int queue_x1 = -1;
static int queue_y1 = -1;
static int queue_x2 = -1;
static int queue_y2 = -1;
static int queue_dirty = 0;
static SDL_Texture *cursor = NULL;


static int cursor_xc = 0;
static int cursor_yc = 0;

static int cursor_w;
static int cursor_h;
static int cursor_on = 1;

void gfx_set_cursor(img_t cur, int xc, int yc)
{
	if (cursor)
		SDL_DestroyTexture(cursor);

	if (!cur) {
		cursor = NULL;
		cursor_w = 0;
		cursor_h = 0;
		return;
	}
	cursor = SDL_CreateTextureFromSurface(Renderer, Surf(cur));

	if (!cursor)
		return;

	cursor_w = gfx_img_w(cur);
	cursor_h = gfx_img_h(cur);

	cursor_xc = xc;
	cursor_yc = yc;
	SDL_SetTextureBlendMode(cursor, SDL_BLENDMODE_BLEND);
}

void gfx_show_cursor(int on)
{
	cursor_on = on;
}

static void gfx_render_copy(SDL_Texture *texture, SDL_Rect *dst, int clear)
{
#ifdef _USE_SWROTATE
	SDL_Rect r2;
	SDL_Point r;
	int w, h;
	if (gfx_flip_rotate) {
		if (clear)
			SDL_RenderClear(Renderer);
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		r2.x = 0; r2.y = -h;
		r2.w = w; r2.h = h;
		r.x = 0; r.y = h;
		SDL_RenderTextureEx(Renderer, texture, NULL, &r2, 90, &r, 0);
		return;
	}
#endif
	SDL_RenderTexture(Renderer, texture, NULL, NULL);
// TODO
//	SDL_RenderTexture(Renderer, texture, dst, dst);
}

static void gfx_render_cursor(void)
{
	int cursor_x = 0;
	int cursor_y = 0;

	SDL_FRect rect;
#ifdef _USE_SWROTATE
	SDL_Point r;
#endif
	if (!cursor_on || !mouse_focus() || nocursor_sw)
		return;

	gfx_cursor(&cursor_x, &cursor_y);

#ifdef _USE_SWROTATE
	if (gfx_flip_rotate) {
		int tmp = cursor_x;
		cursor_x = gfx_height - cursor_y;
		cursor_y = tmp;
		r.x = cursor_xc;
		r.y = cursor_yc;
	}
#endif

	cursor_x -= cursor_xc;
	cursor_y -= cursor_yc;

	rect.x = cursor_x;
	rect.y = cursor_y;
	rect.w = cursor_w; /* - 1; */ /* SDL 2.0 hack? */
	rect.h = cursor_h; /* - 1; */
#ifdef _USE_SWROTATE
	if (gfx_flip_rotate)
		SDL_RenderTextureEx(Renderer, cursor, NULL, &rect, 90, &r, 0);
	else
#endif
		SDL_RenderTexture(Renderer, cursor, NULL, &rect);
}

static void SDL_UpdateRect(SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

int SDL_Flip(SDL_Surface * screen)
{
	SDL_Rect rect;
	int pitch, psize;
	unsigned char *pixels;
	if (!screen)
		return 0;
	pitch = screen->pitch;
	psize = 4; //screen->format->BytesPerPixel;
	pixels = screen->pixels;
	if (queue_dirty) {
		rect.x = queue_x1;
		rect.y = queue_y1;
		rect.w = queue_x2 - queue_x1;
		rect.h = queue_y2 - queue_y1;

		pixels += pitch * queue_y1 + queue_x1 * psize;
		SDL_UpdateTexture(SDL_VideoTexture, &rect, pixels, pitch);
		gfx_render_copy(SDL_VideoTexture, &rect, 1);
		SDL_RenderPresent(Renderer);
	}
	queue_x1 = queue_y1 = queue_x2 = queue_y2 = -1;
	queue_dirty = 0;
	return 0;
}

static void SDL_UpdateRect(SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	if (queue_x1 < 0 || x < queue_x1)
		queue_x1 = x;
	if (queue_y1 < 0 || y < queue_y1)
		queue_y1 = y;
	if ((Sint32)(x + w) > queue_x2)
		queue_x2 = (Sint32)(x + w);
	if ((Sint32)(y + h) > queue_y2)
		queue_y2 = (Sint32)(y + h);
	queue_dirty = 1;
}

void gfx_flip(void)
{
	queue_x1 = queue_y1 = 0;
	queue_x2 = gfx_width;
	queue_y2 = gfx_height;
	queue_dirty = 1;
}

void gfx_resize(int w, int h)
{
#if defined(ANDROID) || defined(IOS)
	current_gfx_w = w;
	current_gfx_h = h;
#endif
}

void gfx_commit(void)
{
	SDL_Flip(Surf(screen));
}

void gfx_update(int x, int y, int w, int h) {
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (w < 0 || h < 0)
		return;
	if (x >= gfx_width || y >= gfx_height)
		return;
	if (x + w > gfx_width) {
		w = gfx_width - x;
	}
	if (y + h > gfx_height) {
		h = gfx_height - y;
	}
	SDL_UpdateRect(Surf(screen), x, y, w, h);
}

void gfx_video_done(void)
{
	if (icon)
		SDL_DestroySurface(icon);
	screen = NULL;
	TTF_Quit();
}

img_t gfx_scale(img_t src, float xscale, float yscale, int smooth)
{
	anim_t ag;
	if ((ag = is_anim(src))) {
		int i;
		int err = 0;
		img_t img = NULL;
		anim_t ag2;
		ag2 = anim_clone(ag);

		if (!ag2)
			return NULL;

		for (i = 0; i < ag->nr_frames; i ++) {
			SDL_Surface *s;
			s = (err) ? NULL : zoomSurface(ag->anim->frames[i], xscale, yscale, 1);

			if (!s) {
				err ++;
				ag2->anim->frames[i] = NULL;
				continue;
			}
			ag2->anim->frames[i] = s;
		}
		if (err) {
			anim_free(ag2);
			return NULL;
		}
		anim_add(ag2); /* scaled anim added */
		img = gfx_new_img(ag2->anim->frames[0], IMG_ANIM, ag2, 0);
		if (!img) {
			anim_del(ag2);
			anim_free(ag2);
		}
		return img;
	}
	return GFX_IMG_REL(zoomSurface(Surf(src), xscale, yscale, smooth));
}

img_t gfx_rotate(img_t src, float angle, int smooth)
{
	anim_t ag;

	if ((ag = is_anim(src))) {
		int i;
		int err = 0;
		img_t img = NULL;

		anim_t ag2;

		ag2 = anim_clone(ag);

		if (!ag2)
			return NULL;

		for (i = 0; i < ag->nr_frames; i ++) {
			SDL_Surface *s;
			s = (err) ? NULL : rotozoomSurface(ag->anim->frames[i], angle, 1.0f, smooth);

			if (!s) {
				err ++;
				ag2->anim->frames[i] = NULL;
				continue;
			}
			ag2->anim->frames[i] = s;
		}
		if (err) {
			anim_free(ag2);
			return NULL;
		}
		anim_add(ag2); /* rotated anim added */
		img = gfx_new_img(ag2->anim->frames[0], IMG_ANIM, ag2, 0);
		if (!img) {
			anim_del(ag2);
			anim_free(ag2);
		}
		return img;
	}
	return GFX_IMG_REL(rotozoomSurface(Surf(src), angle, 1.0f, smooth));
}

#define FN_REG  0
#define FN_BOLD  1
#define FN_ITALIC  2
#define FN_ITALICBOLD 3
#define FN_MAX  4
struct fnt {
	TTF_Font *fn;
	TTF_Font *fonts[FN_MAX];
	int style;
};

/* prefix{regular,italic, bold, bolditalic}.ttf */
static int parse_fn(const char *f, char *files[])
{
	int e;
	int nr = 0;
	int elen;
	const char *ep = f;
	const char *s = f;

	int pref = strcspn(f, "{");
	if (!f[pref])
		goto no;
	f += pref + 1;
	ep = f;
	ep += strcspn(f, "}");
	if (!*ep)
		goto no;
	ep ++;
	elen = strlen(ep);
	while (1) {
		f += strspn(f, " \t");
		e = strcspn(f, ",}");
		if (!e) { /* empty subst */
			files[nr] = NULL;
			goto skip;
		}
		files[nr] = malloc(e + pref + elen + 1);
		if (!files[nr])
			break;
		if (pref)
			memcpy(files[nr], s, pref);
		if (e)
			memcpy(files[nr] + pref, f, e);
		if (elen)
			memcpy(files[nr] + pref + e, ep, elen);
		*(files[nr] + pref + e + elen) = 0;
skip:
		nr ++;
		if (!f[e] || f[e] == '}')
			break;
		f += e + 1;
		if (nr >=4)
			break;
	}
	return nr;
no:
	files[0] = strdup(s);
	return (files[0])?1:0;
}

extern int hinting_sw;

fnt_t fnt_load(const char *fname, int size)
{
	TTF_Font *fn;
	struct fnt *h;
	int i, n = 0;
	char *files[4] = { NULL, NULL, NULL, NULL };
	h = malloc(sizeof(struct fnt));
	if (!h)
		return NULL;
	h->fonts[0] = h->fonts[1] = h->fonts[2] = h->fonts[3] = NULL;
	n = parse_fn(fname, files);
	if (!n)
		goto err;
	for (i = 0; i < n; i++) {
		fn = NULL;
		if (!is_empty(files[i])) {
			SDL_IOStream *rw = RWFromIdf(instead_idf(), files[i]);
			if (!rw || !(fn = TTF_OpenFontIO(rw, 1, size))) {
				fprintf(stderr, "Can not load font: '%s'\n", files[i]);
			}
		}
		if (!fn && i == 0) /* no regular */
			goto err;
#ifdef TTF_HINTING_LIGHT
		if (fn) {
			switch (hinting_sw) {
			case 0:
				TTF_SetFontHinting(fn, TTF_HINTING_NORMAL);
				break;
			case 1:
				TTF_SetFontHinting(fn, TTF_HINTING_LIGHT);
				break;
			case 2:
				TTF_SetFontHinting(fn, TTF_HINTING_MONO);
				break;
			case 3:
				TTF_SetFontHinting(fn, TTF_HINTING_NONE);
				break;
			default:
				break;
			}
		}
#endif
		h->fonts[i] = fn;
	}
	h->fn = h->fonts[FN_REG];
	for (i = 0; i < n; i++)
		free(files[i]);
	return (fnt_t) h;
err:
	for (i = 0; i < n; i++)
		free(files[i]);
	fnt_free(h);
	return NULL;
}

void fnt_style(fnt_t fn, int style)
{
	struct fnt *h = (struct fnt*)fn;
	if (!h)
		return;
	h->style = style;
	if ((style & TTF_STYLE_BOLD) && (style & TTF_STYLE_ITALIC)) {
		if (h->fonts[FN_ITALICBOLD]) {
			h->fn = h->fonts[FN_ITALICBOLD];
			style &= ~TTF_STYLE_BOLD;
			style &= ~TTF_STYLE_ITALIC;
		} else
			h->fn = h->fonts[FN_REG];
	} else if ((style & TTF_STYLE_BOLD)) {
		if (h->fonts[FN_BOLD]) {
			h->fn = h->fonts[FN_BOLD];
			style &= ~TTF_STYLE_BOLD;
		} else
			h->fn = h->fonts[FN_REG];
	} else if ((style & TTF_STYLE_ITALIC)) {
		if (h->fonts[FN_ITALIC]) {
			h->fn = h->fonts[FN_ITALIC];
			style &= ~TTF_STYLE_ITALIC;
		} else
			h->fn = h->fonts[FN_REG];
	} else {
		h->fn = h->fonts[FN_REG];
	}
	TTF_SetFontStyle((TTF_Font *)h->fn, style);
}

img_t fnt_render(fnt_t fn, const char *p, color_t col)
{
	SDL_Color scol = { .r = col.r, .g = col.g, .b = col.b };
	struct fnt *h = (struct fnt*)fn;
	if (!h)
		return NULL;
	return GFX_IMG_REL(TTF_RenderText_Blended((TTF_Font *)h->fn, p, 0, scol));
}

int fnt_height(fnt_t fn)
{
	struct fnt *h = (struct fnt*)fn;
	if (!fn)
		return 0;
	return TTF_GetFontHeight((TTF_Font *)(h->fonts[FN_REG]));
}

void fnt_free(fnt_t fnt)
{
	int i;
	struct fnt *h = (struct fnt*)fnt;
	if (!fnt)
		return;
	for (i = 0; i < FN_MAX; i++) {
		if (h->fonts[i])
			TTF_CloseFont((TTF_Font *)h->fonts[i]);
	}
	free(h);
}

void txt_draw(fnt_t fnt, const char *txt, int x, int y, color_t col)
{
	img_t s = fnt_render(fnt, txt, col);
	if (!s)
		return;
	gfx_draw(s, x, y);
}

#if 0
int txt_width(fnt_t fnt, const char *txt)
{
	const char *p = txt;
	int c = 0;
	int w = 0;
	Uint16 u = 0;
	struct fnt *f = (struct fnt*)fnt;
	if (!f)
		return 0;
	while (*p) {
		if (!c) {
			if (! (*p & 0x80)) { /* ascii */
				c = 1;
				u = *p & 0x7f;
			} else {
				if ((*p & 0xe0) == 0xc0) {
					c = 2;
					u = *p & 0x1f;
				} else if ((*p & 0xf0) == 0xe0) {
					c = 3;
					u = *p & 0xf;
				} else if ((*p & 0xf8) == 0xf0) {
					c = 4;
					u = *p & 0x3;
				} else {
					c = 1;
					u = *p & 0x7f; /* fallback */
				}
			}
		} else {
			if ((*p & 0xc0) != 0x80) {
				c = 1;
				u = *p & 0x7f; /* fallback */
			} else {
				u <<= 6;
				u |= *p & 0x3f;
			}
		}
		c --;
		if (!c) {
			int adv = 0;
			TTF_GlyphMetrics(f->fn, u, NULL, NULL, NULL, NULL, &adv);
			w += adv;
		}
		p ++;
	}
	return w;
}
#endif
void txt_size(fnt_t fnt, const char *txt, int *w, int *h)
{
	int ww, hh;
	struct fnt *f = (struct fnt*)fnt;
	TTF_GetStringSize((TTF_Font *)f->fn, txt, 0, &ww, &hh);
	if (w)
		*w = ww;
	if (h)
		*h = hh;
}

struct word;
struct line;
struct xref;

struct word {
	int style;
	int x;
	int w;
	int unbrake;
	int valign;
	int img_align;

	/* Direction and Script (Language) of the word */
	int rtl;
	int script;		/* See HarfBuzz hb_script_t */
	int isalpha;	/* Whether this word contains alphabets */

	char *word;
	img_t	img;
	struct word *next; /* in line */
	struct word *prev; /* in line */
	struct line *line;
	struct xref *xref;
	img_t prerend;
	img_t hlprerend;
};


img_t	word_image(word_t v)
{
	struct word *w = (struct word*)v;
	if (!w)
		return NULL;
	return w->img;
}

#ifdef _USE_HARFBUZZ
static int hb_dir(int rtl)
{
	return rtl ? HB_DIRECTION_RTL:HB_DIRECTION_LTR;
}
#endif

/* This function detects and configures direction, script and type of a word. */
static void word_detect_rtl(struct word *w, int mode)
{
#ifdef _USE_HARFBUZZ
	const char *str = w->word;
	int rc;
	unsigned long sym = 0;
	if (mode == 0) /* force ltr mode */
		return;
	/*	Find the first alphanumeric utf8 character for a meaningful direction
		or use direction of the first character.
	*/
	while ((rc = get_utf8(str, &sym))) {
		if (g_unichar_isalpha(sym))
			break;
		str += rc;
	}
	/* Is this made of alphabets? */
	w->isalpha = g_unichar_isalpha(sym);
	if (mode == 1) { /* force rtl */
		w->rtl = 1;
		w->script = HB_SCRIPT_COMMON;
		return;
	}
	switch(g_unichar_get_script(sym)) {
		case G_UNICODE_SCRIPT_HEBREW:
			w->rtl = !g_unichar_isdigit(sym);
			w->script = HB_SCRIPT_HEBREW;
			/* Fall through */
		case G_UNICODE_SCRIPT_ARABIC:
			w->rtl = !g_unichar_isdigit(sym);
			w->script = HB_SCRIPT_ARABIC;
			break;
		default:
			w->rtl = 0;
			w->script = HB_SCRIPT_COMMON;
	}
#endif
}

struct word *word_new(const char *str)
{
	struct word *w;
	w = malloc(sizeof(struct word));
	if (!w)
		return NULL;
	w->word = strdup(str);
	w->next = NULL;
	w->x = 0;
	w->w = 0;
	w->valign = 0;
	w->line = NULL;
	w->xref = NULL;
	w->style = 0;
	w->img = NULL;
	w->img_align = 0;
	w->unbrake = 0;
	w->prerend = NULL;
	w->hlprerend = NULL;

	w->rtl = 0;
	w->script = 0;
	w->isalpha = 0;

	return w;
}

struct line {
	int x;
	int y;
	int h;
	int w;
	int num;
	int align;
	int pos;
	int	tabx;
	int	al_tabx;
	int	taby;
	int	al_taby;

	/* Each line could be RTL or LTR regardless of its script */
	int rtl;

	struct word *words;
	struct line *next;
	struct line *prev;
	struct layout *layout;
};

static int vertical_align(struct word *w, int *hh);

static int word_pos_x(struct word *word);

int	word_geom(word_t v, int *x, int *y, int *w, int *h)
{
	int xx, yy, ww, hh;
	struct line *line;
	struct word *word = (struct word*)v;
	if (!word || !word->line)
		return -1;
	line = word->line;
	xx = word_pos_x(word);
	ww = word->w;
	yy = line->y;
	yy += vertical_align(v, &hh);
	if (x)
		*x = xx;
	if (y)
		*y = yy;
	if (w)
		*w = ww;
	if (h)
		*h = hh;
	return 0;
}

struct line *line_new(void)
{
	struct line *l;
	l = malloc(sizeof(struct line));
	if (!l)
		return NULL;
	l->words = NULL;
	l->next = NULL;
	l->prev = NULL;
	l->x = 0;
	l->w = 0;
	l->y = 0;
	l->h = 0;
	l->num = 0;
	l->tabx = -1;
	l->al_tabx = ALIGN_LEFT;
	l->taby = -1;
	l->al_taby = ALIGN_BOTTOM;
	l->layout = NULL;
	l->align = 0;
	l->pos = 0;
	l->rtl = 0;
	return l;
}

int line_empty(struct line *line)
{
	struct word *w;
	w = line->words;
	while (w) {
		if (w->img_align) {
			w = w->next;
			continue;
		}
		return 0;
	}
	return 1;
}

static int line_margin(struct line *line)
{
	struct word *w;
	w = line->words;
	while (w) {
		if (w->img_align)
			return 1;
		w = w->next;
	}
	return 0;
}

static struct word *next_word(struct word *w)
{
	while (w->next && w->next->img_align) /* skip margins */
		w = w->next;
	return w->next;
}

void line_justify(struct line *line, int width)
{
	int x = 0;
	int last_margin = 0;
	int last_unbrake = 0;
	struct word *w;
	int sp, spm, lw = 0;
	int lnum = 0;
	if (!line || line->num <= 1 /*|| width <= line->w*/)
		return;
	w = line->words;
	while (w) {
		lw += w->w;
		if (last_margin && w->unbrake)
			w->unbrake = last_unbrake;

		if (!w->unbrake && !w->img_align)
			lnum ++;

		if (!last_margin && w->img_align)
			last_unbrake = w->unbrake;
		last_margin = w->img_align;
		w = w->next;
	}
	if (lnum <=1 )
		return;
	w = line->words;
	sp = (width - lw) / (lnum - 1);
	spm = (width - lw) % (lnum - 1);
	while (w) {
		if (!w->img_align) {
			w->x = x;
			if (next_word(w) && next_word(w)->unbrake)
				x += w->w;
			else {
				x += w->w + sp + ((spm)?1:0);
				if (spm)
					spm --;
			}
		}
		w = w->next;
	}
}

void line_right(struct line *line, int width)
{
	struct word *w;
	int sp;
	if (!line || line->num == 0)
		return;
	sp = width - line->w;
	w = line->words;
	while (w) {
		if (!w->img_align) {
			w->x += sp;
		}
		w = w->next;
	}
}
void line_center(struct line *line, int width)
{
	struct word *w;
	int sp;
	if (!line || line->num == 0)
		return;
	sp = (width - line->w)/2;
	w = line->words;
	while (w) {
		if (!w->img_align) {
			w->x += sp;
		}
		w = w->next;
	}
}

void line_align(struct line *line, int width, int style, int nl)
{
	if (style == ALIGN_JUSTIFY) {
		if (nl)
			return;
		line_justify(line, width);
		return;
	}
	if (style == ALIGN_CENTER) {
		line_center(line, width);
		return;
	}
	if (style == ALIGN_LEFT)
		return;
	if (style == ALIGN_RIGHT) {
		line_right(line, width);
		return;
	}
}

void word_free(struct word *word);

void line_free(struct line *line)
{
	struct word *w;
	if (!line)
		return;
	w = line->words;
	while (w) {
		struct word *ow = w;
		w = w->next;
		word_free(ow);
	}
	free(line);
}

void line_add_word(struct line *l, struct word *word)
{
	struct word *w = l->words;
	l->num ++;
	word->line = l;
	if (!l->words) {
		l->words = word;
		word->prev = word;

		/*	This is the first word in this line. Let's use its direction
			for the line too. Ideally, something like fribidi should be
			used for mixing directions however. */
		l->rtl = word->rtl;
		return;
	}
	w = w->prev;
	w->next = word;
	word->prev = w;
	l->words->prev = word;
	return;
}

struct image;
struct image {
	struct image *next;
	char	*name;
	img_t	image;
	int	free_it;
};

struct image *image_new(const char *name, img_t img)
{
	struct image *g = malloc(sizeof(struct image));
	if (!g)
		return NULL;
	g->image = img;
	g->name = strdup(name);
	g->next = NULL;
	g->free_it = 0;
	return g;
}

void image_free(struct image *image)
{
	if (!image)
		return;
	if (image->name)
		free(image->name);
	if (image->free_it)
		gfx_free_image(image->image);
	free(image);
}

struct textbox;

#define ALIGN_NEST 16
struct margin;

struct margin {
	struct margin *next;
	int w;
	int h;
	int y;
	int align;
	struct word *word;
};

struct margin *margin_new(void)
{
	struct margin *m = malloc(sizeof(struct margin));
	if (!m)
		return NULL;
	m->w = m->h = m->align = 0;
	m->next = NULL;
	return m;
}

void margin_free(struct margin *m)
{
	if (m)
		free(m);
}

struct layout {
	fnt_t	fn;
	float	fn_height;
	color_t	col;
	color_t	lcol;
	color_t	acol;
	struct image *images;
	struct xref *xrefs;
	struct line *lines;
	struct line *anchor;
	struct textbox *box;
	struct margin *margin;
	int w;
	int h;
	int align;
	int valign;
	int saved_align[ALIGN_NEST];
	int saved_valign[ALIGN_NEST];
	int acnt;
	int vcnt;
	int style;
	int rtl;
	int scnt[4];
	int lstyle;
	cache_t img_cache;
	cache_t prerend_cache;
	cache_t hlprerend_cache;
};

struct xref {
	struct	xref *next;
	struct	xref *prev;
	struct	word **words;
	struct	layout *layout;
	char	*link;
	int	num;
	int	active;
};
struct textbox {
	struct 	layout *lay;
	struct line	*line;
	int	off;
	int	w;
	int	h;
};

void word_free(struct word *word)
{
	if (!word)
		return;
/*	if (word->img)
		gfx_free_image(word->img); */
	if (word->word)
		free(word->word);

	if (word->prerend) {
		cache_forget(word->line->layout->prerend_cache, word->prerend);
/*		SDL_DestroySurface(word->prerend); */
	}

	if (word->hlprerend) {
		cache_forget(word->line->layout->hlprerend_cache, word->hlprerend);
/*		SDL_DestroySurface(word->hlprerend); */
	}
	word->hlprerend = word->prerend = NULL;
	free(word);
}

struct xref *xref_new(char *link)
{
	struct xref *p;
	p = malloc(sizeof(struct xref));
	if (!p)
		return NULL;
	if (link)
		p->link = strdup(link);
	else
		p->link = NULL;
	p->num = 0;
	p->layout = NULL;
	p->next = NULL;
	p->prev = NULL;
	p->active = 0;
	p->words = NULL;
	return p;
}

int xref_add_word(struct xref *xref, struct word *word)
{
	struct word **new_words;

	new_words = realloc(xref->words, (xref->num + 1) * sizeof(struct word*));
	if (!new_words)
		return -1;

	xref->words = new_words;
	xref->words[xref->num ++] = word;
	word->xref = xref;

	return 0;
}

void xref_free(struct xref *xref)
{
	if (xref->link)
		free(xref->link);
	if (xref->words)
		free(xref->words);
	free(xref);
}

void layout_add_line(struct layout *layout, struct line *line)
{
	struct line *l = layout->lines;
	line->layout = layout;
	if (!l) {
		layout->lines = line;
		line->prev = line;
		return;
	}
	l = l->prev;
	l->next = line;
	line->prev = l;
	layout->lines->prev = line;
	return;
}

void layout_add_margin(struct layout *layout, struct margin *margin)
{
	struct margin *m = layout->margin;
	if (!m) {
		layout->margin = margin;
		return;
	}
	while (m->next)
		m = m->next;
	m->next = margin;
	return;
}

#if 1
static int layout_skip_margin(struct layout *layout, int y)
{
	struct margin *m = layout->margin;
	int my = y;

	if (!m)
		return y;

	while (m) {
		if (m->y + m->h > my)
			my = m->y + m->h;
		m = m->next;
	}
	if (y < my)
		y = my;
	return y;
}
#endif

static void margin_rebase(struct layout *layout)
{
	struct margin *m = layout->margin;
	if (!m)
		return;

	while (m) {
		m->y = m->word->line->y;
		m = m->next;
	}
}

static int layout_find_margin(struct layout *layout, int y, int *w)
{
	struct margin *m = layout->margin;
	int xpos = 0;
	int rpos = layout->w;
	if (!m) {
		if (w)
			*w = layout->w;
		return 0;
	}
	while (m) {
		if (y >= m->y && y < m->y + m->h) {
			if (m->align == ALIGN_LEFT)
				xpos = (xpos < m->w)?m->w:xpos;
			else
				rpos = (rpos > layout->w - m->w)?layout->w - m->w:rpos;
		}
		m = m->next;
	}
	if (w)
		*w = rpos - xpos;
	return xpos;
}

struct image *_layout_lookup_image(struct layout *layout, const char *name)
{
	struct image *g;
	for (g = layout->images; g; g = g->next) {
		if (!strcmp(g->name, name))
			return g;
	}
	return NULL;
}

img_t txt_layout_images(layout_t lay, void **v)
{
	struct image **g = (struct image **)v;
	struct layout *layout = (struct layout *)lay;

	if (!layout)
		return NULL;

	if (!*v)
		*v = layout->images;
	else
		*v = (*g)->next;
	if (!*v)
		return NULL;
	return (*g)->image;
}

textbox_t txt_layout_box(layout_t lay)
{
	struct layout *layout = (struct layout *)lay;
	return layout->box;
}

void layout_add_image(struct layout *layout, struct image *image)
{
	struct image *g = layout->images;
	if (!g) {
		layout->images = image;
		return;
	}
	while (g->next)
		g = g->next;
	g->next = image;
	return;
}

img_t layout_lookup_image(struct layout *layout, char *name)
{
	struct image *g = _layout_lookup_image(layout, name);
	return (g)?g->image: NULL;
}

void layout_add_xref(struct layout *layout, struct xref *xref)
{
	struct xref *x = layout->xrefs;
	xref->layout = layout;
	if (!x) {
		layout->xrefs = xref;
		xref->prev = xref;
		return;
	}
	x = x->prev;
	x->next = xref;
	xref->prev = x;
	layout->xrefs->prev = xref;
	return;
}

static void sdl_surface_free(void *p)
{
	gfx_free_img((img_t)p);
}

struct layout *layout_new(fnt_t fn, int w, int h)
{
	struct layout *l;
	l = malloc(sizeof(struct layout));
	if (!l)
		return NULL;
	l->lines = NULL;
	l->anchor = NULL;
	l->images = NULL;
	l->w = w;
	l->h = h;
	l->fn = fn;
	l->fn_height = 1.0f;
	l->align = ALIGN_JUSTIFY;
	l->valign = 0;
	l->style = 0;
	l->lstyle = 0;
	l->rtl = 0;
	l->xrefs = NULL;
	l->margin = NULL;
	l->col = gfx_col(0, 0, 0);
	l->lcol = gfx_col(0, 0, 255);
	l->acol = gfx_col(255, 0, 0);
	l->box = NULL;
	l->img_cache = cache_init(0, gfx_cache_free_image);
	l->prerend_cache = cache_init(0, sdl_surface_free);
	l->hlprerend_cache = cache_init(0, sdl_surface_free);
	memset(l->scnt, 0, sizeof(l->scnt));
	memset(l->saved_align, 0, sizeof(l->saved_align));
	memset(l->saved_valign, 0, sizeof(l->saved_valign));
	l->acnt = 0;
	l->vcnt = 0;
	return l;
}
void txt_layout_size(layout_t lay, int *w, int *h)
{
	struct layout *layout = (struct layout *)lay;
	if (!lay)
		return;
	if (w)
		*w = layout->w;
	if (h)
		*h = layout->h;
}

void txt_layout_set_size(layout_t lay, int w, int h)
{
	struct layout *layout = (struct layout *)lay;
	if (!lay)
		return;
	layout->w = w;
	layout->h = h;
}

int txt_layout_add_img(layout_t lay, const char *name, img_t img)
{
	struct layout *layout = (struct layout *)lay;
	struct image *image;
	image = _layout_lookup_image(layout, name);
	if (image) { /* overwrite */
		image->image = img;
		return 0;
	}
	image = image_new(name, img);
	if (!image)
		return -1;
	layout_add_image(layout, image);
	return 0;
}

void _txt_layout_free(layout_t lay)
{
	struct layout *layout = (struct layout *)lay;
	struct line *l;
	struct image *g;
	struct xref *x;
	struct margin *m;
	if (!layout)
		return;
	l = layout->lines;
	while (l) {
		struct line *ol = l;
		l = l->next;
		line_free(ol);
	}
	x = layout->xrefs;
	while (x) {
		struct xref *ox = x;
		x = x->next;
		xref_free(ox);
	}
	m = layout->margin;
	while (m) {
		struct margin *om = m;
		m = m->next;
		margin_free(om);
	}
	g = layout->images;
	while (g) {
		struct image *og = g;
		g = g->next;
		if (!cache_have(layout->img_cache, og->image))
			og->free_it = 0; /* do not free from cache */
		cache_forget(layout->img_cache, og->image);
		image_free(og);
	}
	layout->images = NULL;
	layout->xrefs = NULL;
	layout->lines = NULL;
	layout->anchor = NULL;
	layout->margin = NULL;

	memset(layout->scnt, 0, sizeof(layout->scnt));
	memset(layout->saved_align, 0, sizeof(layout->saved_align));
	memset(layout->saved_valign, 0, sizeof(layout->saved_valign));
	layout->acnt = 0;
	layout->vcnt = 0;
}

word_t txt_layout_words_ex(layout_t lay, struct line *line, word_t v, int off, int height)
{
	struct layout *layout = (struct layout*)lay;
	struct word *w = (struct word*)v;

	if (!lay)
		return NULL;

	if (!w) {
		if (!line)
			line = layout->lines;
		if (!line)
			return NULL;
		w = line->words;
	} else {
		line = w->line;
		w = w->next;
	}

	for (; (!w || (line->y + line->h) < off) && line->next; line = line->next, w = line->words);
	if ((line->y + line->h) < off)
		w = NULL;
	else if (height >= 0 && line->y - off > height)
		w = NULL;
	return w;
}

word_t txt_layout_words(layout_t lay, word_t v)
{
	return txt_layout_words_ex(lay, NULL, v, 0, -1);
}

void txt_layout_free(layout_t lay)
{
	struct layout *layout = (struct layout *)lay;
	_txt_layout_free(lay);
	if (lay) {
		cache_free(layout->img_cache);
		cache_free(layout->prerend_cache);
		cache_free(layout->hlprerend_cache);
		layout->img_cache = NULL;
		layout->prerend_cache = NULL;
		layout->hlprerend_cache = NULL;
		free(lay);
	}
}

#define TOKEN_NONE	0x000
#define	TOKEN_A		0x001
#define TOKEN_B		0x002
#define	TOKEN_I		0x004
#define	TOKEN_U		0x008
#define	TOKEN_S		0x010
#define	TOKEN_C		0x020
#define	TOKEN_R		0x040
#define	TOKEN_J		0x080
#define	TOKEN_L		0x100
#define	TOKEN_T		0x200
#define TOKEN_D		0x400
#define TOKEN_M		0x800
#define TOKEN_X		0x1000
#define TOKEN_Y		0x2000
#define TOKEN_CLOSE	0x4000
#define TOKEN(x)	(x & 0x3fff)

int gfx_get_token(const char *ptr, char **eptr, char **val, int *sp)
{
	int y_token = 0;
	char *ep, *p;
	int closing = 0;
	if (eptr)
		*eptr = NULL;
	p = (char*)ptr;
	ptr += strspn(ptr, " \t");
	if (sp) {
		*sp = 0;
		if (p != ptr)
			*sp = 1;
	}
	if (val)
		*val = NULL;
	if (!*ptr)
		return 0;
	if (*ptr != '<')
		return 0;
	ptr ++;
	if (*ptr == '/') {
		closing = 1;
		if (!ptr[1] || ptr[2] != '>')
			return 0;
		ptr ++;
	}
	switch (*ptr) {
	case 'y':
		y_token = 1;
		/* Fall through */
	case 'x':
		if (ptr[1] != ':')
			return 0;
		ptr += 2;
		ep = find_in_esc(ptr, "\\>");
		if (*ep != '>')
			return 0;
		if (val) {
			p = malloc(ep - ptr + 1);
			if (!p)
				return 0;
			memcpy(p, ptr, ep - ptr);
			p[ep - ptr] = 0;
			*val = p;
		}
		if (eptr)
			*eptr = ep + 1;
		return (y_token)?TOKEN_Y:TOKEN_X;
	case 'a':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_A | TOKEN_CLOSE;
		}
		if (ptr[1] != ':') {
			return 0;
		}
		ptr += 2;
/*		ep = (char*)ptr + strcspn(ptr, ">"); */
		ep = find_in_esc(ptr, "\\>");
		if (!ep || *ep != '>') {
			return 0;
		}
		if (val) {
			p = malloc(ep - ptr + 1);
			if (!p)
				return 0;
			memcpy(p, ptr, ep - ptr);
			p[ep - ptr] = 0;
			parse_esc_string(p, val);
			if (*val)
				free(p);
			else
				*val = p;
		}
		if (eptr)
			*eptr = ep + 1;
		return TOKEN_A;
	case 'b':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_B | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_B;
		}
		break;
	case 'i':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_I | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_I;
		}
		break;
	case 's':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_S | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_S;
		}
		break;
	case 't':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_T | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_T;
		}
		break;
	case 'd':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_D | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_D;
		}
		break;
	case 'm':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_M | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_M;
		}
		break;
	case 'u':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_U | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_U;
		}
		break;
	case 'c':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_C | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_C;
		}
		break;
	case 'r':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_R | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_R;
		}
		break;
	case 'j':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_J | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_J;
		}
		break;
	case 'l':
		if (closing) {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_L | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			if (eptr)
				*eptr = (char*)ptr + 2;
			return TOKEN_L;
		}
		break;
	}
	return 0;
}

static int is_delim(int c)
{
	switch(c) {
	case '.':
	case ',':
	case ':':
	case '!':
	case '+':
	case '-':
	case '?':
	case '/':
		return 1;
	}
	return 0;
}

static int process_word_token(const char *p, char **eptr, char ch)
{
	char *ep;
	if (eptr)
		*eptr = (char*)p;
	if (!p)
		return 0;
	if (p[0] != '<' || p[1] != ch || p[2] != ':')
		return 0;
	p += 3;
	ep = find_in_esc(p, "\\>");
	if (*ep != '>')
		return 0;
	if (eptr)
		*eptr = ep + 1;
	return (ep - p + 1);
}

static int word_img(const char *p, char **eptr)
{
	return process_word_token(p, eptr, 'g');
}

static int word_token(const char *p, char **eptr)
{
	return process_word_token(p, eptr, 'w');
}

static int lookup_cjk(const char *ptr, int limit)
{
	unsigned long sym;
	int off = 0, rc;
	while ((rc = get_utf8(ptr, &sym))) {
		if (is_cjk(sym))
			return off;
		off += rc;
		ptr += rc;
		if (off >= limit)
			break;
	}
	return off;
}

static int cjk_here(const char *ptr)
{
	unsigned long sym;
	int rc;
	rc = get_utf8(ptr, &sym);
	if (is_cjk(sym))
		return rc;
	return 0;
}

static const char *lookup_token_or_sp(const char *ptr)
{
	char *eptr;
	const char *p = ptr;
	while (*p) {
		int cjk, rc;
		rc = strcspn(p, " .,:!+-?/<\t\n");
		cjk = lookup_cjk(p, rc);
		if (p[cjk] && cjk < rc) { /* cjk symbol found! */
			rc = cjk;
			if (!rc)
				rc += get_utf8(p, NULL);
		}
		p += rc;
		if (*p != '<' ) {
			while (is_delim(*p))
				p ++;
/*			if (is_delim(*p))
				p ++; */
			return p;
		}

		if (!gfx_get_token(p, &eptr, NULL, NULL)) {
			if (word_img(p, &eptr)) {
				if (p == ptr) /* first one */
					p = eptr;
				return p;
			} else if (word_token(p, &eptr)) {
				if (p == ptr) /* first one */
					p = eptr;
				return p;
			}
			p ++;
			continue;
		}
		return p;
	}
	return ptr;
}

#define BREAK_NONE 0
#define BREAK_SPACE 1

static char *get_word(const char *ptr, char **eptr, int *sp)
{
	const char *ep;
	char *o;
	size_t sz;
	*eptr = NULL;
	o = (char*)ptr;
	ptr += strspn(ptr, " \t");
	if (sp) {
		*sp = BREAK_NONE;
		if (o != ptr)
			*sp = BREAK_SPACE;
	}
	if (!*ptr)
		return NULL;

	ep = lookup_token_or_sp(ptr);
/*	ep += strcspn(ep, " \t\n"); */
	sz = ep - ptr;
	o = malloc(sz + 1);
	memcpy(o, ptr, sz);
	o[sz] = 0;

	sz = word_img(ptr, eptr);
	if (sz)
		return o;
	sz = word_token(ptr, eptr);
	if (sz)
		return o;
	*eptr = (char*)ep;
	return o;
}

void	layout_debug(struct layout *layout)
{
	struct line *line;
	struct word *word;
	line = layout->lines;
	while (line) {
		printf("%d of %d)", line->y, line->num);
		word = line->words;
		while (word) {
			printf("%d)%s ", word->x, word->word);
			word = word->next;
		}
		printf("\n");
		line = line->next;
	}
}

void txt_layout_color(layout_t lay, color_t fg)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->col = fg;
}

void txt_layout_rtl(layout_t lay, int rtl)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->rtl = rtl;
}

void	txt_layout_font_height(layout_t lay, float height)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->fn_height = height;
}

void txt_layout_link_color(layout_t lay, color_t link)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->lcol = link;
}
void txt_layout_active_color(layout_t lay, color_t link)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->acol = link;
}
void txt_layout_link_style(layout_t lay, int style)
{
	struct layout *layout = (struct layout*)lay;
	if (!lay)
		return;
	layout->lstyle = style;
}

static char *word_cache_string(struct word *w, Uint32 style)
{
	char *p;
	int len = 0;
	len = (w->word)?strlen(w->word):0;
	len += 16;
	p = malloc(len);
	if (!p)
		return NULL;
	snprintf(p, len, "%s-%08x", (w->word)?w->word:"", style);
	return p;
}

static void word_render(struct layout *layout, struct word *word, int x, int y)
{
	char *wc = NULL;
	img_t s;
	img_t prerend = NULL;
	img_t hlprerend = NULL;
	color_t fgcol = { .r = layout->col.r, .g = layout->col.g, .b = layout->col.b };
	color_t lcol = { .r = layout->lcol.r, .g = layout->lcol.g, .b = layout->lcol.b };
	color_t acol = { .r = layout->acol.r, .g = layout->acol.g, .b = layout->acol.b };
	Uint32 style;

	if (!word->xref) {
		style = (fgcol.r << 24) + (fgcol.g << 16) + (fgcol.b << 8);
	} else if (word->xref->active) {
		style = (acol.r << 24) + (acol.g << 16) + (acol.b << 8);
	} else {
		style = (lcol.r << 24) + (lcol.g << 16) + (lcol.b << 8);
	}

	if (word->xref && !word->style) {
		fnt_style(layout->fn, layout->lstyle);
		wc = word_cache_string(word, layout->lstyle | style);
	} else {
		fnt_style(layout->fn, word->style);
		wc = word_cache_string(word, word->style | style);
	}
	if (!wc)
		return;
#ifdef _USE_HARFBUZZ
	/* Set the language and script for SDL_ttf */
	TTF_SetDirection(hb_dir(word->rtl));
	TTF_SetScript(word->script);
#endif
	if (!word->xref) {
		if (!word->prerend) {
			prerend = cache_get(layout->prerend_cache, wc);
			if (!prerend) {
				word->prerend = fnt_render(layout->fn, word->word, fgcol);
				word->prerend = gfx_display_alpha(word->prerend);
				cache_add(layout->prerend_cache, wc, word->prerend);
			} else {
				word->prerend = prerend;
			}
		}
		s = word->prerend;
	} else if (word->xref->active) {
		if (!word->hlprerend) {
			hlprerend = cache_get(layout->hlprerend_cache, wc);
			if (!hlprerend) {
				word->hlprerend = fnt_render(layout->fn, word->word, acol);
				word->hlprerend = gfx_display_alpha(word->hlprerend);
				cache_add(layout->hlprerend_cache, wc, word->hlprerend);
			} else {
				word->hlprerend = hlprerend;
			}
		}
		s = word->hlprerend;
	} else {
		if (!word->prerend) {
			prerend = cache_get(layout->prerend_cache, wc);
			if (!prerend) {
				word->prerend = fnt_render(layout->fn, word->word, lcol);
				word->prerend = gfx_display_alpha(word->prerend);
				cache_add(layout->prerend_cache, wc, word->prerend);
			} else {
				word->prerend = prerend;
			}
		}
		s = word->prerend;
	}
	free(wc);
#ifdef _USE_HARFBUZZ
	/* Drop to defaults */
	TTF_SetDirection(HB_DIRECTION_LTR);
	TTF_SetScript(HB_SCRIPT_COMMON);
#endif
	if (!s)
		return;
	gfx_draw(s, x, y);
}

fnt_t txt_layout_font(layout_t lay)
{
	struct layout *layout = lay;
	if (!lay)
		return NULL;
	return layout->fn;
}

static int vertical_align(struct word *w, int *hh)
{
	int h;
	struct line *line = w->line;
	struct layout *layout = line->layout;
	if (w->img)
		h = gfx_img_h(w->img);
	else
		h = fnt_height(layout->fn);
	if (hh)
		*hh = h;

	if (w->img && w->img_align)
		return 0;

	if (w->valign == ALIGN_TOP)
		return 0;
	else if (w->valign == ALIGN_BOTTOM)
		return line->h - h;
	return (line->h - h) / 2;
}

/* relative position in line */
static int line_pos(struct line *line, int x)
{
	if (!line->rtl)
		return x;
	return line->layout->w - line->x - x;
}

/* absolute position of word */
static int word_pos_x(struct word *word)
{
	struct line *line = word->line;
	struct layout *layout = line->layout;
	if (word->img && word->img_align) {
		if (line->rtl)
			return layout->w - word->x - gfx_img_w(word->img);
		return word->x;
	}
	if (!line->rtl) /* fast path */
		return line->x + word->x;
	if (word->img)
		return layout->w - line->x - word->x - gfx_img_w(word->img);
	return layout->w - line->x - word->x - word->w;
}

static void word_image_render(struct word *word, int x, int y, clear_fn clear, update_fn update)
{
	struct line *line = word->line;
	struct layout *layout = line->layout;
	int yy; int posx = word_pos_x(word);

	if (clear && !word->xref)
		return;
	yy = vertical_align(word, NULL);
	if (clear) {
		if (word->img) {
			clear(x + posx, y + line->y + yy, gfx_img_w(word->img), gfx_img_h(word->img));
		} else
			clear(x + posx, y + line->y/* + yy*/, word->w, line->h);
	}
	if (word->img) {
		/* We have an image to draw */
		gfx_draw(word->img, x + posx, y + line->y + yy);
		if (update)
			update(x + posx, y + line->y + yy, gfx_img_w(word->img), gfx_img_h(word->img));
	} else {
		word_render(layout, word, x + posx, y + yy + line->y);
		if (update)
			update(x + posx, y + line->y + yy, word->w, line->h);
	}
}

void xref_update(xref_t pxref, int x, int y, clear_fn clear, update_fn update)
{
	int i;
	struct xref *xref = (struct xref*)pxref;
	struct layout *layout;
	struct word *word;
	if (!xref)
		return;

	layout = xref->layout;
	if (layout->box) {
		gfx_clip(x, y, layout->box->w, layout->box->h);
		y -= (layout->box)->off;
	}

	for (i = 0; i < xref->num; i ++) {
		word = xref->words[i];
		if (!word->img_align)
			word_image_render(word, x, y, clear, update);
	}
	gfx_noclip();
}

/* Draws everything */
void txt_layout_draw_ex(layout_t lay, struct line *line, int x, int y, int off, int height, clear_fn clear)
{
	void *v;
	img_t img;
	struct layout *layout = (struct layout*)lay;
	struct margin *margin;
	struct word *word;
/*	line = layout->lines;
	gfx_clip(x, y, layout->w, layout->h); */
	if (!lay)
		return;
	for (v = NULL; (img = txt_layout_images(lay, &v)); )
		gfx_dispose_anim(img);

	for (margin = layout->margin; margin; margin = margin->next) {
		if (margin->y + margin->h < off)
			continue;
		if (margin->y - off > height)
			continue;
		word_image_render(margin->word, x, y, clear, NULL);
	}
	if (!line)
		line = layout->lines;
	for (; line; line= line->next) {
		if ((line->y + line->h) < off)
			continue;
		if (line->y - off > height)
			break;
		for (word = line->words; word; word = word->next ) {
			if (!word->img_align)
				word_image_render(word, x, y, clear, NULL);
		}
	}
	cache_shrink(layout->prerend_cache);
	cache_shrink(layout->hlprerend_cache);
	cache_shrink(layout->img_cache);
/*	gfx_noclip(); */
}

void txt_layout_draw(layout_t lay, int x, int y)
{
	struct layout *layout = (struct layout*)lay;
	txt_layout_draw_ex(lay, NULL, x, y, 0, layout->h, 0);
}


textbox_t txt_box(int w, int h)
{
	struct textbox *box;
	box = malloc(sizeof(struct textbox));
	if (!box)
		return NULL;
	box->lay = NULL; /* (struct layout*)lay; */
	box->w = w;
	box->h = h;
	box->off = 0;
	box->line = NULL; /* (box->lay)->lines; */
	return box;
}


void txt_box_norm(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	int off;
	if (!tbox || !box->lay)
		return;
	off = box->off;
	box->line = box->lay->lines;
	for (line = box->line; line; line = line->next) {
		if (off < line->h)
			break;
		off -= line->h;
		box->line = line;
	}
}

layout_t txt_box_layout(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!box)
		return NULL;
	return box->lay;
}

void txt_box_set(textbox_t tbox, layout_t lay)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!box)
		return;
	box->lay = (struct layout*)lay;
	box->off = 0;
	if (lay)
		box->lay->box = box;
	txt_box_norm(tbox);
}

void txt_box_resize(textbox_t tbox, int w, int h)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	if (w < 0)
		w = 0;
	if (h < 0)
		h = 0;
	box->w = w;
	box->h = h;
	txt_box_norm(tbox);
}

void txt_box_size(textbox_t tbox, int *w, int *h)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	if (w)
		*w = box->w;
	if (h)
		*h = box->h;
}

void txt_box_scroll_next(textbox_t tbox, int disp)
{
	int off, h;
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	if (!tbox)
		return;
	line = box->line;
	if (!line)
		return;

	txt_box_real_size(box, NULL, &h);

	if (h - box->off < box->h)
		return;

	off = h - box->off - box->h;
	if (disp > off)
		disp = off;

	off = box->off - line->y; /* offset from cur line */
	off += disp; /* needed offset */

	while (line->next && off >= line->next->y - line->y) {
		off -= (line->next->y - line->y);
		line = line->next;
	}
	box->line = line;
	box->off = line->y + off;
}

void txt_box_scroll_prev(textbox_t tbox, int disp)
{
	int off;
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	struct layout *l;
	if (!tbox)
		return;
	l = box->lay;
	line = box->line;
	if (!line)
		return;
	off = box->off - line->y; /* offset from cur line */
	off -= disp; /* offset from current line */

	while (line != l->lines && off < 0) {
		line = line->prev;
		off += (line->next->y - line->y);
	}

	box->line = line;
	box->off = line->y + off;

	if (box->off <0)
		box->off = 0;
}

void txt_box_scroll(textbox_t tbox, int disp)
{
	if (!tbox)
		return;
	if (disp >0) {
		txt_box_scroll_next(tbox, disp);
		return;
	}
	else if (disp <0) {
		txt_box_scroll_prev(tbox, -disp);
		return;
	}
}

void txt_box_next_line(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	if (!box || !box->lay)
		return;
	line = box->line;
	if (!line)
		return;
/*	txt_box_norm(tbox);	*/
	if (box->lay->h - box->off < box->h)
		return;
	line = line->next;
	if (line) {
		box->off = line->y;
		box->line = line;
	}
}

void txt_box_prev_line(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	struct layout *l;
	if (!box || !box->lay)
		return;
	l = box->lay;
	line = box->line;
	if (!line)
		return;
	if (line != l->lines) {
		line = line->prev;
		box->line = line;
		box->off = line->y;
	} else
		box->off = 0;
}

int	txt_box_off(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!box)
		return -1;
	return box->off;
}

void txt_box_next(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	if (!tbox)
		return;
	line = box->line;
	if (!line)
		return;
	for (; line; line = line->next) {
		if ((line->y + line->h - box->off) >= box->h)
			break;
	}
	if (line) {
		box->off += (line->y - box->off);
		box->line = line;
	}
}

void txt_box_prev(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct layout *lay;
	struct line  *line;
	if (!tbox)
		return;
	lay = box->lay;
	if (!lay)
		return;
	line = box->line;
	if (!line)
		return;
	for (; line != lay->lines; line = line->prev) {
		if ((box->off - line->y) >= box->h)
			break;
	}
	if (line == lay->lines) {
		box->off = 0;
		box->line = lay->lines;
		return;
	}
	box->off = line->y;
	box->line = line;
}

xref_t txt_box_xrefs(textbox_t tbox)
{
	struct textbox *box = (struct textbox*)tbox;
	struct xref *xref = NULL;
	struct word *word = NULL;
	struct line *line;
	if (!tbox)
		return NULL;
	for (line = box->line; line; line = line->next) {
		if (line->y < box->off)
			continue; /* too high */
		if (line->y + line->h > box->h + box->off)
			break; /* bottom */
		for (word = line->words; word; word = word->next) {
			xref = word->xref;
			if (!xref || word->img_align)
				continue;
			return xref;
		}
	}
	return xref;
}

xref_t txt_box_xref(textbox_t tbox, int x, int y)
{
	struct textbox *box = (struct textbox*)tbox;
	struct xref *xref = NULL;
	struct word *word = NULL;
	struct line *line;
	if (!tbox)
		return NULL;
	y += box->off;
	if (x < 0)
		return NULL;
	if (y < 0)
		return NULL;
	if (x >= box->w)
		return NULL;

	/* Process each word in each line */
	for (line = box->line; line; line = line->next) {
		int hh, yy, lx;
		if (y < line->y)
			break;
		if (y > line->y + line->h)
			continue;
		lx = line_pos(line, x);
		for (word = line->words; word; word = word->next) {
			yy = vertical_align(word, &hh);
			if (y < line->y + yy || y > line->y + yy + hh)
				continue;
			if (lx < line->x + word->x)
				continue;
			xref = word->xref;

			/* Go back. Found nothing. */
			if (!xref)
				continue;
			if (lx < line->x + word->x + word->w)
					break;
			if (word->next && word->next->xref == xref && lx < line->x + word->next->x + word->next->w) {
				yy = vertical_align(word->next, &hh);
				if (y < line->y + yy || y > line->y + yy + hh)
					continue;
				break;
			}
		}
	}
	if (word && xref) {
		/* We found a highlighted word. */
		return xref;
	}
	return NULL;
}

void txt_box_free(textbox_t tbox)
{
	if (!tbox)
		return;
	free(tbox);
}

img_t txt_box_render(textbox_t tbox)
{
	img_t old_screen;
	img_t		dst;
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return NULL;
	dst = gfx_new(box->w, box->h);
	if (!dst)
		return NULL;
	old_screen = screen;
	screen = dst;
	gfx_clear(0, 0, box->w, box->h);
/*	gfx_clip(0, 0, box->w, box->h);
	printf("line: %d\n", box->line->y); */
	txt_layout_draw_ex(box->lay, box->line, 0, - box->off, box->off, box->h, NULL);
/*	gfx_noclip(); */
	screen = old_screen;
	return dst;
}

/* Draws game content */
void txt_box_draw(textbox_t tbox, int x, int y)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	gfx_clip(x, y, box->w, box->h);
/*	printf("line: %d\n", box->line->y); */
	txt_layout_draw_ex(box->lay, box->line, x, y - box->off, box->off, box->h, NULL);
	gfx_noclip();
}

void txt_box_update_links(textbox_t tbox, int x, int y, clear_fn clear)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	gfx_clip(x, y, box->w, box->h);
/*	printf("line: %d\n", box->line->y); */
	txt_layout_draw_ex(box->lay, box->line, x, y - box->off, box->off, box->h, clear);
	gfx_noclip();
}


void txt_layout_update_links(layout_t layout, int x, int y, clear_fn clear)
{
	struct layout *lay = (struct layout *)layout;
/*	gfx_clip(x, y, box->w, box->h);
	printf("line: %d\n", box->line->y); */
	txt_layout_draw_ex(lay, lay->lines, x, y, 0, lay->h, clear);
/*	gfx_noclip(); */
}

img_t get_img(struct layout *layout, char *p, int *al)
{
	int len;
	int align;
	img_t img;
	struct image *image;
	int escaped = 0;
	*al = 0;
	len = word_img(p, NULL);
	if (!len)
		return NULL;
	p += 3;
	p[len - 1] = 0;
	align = strcspn(p, "|");
	if (!p[align])
		align = 0;
	else {
		if (!strcmp(p + align + 1, "left"))
			*al = ALIGN_LEFT;
		else if (!strcmp(p + align + 1, "right"))
			*al = ALIGN_RIGHT;
		if (*al) {
			p[align] = 0;
			if (align && p[align - 1] == '\\') {
				p[align - 1] = 0;
				escaped = 1;
			}
		}
	}
	img = layout_lookup_image(layout, p);
	if (img)
		goto out;
	img = cache_get(layout->img_cache, p);
	if (!img) {
		unix_path(p);
		if (!(img = gfx_load_image(p))) {
			game_res_err_msg(p, debug_sw);
			goto out;
		}
		theme_img_scale(&img); /* bad style, no gfx layer :( */
	}
	image = image_new(p, img);
	if (!image) {
		gfx_free_image(img);
		img = NULL;
	} else {
		layout_add_image(layout, image);
		image->free_it = 1; /* free on layout destroy */
/*		if (gfx_img_w(img) <= GFX_MAX_CACHED_W && gfx_img_h(img) <= GFX_MAX_CACHED_H) */
			cache_add(layout->img_cache, p, img);
	}
out:
	if (align) {
		p[align] = '|';
		if (escaped)
			p[align - 1] = '\\';
	}
	p[len - 1] = '>';
	if (!img)
		*al = 0;
	return img;
}

static char *get_word_token(char *p, int *token)
{
	int len;
	char *r;
	char *val = NULL;
	len = word_token(p, NULL);
	if (token)
		*token = 0;
	if (!len)
		return p;
	if (token)
		*token = 1;
	p[len - 1 + 3] = 0;
	r = strdup((p + 3));
	parse_esc_string(r, &val);
	free(p);
	if (val) {
		free(r);
		r = val;
	}
	return r;
}

char *process_token(char *ptr, struct layout *layout, struct line *line, struct xref **xref, int *sp)
{

	int token;
	char *val = NULL;
	int bit = 0;
	int al = 0;
	int *cnt = NULL;
	char *eptr;

	token = gfx_get_token(ptr, &eptr, &val, sp);
	if (!token)
		return NULL;
	if (TOKEN(token) == TOKEN_B) {
		cnt = &layout->scnt[0];
		bit = TTF_STYLE_BOLD;
	} else if (TOKEN(token) == TOKEN_I) {
		cnt = &layout->scnt[1];
		bit = TTF_STYLE_ITALIC;
	} else if (TOKEN(token) == TOKEN_U) {
		cnt = &layout->scnt[2];
		bit = TTF_STYLE_UNDERLINE;
	} else if (TOKEN(token) == TOKEN_S) {
		cnt = &layout->scnt[3];
#ifdef TTF_STYLE_STRIKETHROUGH
		bit = TTF_STYLE_STRIKETHROUGH;
#else
		bit = TTF_STYLE_ITALIC;
#endif
	}

	if (bit) {
		if (token & TOKEN_CLOSE) {
			-- (*cnt);
			if (*cnt < 0) /* fuzzy */
				*cnt = 0;
			if (!*cnt)
				layout->style &= ~bit;
		} else {
			++ (*cnt);
			layout->style |= bit;
		}
		goto out;
	}

	if (TOKEN(token) == TOKEN_L)
		al = ALIGN_LEFT;
	else if (TOKEN(token) == TOKEN_R)
		al = ALIGN_RIGHT;
	else if (TOKEN(token) == TOKEN_C)
		al = ALIGN_CENTER;
	else if (TOKEN(token) == TOKEN_J)
		al = ALIGN_JUSTIFY;

	if (al) {
		if (token & TOKEN_CLOSE)  {
			layout->acnt --;
			if (layout->acnt <0)
				layout->acnt = 0;
			layout->align = layout->saved_align[layout->acnt];
		} else {
			layout->saved_align[layout->acnt] = layout->align;
			layout->acnt ++;
			if (layout->acnt >= ALIGN_NEST)
				layout->acnt = ALIGN_NEST - 1;
			layout->align = al;
			line->align = al;
		}
		goto out;
	}

	al = 0;

	if (TOKEN(token) == TOKEN_T)
		al = ALIGN_TOP;
	else if (TOKEN(token) == TOKEN_D)
		al = ALIGN_BOTTOM;
	else if (TOKEN(token) == TOKEN_M)
		al = ALIGN_MIDDLE;

	if (al) {
		if (token & TOKEN_CLOSE)  {
			layout->vcnt --;
			if (layout->vcnt <0)
				layout->vcnt = 0;
			layout->valign = layout->saved_valign[layout->vcnt];
		} else {
			layout->saved_valign[layout->vcnt] = layout->valign;
			layout->vcnt ++;
			if (layout->vcnt >= ALIGN_NEST)
				layout->vcnt = ALIGN_NEST - 1;
			layout->valign = al;
		}
		goto out;
	}
	if (TOKEN(token) == TOKEN_X || TOKEN(token) == TOKEN_Y) {
		int pos;
		pos = atoi(val) * game_theme.scale;
		if (strchr(val, '%') && sscanf(val, "%d%%", &pos) == 1) {
			if (TOKEN(token) == TOKEN_Y) {
				if (layout->box)
					pos = layout->box->h * pos / 100;
			} else
				pos = layout->w * pos / 100;
		}
		if (TOKEN(token) == TOKEN_X) {
			line->tabx = pos;
			line->al_tabx = ALIGN_LEFT;
			if (strstr(val, "right"))
				line->al_tabx = ALIGN_RIGHT;
			else if (strstr(val, "center"))
				line->al_tabx = ALIGN_CENTER;
		} else {
			line->taby = pos;
			line->al_taby = ALIGN_BOTTOM;
			if (strstr(val, "top"))
				line->al_taby = ALIGN_TOP;
			else if (strstr(val, "middle"))
				line->al_taby = ALIGN_MIDDLE;
		}
		goto out;
	}
	if (TOKEN(token) == TOKEN_A) {
		if (token & TOKEN_CLOSE) {
			if (*xref)
				layout_add_xref(layout, *xref);
			*xref = NULL;
		} else {
			if (*xref) {
				eptr = NULL;
				goto out;
			}
			if (!strcmp(val, "#")) { /* jump anchor */
				layout->anchor = line;
			} else {
				*xref = xref_new(val);
			}
		}
	}
out:
	if (val)
		free(val);

	return eptr;
}

int get_unbreakable_len(struct layout *layout, const char *ptr)
{
	int w = 0;
	int ww = 0;
	char *p, *eptr;
	while (ptr && *ptr) {
		int sp;
		while (gfx_get_token(ptr, &eptr, NULL, &sp)) {
			if (sp)
				return w;
			ptr = eptr;
		}
		p = get_word(ptr, &eptr, &sp);
		if (!p)
			return w;

		if (sp || !*p || cjk_here(p) || word_img(p, NULL) || (word_token(p, NULL) > 1)) {
			free(p);
			return w;
		}

		txt_size(layout->fn, p, &ww, NULL);

		ptr = eptr;
		w += ww;
		if (!*p)
			ptr ++;
		else if (is_delim(*(ptr - 1))) {
			free(p);
			break;
		}
		free(p);
	}
	return w;
}

int txt_layout_pos2off(layout_t lay, int pos, int *hh)
{
	int off = 0;
	struct line *line;
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return 0;
	for (line = layout->lines; line && (line->pos <= pos); line = line->next) {
		off = line->y; /* + line->h; */
		if (hh)
			*hh = line->h;
	}
	return off;
}

int txt_layout_anchor(layout_t lay, int *hh)
{
	int off;
	struct line *line;
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return -1;
	line = layout->anchor;
	if (!line)
		return -1;
	off = line->y; /* + line->h; */
	if (hh)
		*hh = line->h;
	return off;
}

static void line_y(layout_t lay, struct line *line)
{
	int y = line->taby;
	if (line->taby < 0)
		return;
	if (line->al_taby == ALIGN_BOTTOM) {
		y -= line->h;
	} else if (line->al_taby == ALIGN_MIDDLE)
		y -= line->h/2;
	line->taby = -1;
	if (y > line->y)
		line->y = y;
	margin_rebase(lay);
}

static void word_x(struct line *line, struct word *word, int width)
{
	if (line->tabx < 0)
		return;
	word->x = line->tabx - line->x;
	if (line->al_tabx == ALIGN_RIGHT)
		word->x -= word->w;
	else if (line->al_tabx == ALIGN_CENTER)
		word->x -= word->w/2;

	if (word->x + word->w > width)
		word->x = width - word->w;
	if (word->x < line->w)
		word->x = line->w;
	else
		line->w = word->x;
	line->tabx = -1;
	line->align = ALIGN_LEFT;
}

#ifdef _USE_HARFBUZZ
static void layout_lines_dir(struct layout *layout)
{
	/*	Set direction of each line based on the first non-image,
		alphabet word in that line. */
	struct word *word = NULL;
	struct line *ln = NULL;
	for (ln = layout->lines; ln; ln = ln->next) {
		for (word = ln->words; word; word = word->next ) {
			/* Continue until we get a word with some text (not letters or symbols) */
			if (!word->isalpha)
				continue;

			if (!word->img) {
				ln->rtl = word->rtl;
				break;
			}
		}
	}
}
#endif

void _txt_layout_add(layout_t lay, char *txt)
{
	int sp = 0;
	int saved_style;
	int width;
	int img_align;
	struct margin *m;

	struct line *line, *lastline = NULL;
	struct layout *layout = (struct layout*)lay;
	char *p, *eptr;
	char *ptr = txt;
	struct xref *xref = NULL;
	int w = 0, h = 0, nl = 0;
	int spw;
	img_t img = NULL;
	if (!layout || !layout->fn)
		return;
	saved_style = layout->style;
	fnt_style(layout->fn, 0);
	txt_size(layout->fn, " ", &spw, NULL);

	if (layout->lines) {
		lastline = layout->lines->prev;
		lastline->pos = 0;
	}

	if (!lastline) {
		line = line_new();
		if (!line)
			goto err;
		line->h = h;
		line->align = layout->align;
	} else {
		line = lastline;
	}

	line->x = layout_find_margin(layout, line->y, &width);

	while (ptr && *ptr) {
		struct word *word;
		int sp2, addlen = 0;
		int wtok;
		eptr = process_token(ptr, layout, line, &xref, &sp2);
		if (eptr) {
			ptr = eptr;
			if (xref && layout->style == saved_style)
				fnt_style(layout->fn, layout->lstyle); /* & ~TTF_STYLE_ITALIC); */
			else
				fnt_style(layout->fn, layout->style);/* & ~TTF_STYLE_ITALIC); */

			if (!ptr || !*ptr)
				break;
			if (sp2)
				sp = -1;
			continue;
		}
		if (sp == -1) {
			p = get_word(ptr, &eptr, NULL);
			sp = 1;
		} else
			p = get_word(ptr, &eptr, &sp);

		if (!p)
			break;
		img = get_img(layout, p, &img_align);
		if (!img_align) /* margins reset */
			addlen = get_unbreakable_len(layout, eptr);

		wtok = 0;
		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
			if (img_align) {
				if (!line_margin(line)) {
					line->y = layout_skip_margin(layout, line->y);
					width = layout->w;
					line->x = 0;
				}
				if (width - w <= 0)
					img_align = 0;
			}
		} else {
			p = get_word_token(p, &wtok);
			if (wtok && *p == 0)
				sp = 1;
#ifdef _USE_HARFBUZZ
			/* Correct size depends on the script and direction.
			   Set them correctly before calling txt_size */
			word = word_new(p);
			if (word) {
				word_detect_rtl(word, layout->rtl);
				TTF_SetDirection(hb_dir(word->rtl));
				TTF_SetScript(word->script);
				txt_size(layout->fn, p, &w, &h);
				TTF_SetDirection(HB_DIRECTION_LTR);
				TTF_SetScript(HB_SCRIPT_COMMON);
				free(word);
			}
#else
			txt_size(layout->fn, p, &w, &h);
#endif
			h *= layout->fn_height;
		}
		nl = (wtok)?0:!*p;

		if (!line->h && !img_align && line_empty(line)) /* first word ? */
			line->h = h;

		if (img_align && !line->w)
			line->h = 0;
#if 0
		if (!nl) {
			int ww = width - (line->w + ((sp && line->w)?spw:0) + addlen);
			p = word_hyphen(layout, p, ww, &eptr, &w);
		}
#endif
		if ((line->num && (line->w + ((sp && line->w)?spw:0) + w + addlen) > width) || nl) {
			struct line *ol = line;
			h = 0; /* reset h for new line */
			if ((layout->h) && (line->y + line->h) >= layout->h)
				break;
			if (line != lastline) {
				layout_add_line(layout, line);
			}
			line_y(layout, line);
			line_align(line, width, line->align, nl);

			line = line_new();
			if (!line) {
				free(p);
				goto err;
			}
			line->align = layout->align;
			line->h = 0;/* h; */
			line->y = ol->y + ol->h;

/*			line->x = 0; */
			line->x = layout_find_margin(layout, line->y, &width);
/*			fprintf(stderr,"%d %d\n", line->x, width); */
			if (nl) {
				ptr = eptr + 1;
			}
			free(p);
/*			ptr = eptr; */
			line->pos = (int)(ptr - txt);
			continue;
		}


		if (h > line->h && !img_align)
			line->h = h;

		word = word_new(p);
		if (!word) {
			line_free(line);
			free(p);
			goto err;
		}
		word_detect_rtl(word, layout->rtl);
		word->valign = layout->valign;
		if (!sp && !line_empty(line))
			word->unbrake = 1;

		word->style = layout->style;

		if (line->w && !word->unbrake)
			line->w += spw;

		word->w = w;
		word->x = line->w;

		word_x(line, word, width);

		word->img = img;
		word->img_align = img_align;

		if (img_align && (m = margin_new())) {
			int x2, w2;

			x2 = layout_find_margin(layout, line->y, &w2);

			if (img_align == ALIGN_LEFT) {
				line->x += w;
				m->w = x2 + w;
			}
			else
				m->w = layout->w - x2 - w2 + w;
/*			fprintf(stderr,"w: %d %d %d\n", width, w, width - w); */
			width -= w;
			m->h = h;
			m->y = line->y;
			m->align = img_align;
			m->word = word;
			word->w = 0;
			if (img_align == ALIGN_LEFT)
				word->x = x2;
			else
				word->x = x2 + w2 - w;
			h = 0;
			w = 0;
			layout_add_margin(layout, m);
		}

/*		if (line->w)
			w += spw; */

		line_add_word(line, word);

		if (xref)
			xref_add_word(xref, word);

		line->w += w;

		if (nl)
			eptr ++;
		ptr = eptr;
		free(p);
	}
#ifdef _USE_HARFBUZZ
	layout_lines_dir(layout);
#endif
	if (layout->h == 0)
		layout->h = line->y + line->h;

/*	if (line->num) { */
		if (line != lastline) {
			layout_add_line(layout, line);
		}
		line_y(layout, line);
		line_align(line, width, line->align, nl);
/*	} else
		line_free(line); */
	if (xref)
		layout_add_xref(layout, xref);
	layout->style = saved_style;
	return;
err:
	txt_layout_free(layout);
	return;
}

void txt_layout_add(layout_t lay, char *txt)
{
	struct layout *layout = (struct layout*)lay;
	if (layout)
		layout->h = 0;
	_txt_layout_add(lay, txt);
}

xref_t	xref_next(xref_t x)
{
	if (!x)
		return NULL;
	return ((struct xref*)x)->next;
}

xref_t	xref_prev(xref_t x)
{
	struct layout *l;
	if (!x)
		return NULL;
	l = ((struct xref*)x)->layout;
	if (x == l->xrefs) /* last one */
		return NULL;
	x = ((struct xref*)x)->prev;
	return x;
}

xref_t	txt_layout_xrefs(layout_t lay)
{
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return NULL;
	return layout->xrefs;
}

int xref_valid(xref_t x)
{
	struct xref *xref = (struct xref*)x;
	if (!xref)
		return -1;
	if (!xref->num)
		return -1;
	return 0;
}

int xref_position(xref_t x, int *xc, int *yc)
{
	int i;
	int w = 0;
	struct line *line = NULL;
	struct word *word = NULL;
	struct xref *xref = (struct xref*)x;

	if (xref_valid(x))
		return -1;

	for (i = 0; i < xref->num; i ++) {
		word = xref->words[i];
		if (!word->img_align)
			w += word->w;
	}

	w = w/2;

	for (i = 0; i < xref->num; i ++) {
		word = xref->words[i];
		if (word->img_align)
			continue;
		line = word->line;
		w -= word->w;
		if (w < 0)
			break;
	}

	if (!line || !word)
		return -1;

	if (xc)
		*xc = line_pos(line, line->x + word->x + word->w + w);
	if (yc)
		*yc = line->y + line->h / 2;
	return 0;
}
xref_t txt_layout_xref(layout_t lay, int x, int y)
{
	struct layout *layout = (struct layout*)lay;
	struct xref *xref;
	struct word *word;
	struct line *line;
	int i;
	if (!lay || x < 0 || y < 0)
		return NULL;
	for (xref = layout->xrefs; xref; xref = xref->next) {
		for (i = 0; i < xref->num; i ++) {
			int hh, yy, lx;
			word = xref->words[i];
			line = word->line;
			lx = line_pos(line, x);
			if (word->img_align)
				continue;
			if (y < line->y || y > line->y + line->h)
				continue;
			yy = vertical_align(word, &hh);
			if (y < line->y + yy || y > line->y + yy + hh)
				continue;
			if (lx < line->x + word->x)
				continue;
			if (lx <= line->x + word->x + word->w)
				return xref;
			if (word->next && word->next->xref == xref && lx < line->x + word->next->x + word->next->w) {
				yy = vertical_align(word->next, &hh);
				if (y < line->y + yy || y > line->y + yy + hh)
					continue;
				return xref;
			}
		}
	}
	return NULL;
}

layout_t xref_layout(xref_t x)
{
	struct xref *xref = (struct xref*)x;
	if (!xref)
		return NULL;
	return xref->layout;
}

char	*xref_get_text(xref_t x)
{
	struct xref *xref = (struct xref*)x;
	if (!xref)
		return NULL;
	return xref->link;
}

void xref_set_active(xref_t x, int val)
{
	struct xref *xref = (struct xref*)x;
	if (!xref)
		return;
	xref->active = val;
}

int xref_get_active(xref_t x)
{
	struct xref *xref = (struct xref*)x;
	if (!xref)
		return 0;
	return xref->active;
}


layout_t txt_layout(fnt_t fn, int align, int width, int height)
{
	struct layout *layout;
	layout = layout_new(fn, width, height);
	if (!layout)
		return NULL;
	layout->align = align;
/*	_txt_layout_add(layout, txt); */
	return layout;
}

void txt_layout_set(layout_t lay, char *txt)
{
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return;
	layout->h = 0;
	_txt_layout_free(lay);
	_txt_layout_add(lay, txt);
}
void _txt_layout_real_size(layout_t lay, struct line *line, int *pw, int *ph)
{
	int w = 0;
	int h = 0;
	struct margin *margin;
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return;
	if (!line)
		line = layout->lines;
	for ( ; line; line = line->next) {
		while (!line->num && line->next)
			line = line->next;
		if (line->w > w)
			w = line->w;
		if (line->num && line->y + line->h > h)
			h = line->y + line->h;
	}

	for (margin = layout->margin; margin; margin = margin->next) {
		if (margin->y + margin->h > h)
			h = margin->y + margin->h;
	}

	if (pw)
		*pw = w;
	if (ph)
		*ph = h;
}


void txt_layout_real_size(layout_t lay, int *pw, int *ph)
{
	_txt_layout_real_size(lay, NULL, pw, ph);
}

void txt_box_real_size(textbox_t box, int *pw, int *ph)
{
	if (!box)
		return;
	if (pw)
		_txt_layout_real_size(txt_box_layout(box), NULL, pw, ph);
	else {/* faster path */
		struct line *line = ((struct layout*)txt_box_layout(box))->lines;
		struct line *lines = line;
		if (lines)
			line = lines->prev;
		for (; line != lines && !line->num; line = line->prev);
		_txt_layout_real_size(txt_box_layout(box), line, NULL, ph);
	}
}

int gfx_cursor(int *xp, int *yp)
{
	int x, y;
	x = mouse_x;
	y = mouse_y;
	if (xp)
		*xp = x;
	if (yp)
		*yp = y;
	return SDL_GetMouseState(NULL, NULL);
}

void gfx_warp_cursor(int x, int y)
{
	float sx, sy;
	SDL_Rect rect;
#ifdef _USE_SWROTATE
	if (gfx_flip_rotate) {  /* TODO? */
		int tmp;
		tmp = y;
		y = x;
		x = gfx_height - tmp;
	}
#endif
	SDL_GetRenderViewport(Renderer, &rect);
	SDL_GetRenderScale(Renderer, &sx, &sy);
	x = (x + rect.x) * sx;
	y = (y + rect.y) * sy;
	SDL_WarpMouseInWindow(SDL_VideoWindow, x, y);
}

static int ALPHA_STEPS = 4;
static volatile int   fade_step_nr = -1;

int gfx_fading(void)
{
	return (fade_step_nr != -1);
}

static img_t	fade_bg = NULL;
static img_t	fade_fg = NULL;
static void *fade_aux = NULL;
static void (*fade_cb)(void *) = NULL;
static SDL_TimerID	fade_timer;
static long gfx_change_nr = 0;

static SDL_Texture *fade_bg_texture = NULL;
static SDL_Texture *fade_fg_texture = NULL;

static void update_gfx(void)
{
	img_t img = fade_fg;
	if (fade_step_nr == -1 || !img || !fade_bg || !fade_fg)
		return;
	gfx_render_copy(fade_bg_texture, NULL, 1);
	SDL_SetTextureAlphaMod(fade_fg_texture, (SDL_ALPHA_OPAQUE * (fade_step_nr + 1)) / ALPHA_STEPS);
	gfx_render_copy(fade_fg_texture, NULL, 0);
	if (game_cursor_show)
		gfx_render_cursor();
	SDL_RenderPresent(Renderer);

	fade_step_nr ++;
	if (fade_step_nr == ALPHA_STEPS)
		fade_step_nr = -1;
}

static void gfx_change_screen_step(void *aux)
{
	gfx_change_nr --;
	if (gfx_fading()) {
		update_gfx();
	}
	if (gfx_fading())
		return;
	gfx_cancel_change_screen();
	return;
}

static Uint32 update(void *aux, SDL_TimerID timerID, Uint32 interval)
{
	if (!gfx_fading())
		return 0;
	if (gfx_change_nr > 0)
		return interval;
	gfx_change_nr ++;
	push_user_event(gfx_change_screen_step, NULL);
	return interval;
}

void gfx_cancel_change_screen(void)
{
	if (!fade_bg)
		return;

	fade_step_nr = -1;

	SDL_RemoveTimer(fade_timer);
	SDL_DestroyTexture(fade_fg_texture);
	SDL_DestroyTexture(fade_bg_texture);
	game_cursor(CURSOR_CLEAR);
	gfx_copy(fade_fg, 0, 0);
	game_cursor(CURSOR_ON);
	gfx_flip();
	gfx_commit();
	gfx_free_image(fade_bg);
	fade_bg = NULL;
	if (fade_cb)
		fade_cb(fade_aux);
}

void gfx_change_screen(img_t src, int steps, void (*callback)(void *), void *aux)
{
	struct inp_event ev;
	if (steps <= 1 || !opt_fading) {
		gfx_copy(src, 0, 0);
		game_cursor(CURSOR_ON);
		gfx_flip();
		if (callback)
			callback(aux);
		return;
	}
	gfx_change_nr = 0;
	fade_fg = NULL;
	fade_aux = aux;
	fade_cb = callback;
	fade_bg = gfx_grab_screen(0, 0, gfx_width, gfx_height);

	if (!fade_bg) /* ok, i like kernel logic. No memory, but we must work! */
		return;

	fade_fg = src;

	fade_bg_texture = SDL_CreateTextureFromSurface(Renderer, Surf(fade_bg));
	if (!fade_bg_texture)
		goto err;
	SDL_SetTextureAlphaMod(fade_bg_texture, SDL_ALPHA_OPAQUE);
	SDL_SetTextureBlendMode(fade_bg_texture, SDL_BLENDMODE_NONE);
	fade_fg_texture = SDL_CreateTextureFromSurface(Renderer, Surf(src));
	if (!fade_fg_texture)
		goto err2;
	SDL_SetTextureBlendMode(fade_fg_texture, SDL_BLENDMODE_BLEND);

	memset(&ev, 0, sizeof(ev));
	ALPHA_STEPS = steps;
	fade_step_nr = 0;
	fade_timer = SDL_AddTimer(60, update, NULL);
	return;
err2:
	SDL_DestroyTexture(fade_bg_texture);
err:
	gfx_free_image(fade_bg);
	fade_bg = NULL;
	return;
}

int gfx_init(void)
{
	if (render_sw)
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, render_sw);
#if defined(_WIN32) /* do not use buggy D3D by default: fullscreen problem with NVidia */
	else if (!software_sw)
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return -1;
	}
	if (!(images = cache_init(-1, gfx_cache_free_image))) {
		fprintf(stderr, "Can't init cache subsystem.\n");
		gfx_done();
		return -1;
	}
/*	SDL_DisableScreenSaver(); */
	return 0;
}

void gfx_done(void)
{
	if (screen)
		gfx_free_image(screen);
/*	if (SDL_VideoTexture)
		SDL_DestroyTexture(SDL_VideoTexture); */
	if (Renderer)
		SDL_DestroyRenderer(Renderer);
	if (SDL_VideoWindow)
		SDL_DestroyWindow(SDL_VideoWindow);

	cache_free(images);
	images = NULL;
	SDL_Quit();
}

gtimer_t gfx_add_timer(int delay, int (*fn)(int, void*), void *aux)
{
	return (gtimer_t)SDL_AddTimer(delay, (SDL_TimerCallback)fn, aux);
}

void gfx_del_timer(gtimer_t han)
{
	if (han)
		SDL_RemoveTimer((SDL_TimerID)han);
}

unsigned long gfx_ticks(void)
{
	return SDL_GetTicks();
}

int gfx_pending(void)
{
	return queue_dirty;
}

int gfx_set_title(const char *title)
{
	char stitle[4096];
	if (!title) {
		strcpy( stitle, "INSTEAD - " );
		strcat( stitle, VERSION );
		title = stitle;
	}
	if (SDL_VideoWindow)
		SDL_SetWindowTitle(SDL_VideoWindow, title);
	return 0;
}

int gfx_set_icon(img_t ic)
{
	if (SDL_VideoWindow) {
		if (ic)
			SDL_SetWindowIcon(SDL_VideoWindow, Surf(ic));
		else if (icon)
			SDL_SetWindowIcon(SDL_VideoWindow, icon);
	}
	return 0;
}

float gfx_get_dpi(void)
{
	if (dpi_sw > 0)
		return (float)dpi_sw;
	return (float)(SDL_GetWindowDisplayScale(SDL_VideoWindow) * 96.0f);
}
