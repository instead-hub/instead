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

#include "externals.h"
#include "internals.h"

#define MOTION_TIME (timer_counter - click_time >= 200 / HZ)
int game_running = 1;

char	game_cwd[PATH_MAX];
char	*curgame_dir = NULL;

int game_grab_events = 0;
int game_wait_use = 1;
int game_own_theme = 0;
int game_theme_changed = 0;
unsigned int mouse_filter_delay = 400;

static int need_restart = 0;
static int game_pic_w = 0;
static int game_pic_h = 0;

static	char *last_pict = NULL;
static	char *last_title = NULL;
static	char *last_cmd = NULL;

void game_cursor(int on);

extern void instead_render_callback(void);
extern int instead_render_callback_dirty(int reset);

void game_flip(void)
{
	instead_render_callback();
	gfx_flip();
}

static void _game_update(int x, int y, int w, int h)
{
	if (instead_render_callback_dirty(-1) == 1)
		return;
	gfx_update(x, y, w, h);
}

static void game_update(int x, int y, int w, int h)
{
	if (instead_render_callback_dirty(-1) == 1)
		return;
	game_cursor(CURSOR_DRAW);
	gfx_update(x, y, w, h);
}

void game_res_err_msg(const char *filename, int alert)
{
	static const char preambule[] = "Can't load: ";
	char *msg; unsigned int s;
	if (!filename || (!*filename && !alert))
		return;
	if (alert && curgame_dir) {
		s = sizeof(preambule) + strlen(filename) + 2;
		msg = malloc(s);
		if (msg) {
			snprintf(msg, s, "%s\"%s\"", preambule, filename);
			instead_err_msg(msg);
			free(msg);
		}
	}
	fprintf(stderr, "%s%s\n", preambule, filename);
}

int is_game(const char *path, const char *n)
{
	int rc = 0;
	char *p;
	char *pp;
	if (!n)
		return 0;
	if (!strcmp("..", n) || !strcmp(".", n))
		return 0;
	p = getfilepath(path, n);
	if (!p)
		return 0;

	if (idf_magic(p)) {
		free(p);
		return 1;
	}
/* new api? */
	pp = getfilepath(p, INSTEAD_MAIN3);
	if (!pp)
		goto out;
	if (!access(pp, R_OK))
		rc = 3;
	free(pp);
	if (rc) /* got one! */
		goto out;
/* classic one? */
	pp = getfilepath(p, INSTEAD_MAIN);
	if (!pp)
		goto out;
	if (!access(pp, R_OK))
		rc = 2;
	free(pp);
	if (rc)
		goto out;
out:
	free(p);
	return rc;
}

struct	game *games = NULL;
int	games_nr = 0;

static void game_release(void);

struct game *game_lookup(const char *name)
{
	int i;
	if (!name || !*name) {
		if (games_nr == 1 && name)
			return &games[0];
		return NULL;
	}
	for (i = 0; i<games_nr; i ++) {
		if (!strlowcmp(games[i].dir, name)) {
			return &games[i];
		}
	}
	return NULL;
}

int game_reset(void)
{
	game_release(); /* commit all user events */
	game_release_theme(1);
	if (game_select(curgame_dir))
		goto out;
	if (game_apply_theme())
		goto out;
	return 0;
out:
	game_done(0);
	if (game_init(NULL)) {
		game_error();
		return -1;
	}
	return -1;
}

char *game_reset_name(void)
{
	struct game *g;
	g = game_lookup(curgame_dir);
	if (g) {
		gfx_set_title(g->name);
		return g->name;
	} else {
		gfx_set_title(NULL);
		return NULL;
	}
}
static int game_cfg_load(void);

int game_select(const char *name)
{
	int rc = -1;
	struct game *g;
	// FREE(last_cmd);
	// game_stop_mus(500);
	g = game_lookup(name);
	if ((!name || !*name) && !g) {
		game_use_theme();
		return game_theme_init();
	}
	if (g) {
		char *oldgame = curgame_dir;
		curgame_dir = g->dir;
		instead_done();

		instead_set_debug(debug_sw);
		instead_set_standalone(standalone_sw);

		if (instead_init(g->path)) {
			curgame_dir = oldgame;
			goto err;
		}

		themes_lookup_idf(instead_idf(), "themes/", THEME_GAME);
		if (idf_only(instead_idf(), -1) != 1)
			themes_lookup(dirpath("themes"), THEME_GAME);

		game_cfg_load();

		game_use_theme();

		if (game_theme_init()) {
			curgame_dir = oldgame;
			goto err;
		}
		instead_set_lang(opt_lang);

		if ((rc = instead_load(NULL))) {
			curgame_dir = oldgame;
			goto err;
		}
	} else {
		game_use_theme();
		game_theme_init();
		rc = 0;
	}
err:
	game_reset_name();
	return rc;
}

int game_tag_valid(const char *p)
{
	while (p && *p) {
		p += strcspn(p, "<");
		if (gfx_get_token(p, NULL, NULL, NULL))
			return 0;
		if (*p == '<')
			p ++;
	}
	return 1;
}

static char *game_tag(const char *path, const char *d_name, const char *tag)
{
	char *l = NULL;
	char *p;

	if (idf_magic(path)) {
		idf_t idf = idf_init(path);
		if (idf) {
			l = lookup_lang_tag_idf(idf, INSTEAD_MAIN3, tag, "--", opt_lang);
			if (!l)
				l = lookup_lang_tag_idf(idf, INSTEAD_MAIN, tag, "--", opt_lang);
			idf_done(idf);
		}
		if (l)
			goto ok;
		goto err;
	}
	/* stead3 */
	p = getfilepath(path, INSTEAD_MAIN3);
	if (!p)
		goto err;
	l = lookup_lang_tag(p, tag, "--", opt_lang);
	free(p);
	if (l)
		goto ok;
	/* stead2 */
	p = getfilepath(path, INSTEAD_MAIN);
	if (!p)
		goto err;
	l = lookup_lang_tag(p, tag, "--", opt_lang);
	free(p);
	if (!l)
		goto err;
ok:
	if (!game_tag_valid(l)) { /* avoid dangerous tags */
		free(l);
		return NULL;
	}
	return l;
err:
	return NULL;
}


static char *game_name(const char *path, const char *d_name)
{
	char *p = game_tag(path, d_name, "Name");
	if (!p)
		return strdup(d_name);
	trunc_lines(p, 0);
	return p;
}

static char *game_info(const char *path, const char *d_name)
{
	char *p = game_tag(path, d_name, "Info");
	if (!p)
		return p;
	trunc_lines(p, 4);
	return p;
}

static char *game_author(const char *path, const char *d_name)
{
	char *p = game_tag(path, d_name, "Author");
	trunc_lines(p, 0);
	return p;
}

static char *game_version(const char *path, const char *d_name)
{
	char *p = game_tag(path, d_name, "Version");
	trunc_lines(p, 0);
	return p;
}
#if 0
static char *game_api(const char *path, const char *d_name)
{
	char *p = game_tag(path, d_name, "API");
	trunc_lines(p, 0);
	if (!p)
		return strdup("stead2");
	return p;
}
#endif
static void game_info_free(struct game *g)
{
	FREE(g->name);
	FREE(g->info);
	FREE(g->author);
	FREE(g->version);
/*	FREE(g->api); */
}

static void game_free(struct game *g)
{
	FREE(g->path);
	FREE(g->dir);
	game_info_free(g);
}

static void game_info_fill(struct game *g)
{
	g->name = game_name(dirpath(g->path), g->dir);
	g->info = game_info(dirpath(g->path), g->dir);
	g->author = game_author(dirpath(g->path), g->dir);
	g->version = game_version(dirpath(g->path), g->dir);
/*	g->api = game_api(dirpath(g->path), g->dir); */
}

static void game_fill(struct game *g, const char *p, const char *dir)
{
	g->path = strdup(p);
	g->dir = strdup(dir);
	game_info_fill(g);
}

int games_rename(void)
{
	int i;
	char cwd[PATH_MAX];
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	for (i = 0; i < games_nr; i++) {
		game_info_free(&games[i]);
		game_info_fill(&games[i]);
	}
	setdir(cwd);
	return 0;
}


static int cmp_game(const void *p1, const void *p2)
{
	const struct game *g1 = (const struct game*)p1;
	const struct game *g2 = (const struct game*)p2;
	int g1_s = !!strncmp(g1->path, GAMES_PATH, strlen(GAMES_PATH));
	int g2_s = !!strncmp(g2->path, GAMES_PATH, strlen(GAMES_PATH));
	if (g1_s != g2_s)
		return g1_s - g2_s;
	return strcmp(g1->name, g2->name);
}

static void games_sort()
{
	qsort(games, games_nr, sizeof(struct game), cmp_game);
}


static int games_add(const char *path, const char *dir)
{
	char *p;
	if (!is_game(path, dir))
		return -1;
	p = getfilepath(path, dir);
	if (!p)
		return -1;
	if (!idf_magic(p)) {
		strcat(p, "/");
		games[games_nr].idf = 0;
	} else
		games[games_nr].idf = 1;
	game_fill(&games[games_nr], p, dir);
	free(p);
	games_nr ++;
	return 0;
}

int games_replace(const char *path, const char *dir)
{
	int rc;
	char *p;
	struct game *g;
	struct game *new_games;
	if (!is_game(path, dir))
		return -1;
	g = game_lookup(dir);
	if (g) {
		if (g->idf)
			p = getfilepath(path, dir);
		else
			p = getpath(path, dir);
		if (!p)
			return -1;
		game_free(g);
		game_fill(g, p, dir);
		free(p);
		games_sort();
		return 0;
	}
	new_games = realloc(games, sizeof(struct game) * (1 + games_nr));
	if (!new_games)
		return -1;
	games = new_games;
	rc = games_add(path, dir);
	if (!rc)
		games_sort();
	return rc;
}

int games_lookup(const char *path)
{
	int n = 0, i = 0;
	DIR *d;
	struct dirent *de;
	struct game *new_games;

	if (!path)
		return 0;

	d = opendir(path);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (game_lookup(de->d_name))
			continue;

		if (!is_game(path, de->d_name))
			continue;
		n ++;
	}
	if (!n)
		goto out;
	closedir(d); d = opendir(path);
	if (!d)
		return -1;
	new_games = realloc(games, sizeof(struct game) * (n + games_nr));
	if (!new_games) {
		closedir(d);
		return -1;
	}
	games = new_games;
	while ((de = readdir(d)) && i < n) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (game_lookup(de->d_name))
			continue;

		if (games_add(path, de->d_name))
			continue;
		i ++;
	}
out:
	closedir(d);
	games_sort();
	return 0;
}

int games_remove(int gtr)
{
	int rc;
	struct game *new_games;
	rc = remove_dir(games[gtr].path);
	free(games[gtr].name); free(games[gtr].dir); free(games[gtr].path);
	games_nr --;
	memmove(&games[gtr], &games[gtr + 1], (games_nr - gtr) * sizeof(struct game));
	new_games = realloc(games, games_nr * sizeof(struct game));
	if (new_games) /* failure to shrink otherwise, and it's non-fatal */
		games = new_games;
	return rc;
}

static int motion_mode = 0;
static int motion_id = 0;
static int motion_y = 0;

static int mx, my;
static img_t	menubg = NULL;
static img_t	menu = NULL;

static int menu_shown = 0;
static int browse_dialog = 0;

int game_cmd(char *cmd, int click);

void game_clear(int x, int y, int w, int h)
{
	game_cursor(CURSOR_CLEAR);

	if (game_theme.bg)
		gfx_draw_bg(game_theme.bg, x, y, w, h);
	else
		gfx_clear(x, y, w, h);

	if (menu_shown) {
		int xx = x - mx;
		int yy = y - my;
		gfx_copy_from(menubg, xx, yy, w, h, NULL, x, y);
		gfx_draw_from(menu, xx, yy, w, h, NULL, x, y);
	}
}

void game_clear_all(void)
{
	game_clear(0, 0, game_theme.w, game_theme.h);
}

void game_clear(int x, int y, int w, int h);

struct el {
	int		id;
	int		x;
	int		y;
	int		mx;
	int		my; /* coordinates */
	int		type;
	int		drawn;
/*	int		clone; */
	union {
		layout_t	lay;
		textbox_t	box;
		img_t		img;
		void		*p;
	} p;
};

