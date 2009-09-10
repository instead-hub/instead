#include "externals.h"
#include "internals.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mutex.h>
#include "SDL_rotozoom.h"
	
static SDL_Surface *screen;

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
	{"indianred ", 0xcd5c5c},
	{"indigo ", 0x4b0082},
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

void gfx_free_image(img_t p)
{
	if (!p)
		return;
	SDL_FreeSurface((SDL_Surface *)p);
}

int	gfx_img_w(img_t pixmap)
{
	if (!pixmap)
		return 0;
	return ((SDL_Surface *)pixmap)->w;
}

int	gfx_img_h(img_t pixmap)
{
	if (!pixmap)
		return 0;
	return ((SDL_Surface *)pixmap)->h;
}

void gfx_noclip(void)
{
	SDL_SetClipRect(screen, NULL);
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
	dst = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 
		screen->format->BitsPerPixel, 
		screen->format->Rmask, 
		screen->format->Gmask, 
		screen->format->Bmask, 
		screen->format->Amask);
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

img_t gfx_alpha_img(img_t src, int alpha)
{
	Uint32 *ptr;
	Uint32 col;
	int size;
	
	SDL_Surface *img = SDL_DisplayFormatAlpha((SDL_Surface*)src);
	if (!img)
		return NULL;	
	ptr = (Uint32*)img->pixels;
	size = img->w * img->h;
	while (size --) {
		Uint8 r, g, b, a;
		col = *ptr;
		SDL_GetRGBA(col, img->format, &r, &g, &b, &a);
		col = SDL_MapRGBA(img->format, r, g, b, a * alpha / 255);
		*ptr = col;
		ptr ++;
	}
	return img;
}

