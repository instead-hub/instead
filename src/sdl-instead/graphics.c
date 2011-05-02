#include "externals.h"
#include "internals.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mutex.h>
#include "SDL_rotozoom.h"
#include "SDL_gfxBlitFunc.h"
#include "SDL_anigif.h"
#include "sdl_idf.h"

#define Surf(p) ((SDL_Surface *)p)

static SDL_Surface *screen = NULL;
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
	{"darkturquoise", 0x00ced1},
	{"darkviolet", 0x9400d3},
	{"deeppink", 0xff1493},
	{"deepskyblue", 0x00bfff},
	{"dimgray", 0x696969},
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
	{"lightgrey", 0xd3d3d3},
	{"lightgreen", 0x90ee90},
	{"lightpink", 0xffb6c1},
	{"lightsalmon", 0xffa07a},
	{"lightseagreen", 0x20b2aa},
	{"lightskyblue", 0x87cefa},
	{"lightslateblue", 0x8470ff},
	{"lightslategray", 0x778899},
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

int gfx_parse_color (
	const char *spec,
	color_t *def)
{
	int n, i;
	int r, g, b;
	char c;

        if (!spec)
		return -1;

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
	    n <<= 2;
//	    n = 16 - n;
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

struct _anigif_t;

struct agspawn {
	SDL_Rect clip;
	img_t	bg;	
	int x;
	int y;
};
#define AGSPAWN_BLOCK 8
struct _anigif_t {
	struct _anigif_t *next;
	struct _anigif_t *prev;
	int	cur_frame;
	int	nr_frames;
	int	loop;
	int 	drawn;
	int	active;
	int	delay;
	int	spawn_nr;
	struct	agspawn *spawn;
	AG_Frame frames[0];
};

typedef struct _anigif_t *anigif_t;
extern int timer_counter;

static int anigif_spawn(anigif_t ag, int x, int y, int w, int h)
{
	int nr;
	SDL_Rect clip;
	SDL_GetClipRect(screen, &clip);
	//gfx_free_image(ag->bg);
	if (!ag->spawn && !(ag->spawn = malloc(AGSPAWN_BLOCK * sizeof(struct agspawn))))
		return -1;
	nr = ag->spawn_nr + 1;
	if (!(nr % AGSPAWN_BLOCK)) { /* grow */
		void *p = realloc(ag->spawn, AGSPAWN_BLOCK * sizeof(struct agspawn) * 
						((nr / AGSPAWN_BLOCK) + 1));	
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

static anigif_t anim_gifs = NULL;

static int anigif_drawn_nr = 0;

static anigif_t anigif_find(anigif_t g)
{
	anigif_t p;
	for (p = anim_gifs; p; p = p->next) {
		if (p == g)
			return p;
	}
	return NULL;
}

static void anigif_disposal(anigif_t g)
{
	SDL_Rect dest;
	SDL_Rect clip;
	int i = 0;
	img_t	*img = NULL;
	AG_Frame *frame;
	frame = &g->frames[g->cur_frame];
	SDL_GetClipRect(screen, &clip);
	
	dest.x = 0; //g->x;
	dest.y = 0; //g->y; 

	switch (frame->disposal) {
	case AG_DISPOSE_NA:
	case AG_DISPOSE_NONE: /* just show next frame */
		break;
	case AG_DISPOSE_RESTORE_BACKGROUND:
//		img = g->bg;
//		dest.w = Surf(img)->w; 
//		dest.h = Surf(img)->h;
		break;
	case AG_DISPOSE_RESTORE_PREVIOUS:
		if (g->cur_frame) {
			img = (img_t*)(g->frames[g->cur_frame - 1].surface);
			dest.w = g->frames[g->cur_frame - 1].surface->w; 
			dest.h = g->frames[g->cur_frame - 1].surface->h;
			dest.x += g->frames[g->cur_frame - 1].x; 
			dest.y += g->frames[g->cur_frame - 1].y;
		}
		break;
	}
	for (i = 0; i < g->spawn_nr; i++) {
		SDL_Rect dst;
		SDL_SetClipRect(screen, &g->spawn[i].clip);
		dst = dest;

		dst.x += g->spawn[i].x;
		dst.y += g->spawn[i].y;
		if (frame->disposal == AG_DISPOSE_RESTORE_BACKGROUND) {
			img = g->spawn[i].bg;
			dst.w = Surf(img)->w; 
			dst.h = Surf(img)->h;
		}
		if (img) { /* draw bg */
			SDL_BlitSurface(Surf(img), NULL, screen, &dst);
		}
	}
	SDL_SetClipRect(screen, &clip);
}

static void anigif_frame(anigif_t g)
{
	int i;
	SDL_Rect dest;
	SDL_Rect clip;

	AG_Frame *frame;
	frame = &g->frames[g->cur_frame];
	SDL_GetClipRect(screen, &clip);

	dest.w = frame->surface->w; 
	dest.h = frame->surface->h;

	for (i = 0; i < g->spawn_nr; i++) {	
		dest.x = g->spawn[i].x + frame->x;
		dest.y = g->spawn[i].y + frame->y;
		SDL_SetClipRect(screen, &g->spawn[i].clip);
		SDL_BlitSurface(frame->surface, NULL, screen, &dest);
	}
	g->delay = timer_counter;
	SDL_SetClipRect(screen, &clip);
}	

static anigif_t is_anigif(img_t img)
{
	anigif_t p;
	for (p = anim_gifs; p; p = p->next) {
		if (p->frames[0].surface == img)
			return p;
	}
	return NULL;
}

static anigif_t anigif_add(anigif_t g)
{
	anigif_t p;
	p = anigif_find(g);
	if (p) {
		return p;
	}
	if (!anim_gifs)	{
		anim_gifs = g;
		g->next = NULL;
		g->prev = NULL;
		return g;
	}
	for (p = anim_gifs; p && p->next; p = p->next);
	p->next = g;
	g->next = NULL;
	g->prev = p;
	return g;
}

static anigif_t anigif_del(anigif_t g)
{
	if (g->prev == NULL)
		anim_gifs = g->next;
	else
		g->prev->next = g->next;
	if (g->next)
		g->next->prev = g->prev;
	return g;
}

static void anigif_free_spawn(anigif_t g)
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

static void anigif_free(anigif_t g)
{
	AG_FreeSurfaces(g->frames, g->nr_frames);
	anigif_free_spawn(g);
	free(g);
}


void gfx_free_image(img_t p)
{
	anigif_t ag;
	if (!p)
		return;
	if (!cache_forget(images, p))
		return; /* cached sprite */
	if ((ag = is_anigif(p))) {
		if (ag->drawn)
			anigif_drawn_nr --;
		anigif_del(ag);
		anigif_free(ag);
		return;
	}
	SDL_FreeSurface((SDL_Surface *)p);
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

void gfx_noclip(void)
{
	SDL_SetClipRect(screen, NULL);
}

void gfx_getclip(int *x, int *y, int *w, int *h)
{
	SDL_Rect clip;
	if (!screen)
		return;
	SDL_GetClipRect(screen, &clip);
	if (x)
		*x = clip.x;
	if (y)
		*y = clip.y;
	if (w)
		*w = clip.w;
	if (h)
		*h = clip.h;
}

void gfx_clip(int x, int y, int w, int h)
{
	SDL_Rect src;
	src.x = x;
	src.y = y;
	src.w = w;
	src.h = h;
	SDL_SetClipRect(screen, &src);
}

img_t 	gfx_new(int w, int h)
{
	SDL_Surface *dst;
	if (!screen) {
		Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif
		dst = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 
			32,
			rmask, 
			gmask, 
			bmask, 
			amask);	
	} else {
		dst = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 
			screen->format->BitsPerPixel, 
			screen->format->Rmask, 
			screen->format->Gmask, 
			screen->format->Bmask, 
			screen->format->Amask);
	}
	return dst;
}

void	gfx_img_fill(img_t img, int x, int y, int w, int h, color_t col)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y; 
	dest.w = w; 
	dest.h = h;
	SDL_FillRect(img, &dest, SDL_MapRGB(((SDL_Surface*)img)->format, col.r, col.g, col.b));
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
	SDL_Surface *img = SDL_CreateRGBSurface(screen->flags, w, h, screen->format->BitsPerPixel, 
			screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if (!img)
		return NULL;	
	src.x = x;
	src.y = y;
	src.w = w;
	src.h = h;
	dst.x = 0;
	dst.y = 0;
	dst.w = w;
	dst.h = h;	
	SDL_BlitSurface(screen, &src, img, &dst);
	return img;
}

img_t gfx_display_alpha(img_t src)
{
	SDL_Surface* res;
	if (!src)
		return NULL;
	if (!screen)
		return src;
	if (is_anigif(Surf(src))) /* already optimized */
		return src;
	res = SDL_DisplayFormatAlpha(Surf(src));
	if (!res)
		return src;
	gfx_free_image(src);
	return res;
}

int gfx_get_pixel(img_t src, int x, int y,  color_t *color)
{
	Uint8 r, g, b, a;
	Uint32 col = 0;
	Uint8 *ptr;
	int	bpp;
	SDL_Surface *img = Surf(src);
	if (!img)
		return -1;

	if (x >= img->w || y >= img->h || x < 0 || y < 0)
		return -1;

	if (SDL_LockSurface(img))
		return -1;

	if (img->format)
		bpp = img->format->BytesPerPixel;
	else
		bpp = 1; /* hack? */

	ptr = (Uint8*)img->pixels;
	ptr += img->pitch * y;
	ptr += x * bpp;

	memcpy(&col, ptr, bpp);

	SDL_UnlockSurface(img);	
	if (color)
		SDL_GetRGBA(col, img->format, &r, &g, &b, &a);

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
	int bpp;
	Uint32 col;
	Uint8 *ptr;
	SDL_Surface *img = Surf(src);
	if (!img)
		return -1;

	if (x >= img->w || y >= img->h || x < 0 || y < 0)
		return -1;

	if (SDL_LockSurface(img))
		return -1;

	if (img->format)
		bpp = img->format->BytesPerPixel;
	else
		bpp = 1; /* hack? */

	ptr = (Uint8*)img->pixels;
	ptr += img->pitch * y;
	ptr += x * bpp;
	col = SDL_MapRGBA(img->format, color.r, color.g, color.b, color.a);
	memcpy(ptr, &col, bpp);

	SDL_UnlockSurface(img);
	return 0;
}

img_t gfx_alpha_img(img_t src, int alpha)
{
	Uint8 *ptr;
	Uint32 col;
	int size;
	int bpp;

	SDL_Surface *img;
	if (screen)
		img = SDL_DisplayFormatAlpha((SDL_Surface*)src);
	else
		img = gfx_new(Surf(src)->w, Surf(src)->h);
	if (!img)
		return NULL;

	if (img->format)
		bpp = img->format->BytesPerPixel;
	else
		bpp = 1;

	SDL_SetAlpha(img, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

	if (SDL_LockSurface(img) == 0) {
		int w = img->w;
		ptr = (Uint8*)img->pixels;
		size = img->w * img->h;		
		while (size --) {
			Uint8 r, g, b, a;
			memcpy(&col, ptr, bpp);
			SDL_GetRGBA(col, img->format, &r, &g, &b, &a);
			col = SDL_MapRGBA(img->format, r, g, b, a * alpha / 255);
			memcpy(ptr, &col, bpp);
			ptr += bpp;
			w --;
			if (!w) {
				w = img->w;
				ptr += img->pitch;
				ptr -= w * bpp;
			}
		}
		SDL_UnlockSurface(img);
	}
	return img;
}

void	gfx_set_alpha(img_t src, int alpha)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_SetSurfaceAlphaMod((SDL_Surface *)src, alpha);
	if (alpha == 0xff)
		SDL_SetSurfaceBlendMode((SDL_Surface *)src, SDL_BLENDMODE_NONE);
	else
		SDL_SetSurfaceBlendMode((SDL_Surface *)src, SDL_BLENDMODE_BLEND);
#else
	SDL_SetAlpha((SDL_Surface *)src, SDL_SRCALPHA, alpha);
#endif
}

void	gfx_unset_alpha(img_t src)
{
	SDL_SetAlpha((SDL_Surface *)src, 0, SDL_ALPHA_OPAQUE);
}

img_t gfx_combine(img_t src, img_t dst)
{
	img_t new;
	new = SDL_DisplayFormatAlpha(dst);
	if (!new)
		return NULL;
	SDL_BlitSurface((SDL_Surface *)src, NULL, (SDL_Surface *)new, NULL);
	return new;	
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
	SDL_gfxBlitRGBA(img, NULL, img2, &to);
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
//		filename += 4;
		img2 = cache_get(images, filename);
//		fprintf(stderr, "get:%s %p\n", filename, img2);
		goto out;
	} else if (!strncmp(filename, "box:", 4)) {
		filename += 4;
		alpha = 255;
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

static img_t _gfx_load_image(char *filename, int combined)
{
	SDL_RWops *rw;
	SDL_Surface *img;
	int nr = 0;
	filename = strip(filename);
	img = _gfx_load_special_image(filename, combined);
	if (img)
		return img;
	if (strstr(filename,".gif") || strstr(filename,".GIF"))
		nr = AG_LoadGIF(filename, NULL, 0, NULL);
	if (nr > 1) { /* anigif logic */
		int loop = 0;
		anigif_t agif = malloc(sizeof(struct _anigif_t) + nr * sizeof(AG_Frame));
		if (!agif)
			return NULL;
		memset(agif, 0, sizeof(struct _anigif_t) + nr * sizeof(AG_Frame));
		AG_LoadGIF(filename, agif->frames, nr, &loop);
		AG_NormalizeSurfacesToDisplayFormat( agif->frames, nr);
		agif->loop = loop;
		agif->nr_frames = nr;
		anigif_add(agif);
//		fprintf(stderr, "anigif: %s %p\n", filename, agif->frames[0].surface);
		return agif->frames[0].surface;
	}
	rw = RWFromIdf(game_idf, filename);

	if (!rw || !(img = IMG_Load_RW(rw, 1)))
		return NULL;

	if (img->format->BitsPerPixel == 32) { /* hack for 32 bit BMP :( */
		SDL_RWops *rwop;
		rwop = RWFromIdf(game_idf, filename);
		if (rwop) {
			if (IMG_isBMP(rwop))
				SDL_SetAlpha(img, 0, SDL_ALPHA_OPAQUE);
			SDL_RWclose(rwop);
		}
	}
	img = gfx_display_alpha(img);
	return img;
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
		if (img) {
			to.x = x; to.y = y;
			if (c) {
				to.x -= gfx_img_w(img) / 2;
				to.y -= gfx_img_h(img) / 2;
			}
			to.w = to.h = 0;
			SDL_gfxBlitRGBA(img, NULL, base, &to);
			gfx_free_image(img);
		}
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
	if (!img)
		fprintf(stderr, "Can not load image: '%s'\n", filename);
	return img;
}

void gfx_draw_bg(img_t p, int x, int y, int width, int height)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Rect dest, src;
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = x;
	dest.y = y; 
	dest.w = width; 
	dest.h = height;
	SDL_BlitSurface(pixbuf, &src, screen, &dest);
}

void gfx_draw_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Surface *scr = (SDL_Surface *)to;
	SDL_Rect dest, src;
	if (!scr)
		scr = screen;
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

void gfx_copy_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Surface *scr = (SDL_Surface *)to;
	SDL_Rect dest, src;
	if (!scr)
		scr = screen;
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = xx;
	dest.y = yy; 
	dest.w = width; 
	dest.h = height;
	gfx_unset_alpha(pixbuf);
	SDL_BlitSurface(pixbuf, &src, scr, &dest);
	gfx_set_alpha(pixbuf, 255);
}

void gfx_draw(img_t p, int x, int y)
{
	anigif_t ag;
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Rect dest;
	dest.x = x;
	dest.y = y; 
	dest.w = pixbuf->w; 
	dest.h = pixbuf->h;
	if (!DIRECT_MODE) /* no gifs in direct mode */
		ag = is_anigif(pixbuf);
	else
		ag = NULL;
	if (ag) {
		anigif_spawn(ag, x, y, dest.w, dest.h);
		if (!ag->drawn)
			anigif_drawn_nr ++;
		ag->drawn = 1;
		ag->active = 1;
		anigif_frame(ag);
		return;
	}
	SDL_BlitSurface(pixbuf, NULL, screen, &dest);
}

void gfx_stop_gif(img_t p)
{
	anigif_t ag;
	ag = is_anigif(p);
	if (ag)
		ag->active = 0;

}

void gfx_dispose_gif(img_t p)
{
	anigif_t ag;
	ag = is_anigif(p);
	if (ag) {
		if (ag->drawn)
			anigif_drawn_nr --;
		ag->drawn = 0;
		anigif_free_spawn(ag);
	}
}

void gfx_start_gif(img_t p)
{
	anigif_t ag;
	ag = is_anigif(p);
	if (ag)
		ag->active = 1;
}

int gfx_frame_gif(img_t img)
{
	anigif_t ag;
	ag = is_anigif(img);

	if (!ag)
		return 0;

	if (!ag->drawn || !ag->active)
		return 0;
	if (ag->loop == -1)
		return 0;

	if ((timer_counter - ag->delay) < (ag->frames[ag->cur_frame].delay / HZ))
		return 0;

	if (ag->cur_frame != ag->nr_frames - 1 || ag->loop > 1 || !ag->loop)
		anigif_disposal(ag);
		
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
	if (ag->loop != -1)
		anigif_frame(ag);

	return 1;
}

int gfx_is_drawn_gifs(void)
{
	return anigif_drawn_nr;
}

void gfx_update_gif(img_t img)
{
	int i = 0;
	anigif_t ag;
	ag = is_anigif(img);
	if (!ag)
		return;
	if (!ag->drawn || !ag->active)
		return;
	for (i = 0; i < ag->spawn_nr; i++) {
		gfx_update(ag->spawn[i].x, ag->spawn[i].y, 
			gfx_img_w(img), gfx_img_h(img));
	}
}

void gfx_draw_wh(img_t p, int x, int y, int w, int h)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Rect dest, src;
	src.x = 0;
	src.y = 0; 
	src.w = w; 
	src.h = h;
	dest.x = x;
	dest.y = y; 
	dest.w = w; 
	dest.h = h;
	SDL_BlitSurface(pixbuf, &src, screen, &dest);
}
static SDL_Color bgcol = { .r = 0, .g = 0, .b = 0 };