enum {
	elt_box,
	elt_layout,
	elt_image,
};

enum {
	el_menu = 1,
	el_title,
	el_ways,
	el_inv,
	el_scene,
	el_sup,
	el_sdown,
/*	el_sslide, */
	el_iup,
	el_idown,
/*	el_islide, */
	el_spic,
	el_menu_button,
	el_max,
};

static struct el objs[el_max];

static void	el_set(int i, int t, int x, int y, void *p)
{
	objs[i].id = i;
	objs[i].x = x;
	objs[i].y = y;
	objs[i].p.p = p;
	objs[i].type = t;
	objs[i].drawn = 0;
/*	objs[i].clone = 0; */
}

static void el_zero(int num)
{
	memset(&objs[num], 0, sizeof(struct el));
}

static struct el *el(int num)
{
	return &objs[num];
}
static textbox_t el_box(int num)
{
	return objs[num].p.box;
}

static layout_t el_layout(int num)
{
	return objs[num].p.lay;
}

static img_t el_img(int num)
{
	return objs[num].p.img;
}

char *game_menu_gen(void);

void game_menu(int nr)
{
	mouse_reset(1);
	cur_menu = nr;
	game_menu_box(1, game_menu_gen());
}

int game_error(void)
{
	game_done(1);
	if (game_init(NULL)) {
		fprintf(stderr,"Fatal error! Can't init anything!!!\n");
		exit(1);
	}
	game_menu(menu_error);
	return 0;
}

static void el_draw(int n);

int window_sw = 0;
int fullscreen_sw = 0;
int hires_sw = -1;

int game_load(int nr)
{
	char *s;
	s = game_save_path(0, nr);
	if (s && !access(s, R_OK)) {
		char cmd[PATH_MAX];
		char sav[PATH_MAX];
		strcpy(sav, s);
		snprintf(cmd, sizeof(cmd) - 1, "load %s", s);
		game_cmd(cmd, GAME_CMD_FILE);
		if (nr == -1)
			unlink(sav);
		return 0;
	}
	return -1;
}

int game_saves_enabled(void)
{
	int rc;
	instead_lock();
	instead_function("instead.isEnableSave", NULL);
	rc = instead_bretval(0);
	instead_clear();
	instead_unlock();
	return rc;
}

int game_autosave_enabled(void)
{
	int rc;
	instead_lock();
	instead_function("instead.isEnableAutosave", NULL);
	rc = instead_bretval(0);
	instead_clear();
	instead_unlock();
	return rc;
}

int game_save(int nr)
{
	char *s = game_save_path(1, nr);
	char cmd[PATH_MAX];
	char *p;
	int rc;
	if (!s)
		return -1;

	if (nr == -1 || nr == 0) {
		struct instead_args args_1[] = {
			{ .val = "-1", .type = INSTEAD_NUM },
			{ .val = NULL, }
		};
		struct instead_args args_0[] = {
			{ .val = "0", .type = INSTEAD_NUM },
			{ .val = NULL, }
		};
		if (nr == -1) {
			instead_lock();
			instead_function("instead.autosave", args_1); /* enable saving for -1 */
		} else if (!game_autosave_enabled())
			return 0; /* nothing todo */
		else {
			instead_lock();
			instead_function("instead.autosave", args_0); /* enable saving for 0 */
		}
		instead_clear();
		instead_unlock();
	}
	snprintf(cmd, sizeof(cmd) - 1, "save %s", s);
	instead_lock();
	p = instead_file_cmd(cmd, &rc);
	instead_unlock();
	if (p)
		free(p);
	if (rc || (!p && instead_err())) {
		game_menu(menu_warning);
		return -1;
	}
	data_sync();
	return 0;
}

static int inv_enabled(void)
{
	return (game_theme.inv_mode != INV_MODE_DISABLED);
}


int game_apply_theme(void)
{
	int align = game_theme.win_align;
	layout_t lay = NULL;
	textbox_t box = NULL;

	gfx_bg(game_theme.bgcol);
	if (!DIRECT_MODE)
		game_clear_all();
	gfx_flip();
	if (opt_justify == JUST_NO && align == ALIGN_JUSTIFY)
		align = ALIGN_LEFT;
	else if (opt_justify == JUST_YES && align == ALIGN_LEFT)
		align = ALIGN_JUSTIFY;

	if (!el_box(el_scene)) {
		lay = txt_layout(game_theme.font, align, game_theme.win_w, game_theme.win_h);
		if (!lay)
			return -1;
		box = txt_box(game_theme.win_w, game_theme.win_h);
		if (!box)
			return -1;
		txt_layout_color(lay, game_theme.fgcol);
		txt_layout_link_color(lay, game_theme.lcol);
		/* txt_layout_link_style(lay, 4); */
		txt_layout_active_color(lay, game_theme.acol);
		txt_layout_font_height(lay, game_theme.font_height);
		txt_box_set(box, lay);
		el_set(el_scene, elt_box, game_theme.win_x, 0, box);
	}

	if (inv_enabled()) {
		if (!el_box(el_inv)) {
			lay = txt_layout(game_theme.inv_font, INV_ALIGN(game_theme.inv_mode),
				game_theme.inv_w, game_theme.inv_h);
			if (!lay)
				return -1;
			txt_layout_color(lay, game_theme.icol);
			txt_layout_link_color(lay, game_theme.ilcol);
			txt_layout_active_color(lay, game_theme.iacol);
			txt_layout_font_height(lay, game_theme.inv_font_height);

			box = txt_box(game_theme.inv_w, game_theme.inv_h);
			if (!box)
				return -1;
			txt_box_set(box, lay);
			el_set(el_inv, elt_box, game_theme.inv_x, game_theme.inv_y, box);
		}
	} else
		el_set(el_inv, elt_box, game_theme.inv_x, game_theme.inv_y, NULL);

	if (!el_layout(el_title)) {
		lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
		if (!lay)
			return -1;

		txt_layout_color(lay, game_theme.fgcol);
		txt_layout_link_color(lay, game_theme.lcol);
		txt_layout_active_color(lay, game_theme.acol);
		txt_layout_font_height(lay, game_theme.font_height);

		el_set(el_title, elt_layout, game_theme.win_x, game_theme.win_y, lay);
	}

	if (!el_layout(el_ways)) {
		lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
		if (!lay)
			return -1;

		txt_layout_color(lay, game_theme.fgcol);
		txt_layout_link_color(lay, game_theme.lcol);
		txt_layout_active_color(lay, game_theme.acol);
		txt_layout_font_height(lay, game_theme.font_height);

		el_set(el_ways, elt_layout, game_theme.win_x, 0, lay);
	}

	el_set(el_sdown, elt_image, 0, 0, game_theme.a_down);
	el_set(el_sup, elt_image, 0, 0,  game_theme.a_up);
	el_set(el_idown, elt_image, 0, 0, game_theme.inv_a_down);
	el_set(el_iup, elt_image, 0, 0, game_theme.inv_a_up);

	el_set(el_spic, elt_image, game_theme.win_x, game_theme.win_y, NULL);
	el_set(el_menu, elt_layout, 0, 0, NULL);
	el_set(el_menu_button, elt_image, game_theme.menu_button_x, game_theme.menu_button_y, game_theme.menu_button);

	gfx_set_icon(game_theme.icon);

	if (!DIRECT_MODE) {
		el_draw(el_menu_button);
	}
	return 0;
}

int game_restart(void)
{
	char *og = curgame_dir;
	game_save(-1);
	game_done(0);
	if (game_init(og)) {
		game_error();
		return 0;
	}
	return 0;
}

unsigned long	timer_counter = 0;

gtimer_t timer_han = NULL_TIMER;

static int gfx_commit_event = 0;

static void _game_gfx_commit(void *data)
{
	gfx_commit_event = 0;
	gfx_commit();
}

void game_gfx_commit(int sync)
{
	if (gfx_fading()) /* to avoid flickering */
		return;
	if (gfx_pending()) {
		if (sync) {
			_game_gfx_commit(NULL);
		} else {
			if (!gfx_commit_event) {
				gfx_commit_event ++;
				push_user_event(_game_gfx_commit, NULL);
			}
		}
	}
}

static int game_render_callback_redraw(void);

static void anigif_do(void *data)
{
	void *v;
	img_t img;

	if (browse_dialog || menu_shown || gfx_fading() || minimized())
		return;

	game_cursor(CURSOR_CLEAR);

	if (gfx_frame_gif(el_img(el_spic))) { /* scene */
		game_render_callback_redraw();
		gfx_update_gif(el_img(el_spic), game_update);
	}

	game_cursor(CURSOR_CLEAR);

	for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(el_scene)), &v)); ) { /* scene */
		game_cursor(CURSOR_CLEAR);
		if ((img != el_img(el_spic)) && gfx_frame_gif(img)) {
			game_render_callback_redraw();
			gfx_update_gif(img, game_update);
		}
	}
	game_cursor(CURSOR_CLEAR);

	for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(el_inv)), &v)); ) { /* inv */
		game_cursor(CURSOR_CLEAR);
		if (gfx_frame_gif(img)) {
			game_render_callback_redraw();
			gfx_update_gif(img, game_update);
		}
	}

	game_cursor(CURSOR_CLEAR);

	for (v = NULL; (img = txt_layout_images(el_layout(el_title), &v)); ) { /* title */
		game_cursor(CURSOR_CLEAR);
		if (gfx_frame_gif(img)) {
			game_render_callback_redraw();
			gfx_update_gif(img, game_update);
		}
	}

	game_cursor(CURSOR_CLEAR);

	for (v = NULL; (img = txt_layout_images(el_layout(el_ways), &v)); ) { /* ways */
		game_cursor(CURSOR_CLEAR);
		if (gfx_frame_gif(img)) {
			game_render_callback_redraw();
			gfx_update_gif(img, game_update);
		}
	}
	game_cursor(CURSOR_ON);
	game_flip();
	game_gfx_commit(0);
}

int counter_fn(int interval, void *p)
{
	timer_counter ++;
#ifdef SAILFISHOS /* idle response sometimes */
	if ((timer_counter % 25) == 0) {
		push_user_event(NULL, NULL);
	}
#endif
	if (gfx_is_drawn_gifs() && !DIRECT_MODE)
		push_user_event(anigif_do, NULL);
#ifdef __EMSCRIPTEN__
	if (timer_han) { /* emscripten SDL bug? */
		gfx_del_timer(timer_han);
		timer_han = gfx_add_timer(HZ, counter_fn, NULL);
	}
#endif
	return interval;
}

static int parse_curtheme(const char *v, void *data)
{
	char **p = ((char **)data);
	struct theme *theme;
	theme = theme_lookup(v, THEME_GAME);
	if (theme)
		*p = theme->dir;
	else
		*p = NULL;
	return 0;
}

static struct parser cmd_game_parser[] = {
	{ "theme", parse_curtheme, &curtheme_dir[THEME_GAME], 0 },
};

static int game_cfg_load(void)
{
	char *p = getfilepath(dirname(game_save_path(1, 0)), "config.ini");
	curtheme_dir[THEME_GAME] = NULL;
	if (!p)
		return -1;
	parse_ini(p, cmd_game_parser);
	free(p);
	return 0;
}

int game_cfg_save(void)
{
	FILE *fp;
	char *p;
	if (!curgame_dir)
		return 0;
	if (!curtheme_dir[THEME_GAME]) /* nothing todo */
		return 0;
	p  = getfilepath(dirname(game_save_path(1, 0)), "config.ini");
	if (!p)
		return -1;
	fp = fopen(p, "wb");
	if (fp) {
		if (curtheme_dir[THEME_GAME]) {
			fprintf(fp, "theme = %s\n", curtheme_dir[THEME_GAME]);
		}
		fclose(fp);
	}
	free(p);
	return 0;
}

