#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <SDL_version.h>
/* #define GFX_CACHE_SIZE 64
#define GFX_MAX_CACHED_W 256
#define GFX_MAX_CACHED_H 256
#define WORD_CACHE_SIZE		1024
#define LINK_CACHE_SIZE		64
*/

#if SDL_VERSION_ATLEAST(1,3,0)
typedef int		gtimer_t;
#define	NULL_TIMER	0
#else
typedef void*	gtimer_t;
#define	NULL_TIMER	NULL
#endif
struct _img_t;
typedef struct _img_t*	img_t;
typedef void*	fnt_t;
typedef void*	layout_t;
typedef void*	textbox_t;
typedef void*	xref_t;
typedef void*	word_t;

typedef struct {
	int r;
	int g;
	int b;
	int a;
} color_t;

#define ALIGN_LEFT 	1
#define ALIGN_RIGHT 	2
#define ALIGN_CENTER 	4
#define ALIGN_JUSTIFY 	8
#define ALIGN_TOP		16
#define ALIGN_BOTTOM	32
#define ALIGN_MIDDLE	64

#define STYLE_NORMAL	0x00
#define STYLE_BOLD		0x01
#define STYLE_ITALIC	0x02
#define STYLE_UNDERLINE	0x04

static inline color_t gfx_col(int r, int g, int b)
{
	color_t col;
	col.r = r;
	col.g = g;
	col.b = b;
	col.a = 0;
	return col;
}

extern int 	gfx_init(void);
extern void 	gfx_done(void);

extern int	gfx_parse_color (const char *spec, color_t *def);
extern void	gfx_flip(void);
extern void	gfx_commit(void);

extern cache_t	gfx_image_cache(void);
extern img_t	gfx_screen(img_t nscreen);
extern void 	gfx_bg(color_t col);
extern void 	gfx_noclip(void);
extern void 	gfx_clip(int x, int y, int w, int h);
extern void	gfx_getclip(int *x, int *y, int *w, int *h);
extern int 	gfx_width;
extern int 	gfx_height;
extern int	gfx_video_init(void);
extern int 	gfx_set_mode(int w, int h, int fs);
extern int 	gfx_get_mode(int n, int *w, int *h);
extern int	gfx_get_max_mode(int *w, int *h);

extern int	gfx_next_mode(int *w, int *h);
extern int	gfx_prev_mode(int *w, int *h);
extern void	gfx_update(int x, int y, int w, int h);
extern void	gfx_video_done(void);
extern void	gfx_clear(int x, int y, int w, int h);
extern void	gfx_copy_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy);
extern void	gfx_draw(img_t pixmap, int x, int y);
extern void	gfx_draw_wh(img_t p, int x, int y, int w, int h);
extern img_t	gfx_grab_screen(int x, int y, int w, int h);
extern img_t 	gfx_new(int w, int h);
extern img_t	gfx_load_image(char *filename);
extern void	gfx_free_image(img_t pixmap);
extern int	gfx_img_w(img_t pixmap);
extern int	gfx_img_h(img_t pixmap);
extern img_t	gfx_combine(img_t src, img_t dst);
extern void	gfx_set_alpha(img_t src, int alpha);
extern void	gfx_unset_alpha(img_t src);

extern img_t	gfx_alpha_img(img_t src, int alpha);
extern img_t	gfx_display_alpha(img_t src);
extern img_t	gfx_scale(img_t src, float xscale, float yscale, int smooth);
extern img_t	gfx_rotate(img_t src, float angle, int smooth);

extern void	gfx_draw_bg(img_t p, int x, int y, int width, int height);
extern void	gfx_draw_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy);
extern void	gfx_compose_from(img_t p, int x, int y, int width, int height, img_t to, int xx, int yy);

extern void	gfx_cursor(int *xp, int *yp);
extern void	gfx_warp_cursor(int x, int y);
extern void	gfx_change_screen(img_t src, int steps);
extern int	gfx_fading(void);
extern void	gfx_img_fill(img_t img, int x, int y, int w, int h, color_t col);
extern void	gfx_fill(int x, int y, int w, int h, color_t col);
extern int	gfx_get_pixel(img_t src, int x, int y,  color_t *color);
extern int	gfx_set_pixel(img_t src, int x, int y,  color_t color);

extern fnt_t 	fnt_load(const char *fname, int size);
extern void 	fnt_free(fnt_t);
extern int		fnt_height(fnt_t fn);
extern void 	fnt_style(fnt_t fn, int style);
extern img_t	fnt_render(fnt_t fn, const char *p, color_t col);