void	gfx_set_alpha(img_t src, int alpha)
{
//	SDL_Surface *img = SDL_DisplayFormat((SDL_Surface*)src);
//	if (!img)
//		return NULL;	
	SDL_SetAlpha((SDL_Surface *)src, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
//	return img;
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

img_t gfx_load_image(char *filename)
{

	SDL_Surface *img;
	img = IMG_Load(filename);
	if (!img) {
		fprintf(stderr, "File not found: '%s'\n", filename);
		return NULL;
	}
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

void gfx_draw_from(img_t p, int x, int y, int xx, int yy, int width, int height)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Rect dest, src;
	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;
	dest.x = xx;
	dest.y = yy; 
	dest.w = width; 
	dest.h = height;
	SDL_BlitSurface(pixbuf, &src, screen, &dest);
}
void gfx_draw(img_t p, int x, int y)
{
	SDL_Surface *pixbuf = (SDL_Surface *)p;
	SDL_Rect dest;
	dest.x = x;
	dest.y = y; 
	dest.w = pixbuf->w; 
	dest.h = pixbuf->h;
	SDL_BlitSurface(pixbuf, NULL, screen, &dest);
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

int gfx_width;
int gfx_height;

int gfx_setmode(int w, int h, int fs)
{
	gfx_width = w;
	gfx_height = h;
	SDL_ShowCursor(SDL_DISABLE);
#ifndef MAEMO	
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 32, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
	if (screen == NULL) /* ok, fallback to 16 bit */
		screen = SDL_SetVideoMode(gfx_width, gfx_height, 16, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
#else
	screen = SDL_SetVideoMode(gfx_width, gfx_height, 16, SDL_DOUBLEBUF | SDL_HWSURFACE | ( ( fs ) ? SDL_FULLSCREEN : 0 ) );
#endif
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %dx%d video: %s\n", w, h, SDL_GetError());
		return -1;
	}
		
	gfx_clear(0, 0, gfx_width, gfx_height);
	return 0;
}

unsigned int	timer_counter = 0;

SDL_TimerID timer_han = NULL;
static SDL_Surface *icon = NULL;
Uint32 counter_fn(Uint32 interval, void *p)
{
	timer_counter ++;
	return interval;
}

int gfx_init(void)
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
	timer_han =  SDL_AddTimer(100, counter_fn, NULL);
	return 0;
}
void gfx_flip(void)
{
	SDL_Flip(screen);
}

void gfx_update(int x, int y, int w, int h) {
//	SDL_Flip(screen);
	if (x < 0) {
		x = 0;
		w += x;
	}
	if (y < 0) {
		y = 0;	
		h += y;
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

void gfx_done(void)
{
	if (icon)
		SDL_FreeSurface(icon);
	if (timer_han)
		SDL_RemoveTimer(timer_han);
	timer_han = NULL;
	TTF_Quit();
}

img_t gfx_scale(img_t src, float xscale, float yscale)
{
	return (img_t)zoomSurface((SDL_Surface *)src, xscale, yscale, 1);
}

fnt_t fnt_load(const char *fname, int size)
{
	TTF_Font *fn;
	fn = TTF_OpenFont(fname, size);
	return (fnt_t) fn;
}

void fnt_free(fnt_t fnt)
{
	if (!fnt)
		return;
	TTF_CloseFont((TTF_Font *)fnt);
}

void txt_draw(fnt_t fnt, const char *txt, int x, int y, color_t col)
{
	SDL_Color fgcol = { .r = col.r, .g = col.g, .b = col.b };
	SDL_Surface *s = TTF_RenderUTF8_Blended((TTF_Font *)fnt,
				txt, fgcol);
	gfx_draw(s, x, y);
}

void txt_size(fnt_t fnt, const char *txt, int *w, int *h)
{
	int ww, hh;
	TTF_SizeUTF8((TTF_Font *)fnt, txt, &ww, &hh);
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
	char *word;
	img_t	img;
	struct word *next; /* in line */
	struct line *line;
	struct xref *xref;
	SDL_Surface  *prerend;
};

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
	w->line = NULL;
	w->xref = NULL;
	w->style = 0;
	w->img = NULL;
	w->unbrake = 0;
	w->prerend = NULL;
	return w;
}

void word_free(struct word *word)
{
	if (!word)
		return;
//	if (word->img)
//		gfx_free_image(word->img);
	if (word->word)
		free(word->word);
	if (word->prerend)
		SDL_FreeSurface(word->prerend);
	word->prerend = NULL;
	free(word);
}

struct line {
	int y;
	int h;
	int w;
	int num;
	int align;
	struct word *words;
	struct line *next;
	struct line *prev;
	struct layout *layout;
};

struct line *line_new(void)
{
	struct line *l;
	l = malloc(sizeof(struct line));
	if (!l)
		return NULL;
	l->words = NULL;
	l->next = NULL;
	l->prev = NULL;
	l->w = 0;
	l->y = 0;
	l->h = 0;
	l->num = 0;
	l->layout = NULL;
	l->align = 0;
	return l;
}

void line_justify(struct line *line, int width)
{
	int x = 0;
	struct word *w;
	int sp, spm, lw = 0;
	int lnum = 0;
	if (!line || line->num <= 1 /*|| width <= line->w*/)
		return;
	w = line->words;
	while (w) {
		lw += w->w;
		if (!w->unbrake)
			lnum ++;
		w = w->next;
	}
	if (lnum <=1 )
		return;
	w = line->words;
	sp = (width - lw) / (lnum - 1);
	spm = (width - lw) % (lnum - 1);
	while (w) {
		w->x = x;
		if (w->next && w->next->unbrake)
			x += w->w;
		else {
			x += w->w + sp + ((spm)?1:0);

			if (spm)
				spm --;
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
		w->x += sp;
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
		w->x += sp;
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
};

struct image *image_new(const char *name, img_t img)
{
	struct image *g = malloc(sizeof(struct image));
	if (!g)
		return NULL;
	g->image = img;
	g->name = strdup(name);
	g->next = NULL;
	return g;
}

void image_free(struct image *image)
{
	if (!image)
		return;
	if (image->name)
		free(image->name);
//	gfx_free_image(image->image);
	free(image);
}

struct textbox;
struct layout {
	fnt_t	fn;
	color_t	col;
	color_t	lcol;
	color_t	acol;
	struct image *images;
	struct xref *xrefs;
	struct line *lines;
	struct textbox *box;
	int w;
	int h;
	int align;
	int saved_align;
	int style;
	int lstyle;
};

struct word_list {
	struct word_list *next;
	struct word 	 *word;
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

struct image *_layout_lookup_image(struct layout *layout, const char *name)
{
	struct image *g = layout->images;
	for (g = layout->images; g; g = g->next) {
		if (!strcmp(g->name, name))
			return g;
	}
	return NULL;
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
	l->align = ALIGN_JUSTIFY;
	l->style = 0;
	l->lstyle = 0;
	l->xrefs = NULL;
	l->col = gfx_col(0, 0, 0);
	l->lcol = gfx_col(0, 0, 255);
	l->acol = gfx_col(255, 0, 0);
	l->box = NULL;
	return l;
}
void txt_layout_size(layout_t lay, int *w, int *h)
{
	struct layout *layout = (struct layout *)lay;
	if (w)
		*w = layout->w;
	if (h)
		*h = layout->h;
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
	g = layout->images;
	while (g) {
		struct image *og = g;
		g = g->next;
		image_free(og);
	}
	layout->images = NULL;
	layout->xrefs = NULL;
	layout->lines = NULL;
}

void txt_layout_free(layout_t lay)
{
	_txt_layout_free(lay);
	free(lay);
}

#define TOKEN_NONE	0
#define	TOKEN_A		1
#define TOKEN_B		2
#define	TOKEN_I		4
#define	TOKEN_U		8
#define	TOKEN_C		0x10
#define	TOKEN_R		0x20
#define	TOKEN_J		0x40
#define	TOKEN_L		0x80
#define TOKEN_CLOSE	0x2000
#define TOKEN(x)	(x & 0xff)

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
	case 'a':
		if (closing) {
			*eptr = (char*)ptr + 2;
			return TOKEN_A | TOKEN_CLOSE;
		}
		if (ptr[1] != ':') {
			return 0;
		}
		ptr += 2;
		ep = (char*)ptr + strcspn(ptr, ">");
		if (*ep != '>') {
			return 0;
		}
		if (val) {
			p = malloc(ep - ptr + 1);
			if (!p)
				return 0;
			memcpy(p, ptr, ep - ptr);
			p[ep - ptr] = 0;
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

static const char *lookup_token_or_sp(const char *ptr)
{
	char *eptr;
	const char *p = ptr;
	while (*p) {
		p += strcspn(p, " <\n\t");
		if (*p != '<' )
			return p;
		if (!get_token(p, &eptr, NULL, NULL)) {
			p ++;
			continue;
		}
		return p;
	}
	return ptr;
}

static char *get_word(const char *ptr, char **eptr, int *sp)
{
	const char *ep;
	char *o;
	size_t sz;
	*eptr = NULL;
	o = (char*)ptr;
	ptr += strspn(ptr, " \t");
	if (sp) {
		*sp = 0;
		if (o != ptr)
			*sp = 1;
	}
	if (!*ptr)
		return NULL;
	ep = lookup_token_or_sp(ptr);
//	ep += strcspn(ep, " \t\n");
	sz = ep - ptr;
	o = malloc(sz + 1);
	memcpy(o, ptr, sz);
	o[sz] = 0;
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
	layout->col = fg;
}
void txt_layout_link_color(layout_t lay, color_t link)
{
	struct layout *layout = (struct layout*)lay;
	layout->lcol = link;	
}
void txt_layout_active_color(layout_t lay, color_t link)
{
	struct layout *layout = (struct layout*)lay;
	layout->acol = link;	
}
void txt_layout_link_style(layout_t lay, int style)
{
	struct layout *layout = (struct layout*)lay;
	layout->lstyle = style;	
}

void xref_update(xref_t pxref, int x, int y, clear_fn clear, update_fn update)
{
	int i;
	struct xref *xref = (struct xref*)pxref;
	struct layout *layout;
	struct word *word;
	SDL_Color col;
	if (!xref)
		return;

	layout = xref->layout;
	if (layout->box) {
		gfx_clip(x, y, layout->box->w, layout->box->h);
		y -= (layout->box)->off;
	}

	for (i = 0; i < xref->num; i ++) {
		SDL_Surface *s;
		int yy;
		SDL_Color fgcol = { .r = layout->col.r, .g = layout->col.g, .b = layout->col.b };
		SDL_Color lcol = { .r = layout->lcol.r, .g = layout->lcol.g, .b = layout->lcol.b };
		SDL_Color acol = { .r = layout->acol.r, .g = layout->acol.g, .b = layout->acol.b };

		struct line *line;
		word = xref->words[i];
		line = word->line;

		if (clear) {
			if (word->img)
				clear(x + word->x, y + line->y, gfx_img_w(word->img), gfx_img_h(word->img));
			else
				clear(x + word->x, y + line->y, word->w, line->h);
		}
		if (word->img) {
			gfx_draw(word->img, x + word->x, y + line->y);
			update(x + word->x, y + line->y, gfx_img_w(word->img), gfx_img_h(word->img));
			continue;
		}
		if (!word->style)
			TTF_SetFontStyle((TTF_Font *)(layout->fn), layout->lstyle);
		else
			TTF_SetFontStyle((TTF_Font *)(layout->fn), word->style);
		if (!word->xref)
			col = fgcol;
		else if (word->xref->active)
			col = acol;
		else
			col = lcol;
		s = TTF_RenderUTF8_Blended((TTF_Font *)(layout->fn), word->word, col);
		yy = (line->h - TTF_FontHeight((TTF_Font *)(layout->fn))) / 2; // TODO
		gfx_draw(s, x + word->x, y + line->y + yy);
		update(x + word->x, y + line->y, word->w, line->h);
		SDL_FreeSurface(s);
	}
	gfx_noclip();
}

void txt_layout_draw_ex(layout_t lay, struct line *line, int x, int y, int off, int height, clear_fn clear)
{
	struct layout *layout = (struct layout*)lay;
//	struct line *line;
	struct word *word;
//	line = layout->lines;
	SDL_Color col;
	SDL_Color fgcol = { .r = layout->col.r, .g = layout->col.g, .b = layout->col.b };
	SDL_Color lcol = { .r = layout->lcol.r, .g = layout->lcol.g, .b = layout->lcol.b };
	SDL_Color acol = { .r = layout->acol.r, .g = layout->acol.g, .b = layout->acol.b };
//	gfx_clip(x, y, layout->w, layout->h);
	if (!line)
		line = layout->lines;
	for (; line; line= line->next) {
		int yy;
		if ((line->y + line->h) < off)
			continue;
		if (line->y - off > height)
			break;
		for (word = line->words; word; word = word->next ) {
			if (clear && !word->xref)
				continue;

			if (clear) {
				if (word->img)
					clear(x + word->x, y + line->y, gfx_img_w(word->img), gfx_img_h(word->img));
				else
					clear(x + word->x, y + line->y, word->w, line->h);
			}
			SDL_Surface *s;
			if (word->img) {
				gfx_draw(word->img, x + word->x, y + line->y);
				continue;
			}
			if (word->xref && !word->style)
				TTF_SetFontStyle((TTF_Font *)(layout->fn), layout->lstyle);
			else
				TTF_SetFontStyle((TTF_Font *)(layout->fn), word->style);
			if (!word->xref)
				col = fgcol;
			else if (word->xref->active)
				col = acol;
			else
				col = lcol;
			s = word->prerend;
			if (!s) {
				s = TTF_RenderUTF8_Blended((TTF_Font *)(layout->fn),
					word->word, col);
				word->prerend = s;
			}
			yy = (line->h - TTF_FontHeight((TTF_Font *)(layout->fn))) / 2; // TODO
			gfx_draw(s, x + word->x, y + line->y + yy);
//			SDL_FreeSurface(s);
		}
	}
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
	if (!box->lay)
		return;
	for (line = box->lay->lines; line; line = line->next) {
		if (box->off < line->h) {
			box->line = line;
			break;
		}	
		box->off -= line->h;
	}
	box->line = box->lay->lines;
}

layout_t txt_box_layout(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	return box->lay;
}

void txt_box_set(textbox_t tbox, layout_t lay)
{
	struct textbox *box = (struct textbox *)tbox;
	box->lay = (struct layout*)lay;
	box->off = 0;
	if (lay)
		box->lay->box = box;
	txt_box_norm(tbox);	
}

void txt_box_resize(textbox_t tbox, int w, int h)
{
	struct textbox *box = (struct textbox *)tbox;
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
	int off;
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line = box->line;
	if (!line)
		return;

	if (box->lay->h - box->off < box->h)
		return;

	off = box->lay->h - box->off - box->h;
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
	struct line  *line = box->line;
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
	if (disp >0)
		return txt_box_scroll_next(tbox, disp);
	else if (disp <0)
		return txt_box_scroll_prev(tbox, -disp);
}

void txt_box_next_line(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line = box->line;
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
	struct line  *line = box->line;
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
	return box->off;
}

void txt_box_next(textbox_t tbox)
{
	struct textbox *box = (struct textbox *)tbox;
	struct line  *line = box->line;
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
	struct layout *lay = box->lay;
	struct line  *line = box->line;
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

	for (line = box->line; line; line = line->next) {
		if (line->y < box->off)
			continue; /* too high */
		if (line->y + line->h > box->h + box->off)
			break; /* bottom */
		for (word = line->words; word; word = word->next) {
			xref = word->xref;
			if (!xref)
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
	y += box->off;
	if (x < 0)
		return NULL;
	if (y < 0)
		return NULL;
	if (x >= box->w)
		return NULL;
	for (line = box->line; line; line = line->next) {
		if (y < line->y)
			break;
		if (y > line->y + line->h)
			continue;
		for (word = line->words; word; word = word->next) {
			if (x < word->x)
				continue;
			xref = word->xref;
			if (!xref)
				continue;
			if (x < word->x + word->w)
				break;
			if (word->next && word->next->xref == xref && x < word->next->x + word->next->w)
				break;
		}
	}
	if (word && xref) {
		return xref;
	}
	return NULL;
}

void txt_box_free(textbox_t tbox)
{
	free(tbox);
}

img_t txt_box_render(textbox_t tbox)
{
	SDL_Surface *old_screen;
	img_t		dst;
	struct textbox *box = (struct textbox *)tbox;
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
	gfx_clip(x, y, box->w, box->h);
//	printf("line: %d\n", box->line->y);
	txt_layout_draw_ex(box->lay, box->line, x, y - box->off, box->off, box->h, NULL);
	gfx_noclip();
}

void txt_box_update_links(textbox_t tbox, int x, int y, clear_fn clear)
{
	struct textbox *box = (struct textbox *)tbox;
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
img_t get_img(struct layout *layout, char *p)
{
	int len;
	img_t img;
	if (!p)
		return NULL;

	if (p[0] != '<' || p[1] != 'g' || p[2] != ':')
		return NULL;
	p += 3;
	len = strcspn(p, ">");
	if (p[len] != '>')
		return NULL;
	p[len] = 0;
	img = layout_lookup_image(layout, p);
	p[len] = '>';
	return img;
}

char *process_token(char *ptr, struct layout *layout, struct line *line, struct xref **xref, int *sp)
{

	int token;
	char *val = NULL;
	int bit = 0;
	int al = 0;
	char *eptr;

	token = get_token(ptr, &eptr, &val, sp);
	if (!token)
		return NULL;
	if (TOKEN(token) == TOKEN_B)
		bit = TTF_STYLE_BOLD;
	else if (TOKEN(token) == TOKEN_I)
		bit = TTF_STYLE_ITALIC;
	else if (TOKEN(token) == TOKEN_U)
		bit = TTF_STYLE_UNDERLINE;

	if (bit) {
		if (token & TOKEN_CLOSE)
			layout->style &= ~bit;
		else
			layout->style |= bit;
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
			layout->align = layout->saved_align;
		} else {
			layout->saved_align = layout->align;
			layout->align = al;
			line->align = al;
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
			*xref = xref_new(val);
		}
	}
out:
	if (val)
		free(val);

	return eptr;
//	TTF_SetFontStyle((TTF_Font *)(layout->fn), style);
}

int get_unbrakable_len(struct layout *layout, const char *ptr)
{		
	int w = 0;
	int ww = 0;
	char *p, *eptr;
	while (ptr && *ptr) {
		int sp, sp2 = 0;
		while(get_token(ptr, &eptr, NULL, &sp)) {
			if (sp)
				sp2 ++;
			ptr = eptr;
		}
		if (sp2)
			return w;
		p = get_word(ptr, &eptr, &sp);
		if (!p)
			return w;

		if (sp || !*p) {
			free(p);
			return w;
		}
		TTF_SizeUTF8((TTF_Font *)(layout->fn), p, &ww, NULL);
		ptr = eptr;
		if (!*p)
			ptr ++;
		w += ww;
		free(p);
	}
	return ww;
}

void _txt_layout_add(layout_t lay, char *txt)
{
	int sp = 0;
	int saved_style;
	struct line *line, *lastline = NULL;
	struct layout *layout = (struct layout*)lay;
	char *p, *eptr;
	char *ptr = txt;
	struct xref *xref = NULL;
	int w, h, nl = 0;
	int spw;
	img_t img = NULL;
	if (!layout || !layout->fn)
		return;
	saved_style = TTF_GetFontStyle((TTF_Font *)(layout->fn));

	TTF_SetFontStyle((TTF_Font *)(layout->fn), 0);
	TTF_SizeUTF8((TTF_Font *)(layout->fn), " ", &spw, &h);

	for (line = layout->lines; line; line = line->next) 
		lastline = line;
	
	if (!lastline) {
		line = line_new();
		if (!line)
			goto err;
		line->h = h;
		line->align = layout->align;
	} else {
		line = lastline;
	}

	while (ptr && *ptr) {
		struct word *word;
		int sp2, addlen;
		
		eptr = process_token(ptr, layout, line, &xref, &sp2);
		if (eptr) {
			ptr = eptr;
			TTF_SetFontStyle((TTF_Font *)(layout->fn), layout->style);
			if (!ptr || !*ptr)
				break;
			if (sp2)
				sp = 2;
			continue;
		} 
		if (sp == 2) {
			p = get_word(ptr, &eptr, NULL);
			sp = 1;
		} else
			p = get_word(ptr, &eptr, &sp);

		if (!p)
			break;

		addlen = get_unbrakable_len(layout, eptr);
		img = get_img(layout, p);
		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
		} else {
			TTF_SizeUTF8((TTF_Font *)(layout->fn), p, &w, &h);
		}
		nl = !*p;
		if ((line->num && (line->w + ((sp && line->w)?spw:0) + w + addlen) > layout->w) || nl) {
			struct line *ol = line;
			if ((layout->h) && (line->y + line->h) >= layout->h)
				break;
			if (line != lastline) {
				layout_add_line(layout, line);
			}
			line_align(line, layout->w, line->align, nl);
				
			line = line_new();
			if (!line) {
				free(p);
				goto err;
			}
			line->align = layout->align;
			line->h = h;
			line->y = ol->y + ol->h;
			if (nl) {
				ptr = eptr + 1;
//				line->y += TTF_FontLineSkip((TTF_Font*)(layout->fn));
			}
			free(p);
//			ptr = eptr;
			continue;
		}
		if (h > line->h)
			line->h = h;
		word = word_new(p);
		if (!word) {
			line_free(line);
			goto err;
		}
		if (!sp && line->num)
			word->unbrake = 1;
		word->style = layout->style;
		
		if (line->w && !word->unbrake)
			line->w += spw;

		word->w = w;
		word->x = line->w;

		word->img = img;

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
		line_align(line, layout->w, line->align, nl);
//	} else
//		line_free(line);
	if (xref)
		layout_add_xref(layout, xref);
//	layout->align = align;
//	layout_debug(layout);
//	fnt_draw_layout(layout, 300, 100, gfx_col(255,255,255));
	TTF_SetFontStyle((TTF_Font *)(layout->fn), saved_style);
	return;
err:
	txt_layout_free(layout);
	TTF_SetFontStyle((TTF_Font *)(layout->fn), saved_style);
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
		w += word->w;
	}
	
	w = w/2;
	
	for (i = 0; i < xref->num; i ++) {
		word = xref->words[i];
		line = word->line;
		w -= word->w;
		if (w < 0)
			break;
	}

	if (!line || !word)
		return -1;

	if (xc)
		*xc = word->x + (word->w + w);
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
	if (x < 0 || y < 0)
		return NULL;
	for (xref = layout->xrefs; xref; xref = xref->next) {
		for (i = 0; i < xref->num; i ++) {
			word = xref->words[i];
			line = word->line;
			if (y < line->y || y > line->y + line->h)
				continue;
			if (x < word->x)
				continue;
			if (x <= word->x + word->w)
				return xref;
			if (word->next && word->next->xref == xref && x < word->next->x + word->next->w)
				return xref;
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
	struct line *line;
	struct layout *layout = (struct layout*)lay;
	if (!layout)
		return;	
	for (line = layout->lines; line; line = line->next) {
		if (line->w > w)
			w = line->w;
		if (line->y + line->h > h)
			h = line->y + line->h;
	}
	if (pw)
		*pw = w;
	if (ph)
		*ph = h;
}

void gfx_cursor(int *xp, int *yp, int *w, int *h)
{
	int x, y;
	SDL_Cursor *c = SDL_GetCursor();
	if (!c)
		return;
	SDL_GetMouseState(&x, &y);
	if (w) 
		*w = c->area.w - c->hot_x;
	if (h)
		*h = c->area.h - c->hot_y;
	if (xp)
		*xp = x;
	if (yp)
		*yp = y;
}
void gfx_warp_cursor(int x, int y)
{
	SDL_WarpMouse(x, y);
}
#define ALPHA_STEPS 5
volatile int   step_nr = -1;

static void update_gfx(void *aux)
{
	img_t img = (img_t) aux;
	if (step_nr == -1)
		return;
	game_cursor(-1);
	gfx_set_alpha(img, (255 * (step_nr + 1)) / ALPHA_STEPS);
	gfx_draw(img, 0, 0);
	game_cursor(CURSOR_DRAW);
	gfx_flip();
	step_nr ++;
	if (step_nr == ALPHA_STEPS) {
		step_nr = -1;
	}
}

static Uint32 update(Uint32 interval, void *aux)
{
	push_user_event(update_gfx, aux);
	return interval;	
}

extern void nsleep(int delay);

void gfx_change_screen(img_t src)
{
	struct inp_event ev;
	memset(&ev, 0, sizeof(ev));
	SDL_TimerID han;
	step_nr = 0;
	han = SDL_AddTimer(60, update, src);
	while (input(&ev, 1) >=0 && step_nr != -1) /* just wait for change */
		game_cursor(CURSOR_ON);
	SDL_RemoveTimer(han);
}