int game_use_theme(void)
{
	int rc = 0;
	game_theme_changed = 0;
	game_own_theme = 0;

	game_theme.changed = CHANGED_ALL;
	memset(objs, 0, sizeof(struct el) * el_max);

	if (game_default_theme()) {
		fprintf(stderr, "Can't load default theme.\n");
		return -1;
	}

	if (themes_count(THEME_GAME) > 0) { /* new scheme with own themes? */
		game_own_theme = 2;
		if (opt_owntheme) {
			fprintf(stderr, "Using own themes directory...\n");
			if (curtheme_dir[THEME_GAME] && strlowcmp(DEFAULT_THEME, curtheme_dir[THEME_GAME])) {
				rc = game_theme_load(curtheme_dir[THEME_GAME], THEME_GAME);
			} else
				curtheme_dir[THEME_GAME] = DEFAULT_THEME;
			return rc;
		}
	} else if (curgame_dir && (!idf_access(instead_idf(), THEME_FILE) || !access(dirpath(THEME_FILE), R_OK))) {
		game_own_theme = 1;
		if (opt_owntheme) {
			fprintf(stderr, "Using own theme file...\n");
			theme_relative = 1;
			rc = theme_load(THEME_FILE);
			theme_relative = 0;
			return rc;
		}
	}
	if (curtheme_dir[THEME_GLOBAL] && strlowcmp(DEFAULT_THEME, curtheme_dir[THEME_GLOBAL])) {
		rc = game_theme_load(curtheme_dir[THEME_GLOBAL], THEME_GLOBAL);
	}
	return rc;
}

int game_init(const char *name)
{
	getdir(game_cwd, sizeof(game_cwd));
	unix_path(game_cwd);

	if (game_select(name))
		return -1;
	if (game_theme_optimize())
		return -1;

	if (game_apply_theme()) {
		game_theme_select(DEFAULT_THEME);
		return -1;
	}
	timer_han = gfx_add_timer(HZ, counter_fn, NULL);
	if (!curgame_dir) {
		game_menu(menu_games);
	} else {
		if (!game_load(-1)) /* tmp save */
			goto out;
		if ((opt_autosave & 1) && !game_load(0))  /* autosave */
			goto out;
		game_cmd("look", 0);
		custom_theme_warn();
		if (opt_autosave)
			game_save(0);
	}
out:
	return 0;
}

static void game_release(void)
{
	input_uevents(); /* all callbacks */
	if (last_pict)
		free(last_pict);
	if (last_title)
		free(last_title);
	if (last_cmd)
		free(last_cmd);
	last_pict = last_title = last_cmd = NULL;
	// game_stop_mus(500);
	// sounds_free();
}

void game_release_theme(int force)
{
	int i;
	mouse_reset(1);
	game_cursor(CURSOR_OFF);
	if (el_img(el_spic)) {
		gfx_free_image(el_img(el_spic));
		el_zero(el_spic);
	}

	for (i = 0; i < el_max; i++) {
		struct el *o;
		if (!force) {
			switch (i) {
			case el_title:
			case el_ways:
			case el_scene:
				if (!(game_theme.changed & CHANGED_WIN))
					continue;
				break;
			case el_inv:
				if (!(game_theme.changed & CHANGED_INV))
					continue;
				break;
			default:
				break;
			}
		}
		o = el(i);
		if (o->type == elt_layout && o->p.p) {
			txt_layout_free(o->p.lay);
		} else if (o->type == elt_box && o->p.p) {
			txt_layout_free(txt_box_layout(o->p.box));
			txt_box_free(o->p.box);
		}
		el_zero(i);
	}
	if (menu)
		gfx_free_image(menu);
	if (menubg)
		gfx_free_image(menubg);
	menu = menubg = NULL;
	_game_update(0, 0, game_theme.w, game_theme.h);
}

static int game_event(const char *ev);

void game_done(int err)
{
	game_event("quit");
	if (curgame_dir && !err) {
		if (opt_autosave)
			game_save(0);
		game_cfg_save();
	}
	gfx_del_timer(timer_han);
	timer_han = NULL_TIMER;
	if (menu_shown)
		menu_toggle(-1);
	game_release(); /* here all lost user callback are */
	game_release_theme(1);
	game_theme_free();
	themes_drop(THEME_GAME);
	input_clear();

	instead_done();
	curgame_dir = NULL;

/* #ifndef ANDROID
	gfx_video_done();
#endif */
	game_own_theme = 0;
	need_restart = 0;
/*	SDL_Quit(); */
}

static void el_size(int i, int *w, int *h)
{
	int type;
	type = el(i)->type;
	if (type == elt_layout)
		txt_layout_size(el_layout(i), w, h);
	else if (type == elt_box)
		txt_box_size(el_box(i), w, h);
	else if (type  == elt_image) {
		if (w)
			*w = gfx_img_w(el_img(i));
		if (h)
			*h = gfx_img_h(el_img(i));
	} else {
		/* impossible type */
		if (w)
			*w = 0;
		if (h)
			*h = 0;
	}
}

#define el_clear(n) _el_clear(n, game_clear)
#define el_clear_nobg(n) _el_clear(n, NULL)

static int _el_clear(int n, clear_fn clear)
{
	void *v;
	img_t img;
	int x, y, w, h;
	struct el *o;
	o = el(n);
	if (!o || !o->drawn)
		return 0;
	x = o->x;
	y = o->y;
	el_size(n, &w, &h);
	o->drawn = 0;
	if (clear)
		clear(x, y, w, h);
	if (o->type == elt_box) {
		for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(n)), &v)); )
			gfx_dispose_gif(img);
	} else if (o->type == elt_layout) {
		for (v = NULL; (img = txt_layout_images(el_layout(n), &v)); )
			gfx_dispose_gif(img);
	} else if (o->type == elt_image)
		gfx_dispose_gif(el_img(n));

	return 1;
}

static void el_update(int n)
{
	int x, y, w, h;
	struct el *o;
	o = el(n);
/*	if (!o->drawn)
		return; */
	x = o->x;
	y = o->y;
	el_size(n, &w, &h);
	game_update(x, y, w, h);
	return;
}

static void el_clip(int n)
{
	int x, y, w, h;
	struct el *o;
	o = el(n);
	x = o->x;
	y = o->y;
	el_size(n, &w, &h);
	gfx_clip(x, y, w, h);
	return;
}

int box_isscroll_up(int n)
{
	if (el(n)->type != elt_box)
		return -1;
	if (txt_box_off(el_box(n)))
		return 0;
	return -1;
}

int box_isscroll_down(int n)
{
	int off;
	int h, hh;
	if (el(n)->type != elt_box)
		return -1;
	el_size(n, NULL, &h);
	txt_box_real_size(el_box(n), NULL, &hh);
	off = txt_box_off(el_box(n));

	if (hh - off > h)
		return 0;
	return -1;
}

void box_update_scrollbar(int n)
{
	struct el *elup = NULL;
	struct el *eldown = NULL;
/*	struct el *elslide; */

	int x1, y1;
	int x2, y2;

	int off;
	int w, h, hh;

	if (n == el_scene) {
		elup = el(el_sup);
		eldown = el(el_sdown);
		x1 = game_theme.a_up_x;
		y1 = game_theme.a_up_y;
		x2 = game_theme.a_down_x;
		y2 = game_theme.a_down_y;
/*		elslide = el(el_sslide); */
	} else if (n == el_inv) {
		elup = el(el_iup);
		eldown = el(el_idown);
		x1 = game_theme.inv_a_up_x;
		y1 = game_theme.inv_a_up_y;
		x2 = game_theme.inv_a_down_x;
		y2 = game_theme.inv_a_down_y;
/*		elslide = el(el_islide); */
	} else /* impossible case */
		return;
	if (!elup || !eldown)
		return;

/*	if (x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1) */
	el_size(n, &w, &h);

	if (x1 == -1)
		x1 = el(n)->x + w + game_theme.pad;

	if (y1 == -1)
		y1 = el(n)->y;

	if (x2 == -1)
		x2 = x1;

	if (y2 == -1)
		y2 = y1 + h - gfx_img_h(game_theme.a_down);

	txt_box_real_size(el_box(n), NULL, &hh);
	off = txt_box_off(el_box(n));

	if (el_clear(elup->id)) {
		if (elup->x != x1 || elup->y != y1)
			el_update(elup->id);
	}

	if (el_clear(eldown->id)) {
		if (eldown->x != x2 || eldown->y != y2)
			el_update(eldown->id);
	}

	elup->x = x1;
	elup->y = y1;
	eldown->x = x2;
	eldown->y = y2;

	el_clear(elup->id);
	el_clear(eldown->id);

	if (hh - off > h)
		el_draw(eldown->id);
	if (off)
		el_draw(elup->id);
	el_update(elup->id);
	el_update(eldown->id);
}

void el_draw(int n)
{
	int x, y;
	struct el *o;
	o = el(n);
	x = o->x;
	y = o->y;
	if (!o->p.p)
		return;
	game_gfx_clip();
	game_cursor(CURSOR_CLEAR);
	if (o->type == elt_image)
		gfx_draw(o->p.img, x, y);
	else if (o->type == elt_layout)
		txt_layout_draw(o->p.lay, x, y);
	else if (o->type == elt_box) {
		txt_box_draw(o->p.box, x, y);
		box_update_scrollbar(o->id);
	}
	o->drawn = 1;
	game_gfx_noclip();
	return;
}

img_t	game_pict_scale(img_t img, int ww, int hh)
{
	img_t img2 = img;
	int w, h, www, hhh;
	float scale1, scale2, scale = 1.0f;

	game_pic_w = gfx_img_w(img);
	game_pic_h = gfx_img_h(img);

	if (!cache_have(gfx_image_cache(), img)) {
		game_pic_w = (int)((float)game_pic_w / (float)game_theme.scale);
		game_pic_h = (int)((float)game_pic_h / (float)game_theme.scale);
		return img; /* do not scale sprites! */
	}

	if (game_theme.scale > 1.0f)
		theme_img_scale(&img);

	w = gfx_img_w(img);
	h = gfx_img_h(img);

	if (ww == -1)
		ww = w;
	if (hh == -1)
		hh = h;

	if (w <= ww && h <= hh)
		return img;

	www = ww;
	hhh = hh;

	while (scale * (float)w > ww || scale * (float)h > hh) {
		scale1 = (float)(www - 2) / (float)(w);
		scale2 = (float)(hhh - 2) / (float)(h);
		scale = (scale1<scale2) ? scale1:scale2;
		www -= 1;
		hhh -= 1;
		if (www <= 0 || hhh <=0)
			break;
	}
	if (scale < 0)
		scale = 0;
	img2 = gfx_scale(img, scale, scale, SCALABLE_THEME_SMOOTH);
	gfx_free_image(img);
	return img2;
}

int game_menu_box_wh(const char *txt, int *w, int *h)
{
	layout_t lay;
	int b = game_theme.border_w;
	int pad = game_theme.pad;

	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, game_theme.w - 2 * (b + pad), 0);
	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, w, h);
	txt_layout_free(lay);
	return 0;
}

int game_menu_box_width(int show, const char *txt, int width)
{
/*	img_t	menu; */
	int w, h, mw, mh;
	int x, y;
	int b = game_theme.border_w;
	int pad = game_theme.pad;
	layout_t lay = NULL;

	el(el_menu)->drawn = 0;

	if (el_layout(el_menu)) {
		_txt_layout_free(el_layout(el_menu));
		lay = el_layout(el_menu);
	}

	if (menubg) {
		game_cursor(CURSOR_CLEAR);
		gfx_copy(menubg, mx, my);
		gfx_free_image(menubg);
		menubg = NULL;
	}

	if (menu) {
		gfx_free_image(menu);
		menu = NULL;
	}

	if (!DIRECT_MODE) {
		el_clear(el_menu_button);

		if (!show)
			el_draw(el_menu_button);
	}

	if (!show) {
		menu_shown = 0;
		game_cursor(CURSOR_DRAW);
		gfx_flip();
		return 0;
	}
	instead_render_callback();
	menu_shown = 1;

	if (!lay) {
		lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, game_theme.w - 2 * (b + pad), 0);
		txt_layout_color(lay, game_theme.menu_fg);
		txt_layout_link_color(lay, game_theme.menu_link);
		txt_layout_active_color(lay, game_theme.menu_alink);
		txt_layout_font_height(lay, game_theme.menu_font_height);
	}
	else
		txt_layout_set_size(lay, game_theme.w - 2 * (b + pad), 0);

	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);
	if (width)
		w = width;

	txt_layout_set_size(lay, w, h);
	txt_layout_set(lay, (char*)txt);
	menu = gfx_new(w + (b + pad)*2, h + (b + pad)*2);
	gfx_img_fill(menu, 0, 0, w + (b + pad)*2, h + (b + pad)*2, game_theme.border_col);
	gfx_img_fill(menu, b, b, w + pad*2, h + pad*2, game_theme.menu_bg);
	gfx_set_alpha(menu, game_theme.menu_alpha);
	x = (game_theme.w - w)/2;
	y = (game_theme.h - h)/2;
	mx = x - b - pad;
	my = y - b - pad;
	mw = w + (b + pad) * 2;
	mh = h + (b + pad) * 2;

	game_cursor(CURSOR_CLEAR);
	menubg = gfx_grab_screen(mx, my, mw, mh);
	gfx_draw(menu, mx, my);
	el_set(el_menu, elt_layout, /*game_theme.win_x*/  x, y, lay);
	el_draw(el_menu);
	game_cursor(CURSOR_DRAW);
	gfx_flip();
	return 0;
}