extern void 	txt_draw(fnt_t fnt, const char *txt, int x, int y, color_t col);
extern void 	txt_size(fnt_t fnt, const char *txt, int *w, int *h);
// extern int 		txt_width(fnt_t fnt, const char *txt);

//extern layout_t txt_layout(fnt_t fn, char *txt, int width, int height);
extern layout_t txt_layout(fnt_t fn, int align, int width, int height);
extern void	txt_layout_add(layout_t lay, char *txt);
extern void	txt_layout_set(layout_t lay, char *txt);
extern void 	txt_layout_draw(layout_t lay, int x, int y);
extern void	txt_layout_free(layout_t lay);
extern void	_txt_layout_free(layout_t lay); /* do not free lay obj */
extern xref_t 	txt_layout_xref(layout_t lay, int x, int y);
extern void 	txt_layout_color(layout_t lay, color_t fg);
extern fnt_t 	txt_layout_font(layout_t lay);
extern void 	txt_layout_font_height(layout_t lay, float height);

extern void 	txt_layout_link_color(layout_t lay, color_t link);
extern void 	txt_layout_active_color(layout_t lay, color_t link);
extern void	txt_layout_link_style(layout_t lay, int style);
extern int 	txt_layout_add_img(layout_t lay, const char *name, img_t img);
extern img_t 	txt_layout_images(layout_t lay, void **v); /* enumerator */
extern word_t 	txt_layout_words(layout_t lay, word_t w); /* enumerator */
extern int	word_geom(word_t v, int *x, int *y, int *w, int *h);
extern img_t	word_image(word_t v);
extern void	txt_layout_size(layout_t lay, int *w, int *h);
extern textbox_t txt_box(int w, int h);
extern layout_t txt_box_layout(textbox_t tbox);
extern void 	txt_box_set(textbox_t tbox, layout_t lay);
extern void 	txt_box_free(textbox_t tbox);
extern void 	txt_box_draw(textbox_t tbox, int x, int y);
extern void 	txt_box_next(textbox_t tbox);
extern void 	txt_box_prev(textbox_t tbox);
extern void 	txt_box_next_line(textbox_t tbox);
extern void 	txt_box_prev_line(textbox_t tbox);
extern void	txt_box_scroll(textbox_t tbox, int disp);
extern xref_t	txt_box_xref(textbox_t tbox, int x, int y);
extern xref_t 	txt_box_xrefs(textbox_t tbox);
extern void	txt_box_real_size(textbox_t box, int *pw, int *ph);

extern int	txt_box_off(textbox_t tbox);
extern void	txt_box_size(textbox_t tbox, int *w, int *h);
extern void 	txt_box_resize(textbox_t tbox, int w, int h);

typedef void 	(*clear_fn)(int x, int y, int w, int h);

extern void 	txt_box_update_links(textbox_t tbox, int x, int y, clear_fn);
extern void 	txt_layout_update_links(layout_t layout, int x, int y, clear_fn clear);
extern void 	txt_layout_real_size(layout_t lay, int *w, int *h);
extern void	txt_layout_set_size(layout_t lay, int w, int h); /* without text only */

extern int 	txt_layout_pos2off(layout_t lay, int pos, int *hh);
extern int 	txt_layout_anchor(layout_t lay, int *hh);

extern img_t 	txt_box_render(textbox_t tbox);

extern char	*xref_get_text(xref_t x);
extern void 	xref_set_active(xref_t x, int val);
extern int	xref_get_active(xref_t x);

extern xref_t	xref_next(xref_t x);
extern xref_t	xref_prev(xref_t x);
extern int 	xref_position(xref_t x, int *xc, int *yc);

extern xref_t	txt_layout_xrefs(layout_t lay);

extern layout_t	xref_layout(xref_t x);

typedef void 	(*update_fn)(int x, int y, int w, int h);
extern void	xref_update(xref_t xref, int x, int y, clear_fn clear, update_fn update);
extern void 	gfx_start_gif(img_t img);
extern int 	gfx_is_drawn_gifs(void);
extern void	gfx_stop_gif(img_t img);
extern int	gfx_frame_gif(img_t img);
extern void	gfx_del_timer(gtimer_t han);
extern gtimer_t gfx_add_timer(int delay, int (*fn)(int, void*), void *aux);
extern void	gfx_update_gif(img_t img);
extern void 	gfx_dispose_gif(img_t p);
extern unsigned long gfx_ticks(void);

extern void gfx_set_cursor(img_t cur, int xc, int yc);

#endif