void gfx_bg(color_t col)
{
	bgcol.r = col.r;
	bgcol.g = col.g;
	bgcol.b = col.b;
}

void gfx_clear(int x, int y, int w, int h)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y; 
	dest.w = w; 
	dest.h = h;
	SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, bgcol.r, bgcol.g, bgcol.b));
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
int gfx_modes(void)
{
	int i = 0;
	SDL_Rect** modes;
#ifdef __APPLE__
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_SWSURFACE | SDL_ANYFORMAT);
#else
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT);
#endif
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

int gfx_get_max_mode(int *w, int *h)
{
#ifdef MAEMO
	*w = 800;
	*h = 480;
#else
	int ww = 0, hh = 0;
	int i = 0;
	*w = 0;
	*h = 0;
	if (!vid_modes)
		gfx_modes();

	if (!vid_modes)
		return -1;

	while (!gfx_get_mode(i, &ww, &hh)) {
		if ((ww * hh >= (*w) * (*h)) && ww > (*w)) {
			*w = ww;
			*h = hh;
		}
		i ++;
	}
#endif
	return 0;
}

int gfx_set_mode(int w, int h, int fs)
{
	if (gfx_width == w && gfx_height == h && gfx_fs == fs)
		return 0; /* already done */
	gfx_fs = fs;
	gfx_width = w;
	gfx_height = h;
	SDL_ShowCursor(SDL_DISABLE);
#ifdef S60
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 0, SDL_ANYFORMAT | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
#else
 #ifdef ANDROID
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 0, SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
 #else
  #ifdef MAEMO
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 16, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
  #else
   #ifdef __APPLE__	
	screen = SDL_SetVideoMode(gfx_width, gfx_height, (fs)?32:0, SDL_SWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
	if (screen == NULL) /* ok, fallback to anyformat */
		screen = SDL_SetVideoMode(gfx_width, gfx_height, 0, SDL_ANYFORMAT | SDL_SWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
   #else
    #ifndef _WIN32_WCE
	#if SDL_VERSION_ATLEAST(1,3,0)
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 32, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
	#else
	screen = SDL_SetVideoMode(gfx_width, gfx_height, (fs)?32:0, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
	#endif
	if (screen == NULL) /* ok, fallback to anyformat */
    #endif
		screen = SDL_SetVideoMode(gfx_width, gfx_height, 0, SDL_ANYFORMAT | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
   #endif
  #endif
 #endif
#endif
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %dx%d video: %s\n", w, h, SDL_GetError());
		return -1;
	}
	fprintf(stderr,"Video mode: %dx%d@%dbpp\n", screen->w, screen->h, screen->format->BitsPerPixel);
	gfx_clear(0, 0, gfx_width, gfx_height);
	return 0;
}


static SDL_Surface *icon = NULL;

int gfx_video_init(void)
{
	char title[4096];

	strcpy( title, "INSTEAD SDL - " );
	strcat( title, VERSION );

	if (TTF_Init()) {
		fprintf(stderr, "Can't init TTF subsystem.\n");
		return -1;
	}
  	
  	SDL_WM_SetCaption( title, title );

#ifndef ICON_PATH
#define ICON_PATH "./icon"
#endif

	icon = IMG_Load( ICON_PATH"/sdl_instead.png" );
	if ( icon ) {
		SDL_WM_SetIcon( icon, NULL );
	}
	return 0;
}
void gfx_flip(void)
{
	SDL_Flip(screen);
}


void gfx_update(int x, int y, int w, int h) {
//	SDL_Flip(screen);
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
	SDL_UpdateRect(screen, x, y, w, h);
}

void gfx_video_done(void)
{
	if (icon)
		SDL_FreeSurface(icon);
	screen = NULL;
	TTF_Quit();
}

img_t gfx_scale(img_t src, float xscale, float yscale)
{
	anigif_t ag;
	if ((ag = is_anigif(Surf(src)))) {
		int i;
		for (i = 0; i < ag->nr_frames; i ++) {
			SDL_Surface *s = zoomSurface(ag->frames[i].surface, xscale, yscale, 1);
			if (i)
				SDL_FreeSurface(ag->frames[i].surface);
			ag->frames[i].surface = s;
			ag->frames[i].x = (float)(ag->frames[i].x) * xscale;
			ag->frames[i].y = (float)(ag->frames[i].y) * yscale;
		}
		return ag->frames[0].surface;
	}
	return (img_t)zoomSurface((SDL_Surface *)src, xscale, yscale, 1);
}

img_t gfx_rotate(img_t src, float angle)
{
	anigif_t ag;

	float rangle = angle * (M_PI / 180.0);

	if ((ag = is_anigif(Surf(src)))) {
		int i;
		int w,h;
		float x, y, x1, y1;

		w = gfx_img_w(src);
		h = gfx_img_h(src);

		for (i = 0; i < ag->nr_frames; i ++) {
			SDL_Surface *s = rotozoomSurface(ag->frames[i].surface, angle, 1.0, 11);
			if (i)
				SDL_FreeSurface(ag->frames[i].surface);

			ag->frames[i].surface = s;

			x = (float)(ag->frames[i].x) - w / 2;
			y = (float)(ag->frames[i].y) - h / 2;

			x1 = x*cos(rangle) - y*sin(rangle);
			y1 = y*cos(rangle) + x*sin(rangle);
			
			ag->frames[i].x = x1 + w / 2;
			ag->frames[i].y = y1 + h / 2;
		}
		return ag->frames[0].surface;
	}
	return (img_t)rotozoomSurface(Surf(src), angle, 1.0, 1);
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
			SDL_RWops *rw = RWFromIdf(game_idf, files[i]);
			if (!rw || !(fn = TTF_OpenFontRW(rw, 1, size))) {
				fprintf(stderr, "Can not load font: '%s'\n", files[i]);
			}
		} 
		if (!fn && i == 0) /* no regular */
			goto err;
#ifdef TTF_HINTING_LIGHT
		if (fn) /* todo? */
			TTF_SetFontHinting(fn, TTF_HINTING_LIGHT);
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
	return TTF_RenderUTF8_Blended((TTF_Font *)h->fn, p, scol);
}

int fnt_height(fnt_t fn)
{
	struct fnt *h = (struct fnt*)fn;
	if (!fn)
		return 0;
	return TTF_FontHeight((TTF_Font *)(h->fonts[FN_REG]));
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
			if (! (*p & 0x80)) { // ascii
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
	TTF_SizeUTF8((TTF_Font *)f->fn, txt, &ww, &hh);
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
	char *word;
	img_t	img;
	struct word *next; /* in line */
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
	struct word *words;
	struct line *next;
	struct line *prev;
	struct layout *layout;
};

static int vertical_align(struct word *w, int *hh);

int	word_geom(word_t v, int *x, int *y, int *w, int *h)
{
	int xx, yy, ww, hh;
	struct line *line;
	struct word *word = (struct word*)v;
	if (!word || !word->line)
		return -1;
	line = word->line;
	xx = word->x + line->x;
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
	l->layout = NULL;
	l->align = 0;
	l->pos = 0;
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
	struct word *w;
	int sp, spm, lw = 0;
	int lnum = 0;
	if (!line || line->num <= 1 /*|| width <= line->w*/)
		return;
	w = line->words;
	while (w) {
		lw += w->w;
		if (last_margin)
			w->unbrake = 0;
		if (!w->unbrake && !w->img_align)
			lnum ++;
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
		return line_justify(line, width);
	}
	if (style == ALIGN_CENTER) 
		return line_center(line, width);
	if (style == ALIGN_LEFT) 
		return;
	if (style == ALIGN_RIGHT) 
		return line_right(line, width);
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
		return;
	}
	while (w->next)
		w = w->next;
	w->next = word;
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
	int scnt[4];
	int lstyle;
	cache_t img_cache;
	cache_t prerend_cache;
	cache_t hlprerend_cache;
};

struct xref {
	struct  xref *next;
	struct 	xref *prev;
	struct 	word **words;
	struct  layout *layout;
	char  	*link;
	int	num;
	int 	active;
};
struct textbox {
	struct 	layout *lay;
	struct  line    *line;
	int 	off;
	int 	w;
	int 	h;
};

void word_free(struct word *word)
{
	if (!word)
		return;
//	if (word->img)
//		gfx_free_image(word->img);
	if (word->word)
		free(word->word);

	if (word->prerend) {
		cache_forget(word->line->layout->prerend_cache, word->prerend);
//		SDL_FreeSurface(word->prerend);
	}

	if (word->hlprerend) {
		cache_forget(word->line->layout->hlprerend_cache, word->hlprerend);
//		SDL_FreeSurface(word->hlprerend);
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

void xref_add_word(struct xref *xref, struct word *word)
{
	xref->words = realloc(xref->words, (xref->num + 1) * sizeof(struct word*));
	xref->words[xref->num ++] = word;
	word->xref = xref;
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
//	if (line->w > layout->w)
//		layout->w = line->w;
	if (!l) {
		layout->lines = line;
		line->prev = NULL;
		return;
	}
	while (l->next)
		l = l->next;
	l->next = line;
	line->prev = l;
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

int layout_find_margin(struct layout *layout, int y, int *w)
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
	struct image *g = layout->images;
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
		
	if (!*g)
		*g = layout->images;
	else
		*g = (*g)->next;
	if (!*g)
		return NULL;
	return (*g)->image;
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
		xref->prev = NULL;
		return;
	}
	while (x->next)
		x = x->next;
	x->next = xref;
	xref->prev = x;
	return;
}

void sdl_surface_free(void *p)
{
	SDL_FreeSurface(p);
}

struct layout *layout_new(fnt_t fn, int w, int h)
{
	struct layout *l;
	l = malloc(sizeof(struct layout));
	if (!l)
		return NULL;
	l->lines = NULL;
	l->images = NULL;
	l->w = w;
	l->h = h;
	l->fn = fn;
	l->fn_height = 1.0f;
	l->align = ALIGN_JUSTIFY;
	l->valign = 0;
	l->style = 0;
	l->lstyle = 0;
	l->xrefs = NULL;
	l->margin = NULL;
	l->col = gfx_col(0, 0, 0);
	l->lcol = gfx_col(0, 0, 255);
	l->acol = gfx_col(255, 0, 0);
	l->box = NULL;
	l->img_cache = cache_init(0, gfx_free_image);
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
#define TOKEN_CLOSE	0x2000
#define TOKEN(x)	(x & 0x1fff)

int get_token(const char *ptr, char **eptr, char **val, int *sp)
{
	char *ep, *p;
	int closing = 0;
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
		*eptr = ep + 1;
		return TOKEN_X;
	case 'a':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_A | TOKEN_CLOSE;
		}
		if (ptr[1] != ':') {
			return 0;
		}
		ptr += 2;
//		ep = (char*)ptr + strcspn(ptr, ">");
		ep = find_in_esc(ptr, "\\>");
		if (*ep != '>') {
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
		*eptr = ep + 1;
		return TOKEN_A;
	case 'b':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_B | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_B;
		}
		break;
	case 'i':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_I | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_I;
		}
		break;
	case 's':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_S | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_S;
		}
		break;
	case 't':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_T | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_T;
		}
		break;
	case 'd':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_D | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_D;
		}
		break;
	case 'm':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_M | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_M;
		}
		break;
	case 'u':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_U | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_U;
		}
		break;
	case 'c':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_C | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_C;
		}
		break;
	case 'r':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_R | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_R;
		}
		break;
	case 'j':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_J | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
			*eptr = (char*)ptr + 2;
			return TOKEN_J;
		}
		break;
	case 'l':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_L | TOKEN_CLOSE;
		}
		if (ptr[1] == '>') {
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


static const char *lookup_token_or_sp(const char *ptr)
{
	char *eptr;
	const char *p = ptr;
	while (*p) {
		p += strcspn(p, " .,:!+-?/<\t\n");
		if (*p != '<' ) {
			while (is_delim(*p))
				p ++;
/*			if (is_delim(*p))
				p ++; */
			return p;
		}
		
		if (!get_token(p, &eptr, NULL, NULL)) {
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
//	ep += strcspn(ep, " \t\n");
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
	sprintf(p, "%s-%08x", (w->word)?w->word:"", style);
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

static void word_image_render(struct word *word, int x, int y, clear_fn clear, update_fn update)
{
	struct line *line = word->line;
	struct layout *layout = line->layout;
	int yy;

	if (clear && !word->xref)
		return;
	yy = vertical_align(word, NULL);

	if (clear) {
		if (word->img) {
			if (word->img_align) 
				clear(x + word->x, y + line->y + yy, gfx_img_w(word->img), gfx_img_h(word->img));
			else
				clear(x + line->x + word->x, y + line->y + yy, gfx_img_w(word->img), gfx_img_h(word->img));
		} else
			clear(x + line->x + word->x, y + line->y/* + yy*/, word->w, line->h);
	}
	if (word->img) {
		if (word->img_align)
			gfx_draw(word->img, x + word->x, y + line->y + yy);
		else
			gfx_draw(word->img, x + line->x + word->x, y + line->y + yy);
		if (update)
			update(x + word->x, y + line->y + yy, gfx_img_w(word->img), gfx_img_h(word->img));
	} else {
		word_render(layout, word, x + line->x + word->x, y + yy + line->y);
		if (update)
			update(x + line->x + word->x, y + line->y + yy, word->w, line->h);
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

void txt_layout_draw_ex(layout_t lay, struct line *line, int x, int y, int off, int height, clear_fn clear)
{
	void *v;
	img_t img;
	struct layout *layout = (struct layout*)lay;
	struct margin *margin;
	struct word *word;
//	line = layout->lines;
//	gfx_clip(x, y, layout->w, layout->h);
	if (!lay)
		return;
	for (v = NULL; (img = txt_layout_images(lay, &v)); )
		gfx_dispose_gif(img);
	
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
//	gfx_noclip();
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
	box->lay = NULL; //(struct layout*)lay;
	box->w = w;
	box->h = h;
	box->off = 0;
	box->line = NULL; //(box->lay)->lines;
	return box;
}


void txt_box_norm(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line;
	int off = box->off;

	if (!tbox || !box->lay)
		return;
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

	txt_layout_real_size(box->lay, NULL, &h);
	
	if (h - box->off < box->h)
		return;

	off = h - box->off - box->h;
	if (disp > off)
		disp = off;
	
	off = box->off - line->y; /* offset from cur line */
	off += disp; /* needed offset */
	while (line->next && off > line->h) {
		off -= line->h;
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
	if (!tbox)
		return;
	line = box->line;
	if (!line)
		return;
	off = box->off - line->y; /* offset from cur line */
	off -= disp; /* offset from current line */
	
	while (line->prev && off < 0) {
		line = line->prev;
		off += line->h;
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
	if (disp >0)
		return txt_box_scroll_next(tbox, disp);
	else if (disp <0)
		return txt_box_scroll_prev(tbox, -disp);
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
//	txt_box_norm(tbox);		
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
	if (!box || !box->lay)
		return;
	line = box->line;
	if (!line)
		return;
	line = line->prev;
	if (line) {
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
	for (; line; line = line->prev) {
		if ((box->off - line->y) >= box->h)
			break;
	}
	if (!line) {
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
	for (line = box->line; line; line = line->next) {
		int hh, yy;
		if (y < line->y)
			break;
		if (y > line->y + line->h)
			continue;
		for (word = line->words; word; word = word->next) {
			yy = vertical_align(word, &hh);
			if (y < line->y + yy || y > line->y + yy + hh)
				continue;
			if (x < line->x + word->x)
				continue;
			xref = word->xref;
			if (!xref)
				continue;
			if (x < line->x + word->x + word->w)
				break;
			if (word->next && word->next->xref == xref && x < line->x + word->next->x + word->next->w) {
				yy = vertical_align(word->next, &hh);
				if (y < line->y + yy || y > line->y + yy + hh)
					continue;
				break;
			}
		}
	}
	if (word && xref) {
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
	SDL_Surface *old_screen;
	img_t		dst;
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return NULL;
	dst = gfx_new(box->w, box->h);
	if (!dst)
		return NULL;
	old_screen = screen;
	screen = (SDL_Surface*)dst;
	gfx_clear(0, 0, box->w, box->h);
//	gfx_clip(0, 0, box->w, box->h);
//	printf("line: %d\n", box->line->y);
	txt_layout_draw_ex(box->lay, box->line, 0, - box->off, box->off, box->h, NULL);
//	gfx_noclip();
	screen = old_screen;
	return dst;
}

void txt_box_draw(textbox_t tbox, int x, int y)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	gfx_clip(x, y, box->w, box->h);
//	printf("line: %d\n", box->line->y);
	txt_layout_draw_ex(box->lay, box->line, x, y - box->off, box->off, box->h, NULL);
	gfx_noclip();
}

void txt_box_update_links(textbox_t tbox, int x, int y, clear_fn clear)
{
	struct textbox *box = (struct textbox *)tbox;
	if (!tbox)
		return;
	gfx_clip(x, y, box->w, box->h);
//	printf("line: %d\n", box->line->y);
	txt_layout_draw_ex(box->lay, box->line, x, y - box->off, box->off, box->h, clear);
	gfx_noclip();
}


void txt_layout_update_links(layout_t layout, int x, int y, clear_fn clear)
{
	struct layout *lay = (struct layout *)layout;
//	gfx_clip(x, y, box->w, box->h);
//	printf("line: %d\n", box->line->y);
	txt_layout_draw_ex(lay, lay->lines, x, y, 0, lay->h, clear);
//	gfx_noclip();
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
		if (!(img = gfx_load_image(p)))
			goto out;
		theme_img_scale(&img); /* bad style, no gfx layer :( */
	}	
	image = image_new(p, img);
	if (!image) {
		gfx_free_image(img);
		img = NULL;
	} else {
		layout_add_image(layout, image);
		image->free_it = 1; /* free on layout destroy */
//		if (gfx_img_w(img) <= GFX_MAX_CACHED_W && gfx_img_h(img) <= GFX_MAX_CACHED_H)
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

	token = get_token(ptr, &eptr, &val, sp);
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
	if (TOKEN(token) == TOKEN_X) {
		int xpos;
		if (strchr(val, '%') && sscanf(val, "%d%%", &xpos) == 1) {
			xpos = layout->w * xpos / 100;
		} else  {
			xpos = atoi(val);
			xpos = xpos * game_theme.scale;
		}
		line->tabx = xpos;
		line->al_tabx = ALIGN_LEFT;
		if (strstr(val, "right"))
			line->al_tabx = ALIGN_RIGHT;
		else if (strstr(val, "center"))
			line->al_tabx = ALIGN_CENTER;
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
			*xref = xref_new(val);
		}
	}
out:
	if (val)
		free(val);

	return eptr;
}

int get_unbrakable_len(struct layout *layout, const char *ptr)
{		
	int w = 0;
	int ww = 0;
	char *p, *eptr;
	while (ptr && *ptr) {
		int sp, sp2 = 0;
		while (get_token(ptr, &eptr, NULL, &sp)) {
			if (sp)
				sp2 ++;
			ptr = eptr;
		}
		if (sp2)
			return w;
		p = get_word(ptr, &eptr, &sp);
		if (!p)
			return w;

		if (sp || !*p || word_img(p, NULL) || word_token(p, NULL)) {
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
		off = line->y; // + line->h;
		if (hh)
			*hh = line->h;
	}
	return off;
}

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
	int w, h = 0, nl = 0;
	int spw;
	img_t img = NULL;
	if (!layout || !layout->fn)
		return;
	saved_style = layout->style; 
	fnt_style(layout->fn, 0);
	txt_size(layout->fn, " ", &spw, NULL);

	for (line = layout->lines; line; line = line->next) {
		lastline = line;
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
				fnt_style(layout->fn, layout->lstyle); // & ~TTF_STYLE_ITALIC);
			else
				fnt_style(layout->fn, layout->style);// & ~TTF_STYLE_ITALIC);

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
			addlen = get_unbrakable_len(layout, eptr);

		wtok = 0;
		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
			if (img_align && width - w <= 0)
				img_align = 0;
		} else {
			p = get_word_token(p, &wtok);
			txt_size(layout->fn, p, &w, &h);
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
			line_align(line, width, line->align, nl);

			line = line_new();
			if (!line) {
				free(p);
				goto err;
			}
			line->align = layout->align;
			line->h = 0;//h;
			line->y = ol->y + ol->h;
//			line->x = 0;
			line->x = layout_find_margin(layout, line->y, &width);
//			fprintf(stderr,"%d %d\n", line->x, width);
			if (nl) {
				ptr = eptr + 1;
			}
			free(p);
//			ptr = eptr;
			line->pos = (int)(ptr - txt);
			continue;
		}

	
		if (h > line->h && !img_align)
			line->h = h;

		word = word_new(p);
		if (!word) {
			line_free(line);
			goto err;
		}
		word->valign = layout->valign;
		if (!sp && !line_empty(line))
			word->unbrake = 1;
		
		word->style = layout->style;
		
		if (line->w && !word->unbrake)
			line->w += spw;

		word->w = w;
		word->x = line->w;
		if (line->tabx > 0) {
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
//			fprintf(stderr,"w: %d %d %d\n", width, w, width - w);
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

//		if (line->w)
//			w += spw;

		line_add_word(line, word);

		if (xref)
			xref_add_word(xref, word);

		line->w += w;

		if (nl) 
			eptr ++;
		ptr = eptr;
		free(p);
	}
	if (layout->h == 0)
		layout->h = line->y + line->h;

//	if (line->num) {
		if (line != lastline) 
			layout_add_line(layout, line);
		line_align(line, width, line->align, nl);
//	} else
//		line_free(line);
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
	if (!x)
		return NULL;
	return ((struct xref*)x)->prev;
}

xref_t	txt_layout_xrefs(layout_t lay)
{
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return NULL;
	return layout->xrefs;
}

int xref_position(xref_t x, int *xc, int *yc)
{
	int i;
	int w = 0;
	struct line *line = NULL;
	struct word *word = NULL;
	struct xref *xref = (struct xref*)x;

	if (!xref || !xref->num)
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
		*xc = line->x + word->x + (word->w + w);
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
			int hh,yy;
			word = xref->words[i];
			line = word->line;
			if (word->img_align)
				continue;
			if (y < line->y || y > line->y + line->h)
				continue;
			yy = vertical_align(word, &hh);
			if (y < line->y + yy || y > line->y + yy + hh)
				continue;
			if (x < line->x + word->x)
				continue;
			if (x <= line->x + word->x + word->w)
				return xref;
			if (word->next && word->next->xref == xref && x < line->x + word->next->x + word->next->w) {
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
//	_txt_layout_add(layout, txt);
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


void txt_layout_real_size(layout_t lay, int *pw, int *ph)
{
	int w = 0;
	int h = 0;
	struct margin *margin;
	struct line *line;
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return;	
	for (line = layout->lines; line; line = line->next) {
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

void gfx_cursor(int *xp, int *yp)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (xp)
		*xp = x;
	if (yp)
		*yp = y;
}

void gfx_warp_cursor(int x, int y)
{
	SDL_WarpMouse(x, y);
}

int ALPHA_STEPS = 4;
static int   fade_step_nr = -1;

int gfx_fading(void)
{
	return (fade_step_nr != -1);
}

img_t	*fade_bg = NULL;

static void update_gfx(void *aux)
{
	img_t img = (img_t) aux;
	if (fade_step_nr == -1 || !img || !fade_bg)
		return;
	game_cursor(CURSOR_CLEAR);
	gfx_set_alpha(img, (255 * (fade_step_nr + 1)) / ALPHA_STEPS);
	gfx_draw(fade_bg, 0, 0);
	gfx_draw(img, 0, 0);
	game_cursor(CURSOR_DRAW);
	gfx_flip();
	fade_step_nr ++;
	if (fade_step_nr == ALPHA_STEPS) {
		fade_step_nr = -1;
	}
}

static Uint32 update(Uint32 interval, void *aux)
{
	push_user_event(update_gfx, aux);
	return interval;
}

extern void nsleep(int delay);

void gfx_change_screen(img_t src, int steps)
{
	struct inp_event ev;
	memset(&ev, 0, sizeof(ev));
	ALPHA_STEPS = steps;
	SDL_TimerID han;
	fade_step_nr = 0;
	fade_bg = gfx_grab_screen(0, 0, gfx_width, gfx_height);
	if (!fade_bg) /* ok, i like kernel logic. No memory, but we must work! */
		return;
	han = SDL_AddTimer(60, update, src);
	while (input(&ev, 1) >=0 && gfx_fading()) /* just wait for change */
		game_cursor(CURSOR_ON);
	SDL_RemoveTimer(han);
	gfx_free_image(fade_bg);
	fade_bg = NULL;
}

int gfx_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return -1;
	}
	if (!(images = cache_init(-1, gfx_free_image))) {
		fprintf(stderr, "Can't init cache subsystem.\n");
		gfx_done();
		return -1;
	}
	return 0;
}

void gfx_done(void)
{
	cache_free(images);
	images = NULL;
	SDL_Quit();
}

gtimer_t gfx_add_timer(int delay, int (*fn)(int, void*), void *aux)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	return (gtimer_t)SDL_AddTimer(delay, (SDL_TimerCallback)fn, aux);
#else
	return (gtimer_t)SDL_AddTimer(delay, (SDL_NewTimerCallback)fn, aux);
#endif
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