int game_menu_box(int show, const char *txt)
{
	int w = 0, rc;
	if (show && cur_menu != menu_wait)
		gfx_cancel_change_screen();
	if (show)
		game_event("pause");
	if (cur_menu == menu_games) { /* hack a bit :( */
		w = games_menu_maxw();
		game_menu_gen();
	} else if (cur_menu == menu_themes) {
		w = themes_menu_maxw();
		game_menu_gen();
	}
	rc = game_menu_box_width(show, txt, w);
	if (!show) {
		game_event("resume");
	}
#ifdef __EMSCRIPTEN__
	if (!show)
		cfg_save();
#endif
	return rc;
}

int check_new_place(char *title)
{
	int rc = 0;
	if (!title && !last_title)
		return 0;

	if (!title && last_title) {
		rc = 1;
	} else if (!last_title || strcmp(title, last_title)) {
		rc = 1;
	}
	if (last_title) {
		free(last_title);
	}
	last_title = title;
	return rc;
}

int check_new_pict(char *pict)
{
	int rc = 0;

	if (!pict && !last_pict)
		return 0;

	if (!pict && last_pict) {
		rc = 1;
	} else if (!last_pict || strcmp(pict, last_pict)) {
		rc = 1;
	}
	if (last_pict) {
		free(last_pict);
	}
	last_pict = pict;
	return rc;
}

static int check_fading(int *new_scene)
{
	int rc;
	int st;
	instead_lock();
	instead_function("instead.get_fading", NULL);
	rc = instead_bretval(0);
	st = instead_iretval(1);

	if (st < 0)
		st = 0;
	else if (st > 255)
		st = 255;

	instead_clear();
	instead_unlock();
	if (new_scene)
		*new_scene = rc;
	return st;
}

static void game_autosave(void)
{
	int b,r;
	if (!curgame_dir)
		return;
	instead_lock();
	instead_function("instead.get_autosave", NULL);
	b = instead_bretval(0);
	r = instead_iretval(1);
	instead_clear();
	instead_unlock();
	if (b) {
		r = r % MAX_SAVE_SLOTS;
		game_save(r);
/*		instead_eval("game.autosave = false;"); instead_clear();*/
	}
}

static void game_instead_restart(void)
{
	int b;
	if (!curgame_dir)
		return;
	instead_lock();
	instead_function("instead.get_restart", NULL);
	b = instead_bretval(0);
	instead_clear();
	instead_unlock();
	need_restart = b;
}

static void game_instead_menu(void)
{
	char *menu;
	if (!curgame_dir)
		return;
	instead_lock();
	instead_function("instead.get_menu", NULL);
	menu = instead_retval(0);
	instead_clear();
	instead_unlock();
	if (!menu)
		return;
	if (!strcmp(menu, "save"))
		menu_toggle(menu_save);
	else if (!strcmp(menu, "load"))
		menu_toggle(menu_load);
	else if (!strcmp(menu, "quit") || !strcmp(menu, "exit"))
		menu_toggle(menu_askquit);
	else if (!strcmp(menu, "themes"))
		menu_toggle(menu_themes);
	else if (!strcmp(menu, "settings"))
		menu_toggle(menu_settings);
	else if (!strcmp(menu, "main"))
		menu_toggle(menu_main);
	else
		menu_toggle(-1);
	free(menu);
}

static char *get_inv(void)
{
	char *ni;
	struct instead_args args[] = {
		{ .val = NULL, .type = INSTEAD_BOOL },
		{ .val = NULL, },
	};
	args[0].val = (INV_MODE(game_theme.inv_mode) == INV_MODE_HORIZ)?"true":"false";
	instead_lock();
	instead_function("instead.get_inv", args);
	ni = instead_retval(0);
	instead_clear();
	instead_unlock();
	return ni;
}

static int find_diff_pos(const char *p1, const char *p2)
{
	int pos = 0;
	if (!p1 || !p2)
		return -1;

	while ((*p1 == *p2) && *p1) {
		p1 ++;
		p2 ++;
		pos ++;
	}
	if (!*p1)
		return -1;
	return pos;
}

static void scroll_to_diff(const char *cmdstr, int cur_off, int new_scene)
{
	int off = 0;
	int pos = 0;
	int h = 0;
	int hh = 0;

	off = txt_layout_anchor(txt_box_layout(el_box(el_scene)), &hh); /* <a:#> tag? */
	if (off == -1) {
		if (new_scene)
			return; /* nothing to do */
		pos = find_diff_pos(cmdstr, last_cmd);
		if (pos != -1)
			off = txt_layout_pos2off(txt_box_layout(el_box(el_scene)), pos, &hh);
		if (off == -1)
			off = cur_off;
	}

	el_size(el_scene, NULL, &h);

	if (game_theme.win_scroll_mode == 2 && (cur_off <= off && cur_off + h >= off + hh)) { /* do not scroll */
		off = cur_off;
	}

	txt_box_scroll(el_box(el_scene), off);
}

static void scroll_to_last(void)
{
	int w, h;

	txt_layout_size(txt_box_layout(el_box(el_scene)), &w, &h);
	txt_box_scroll(el_box(el_scene), h);
}

int game_highlight(int x, int y, int on);

int game_pict_modify(img_t p)
{
	static int modify = 0;
	int last = modify;
	game_bg_modify(p);
	if (p && ((el_img(el_spic) == p) || p == gfx_screen(NULL)))
		modify = 1;
	else if (!p) /* use NULL to reset modify flag */
		modify = 0;
	return last;
}

int game_bg_modify(img_t p)
{
	static int modify = 0;
	int last = modify;
	if (p && p == game_theme.bg)
		modify = 1;
	else if (!p) /* use NULL to reset modify flag */
		modify = 0;
	return last;
}

static void game_pict_clip(void)
{
	int x, y, w, h;

	if (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_EMBEDDED) {
		el_clip(el_scene);
		return;
	}

	if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_FLOAT) {
		x = game_theme.win_x;
		y = game_theme.win_y;
		w = game_theme.win_w;
		h = game_theme.win_h;
	} else {
		x = game_theme.gfx_x;
		y = game_theme.gfx_y;
		w = game_theme.max_scene_w;
		h = game_theme.max_scene_h;
	}
	gfx_clip(x, y, w, h);
}

void game_gfx_clip(void)
{
	if (game_theme.bg) {
		gfx_img_clip(game_theme.bg, game_theme.xoff, game_theme.yoff,
		game_theme.w - 2 * game_theme.xoff,
		game_theme.h - 2 * game_theme.yoff);
	}
	gfx_clip(game_theme.xoff, game_theme.yoff,
		game_theme.w - 2 * game_theme.xoff,
		game_theme.h - 2 * game_theme.yoff);
}

void game_gfx_noclip(void)
{
	if (game_theme.bg) {
		gfx_img_noclip(game_theme.bg);
	}
	gfx_noclip();
}

static void game_redraw_pic(void)
{
	int x, y, ox, oy;
	if (game_pict_coord(&x, &y, NULL, NULL))
		return;
	game_pict_clip();

	ox = el(el_spic)->x;
	oy = el(el_spic)->y;

	el(el_spic)->x = x;
	el(el_spic)->y = y;

	el_clear(el_spic);
	el_draw(el_spic);

	gfx_noclip();
	el_update(el_spic);

	el(el_spic)->x = ox;
	el(el_spic)->y = oy;
}

static xref_t hl_xref = NULL;
static struct el *hl_el = NULL;

static struct {
	img_t offscreen;
	int flags;
	int m_restore;
} fade_ctx;

static void after_click(int flags, int m_restore)
{
	if (DIRECT_MODE)
		return;
#if 1
	{
		int x, y;
		if (!(flags & GAME_CMD_NOHL) &&
			(!m_restore || mouse_restore())) {
			gfx_cursor(&x, &y);
			game_highlight(x, y, 1); /* highlight new scene, to avoid flickering */
		}
	}
#endif
}

extern void instead_ready(void);
static void after_cmd(void)
{
	game_autosave();
	game_instead_restart();
	game_instead_menu();
}

static void after_fading(void *aux)
{
	gfx_start_gif(el_img(el_spic));
	gfx_free_image(fade_ctx.offscreen);
	game_render_callback_redraw();
	after_click(fade_ctx.flags, fade_ctx.m_restore);
	after_cmd();
	game_cursor(CURSOR_DRAW);
	game_flip();
}

void game_redraw_all(void)
{
	if (menu_shown || DIRECT_MODE)
		return;
	game_clear_all();
	el_draw(el_title);
	el_draw(el_ways);
	if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED)
		el_draw(el_spic);
	el_draw(el_scene);
	if (inv_enabled())
		el_draw(el_inv);
	el_draw(el_menu_button);
	game_update(0, 0, game_theme.w, game_theme.h);
}

static int game_render_callback_redraw(void)
{
	if (instead_render_callback_dirty(0) == -1) {
		instead_render_callback_dirty(1); /* disable updates */
		game_redraw_all();
		return 1;
	}
	return 0;
}

int game_cmd(char *cmd, int flags)
{
	int		old_off;
	int		fading = 0;
	int		new_pict = 0;
	int		new_place = 0;
	int		redraw_pict = 0;
	int		title_h = 0, ways_h = 0, pict_h = 0;
	char		buf[1024];
	char		*cmdstr = NULL;
	char		*invstr = NULL;
	char		*waystr = NULL;
	char		*title = NULL;
	char		*pict = NULL;
	img_t		oldscreen = NULL;
	int		dd = (DIRECT_MODE);
	int		rc = 0;
	int		new_scene = 0;
	int		m_restore = 0;
	int		win_spacing;

	if (menu_shown)
		return -1;
/*	if (dd) */
		game_cursor(CURSOR_CLEAR);

	instead_lock();
	if (flags & GAME_CMD_FILE) /* file command */
		cmdstr = instead_file_cmd(cmd, &rc);
	else
		cmdstr = instead_cmd(cmd, &rc);
	instead_unlock();
	instead_ready();
	if (opt_click && (flags & GAME_CMD_CLICK) && !rc)
		sound_play_click();

	if (DIRECT_MODE) {
		if (cmdstr)
			free(cmdstr);

		if (game_theme_changed) { /* cursor change only? */
			img_t offscreen = gfx_new(game_theme.w, game_theme.h);
			if (!offscreen)
				goto fatal;
			oldscreen = gfx_screen(offscreen);
			gfx_copy(oldscreen, 0, 0);
			if ((rc = game_theme_update()))
				goto out;
			offscreen = gfx_screen(oldscreen);
			gfx_change_screen(offscreen, 1, NULL, NULL);
			gfx_free_image(offscreen);
		}

		if (game_pict_modify(NULL))
			goto out;
		return rc;
	} else if (dd) { /* disable direct mode on the fly */
		game_theme_changed = 1;  /* force redraw */
		game_cursor(CURSOR_DRAW);
	}

	if (!cmdstr) {
		game_render_callback_redraw();
		if (game_bg_modify(NULL)) {
			game_redraw_all();
		} else if (game_pict_modify(NULL)) /* redraw pic only */
			game_redraw_pic();
		if (!rc) {
			if (hl_el == el(el_inv)) {
				m_restore = !(flags & GAME_CMD_CLICK);
				mouse_reset(0);
			}
			goto inv; /* hackish? ok, yes  it is... */
		}
		if (instead_render_callback_dirty(-1) == 1) {
			game_cursor(CURSOR_DRAW);
			game_flip();
		}
		goto err; /* really nothing to do */
	}

	if (game_bg_modify(NULL))
		game_theme_changed = 1;  /* force redraw */

	m_restore = !(flags & GAME_CMD_CLICK);
	mouse_reset(0); /* redraw all, so, reset mouse */

	fading = check_fading(&new_scene);

	instead_lock();
	instead_function("instead.get_title", NULL);
	title = instead_retval(0);
	instead_clear();
	instead_unlock();

	new_place = check_new_place(title);

	instead_lock();
	instead_function("instead.get_picture", NULL);
	pict = instead_retval(0);
	instead_clear();
	instead_unlock();

	unix_path(pict);

	new_pict = check_new_pict(pict);

	if (game_theme_changed && !fading)
		fading = 1; /* one frame at least */

	if (fading) { /* take old screen */
		game_cursor(CURSOR_CLEAR);
		img_t offscreen = gfx_new(game_theme.w, game_theme.h);
		if (!offscreen)
			goto fatal;
		oldscreen = gfx_screen(offscreen);
		gfx_copy(oldscreen, 0, 0);
	}

	game_render_callback_redraw();

	if (game_theme_changed) {
		if ((rc = game_theme_update()))
			goto out;
		new_place = 1;
		if (pict)
			new_pict = 1;
	}

	if (new_place)
		el_clear(el_title);

	win_spacing = game_theme.font_size * game_theme.font_height / 2;

	if (title && *title) {
		snprintf(buf, sizeof(buf), "<a:look>%s</a>", title);
		txt_layout_set(el_layout(el_title), buf);
		txt_layout_size(el_layout(el_title), NULL, &title_h);
		title_h += win_spacing;
	} else
		txt_layout_set(el_layout(el_title), NULL);

	if (new_pict || fading || game_pict_modify(NULL) ||
		(new_place && (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_FIXED))) {
		redraw_pict = 1;
	}
	game_pict_clip();

	if (redraw_pict) {
		if (el_img(el_spic)) {
			if (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_EMBEDDED)
				el_clear_nobg(el_spic);
			else
				el_clear(el_spic);
			if (new_pict) {
				gfx_free_image(el_img(el_spic));
				el(el_spic)->p.p = NULL;
				if (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_EMBEDDED &&
					!el(el_spic)->p.p) /* clear embedded gfx */
				txt_layout_add_img(txt_box_layout(el_box(el_scene)),
					"scene", NULL);
			}
		}
	}

	if (pict) {
		int w, h, x;
		img_t img;

		if (new_pict) {
			img = gfx_load_image(pict);
			if (!img)
				game_res_err_msg(pict, debug_sw);
			if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_FLOAT)
				img = game_pict_scale(img, game_theme.win_w, game_theme.max_scene_h);
			else
				img = game_pict_scale(img, game_theme.max_scene_w, game_theme.max_scene_h);
		} else
			img = el_img(el_spic);

		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
			if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_FLOAT) {
				x = (game_theme.win_w - w)/2 + game_theme.win_x;
				if (redraw_pict)
					el_set(el_spic, elt_image, x, game_theme.win_y + title_h, img);
			} else {
				int xx, yy;

				if (GFX_ALIGN(game_theme.gfx_mode) & ALIGN_TOP)
					yy = 0;
				else if (GFX_ALIGN(game_theme.gfx_mode) & ALIGN_BOTTOM)
					yy = game_theme.max_scene_h - h;
				else
					yy = (game_theme.max_scene_h - h)/2;

				if (GFX_ALIGN(game_theme.gfx_mode) & ALIGN_LEFT)
					xx = 0;
				else if (GFX_ALIGN(game_theme.gfx_mode) & ALIGN_RIGHT)
					xx = game_theme.max_scene_w - w;
				else
					xx = (game_theme.max_scene_w - w)/2;

				x = xx + game_theme.gfx_x;
				if (redraw_pict)
					el_set(el_spic, elt_image, x, game_theme.gfx_y + yy, img);
			}
			pict_h = h;
		}
	}
	gfx_noclip();

	/* clear area */
	el_clear(el_ways);

	el_clear(el_scene);

	instead_lock();
	instead_function("instead.get_ways", NULL);
	waystr = instead_retval(0);
	instead_clear();
	instead_unlock();

	if (waystr) {
		int l = strlen(waystr);
		if (l && waystr[l - 1] == '\n')
			waystr[l - 1] = 0;
	}

	if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED) {
		txt_layout_set(el_layout(el_ways), waystr);
		txt_layout_size(el_layout(el_ways), NULL, &ways_h);
		if ((ways_h == 0 || WAYS_BOTTOM) && pict_h != 0)
			pict_h += win_spacing;
	}
	old_off = txt_box_off(el_box(el_scene));

	if (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_EMBEDDED) {
		char *p;
		pict_h = 0; /* to fake code bellow */
		txt_box_resize(el_box(el_scene), game_theme.win_w, game_theme.win_h - title_h - ways_h - pict_h);
		txt_layout_set(txt_box_layout(el_box(el_scene)), ""); /* hack, to null layout, but not images */
		if (el_img(el_spic)) {
			txt_layout_add_img(txt_box_layout(el_box(el_scene)),"scene", el_img(el_spic));
			txt_layout_add(txt_box_layout(el_box(el_scene)), "<c><g:scene></c>\n");
		}
		p = malloc(strlen(cmdstr) + ((waystr)?strlen(waystr):0) + 16);
		if (p) {
			*p = 0;
			if (!WAYS_BOTTOM && waystr && *waystr) {
				strcpy(p, waystr);
				strcat(p, "\n");
			} else if (el_img(el_spic))
				strcat(p, "\n");
			strcat(p, cmdstr);
			if (WAYS_BOTTOM && waystr) {
				strcat(p, "\n");
				strcat(p, waystr);
			}
			free(cmdstr);
			cmdstr = p;
		} else { /* paranoia? Yes... */
			txt_layout_add(txt_box_layout(el_box(el_scene)), waystr);
			txt_layout_add(txt_box_layout(el_box(el_scene)), "<l></l>\n"); /* small hack */
		}
		txt_layout_add(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	} else {
		if (GFX_MODE(game_theme.gfx_mode) == GFX_MODE_FLOAT)
			pict_h = 0;
		txt_box_resize(el_box(el_scene), game_theme.win_w, game_theme.win_h - title_h - ways_h - pict_h);
		txt_layout_set(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	}

	if (WAYS_BOTTOM)
		el(el_ways)->y = game_theme.win_h - ways_h + game_theme.win_y;
	else
		el(el_ways)->y = el(el_title)->y + title_h + pict_h;

	if (waystr)
		free(waystr);

	if (WAYS_BOTTOM)
		el(el_scene)->y = el(el_title)->y + title_h + pict_h;
	else
		el(el_scene)->y = el(el_ways)->y + ways_h;

	/*
	game_clear(game_theme.win_x, game_theme.win_y + pict_h + title_h,
		game_theme.win_w, game_theme.win_h - pict_h - title_h); */

	/* draw title and ways */
	if (new_place)
		el_draw(el_title);

	if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED) {
		el_draw(el_ways);
		if (redraw_pict) {
			game_pict_clip();
			el_draw(el_spic);
			gfx_noclip();
		}
	}

	if (game_theme.win_scroll_mode == 1 || game_theme.win_scroll_mode == 2) {
		scroll_to_diff(cmdstr, old_off, new_scene);
	} else if (game_theme.win_scroll_mode == 3) {
		scroll_to_last();
	}
	FREE(last_cmd);
	last_cmd = cmdstr;

	el_draw(el_scene);

inv:
	if (inv_enabled()) {
		int off;

		invstr = get_inv();

		off = txt_box_off(el_box(el_inv));
		txt_layout_set(txt_box_layout(el_box(el_inv)), invstr);
		txt_box_set(el_box(el_inv), txt_box_layout(el_box(el_inv)));
		txt_box_scroll(el_box(el_inv), off);

		if (invstr)
			free(invstr);

		el_clear(el_inv);
		el_draw(el_inv);
	}

	if (fading) {
		img_t offscreen;
		game_cursor(CURSOR_OFF);
		gfx_stop_gif(el_img(el_spic));
		instead_render_callback();
		offscreen = gfx_screen(oldscreen);
		fade_ctx.offscreen = offscreen;
		fade_ctx.flags = flags;
		fade_ctx.m_restore = m_restore;
		gfx_change_screen(offscreen, fading, after_fading, offscreen);
		return 0;
	}
	after_click(flags, m_restore);
out:
	game_cursor(CURSOR_DRAW);
	game_flip();
/*	input_clear(); */
err:
	after_cmd();
	return rc;
fatal:
	fprintf(stderr, "Fatal error! (can't alloc offscreen)\n");
	exit(1);
}


/*
void game_update(int x, int y, int w, int h)
{
	game_cursor(CURSOR_DRAW);
	gfx_update(x, y, w, h);
}*/

void game_xref_update(xref_t xref, int x, int y)
{
	if (instead_render_callback_dirty(-1) == -1)
		return;
	game_cursor(CURSOR_CLEAR);
	xref_update(xref, x, y, game_clear, game_update);
}

xref_t	use_xref = NULL;


int disable_use(void)
{
	if (use_xref) {
		xref_set_active(use_xref, 0);
		if (xref_layout(use_xref) == txt_box_layout(el_box(el_inv)))
			game_xref_update(use_xref, el(el_inv)->x, el(el_inv)->y);
		else
			game_xref_update(use_xref, el(el_scene)->x, el(el_scene)->y);
		use_xref = NULL;
		return 1;
	}
	return 0;
}

void enable_use(xref_t xref)
{
	use_xref = xref;
	xref_set_active(xref, 1);
	if (xref_layout(use_xref) == txt_box_layout(el_box(el_inv)))
		game_xref_update(use_xref, el(el_inv)->x, el(el_inv)->y);
	else
		game_xref_update(use_xref, el(el_scene)->x, el(el_scene)->y);
}


struct el *look_obj(int x, int y)
{
	int i;
	for (i = 0; i < el_max; i++) {
		int w, h;

		if (el(i)->drawn && el(i)->id == el_menu) {
			return el(i);
		}
		if (x < el(i)->x || y < el(i)->y || !el(i)->drawn)
			continue;
		el_size(i, &w, &h);
		if (x >= el(i)->x && y >= el(i)->y && x < el(i)->x + w && y < el(i)->y + h)
			return el(i);
	}
	return NULL;
}

static xref_t get_nearest_xref(int i, int mx, int my);

xref_t look_xref(int x, int y, struct el **elem)
{
	struct el *o;
	int type;
	xref_t xref = NULL;
	o = look_obj(x, y);
	if (elem)
		*elem = o;
	if (!o)
		return NULL;
	type = o->type;
	if (type == elt_layout)
		xref = txt_layout_xref(o->p.lay, x - o->x, y - o->y);
	else if (type == elt_box)
		xref = txt_box_xref(o->p.box, x - o->x, y - o->y);
#if defined(ANDROID) || defined(IOS) || defined(SAILFISHOS) || defined(WINRT)
	if (!xref) {
		int xc, yc, r;
		xref = get_nearest_xref(o->id, x, y);
		if (!xref)
			return NULL;
		r = fnt_height(txt_layout_font(xref_layout(xref))) * 2; /* radius is here */
		if (!xref_position(xref, &xc, &yc)) {
				if (o->type == elt_box && yc)
					yc -= txt_box_off(el_box(o->id));
				xc += o->x;
				yc += o->y;
				if (((x - xc)*(x - xc) + (y - yc)*(y - yc)) < (r * r))
					return xref;
		}
		return NULL;
	}
#endif
	return xref;
}

static char click_xref[1024];
static struct el *click_el = NULL;
static unsigned long click_time = 0;
static int click_x = -1;
static int click_y = -1;
int menu_visible(void)
{
	if (menu_shown)
		return cur_menu;
	return 0;
}

int game_freezed(void)
{
	return browse_dialog || menu_shown || gfx_fading() || minimized();
}


int game_paused(void)
{
	return game_freezed() || (use_xref && game_wait_use) || instead_busy();
}

void menu_update(struct el *elem)
{
	gfx_copy(menubg, mx, my);
	gfx_draw(menu, mx, my);
	txt_layout_draw(elem->p.lay, elem->x, elem->y);
	gfx_update(mx, my, gfx_img_w(menu), gfx_img_h(menu));
/*	gfx_fill(x, y, w, h, game_theme.menu_bg); */
}

int game_highlight(int x, int y, int on)
{
	struct el	*elem = NULL;
	xref_t		xref = NULL;

	if (on == 1) {
		xref = look_xref(x, y, &elem);
		if (xref && opt_hl && !xref_get_active(xref)) {
			xref_set_active(xref, 1);
			game_xref_update(xref, elem->x, elem->y);
		}
	}

	if (hl_xref != xref && hl_el) {
		if (hl_xref != use_xref && xref_get_active(hl_xref)) {
			xref_set_active(hl_xref, 0);
			game_xref_update(hl_xref, hl_el->x, hl_el->y);
		}
	}

	hl_xref = xref;
	hl_el = elem;

	return 0;
}


void mouse_reset(int hl)
{
	if (hl && (menu_shown || !DIRECT_MODE)) {
		game_highlight(-1, -1, 0);
	} else
		hl_xref = hl_el = NULL;

	disable_use();

	motion_mode = 0;
	click_el = NULL;
	click_xref[0] = 0;
}


void menu_toggle(int menu)
{
	int on = menu_shown;
	mouse_reset(1);
	on ^= 1;
	if (!on)
		cur_menu = menu_main;
	else if (menu != -1)
		cur_menu = menu;
	top_menu = cur_menu;
	game_menu_box(on, game_menu_gen());
	menu_shown = on;
}

static int scroll_pup(int id)
{
	int hh;
	if (box_isscroll_up(id))
		return -1;
	el_size(el_scene, NULL, &hh);
	txt_box_scroll(el_box(id), -hh);
	el_clear(id);
	el_draw(id);
	el_update(id);
	return 0;
}

static int scroll_pdown(int id)
{
	int hh;
	if (box_isscroll_down(id))
		return -1;
	el_size(el_scene, NULL, &hh);
	txt_box_scroll(el_box(id), hh);
	el_clear(id);
	el_draw(id);
	el_update(id);
	return 0;
}

int mouse_filter(int filter)
{
	static unsigned long old_counter = 0;
	if (!opt_filter || !mouse_filter_delay)
		return 0;
	if (filter && (old_counter - timer_counter <= (mouse_filter_delay / HZ))) /* 400 ms */
		return -1;
	old_counter = timer_counter;
	return 0;
}
/* action: 0 - first click,1 - second, -1 - restore */
int game_click(int x, int y, int action, int filter)
{
	int menu_mode	= 0;
	int use_mode	= 0;
	int go_mode	= 0;
	struct el	*elem = NULL;
	char		buf[1024];
	xref_t		xref = NULL;
	char		*xref_txt = NULL;
	xref_t		new_xref = NULL;
	struct el 	*new_elem = NULL;

	int was_motion = (motion_mode == 2);

	if (!action) {
		click_x = x;
		click_y = y;
		motion_y = y;
		click_time = timer_counter;
	} else if (action == 1) {
		click_x = -1;
		click_y = -1;
	}

	if (action)
		motion_mode = 0;

	if (action == 1) {
		char *link;

		new_xref = look_xref(x, y, &new_elem);
		link = (new_xref)?xref_get_text(new_xref):"";

		if (new_elem != click_el || strcmp(link, click_xref)) {
			click_el = NULL;
			new_xref = NULL;
			new_elem = NULL;
			if (click_xref[0]) {
				click_xref[0] = 0;
				return 0; /* just filtered */
			}
		}
	}

	if (action == 1) {
		xref = new_xref;
		elem = new_elem;
		click_xref[0] = 0;
		click_el = NULL;
	} else  { /* just press */
		xref = look_xref(x, y, &elem);
		click_xref[0] = 0;
		if (xref) {
			xref_set_active(xref, 1);
			game_xref_update(xref, elem->x, elem->y);
		} else if (elem && elem->type == elt_box && opt_motion &&
				(!box_isscroll_up(elem->id) || !box_isscroll_down(elem->id))) {
			motion_mode = 1;
			motion_id = elem->id;
			return 0;
		}
		if (xref) {
			snprintf(click_xref, sizeof(click_xref), "%s", xref_get_text(xref));
			click_xref[sizeof(click_xref) - 1] = 0;
		}
		click_el = elem;
		return 0;
	}
	/* now look only second press */


	if (!xref) {
		if (elem) {
			if (elem->id == el_menu_button) {
				menu_toggle(-1);
			} else if (elem->id == el_sdown) {
				scroll_pdown(el_scene);
			} else if (elem->id == el_sup) {
				scroll_pup(el_scene);
			} else if (elem->id == el_idown) {
				scroll_pdown(el_inv);
			} else if (elem->id == el_iup) {
				scroll_pup(el_inv);
			} else disable_use();
/*				el_update(el_inv); */
			motion_mode = 0;
		} else if (!(was_motion && MOTION_TIME)) {
			disable_use();
/*			el_update(el_inv);
			gfx_flip(); */
		}
		return 0;
	}
/* second xref */

	if (elem->id == el_menu) {
/*		xref_set_active(xref, 0);
		txt_layout_update_links(elem->p.lay, elem->x, elem->y, game_clear); */
		if (game_menu_act(xref_get_text(xref))) {
			return -1;
		}
/*		game_menu_box(menu_shown, game_menu_gen());
		gfx_flip(); */
		return 1;
	}

	xref_txt = xref_get_text(xref);

	if (!strncmp("act ", xref_get_text(xref), 4)) {
		menu_mode = 1;
		xref_txt += 4;
	} else if (!strncmp("use ", xref_get_text(xref), 4)) {
		use_mode = 1;
		xref_txt += 4;
	} else if (!strncmp("go ", xref_get_text(xref), 3)) {
		go_mode = 1;
		/* xref_txt += 3; */
	} else if (!strncmp("obj/act ", xref_get_text(xref), 8)) {
		if (!use_xref)
			xref_txt += 4; /* act */
		else
			xref_txt += 8; /* obj */
	} else if (elem->id == el_inv) {
		use_mode = 1;
	}

	if (!use_xref) {
		if (use_mode) {
			enable_use(xref);
/*			el_update(el_inv); */
			return 0;
		}
		if (menu_mode) {
			if (elem->id == el_inv)
				snprintf(buf, sizeof(buf), "use %s", xref_txt);
			else
				snprintf(buf, sizeof(buf), "act %s", xref_txt);
		} else
			snprintf(buf, sizeof(buf), "%s", xref_txt);
		if (mouse_filter(filter))
			return 0;
		buf[sizeof(buf) - 1] = 0;
		game_cmd(buf, GAME_CMD_CLICK);
		return 1;
	}

	if (menu_mode || go_mode || elem->id == el_title)
		return 0;

	if (use_xref == xref)
		snprintf(buf,sizeof(buf), "use %s", xref_txt);
	else {
		if (!strncmp("use ", xref_get_text(use_xref), 4)) /* already use */
			snprintf(buf,sizeof(buf), "%s,%s", xref_get_text(use_xref), xref_txt);
		else
			snprintf(buf,sizeof(buf), "use %s,%s", xref_get_text(use_xref), xref_txt);
	}
	if (mouse_filter(filter))
		return 0;

	disable_use();
	buf[sizeof(buf) - 1] = 0;
	game_cmd(buf, GAME_CMD_CLICK);
	return 1;
}

int mouse_restore(void)
{
	if (click_x == -1 || click_y == -1)
		return -1;
	game_click(click_x, click_y, -1, 0);
	return 0;
}

int game_cursor_show = 1;

void game_cursor(int on)
{
	static img_t	grab = NULL;
	static img_t 	cur = NULL;
	static int xc = 0, yc = 0, w = 0, h = 0; /*, w, h; */
	int xx, yy, ww, hh;
	gfx_getclip(&xx, &yy, &ww, &hh);
	gfx_noclip();
	if (on == CURSOR_OFF)
		cur = NULL;

	if (grab) {
		gfx_copy(grab, xc, yc);
		gfx_free_image(grab);
		grab = NULL;
	}

	if (on == CURSOR_OFF) {
		_game_update(xc, yc, w, h);
		goto out;
	}

	if (on == CURSOR_CLEAR)
		goto out;

	if (on != CURSOR_DRAW)
		cur = (use_xref) ? game_theme.use:game_theme.cursor;
	if (!cur)
		goto out;
	do {
		int ox = xc;
		int oy = yc;
		int ow = w;
		int oh = h;

		if (on != CURSOR_DRAW) {
			gfx_cursor(&xc, &yc);
			xc -= game_theme.cur_x;
			yc -= game_theme.cur_y;
		}

		w = gfx_img_w(cur);
		h = gfx_img_h(cur);

		grab = gfx_grab_screen(xc, yc, w, h);
		if (!nocursor_sw && mouse_focus() && (game_cursor_show || menu_shown))
			gfx_draw(cur, xc, yc);

		if (on != CURSOR_DRAW) {
			_game_update(xc, yc, w, h);
			_game_update(ox, oy, ow, oh);
		}
	} while (0);
out:
	gfx_clip(xx, yy, ww, hh);
	return;
}


static void scroll_up(int id, int count)
{
/*	int i; */
	if (box_isscroll_up(id))
		return;
	txt_box_scroll(el_box(id), -(FONT_SZ(game_theme.font_size)) * count);
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static void scroll_down(int id, int count)
{
/*	int i; */
	if (box_isscroll_down(id))
		return;
	txt_box_scroll(el_box(id), (FONT_SZ(game_theme.font_size)) * count);
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static int scroll_possible(int id, int off)
{
	if (!off || (off > 0 && box_isscroll_down(id)) ||
		(off < 0  && box_isscroll_up(id)))
		return -1;
	return 0;
}

static void scroll_motion(int id, int off)
{
	if (scroll_possible(id, off))
		return;
	game_highlight(-1, -1, 0);
	txt_box_scroll(el_box(id), off);
	el_clear(id);
	el_draw(id);
	el_update(id);
}


static int sel_el = 0;

static void frame_next(void)
{
	if (sel_el == el_scene && !inv_enabled())
		sel_el = el_inv;
	switch(sel_el) {
	default:
	case 0:
		sel_el = el_scene;
		break;
	case el_ways:
		sel_el = el_scene;
		break;
	case el_scene:
		sel_el = el_inv;
		break;
	case el_inv:
		if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED &&
			txt_layout_xrefs(el_layout(el_ways)))
			sel_el = el_ways;
		else
			sel_el = el_scene;
		break;
	}
}

static void frame_prev(void)
{
	switch(sel_el) {
	default:
	case 0:
		sel_el = el_inv;
		break;
	case el_title:
		sel_el = el_inv;
		break;
	case el_ways:
		sel_el = el_inv;
		break;
	case el_scene:
		if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED &&
			txt_layout_xrefs(el_layout(el_ways)))
			sel_el = el_ways;
		else
			sel_el = el_inv;
		break;
	case el_inv:
		sel_el = el_scene;
		break;
	}
	if (sel_el == el_inv && !inv_enabled())
		sel_el = el_scene;
}

static int select_ref(int prev, int last);
static xref_t get_xref(int i, int last);
static void xref_jump(xref_t xref, struct el* elem);


static void select_frame(int prev)
{
	struct el *elem = NULL;
	int x, y, w, h;

	gfx_cursor(&x, &y);

	elem = look_obj(x, y);

	if (elem)
		sel_el = elem->id;

	el(sel_el)->mx = x;
	el(sel_el)->my = y;

	if (menu_shown) {
		sel_el = el_menu;
	} else {
/*		int old_sel;
		if (!sel_el)
			frame_next();
		old_sel = sel_el;
		do { */
			if (prev) {
				frame_prev();
			} else {
				frame_next();
			}
/*		} while (!get_xref(sel_el, 0) && sel_el != old_sel); */
	}
	el_size(sel_el, &w, &h);
	x = el(sel_el)->mx;
	y = el(sel_el)->my;
	if (x < el(sel_el)->x || y < el(sel_el)->y ||
		x > el(sel_el)->x + w || y > el(sel_el)->y + h) {
		x = el(sel_el)->x + w / 2;
		y = el(sel_el)->y + h / 2;
	}

	gfx_warp_cursor(x, y);

	if (!look_xref(x, y, &elem) && elem) {
		xref_t xref = get_nearest_xref(elem->id, x, y);
		xref_jump(xref, elem);
	}
}

static int xref_rel_position(xref_t xref, struct el *elem, int *x, int *y)
{
	int rc = xref_position(xref, x, y);
	if (!rc && elem->type == elt_box && y) {
		*y -= txt_box_off(el_box(elem->id));
	}
	return rc;
}

static int xref_visible(xref_t xref, struct el *elem)
{
	int x, y, w, h;
	if (!elem || !xref)
		return -1;

	if (xref_rel_position(xref, elem, &x, &y))
		return -1;

	el_size(elem->id, &w, &h);
	if (y < 0 || y >= h)
		return -1;
	return 0;
}

static xref_t get_nearest_xref(int i, int mx, int my)
{
	xref_t		xref = NULL;
	xref_t		min_xref = NULL;
	int min_disp = game_theme.h * game_theme.h + game_theme.w * game_theme.w;
	if (!i)
		return NULL;
	for (xref = get_xref(i, 0); !xref_visible(xref, el(i)); xref = xref_next(xref)) {
		int x, y, disp;
		if (xref_rel_position(xref, el(i), &x, &y))
			continue;

		disp = (x + el(i)->x - mx) * (x + el(i)->x - mx) + (y + el(i)->y - my) * (y + el(i)->y - my);
		if (disp < min_disp) {
			min_disp = disp;
			min_xref = xref;
		}
	}
	return min_xref;
}

static xref_t get_xref(int i, int last)
{
	xref_t		xref = NULL;
	int type;
	type = el(i)->type;
	if (type == elt_layout) {
		xref = txt_layout_xrefs(el_layout(i));
		while (last && xref && xref_next(xref))
			xref = xref_next(xref);
	} else if (type == elt_box) {
		xref = txt_box_xrefs(el_box(i));
		while (!last && xref && !xref_visible(xref_prev(xref), el(i))) /* try find visible one */
			xref = xref_prev(xref);
		while (last && xref && !xref_visible(xref_next(xref), el(i)))
			xref = xref_next(xref);
	}
	return xref;
}

static void xref_jump(xref_t xref, struct el* elem)
{
	int x, y;
	if (!elem || !xref || xref_visible(xref, elem) ||
		xref_rel_position(xref, elem, &x, &y))
		return;
	gfx_warp_cursor(elem->x + x, elem->y + y);
}

static int select_ref(int prev, int last)
{
	int x, y;
	struct el	 *elem = NULL;
	xref_t		xref = NULL;
	gfx_cursor(&x, &y);

	xref = look_xref(x, y, &elem);

	if (!elem) {
		if (!sel_el)
			select_frame(0);
		elem = el(sel_el);
	}
	if (last) {
		if (!(xref = get_xref(elem->id, !prev)))
			return -1;
	} else if (xref) {
		if (prev) {
			do {
				xref = xref_prev(xref);
			} while (xref && xref_valid(xref));
			if (!xref || xref_visible(xref, elem)) {
				if (!box_isscroll_up(elem->id) || !box_isscroll_down(elem->id))
					return -1;
				else
					xref = get_xref(elem->id, 1);
			}
		} else {
			do {
				xref = xref_next(xref);
			} while (xref && xref_valid(xref));
			if (!xref || xref_visible(xref, elem)) {
				if (!box_isscroll_down(elem->id) || !box_isscroll_up(elem->id))
					return -1;
				else
					xref = get_xref(elem->id, 0);
			}
		}
	}
	if (!xref)
		xref = get_nearest_xref(elem->id, x, y);
	if (!xref)
		return -1;
	xref_jump(xref, elem);
	return 0;
}

static void game_scroll_up(int count)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		scroll_up(o->id, count);
	}
}

static void game_scroll_down(int count)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		scroll_down(o->id, count);
	}
}

static int game_scroll_pup(void)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		return scroll_pup(o->id);
	}
	return -1;
}

static int game_scroll_pdown(void)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		return scroll_pdown(o->id);
	}
	return -1;
}

static int is_key(struct inp_event *ev, const char *name)
{
	return strcmp(ev->sym, name);
}
int game_pict_coord(int *x, int *y, int *w, int *h)
{
	img_t	img;
	int ww, hh;
	int xx, yy;
	word_t	word;

	img = el_img(el_spic);
	if (!img)
		return -1;
	if (GFX_MODE(game_theme.gfx_mode) != GFX_MODE_EMBEDDED) {
		xx = el(el_spic)->x;
		yy = el(el_spic)->y;
		ww = gfx_img_w(img);
		hh = gfx_img_h(img);
		goto out;
	}
	el_size(el_scene, &ww, &hh);
	for (word = NULL; (word = txt_layout_words(txt_box_layout(el_box(el_scene)), word)); ) { /* scene */
		if (word_image(word) != img) {
			word = NULL;
			/* first word is always pic */
			break;
/*			continue; */
		}
		word_geom(word, &xx, &yy, &ww, &hh);
		yy -= txt_box_off(el_box(el_scene));
		xx += el(el_scene)->x;
		yy += el(el_scene)->y;
		break;
	}
	if (!word)
		return -1;
out:
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

static int game_pic_click(int x, int y, int *ox, int *oy)
{
	int xx, yy, ww, hh;

	if (game_pict_coord(&xx, &yy, &ww, &hh))
		return -1;

	if (x >= xx && y >= yy && x < (xx + ww) && y < (yy + hh)) {
		*ox = x - xx;
		*oy = y - yy;
		if (ww)
			*ox = (int)((float)(*ox) * (float)game_pic_w / (float)ww);
		else
			*ox = 0;
		if (hh)
			*oy = (int)((float)(*oy) * (float)game_pic_h / (float)hh);
		else
			*oy = 0;
		return 0;
	}
	return -1;
}

static int game_bg_click(int mb, int x, int y, int *ox, int *oy)
{
	struct el *o = NULL;
	struct game_theme *t = &game_theme;
	int bg = 1;
	if (x < t->xoff || y < t->yoff || x >= (t->w - t->xoff) || y >= (t->h - t->yoff))
		bg = 0;
	else
		o = look_obj(x, y);
	*ox = (int)((float)(x - t->xoff) / (float)t->scale);
	*oy = (int)((float)(y - t->yoff) / (float)t->scale);

	if (!game_grab_events && ((o && (o->id == el_sup || o->id == el_sdown ||
		o->id == el_iup || o->id == el_idown ||
		o->id == el_menu_button)) ||
		look_xref(x, y, NULL)))
		return -1; /* ask Odyssey for that ;) */

	if (bg || mb == EV_CODE_FINGER) /* fingers area may be larger */
		return 0;
	return -1;
}

static int game_event(const char *ev)
{
	char *p; int rc;
	struct instead_args args[8];
	if (!curgame_dir)
		return -1;
	if (game_paused())
		return -1;
	args[0].val = "event"; args[0].type = INSTEAD_STR;
	args[1].val = ev; args[1].type = INSTEAD_STR;
	args[2].val = NULL;
	instead_lock();
	if (instead_function("iface:input", args)) {
		instead_clear();
		instead_unlock();
		return -1;
	}
	p = instead_retval(0); instead_clear();
	instead_unlock();
	if (!p)
		return -1;
	rc = game_cmd(p, GAME_CMD_NOHL); free(p);
	return (rc)?-1:0;
}

static int game_input(int down, const char *key, int x, int y, int mb)
{
	char *p;
	struct instead_args args[8];
	int rc = 0;

	char tx[16];
	char ty[16];
	char tpx[16];
	char tpy[16];
	char tmb[16];

	if (game_paused())
		return -1;

	if (mb == EV_CODE_KBD) {
		const char *k;
		args[0].val = "kbd"; args[0].type = INSTEAD_STR;
		args[1].val = (down)?"true":"false"; args[1].type = INSTEAD_BOOL;
		k = (key)?key:"unknown";
		args[2].val = (char*)k; args[2].type = INSTEAD_STR;
		args[3].val = NULL;
	} else {
		const char *k;
		int px = -1;
		int py = -1;
		game_pic_click(x, y, &px, &py); /* got picture coord */
		if (game_bg_click(mb, x, y, &x, &y)) { /* no click on bg */
			return -1;
		}
		snprintf(tx, sizeof(tx), "%d", x);
		snprintf(ty, sizeof(ty), "%d", y);
		snprintf(tmb, sizeof(tmb), "%d", mb);
		if (mb == EV_CODE_FINGER) {
			args[0].val = "finger"; args[0].type = INSTEAD_STR;
		} else {
			args[0].val = "mouse"; args[0].type = INSTEAD_STR;
		}
		args[1].val = (down)?"true":"false"; args[1].type = INSTEAD_BOOL;
		if (mb == EV_CODE_FINGER) {
			k = (key)?key:"unknown";
			args[2].val = k; args[2].type = INSTEAD_STR;
		} else {
			args[2].val = tmb; args[2].type = INSTEAD_NUM;
		}
		args[3].val = tx; args[3].type = INSTEAD_NUM;
		args[4].val = ty; args[4].type = INSTEAD_NUM;
		args[5].val = NULL;
		if (px != -1) {
			snprintf(tpx, sizeof(tpx), "%d", px);
			snprintf(tpy, sizeof(tpy), "%d", py);
			args[5].val = tpx; args[5].type = INSTEAD_NUM;
			args[6].val = tpy; args[6].type = INSTEAD_NUM;
			args[7].val = NULL;
		}
	}
	instead_lock();
	if (instead_function("iface:input", args)) {
		instead_clear();
		instead_unlock();
		return -1;
	}

	p = instead_retval(0); instead_clear();
	instead_unlock();
	if (!p) {
		return -1;
	}

	rc = game_cmd(p, (mb != -1)?GAME_CMD_CLICK:0); free(p);

	return (rc)?-1:0;
}

extern char zip_game_dirname[];
extern int unpack(const char *zipfilename, const char *dirname);
#ifdef _USE_BROWSE
int game_from_disk(void)
{
	int i = 0;
	char *g, *p, *b, *d;
	char dir[PATH_MAX];
	char base[PATH_MAX];
#ifndef MAEMO
#ifndef S60
	if (opt_fs) {
		int old_menu = (menu_shown) ? cur_menu: -1;
		opt_fs ^= 1;
		game_restart();
		if (old_menu != -1)
			game_menu(old_menu);
	}
#endif
#endif
	mouse_cursor(1);
	game_cursor(CURSOR_OFF);
	browse_dialog = 1;
	getdir(dir, sizeof(dir));
#ifdef LC_MESSAGES
	setlocale(LC_MESSAGES, "");
#endif
	g = p = open_file_dialog();
#ifdef LC_MESSAGES
	setlocale(LC_MESSAGES, "C");
#endif
	setdir(dir); /* dir can be changed */
	browse_dialog = 0;
	game_cursor(CURSOR_ON);
	mouse_cursor(0);
	gfx_flip();
	if (!p)
		return -1;
	game_done(0);
	strcpy(dir, p);
	strcpy(base, p);
	d = dir; b = base;
	i = strlen(d);
	if (i && d[i - 1] != '/') { /* file */
		if (!idf_magic(d)) {
			d = dirname(d);
			strcpy(b, d);
		}
	}
	d = dirname(d);
	b = basename(b);
#ifdef _USE_UNPACK
	p = games_sw ? games_sw:game_local_games_path(1);
	fprintf(stderr,"Trying to install: %s\n", g);
	if (!unpack(g, p)) {
		if (!zip_game_dirname[0])
			goto err;
		if (games_replace(p, zip_game_dirname))
			goto clean;
		p = zip_game_dirname;
	} else if (zip_game_dirname[0]) { /* error, needs to clean */
		goto clean;
#else
	if (0) {
#endif
	} else if (games_replace(d, b)) {
		goto err;
	} else
		p = b;

	if (game_init(p)) {
		game_error();
	}
	return 0;
#ifdef _USE_UNPACK
clean:
	p = getpath(p, zip_game_dirname);
	fprintf(stderr, "Cleaning: '%s'...\n", p);
	remove_dir(p);
	free(p);
err:
	game_error();
	return -1;
#endif
}
#endif

static int game_input_events(struct inp_event *ev)
{
	if (!curgame_dir)
		return 0;
	if (ev->type == KEY_DOWN || ev->type == KEY_UP) {
		if (!game_input((ev->type == KEY_DOWN), ev->sym, -1, -1, EV_CODE_KBD))
			return 1;
	}
	if (ev->type == FINGER_DOWN || ev->type == FINGER_UP) {
		if (!game_input((ev->type == FINGER_DOWN), ev->sym, ev->x, ev->y, EV_CODE_FINGER))
			return 1;
	}
	if (ev->type == MOUSE_DOWN || ev->type == MOUSE_UP) {
		if (!game_input((ev->type == MOUSE_DOWN), "mouse", ev->x, ev->y, ev->code))
			return 1;
	}
	return 0;
}

static int alt_pressed = 0;
static int shift_pressed = 0;
static int control_pressed = 0;

static int kbd_modifiers(struct inp_event *ev)
{
	if (ev->type != KEY_DOWN && ev->type != KEY_UP)
		return 0;
	if (!is_key(ev, "left alt") || !is_key(ev, "right alt")) {
		alt_pressed = (ev->type == KEY_DOWN);
		return 1;
	} else if (!is_key(ev, "left shift") || !is_key(ev, "right shift")) {
		shift_pressed = (ev->type == KEY_DOWN);
		return 1;
	} else if (!is_key(ev, "left ctrl") || !is_key(ev, "right ctrl")) {
		control_pressed = (ev->type == KEY_DOWN);
		return 1;
	}
	return 0;
}

static int is_key_back(struct inp_event *ev)
{
	if (!is_key(ev, "escape")
#if defined(S60) || defined(_WIN32_WCE) || defined(WINRT)
	    || !is_key(ev, "space")
#endif
#if defined(_WIN32_WCE) || defined(WINRT)
	    || (ev->code >= 0xc0 && ev->code <= 0xcf) ||
	    !is_key(ev, "f1") ||
	    !is_key(ev, "f2") ||
	    !is_key(ev, "f3") ||
	    !is_key(ev, "f4") ||
	    !is_key(ev, "f5")
#endif
#ifdef ANDROID
	    || ev->code == 118
#endif
	    )
		return 0;
	return -1;
}

static int kbd_instead(struct inp_event *ev, int *x, int *y)
{
	if (ev->type != KEY_DOWN)
		return 0;

	if (!is_key_back(ev)) {
		if (use_xref)
			disable_use();
		else
			menu_toggle(-1);
	} else if (!is_key(ev, "f1")) {
		if (!menu_shown)
			menu_toggle(-1);
	} else if (!is_key(ev, "f2") && curgame_dir) {
		game_menu(menu_save);
	} else if (!is_key(ev, "f3") && curgame_dir) {
		game_menu(menu_load);
	} else if (!is_key(ev, "f8") && curgame_dir && !menu_shown) {
		if (game_saves_enabled())
			game_save(9);
	} else if (!is_key(ev, "f9") && curgame_dir && !menu_shown) {
		if (game_saves_enabled()) {
			if (!access(game_save_path(0, 9), R_OK)) {
				if (!game_reset())
					game_load(9);
			}
		}
	} else if (!is_key(ev, "f5") && curgame_dir && !menu_shown) {
		mouse_reset(1);
		game_cmd("look", 0);
	} else if ((alt_pressed || control_pressed) && !is_key(ev, "r") && curgame_dir && !menu_shown && debug_sw) {
		mouse_reset(1);
		game_menu_act("/new");
		shift_pressed = alt_pressed = control_pressed = 0;
	} else if (!is_key(ev, "f10")
#ifdef ANDROID
		   || ev->code == 270
#endif
		   ) {
#ifdef ANDROID
		return -1;
#else
		game_menu(menu_askquit);
#endif
	} else if ((alt_pressed | control_pressed) && (!is_key(ev, "q") || !is_key(ev, "f4"))) {
		game_running = 0;
		return -1;
	} else if (alt_pressed &&
		   (!is_key(ev, "enter") || !is_key(ev, "return"))) {
		int old_menu = (menu_shown) ? cur_menu: -1;
		shift_pressed = alt_pressed = control_pressed = 0;
		opt_fs ^= 1;
		game_restart();
		if (old_menu != -1)
			game_menu(old_menu);
	} else if (!is_key(ev, "f4") && !alt_pressed && !standalone_sw) {
#ifdef _USE_UNPACK
#ifdef _USE_BROWSE
		mouse_reset(1);
		if (!game_from_disk()) {
			shift_pressed = alt_pressed = control_pressed = 0;
		}
#endif
#endif
	} else if (DIRECT_MODE && !menu_shown) {
		; /* nothing todo */
	} else if (!alt_pressed && (!is_key(ev, "return") || !is_key(ev, "enter")
#ifdef S60
				    || !is_key(ev, ".")
#endif
				    )) {
		gfx_cursor(x, y);
		game_highlight(-1, -1, 0); /* reset */

		game_click(*x, *y, 0, 0);
		game_highlight(*x, *y, 1); /* hl on/off */
		game_highlight(*x, *y, 0);

		if (game_click(*x, *y, 1, 0) == -1) {
			game_running = 0;
			return -1;
		}
	} else if (!is_key(ev, "tab")) {
		select_frame(shift_pressed);
	} else if (!is_key(ev, "up") || !is_key(ev, "down") ||
		   !is_key(ev, "[8]") || !is_key(ev, "[2]")) {

		int lm;
		int prev = !is_key(ev, "up") || !is_key(ev, "[8]");

		if (opt_kbd == KBD_INVERSE)
			lm = (alt_pressed || shift_pressed);
		else
			lm = (!alt_pressed && !shift_pressed);

		if (menu_shown || lm) {
			if (select_ref(prev, 0)) {
				if (opt_kbd == KBD_SMART) {
					(prev)?game_scroll_up(1):game_scroll_down(1);
					select_ref(prev, 1);
				} else
					select_ref(prev, 1);
			}
		} else
			(prev)?game_scroll_up(1):game_scroll_down(1);
	} else if (!is_key(ev, "page up") || !is_key(ev, "[9]") ||
		   !is_key(ev, "page down") || !is_key(ev, "[3]")) {
		int lm;
		int prev = !is_key(ev, "page up") || !is_key(ev, "[9]");

		if (opt_kbd == KBD_INVERSE)
			lm = (alt_pressed || shift_pressed);
		else
			lm = (!alt_pressed && !shift_pressed);
		if (menu_shown || lm) {
			if (select_ref(prev, 0) || select_ref(prev, 1)) {
				if (opt_kbd == KBD_SMART) {
					int s = (prev)?game_scroll_pup():game_scroll_pdown();
					if (!s)
						select_ref(!prev, 1);
				} else
					select_ref(prev, 0);
			}
		} else {
			if (prev)
				game_scroll_pup();
			else
				game_scroll_pdown();
		}
#if !defined(S60) && !defined(_WIN32_WCE) && !defined(WINRT)
	} else if (!is_key(ev, "left") || !is_key(ev, "[4]")) {
		select_ref(1, 0);
	} else if (!is_key(ev, "right") || !is_key(ev, "[6]")) {
		select_ref(0, 0);
	} else if (!is_key(ev, "backspace") && !menu_shown) {
		scroll_pup(el_scene);
	} else if (!is_key(ev, "space") && !menu_shown) {
		scroll_pdown(el_scene);
#else
	} else if (!is_key(ev, "left") || !is_key(ev, "[4]")) {
		if (menu_shown)
			select_ref(1, 0);
		else
			select_frame(1);
	} else if (!is_key(ev, "right") || !is_key(ev, "[6]")) {
		if (menu_shown)
			select_ref(0, 0);
		else
			select_frame(0);
#endif
	} else
		return 0;
	return 1;
}

static int mouse_instead(struct inp_event *ev, int *x, int *y)
{
	if (ev->type == MOUSE_DOWN) {
		if (ev->code != 1)
			disable_use();
		else {
			game_highlight(-1, -1, 0);
			game_click(ev->x, ev->y, 0, 1);
			*x = ev->x;
			*y = ev->y;
		}
	} else if (ev->type == MOUSE_UP && ev->code == 1) {
		game_highlight(-1, -1, 0);
		if (game_click(ev->x, ev->y, 1, 1) == -1)
			return -1;
	} else if (ev->type == MOUSE_WHEEL_UP && !menu_shown) {
		game_scroll_up(ev->count);
	} else if (ev->type == MOUSE_WHEEL_DOWN && !menu_shown) {
		game_scroll_down(ev->count);
	} else if (ev->type == MOUSE_MOTION) {
		if (opt_motion && !motion_mode && click_el &&
		    MOTION_TIME &&
		    !scroll_possible(click_el->id, click_y - ev->y)) {
			motion_id = click_el->id;
			motion_y = click_y;
			motion_mode = 1;
			click_el = NULL;
			click_xref[0] = 0;
		}
		if (motion_mode) {
			motion_mode = 2;
			scroll_motion(motion_id, motion_y - ev->y);
			motion_y = ev->y;
		}
		/*	game_highlight(ev.x, ev.y, 1); */
	}
	return 0;
}

static __inline int game_cycle(void)
{
	static int x = 0, y = 0, rc;
	struct inp_event ev;
	ev.x = -1;

	/* game_cursor(CURSOR_CLEAR); */ /* release bg */
	if (((rc = input(&ev, 1)) == AGAIN) && !need_restart) {
		game_gfx_commit(1);
		return rc;
	}

	if (!rc || gfx_fading()) /* just skip */
		return 0;

	if (rc == -1) {/* close */
		goto out;
	} else if (game_input_events(&ev)) { /* kbd, mouse and touch -> pass in game */
		; /* all is done in game_input */
	} else if (kbd_modifiers(&ev)) { /* ctrl, alt, shift */
		; /* got modifiers */
	} else if ((rc = kbd_instead(&ev, &x, &y))) { /* ui keys */
		if (rc < 0)
			goto out;
	} else if (DIRECT_MODE && !menu_shown) {
		; /* nothing todo */
	} else if ((rc = mouse_instead(&ev, &x, &y)) < 0) { /* ui mouse */
		goto out;
	}

	if (gfx_fading()) /* just fading */
		return 0;

	game_render_callback_redraw();
	if (need_restart) {
		need_restart = 0;
		game_menu_act("/new");
	}
	if ((!DIRECT_MODE || menu_shown)) {
		if (click_xref[0]) {
			game_highlight(x, y, 1);
		} else if (!motion_mode) {
			int x, y;
			gfx_cursor(&x, &y);
			game_highlight(x, y, 1);
		}
	}
	game_cursor(CURSOR_ON);
	if (instead_err()) {
		game_menu(menu_warning);
	}
	rc = 0;
out:
	game_flip();
	game_gfx_commit(rc < 0);
	if (rc < 0)
		game_render_callback_redraw();
	return rc;
}
#ifdef __EMSCRIPTEN__
static void game_void_cycle(void)
{
	int rc;
	while ((rc = game_cycle()) == AGAIN);
	if (rc < 0) {
		cfg_save();
		if (curgame_dir)
			game_done(0);
		gfx_clear(0, 0, game_theme.w, game_theme.h);
		gfx_flip();
		gfx_commit();
		emscripten_cancel_main_loop();
		emscripten_force_exit(1);
	}
}
#endif
int game_loop(void)
{
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(game_void_cycle, 0, 0);
	return -1;
#else
	while (game_running) {
		if (game_cycle() < 0) {
			break;
		}
	}
#endif
	return 0;
}

extern int instead_bits_init(void);
extern int instead_timer_init(void);
extern int instead_sprites_init(void);
extern int instead_sound_init(void);
extern int instead_paths_init(void);

int game_instead_extensions(void)
{
	instead_bits_init();
	instead_timer_init();
	instead_sprites_init();
	instead_sound_init();
	instead_paths_init();
	return 0;
}
