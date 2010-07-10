#include "externals.h"
#include "internals.h"

int game_running = 1;

char *err_msg = NULL;

#define ERR_MSG_MAX 512
char	game_cwd[PATH_MAX];
char	*curgame_dir = NULL;

int game_own_theme = 0;

static int game_pic_w = 0;
static int game_pic_h = 0;

void game_cursor(int on);
void mouse_reset(int hl);
static void menu_toggle(void);

void game_err_msg(const char *s)
{
	if (err_msg)
		free(err_msg);
	if (s) {
		err_msg = strdup(s);
		if (err_msg && strlen(err_msg) > ERR_MSG_MAX) {
			err_msg[ERR_MSG_MAX - 4] = 0;
			strcat(err_msg, "...");
		}
	} else
		err_msg = NULL;
}


static int is_game(const char *path, const char *n)
{
	int rc = 0;
	char *p = getpath(path, n);
	char *pp;
	if (!p)
		return 0;
	pp = malloc(strlen(p) + strlen(MAIN_FILE) + 1);
	if (pp) {
		strcpy(pp, p);
		strcat(pp, MAIN_FILE);
		if (!access(pp, R_OK))
			rc = 1;
		free(pp);
	}
	free(p);
	return rc;
}

struct	game *games = NULL;
int	games_nr = 0;

void free_last(void);

static struct game *game_lookup(const char *name)
{
	int i;
	if (!name || !*name) {
		if (games_nr == 1) 
			return &games[0];
		return NULL;
	}
	for (i = 0; i<games_nr; i ++) {
		if (!strcmp(games[i].dir, name)) {
			return &games[i];
		}
	}
	return NULL;
}

int game_select(const char *name)
{
	struct game *g;
	free_last();
	g = game_lookup(name);
	if ((!name || !*name) && !g)
		return 0;
	if (chdir(game_cwd))
		return -1;
	if (g) {
		char *oldgame = curgame_dir;
		curgame_dir = g->dir;
		instead_done();
		if (instead_init()) {
			curgame_dir = oldgame;
			return -1;
		}
		if (chdir(g->path)) {
			curgame_dir = oldgame;
			return -1;
		}
		if (instead_load(MAIN_FILE)) {
			curgame_dir = oldgame;
			return -1;
		}
		instead_eval("game:ini()"); instead_clear();
		return 0;
	}
	return 0;
}


static char *game_name(const char *path, const char *d_name)
{
	char *l;
	char *p = getfilepath(path, MAIN_FILE);
	if (!p)
		goto err;
	l = lookup_tag(p, "Name", "--");
	free(p);
	if (l)
		return l;
err:
	return strdup(d_name);
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
	p = getpath(path, dir);
	if (!p)
		return -1;
	games[games_nr].path = p;
	games[games_nr].dir = strdup(dir);
	games[games_nr].name = game_name(p, dir);
	games_nr ++;
	return 0;
}

int games_replace(const char *path, const char *dir)
{
	int rc;
	char *p;
	struct game *g;
	if (!is_game(path, dir))
		return -1;
	g = game_lookup(dir);
	if (g) {
		p = getpath(path, dir);
		if (!p)
			return -1;
		free(g->path);
		free(g->dir);
		free(g->name);
		g->path = p;
		g->dir = strdup(dir);
		g->name = game_name(p, dir);
		games_sort();
		return 0;
	}
	games = realloc(games, sizeof(struct game) * (1 + games_nr));
	if (!games)
		return -1;
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
		
	rewinddir(d);
	if (!n)
		goto out;
	games = realloc(games, sizeof(struct game) * (n + games_nr));
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
	rc = remove_dir(games[gtr].path);
	free(games[gtr].name); free(games[gtr].dir); free(games[gtr].path);
	games_nr --;
	memmove(&games[gtr], &games[gtr + 1], (games_nr - gtr) * sizeof(struct game));
	games = realloc(games, games_nr * sizeof(struct game));
	return rc;
}

static int motion_mode = 0;
static int motion_id = 0;
static int motion_y = 0;

static		char *last_music = NULL;
static 		char *last_pict = NULL;
static 		char *last_cmd = NULL;
static int mx, my;
static img_t 	menubg = NULL;
static img_t	menu = NULL;

static int menu_shown = 0;
static int browse_dialog = 0;

int game_cmd(char *cmd);
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
		gfx_draw_from(menubg, xx, yy, x, y, w, h); 
		gfx_draw_from(menu, xx, yy, x, y, w, h);
	}
}

void game_clear(int x, int y, int w, int h);

struct el {
	int		id;
	int 		x;
	int 		y;
	int		mx;
	int		my; /* coordinates */
	int 		type;
	int		drawn;
//	int 		clone;
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
//	el_sslide, 
	el_iup, 
	el_idown,
//	el_islide, 
	el_spic, 
	el_menu_button,
	el_max,
};

struct el objs[el_max];

void 	el_set(int i, int t, int x, int y, void *p)
{
	objs[i].id = i;
	objs[i].x = x;
	objs[i].y = y;
	objs[i].p.p = p;
	objs[i].type = t;
	objs[i].drawn = 0;
//	objs[i].clone = 0;
}
void 	el_set_clone(int i, int t, int x, int y, void *p)
{
	el_set(i, t, x, y, p);
//	objs[i].clone = 1;
}

struct el *el(int num)
{
	return &objs[num];
}
textbox_t el_box(int num)
{
	return objs[num].p.box;
}

layout_t el_layout(int num)
{
	return objs[num].p.lay;
}

img_t el_img(int num)
{
	return objs[num].p.img;
}

char *game_menu_gen(void);

void game_menu(int nr)
{
	cur_menu = nr;
	game_menu_box(1, game_menu_gen());
}

int game_error(const char *name)
{
	game_done(1);
	if (game_init(NULL)) {
		fprintf(stderr,"Fatal error! Can't init anything!!!\n");
		exit(1);
	}
	game_menu(menu_error);
	return 0;
}

void el_draw(int n);

int window_sw = 0;
int fullscreen_sw = 0;

int game_load(int nr)
{
	char *s;
	s = game_save_path(0, nr);
	if (s && !access(s, R_OK)) {
		char cmd[PATH_MAX];
		char sav[PATH_MAX];
		strcpy(sav, s);
		snprintf(cmd, sizeof(cmd) - 1, "load %s", s);
		game_cmd(cmd);
		if (nr == -1)
			unlink(sav);
		return 0;
	}
	return -1;
}

int game_saves_enabled(void)
{
	int rc;
	instead_eval("return isEnableSave()");
	rc = instead_bretval(0);
	instead_clear();
	return rc;
}

int game_save(int nr)
{
	char *s = game_save_path(1, nr);
	char cmd[PATH_MAX];
	char *p;
	if (s) {
		if (nr == -1) {
			instead_eval("autosave(-1)"); /* enable saving for -1 */
			instead_clear();
		}
		snprintf(cmd, sizeof(cmd) - 1, "save %s", s);
		p = instead_cmd(cmd);
		if (p)
			free(p);
		if (!instead_bretval(1) || (!p && err_msg)) {
			instead_clear();
			game_menu(menu_warning);
			return -1;
		}
		instead_clear();
		return 0;
	}
	return -1;
}

static int inv_enabled(void)
{
	return (game_theme.inv_mode != INV_MODE_DISABLED);
}

int game_apply_theme(void)
{
	layout_t lay;
	textbox_t box;

	if (game_theme_init(opt_mode[0], opt_mode[1]))
		return -1;
		
	memset(objs, 0, sizeof(struct el) * el_max);

	if (gfx_set_mode(game_theme.w, game_theme.h, opt_fs)) {
		opt_mode[0] = opt_mode[1] = -1; opt_fs = 0; /* safe options */
		return -1;
	}
	
	gfx_bg(game_theme.bgcol);
	game_clear(0, 0, game_theme.w, game_theme.h);
	gfx_flip();
	lay = txt_layout(game_theme.font, ALIGN_JUSTIFY, game_theme.win_w, game_theme.win_h);
	if (!lay)
		return -1;
	box = txt_box(game_theme.win_w, game_theme.win_h);
	if (!box)
		return -1;
	txt_layout_color(lay, game_theme.fgcol);
	txt_layout_link_color(lay, game_theme.lcol);
//	txt_layout_link_style(lay, 4);
	txt_layout_active_color(lay, game_theme.acol);

	txt_box_set(box, lay);
	el_set(el_scene, elt_box, game_theme.win_x, 0, box);


	if (inv_enabled()) {
		lay = txt_layout(game_theme.inv_font, INV_ALIGN(game_theme.inv_mode), 
			game_theme.inv_w, game_theme.inv_h);
		if (!lay)
			return -1;
		txt_layout_color(lay, game_theme.icol);
		txt_layout_link_color(lay, game_theme.ilcol);
		txt_layout_active_color(lay, game_theme.iacol);

		box = txt_box(game_theme.inv_w, game_theme.inv_h);
		if (!box)
			return -1;
		txt_box_set(box, lay);
		el_set(el_inv, elt_box, game_theme.inv_x, game_theme.inv_y, box);
	} else
		el_set(el_inv, elt_box, game_theme.inv_x, game_theme.inv_y, NULL);

	lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
	if (!lay)
		return -1;

	txt_layout_color(lay, game_theme.fgcol);
	txt_layout_link_color(lay, game_theme.lcol);
	txt_layout_active_color(lay, game_theme.acol);

	el_set(el_title, elt_layout, game_theme.win_x, game_theme.win_y, lay);

	lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
	if (!lay)
		return -1;
	
	txt_layout_color(lay, game_theme.fgcol);
	txt_layout_link_color(lay, game_theme.lcol);
	txt_layout_active_color(lay, game_theme.acol);
	
	el_set(el_ways, elt_layout, game_theme.win_x, 0, lay);

	el_set(el_sdown, elt_image, 0, 0, game_theme.a_down);
	el_set(el_sup, elt_image, 0, 0,  game_theme.a_up);
	el_set(el_idown, elt_image, 0, 0, game_theme.inv_a_down);
	el_set(el_iup, elt_image, 0, 0, game_theme.inv_a_up);

	el_set(el_spic, elt_image, game_theme.win_x, game_theme.win_y, NULL);
	el_set(el_menu, elt_layout, 0, 0, NULL);
	el_set(el_menu_button, elt_image, game_theme.menu_button_x, game_theme.menu_button_y, game_theme.menu_button);
	
	el_draw(el_menu_button);
	return 0;
}

int game_restart(void)
{
	char *og = curgame_dir;
	game_save(-1);
	game_done(0);
	if (game_init(og)) {
		game_error(og);
		return 0;
	}
	return 0;
}
int static cur_vol = 0;
void free_last_music(void);

void game_stop_mus(int ms)
{
	snd_stop_mus(ms);
	free_last_music();
}

int game_change_vol(int d, int val)
{
	int v = snd_volume_mus(-1);
	int pc = snd_vol_to_pcn(v);
	int opc = pc;
	if (d) {
		pc += d;
		if (pc < 0)
			pc = 0;
		if (pc > 100)
			pc = 100;
		while (snd_vol_to_pcn(v) != pc)
			v += (d<0)?-1:1;
	} else {
		v = val;
		pc = snd_vol_to_pcn(v);
	}
	if (!pc)
		v = 0;
	snd_volume_mus(v);
	if (opc && !pc) {
		game_stop_mus(0);
	} 
	if (!opc && pc) {
		game_music_player();
	}
	cur_vol = snd_volume_mus(-1);
	opt_vol = cur_vol;
	return 0;
}

int game_change_hz(int hz)
{
	if (!hz)
		return -1;
	snd_done();
	free_last_music();
	snd_init(hz);
	snd_volume_mus(cur_vol);
	snd_free_wav(game_theme.click);
	game_theme.click = snd_load_wav(game_theme.click_name);
	game_music_player();
	opt_hz = snd_hz();
	return 0;
}

unsigned int	timer_counter = 0;

gtimer_t timer_han = NULL;

static void anigif_do(void *data)
{
	void *v;
	img_t img;

	if (browse_dialog || menu_shown || gfx_fading() || minimized())
		return;

	game_cursor(CURSOR_CLEAR);
	
	if (gfx_frame_gif(el_img(el_spic))) { /* scene */
		game_cursor(CURSOR_DRAW);
		gfx_update_gif(el_img(el_spic));
	}
	
	game_cursor(CURSOR_CLEAR);
	
	for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(el_scene)), &v)); ) { /* scene */
		if ((img != el_img(el_spic)) && gfx_frame_gif(img)) {
			game_cursor(CURSOR_DRAW);
			gfx_update_gif(img);
		}
	}
	
	game_cursor(CURSOR_CLEAR);
	
	for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(el_inv)), &v)); ) { /* inv */
		if (gfx_frame_gif(img)) {
			game_cursor(CURSOR_DRAW);
			gfx_update_gif(img);
		}
	}
	
	game_cursor(CURSOR_CLEAR);
	
	for (v = NULL; (img = txt_layout_images(el_layout(el_title), &v)); ) { /* title */
		if (gfx_frame_gif(img)) {
			game_cursor(CURSOR_DRAW);
			gfx_update_gif(img);
		}
	}
	
	game_cursor(CURSOR_CLEAR);
	
	for (v = NULL; (img = txt_layout_images(el_layout(el_ways), &v)); ) { /* ways */
		if (gfx_frame_gif(img)) {
			game_cursor(CURSOR_DRAW);
			gfx_update_gif(img);
		}
	}
	game_cursor(CURSOR_ON);
}

int counter_fn(int interval, void *p)
{
	timer_counter ++;
	if (gfx_is_drawn_gifs())
		push_user_event(anigif_do, NULL);
	return interval;
}

int game_init(const char *name)
{
	getcwd(game_cwd, sizeof(game_cwd));
	unix_path(game_cwd);

	if (name)
		game_err_msg(NULL);

	if (gfx_video_init() || input_init())
		return -1;	

	snd_init(opt_hz);
	game_change_vol(0, opt_vol);

	if (game_default_theme()) {
		fprintf(stderr, "Can't load default theme.\n");
		return -1;
	}

	if (game_select(name))
		return -1;
	
	if (curgame_dir && !access(THEME_FILE, R_OK)) {
		game_own_theme = 1;
	}
	
	if (game_own_theme && opt_owntheme) {
		if (theme_load(THEME_FILE))
			return -1;
	} else if (curtheme_dir && strcmp(DEFAULT_THEME, curtheme_dir)) {
		game_theme_load(curtheme_dir);
	}

	if (game_apply_theme()) {
		game_theme_select(DEFAULT_THEME);
		return -1;
	}
	timer_han =  gfx_add_timer(HZ, counter_fn, NULL); 

	if (!curgame_dir) {
		game_menu(menu_games);
	} else {
		if (!game_load(-1)) /* tmp save */
			return 0;
		if (opt_autosave && !game_load(0))  /* autosave */
			return 0;
		game_cmd("look");
		custom_theme_warn();
		if (opt_autosave)
			game_save(0);
	}
	return 0;
}

void free_last_music(void)
{
	if (last_music)
		free(last_music);
	last_music = NULL;
}

static void sounds_free(void);

void free_last(void)
{
	if (last_pict)
		free(last_pict);
	if (last_cmd)
		free(last_cmd);
	last_pict = last_cmd = NULL;
	game_stop_mus(500);
	sounds_free();
}

void game_done(int err)
{
	int i;

	gfx_del_timer(timer_han);
	timer_han = NULL;

	if (opt_autosave && curgame_dir && !err)
		game_save(0);
	chdir(game_cwd);
//	cfg_save();

	if (menu_shown)
		menu_toggle();
	mouse_reset(1);
	game_cursor(CURSOR_OFF);	
	if (el_img(el_spic))
		gfx_free_image(el_img(el_spic));

	for (i = 0; i < el_max; i++) {
		struct el *o;
		o = el(i);
//		if (o->type == elt_image && o->p.p) {
//			if (!o->clone)
//				gfx_free_image(o->p.img);
//		} else 
		if (o->type == elt_layout && o->p.p) {
			txt_layout_free(o->p.lay);
		} else if (o->type == elt_box && o->p.p) {
			txt_layout_free(txt_box_layout(o->p.box));
			txt_box_free(o->p.box);
		}
		o->p.p = NULL;
		o->drawn = 0;
	}
	free_last();
	if (menu)
		gfx_free_image(menu);
	if (menubg)
		gfx_free_image(menubg);
	menu = menubg = NULL;
	game_theme_free();
	input_clear();
	snd_done();
	instead_done();
	gfx_video_done();
	curgame_dir = NULL;
	game_own_theme = 0;
//	SDL_Quit();
}	

void el_size(int i, int *w, int *h)
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
	} 
}

int el_clear(int n)
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
	game_clear(x, y, w, h);
	if (o->type == elt_box) {
		for (v = NULL; (img = txt_layout_images(txt_box_layout(el_box(n)), &v)); )
			gfx_dispose_gif(img);
	} else if (o->type == elt_layout) {
		for (v = NULL; (img = txt_layout_images(el_layout(n), &v)); )
			gfx_dispose_gif(img);
	}
	return 1;
}

void el_update(int n)
{
	int x, y, w, h;
	struct el *o;
	o = el(n);
//	if (!o->drawn)
//		return;
	x = o->x;
	y = o->y;
	el_size(n, &w, &h);
	game_cursor(CURSOR_DRAW);
	gfx_update(x, y, w, h);
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
	layout_t l;
	int off;
	int h, hh;
	if (el(n)->type != elt_box)
		return -1;
	el_size(n, NULL, &h);
	l = txt_box_layout(el_box(n));
	txt_layout_real_size(l, NULL, &hh);
	off = txt_box_off(el_box(n));

	if (hh - off > h)
		return 0;
	return -1;
}

void box_update_scrollbar(int n)
{
	struct el *elup = NULL;
	struct el *eldown = NULL;
//	struct el *elslide;
	layout_t l;

	int x1, y1;
	int x2, y2;

	int off;
	int w, h, hh;

	el_size(n, &w, &h);

	x1 = el(n)->x + w + game_theme.pad;
	y1 = el(n)->y;

	x2 = x1;
	y2 = y1 + h - gfx_img_h(game_theme.a_down);

	l = txt_box_layout(el_box(n));
	txt_layout_real_size(l, NULL, &hh);
	off = txt_box_off(el_box(n));
	if (n == el_scene) {
		elup = el(el_sup);
		eldown = el(el_sdown);
//		elslide = el(el_sslide);
	} else if (n == el_inv) {
		elup = el(el_iup);
		eldown = el(el_idown);
//		elslide = el(el_islide);
	}
	if (!elup || !eldown)
		return;	

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
	return;
}

img_t	game_pict_scale(img_t img, int ww, int hh)
{
	img_t img2 = img;
	int w, h, www, hhh;
	float scale1, scale2, scale = 1.0f;

	game_pic_w = gfx_img_w(img);
	game_pic_h = gfx_img_h(img);

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
	img2 = gfx_scale(img, scale, scale);
	gfx_free_image(img);
	return img2;
}

int game_menu_box_wh(const char *txt, int *w, int *h)
{
	layout_t lay;
	int b = game_theme.border_w;
	int pad = game_theme.pad;

	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, game_theme.win_w - 2 * (b + pad), 0);
	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, w, h);
	txt_layout_free(lay);
	return 0;
}

void game_menu_box_width(int show, const char *txt, int width)
{	
//	img_t	menu;
	int w, h, mw, mh;
	int x, y;
	int b = game_theme.border_w;
	int pad = game_theme.pad;
	layout_t lay;

	menu_shown = show;
	el(el_menu)->drawn = 0;

	if (el_layout(el_menu)) {
		txt_layout_free(el_layout(el_menu));
		el(el_menu)->p.p = NULL;
	}
	if (menubg) {
		game_cursor(CURSOR_CLEAR);
		gfx_draw(menubg, mx, my);
		gfx_free_image(menubg);
		menubg = NULL;
	}

	el_clear(el_menu_button);
	if (!show)
		el_draw(el_menu_button);

//	el_update(el_menu_button);

	if (!show) {
		game_cursor(CURSOR_DRAW);
		gfx_flip();
		return;
	}


	game_menu_box_wh(txt, &w, &h);

	if (width)
		w = width;

	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, w, 0);

	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);
	if (width)
		w = width;

	txt_layout_color(lay, game_theme.menu_fg);
	txt_layout_link_color(lay, game_theme.menu_link);
	txt_layout_active_color(lay, game_theme.menu_alink);
	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);	
	if (width)
		w = width;
	if (menu) {
		gfx_free_image(menu);
		menu = NULL;
	}
	menu = gfx_new(w + (b + pad)*2, h + (b + pad)*2);
	gfx_img_fill(menu, 0, 0, w + (b + pad)*2, h + (b + pad)*2, game_theme.border_col);
	gfx_img_fill(menu, b, b, w + pad*2, h + pad*2, game_theme.menu_bg);
	gfx_set_alpha(menu, game_theme.menu_alpha);
	x = (game_theme.win_w - w)/2 + game_theme.win_x; //(game_theme.w - w)/2;
	y = (game_theme.win_h - h)/2 + game_theme.win_y; //(game_theme.h - h)/2;
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
}

void game_menu_box(int show, const char *txt)
{
	int w = 0;
	if (cur_menu == menu_games) { /* hack a bit :( */
		w = games_menu_maxw();
		game_menu_gen();
	} else if (cur_menu == menu_themes) {
		w = themes_menu_maxw();
		game_menu_gen();
	}
	return game_menu_box_width(show, txt, w);
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

static int check_fading(void)
{
	int rc;
	int st;
	instead_eval("return get_fading()");
	rc = instead_bretval(0);
	st = instead_iretval(1);
	instead_clear();
	return rc?st:0;
}

void scene_scrollbar(void)
{
	layout_t l;
	int h, off;
	int hh;
	el_clear(el_sdown);
	el_clear(el_sup);
	el_size(el_scene, NULL, &hh);
	el(el_sup)->y = el(el_scene)->y;
	l = txt_box_layout(el_box(el_scene));
	txt_layout_size(l, NULL, &h);
	off = txt_box_off(el_box(el_scene));
	if (h - off >= hh)
		el_draw(el_sdown);
	if (off)
		el_draw(el_sup);
}

static void game_autosave(void)
{
	int b,r;
	if (!curgame_dir)
		return;
	instead_eval("return get_autosave();");
	b = instead_bretval(0); 
	r = instead_iretval(1); 
	instead_clear();
	if (b) {
		r = r % MAX_SAVE_SLOTS;
		game_save(r);
/*		instead_eval("game.autosave = false;"); instead_clear();*/
	}
}

static void dec_music(void *data)
{
	int rc;
	if (!curgame_dir)
		return;
	instead_eval("return dec_music_loop()");
	rc = instead_iretval(0); 
	instead_clear();
	if (rc == -1)
		free_last_music();
	snd_volume_mus(cur_vol); /* reset volume */
}

void game_music_finished(void)
{
	push_user_event(&dec_music, NULL);
}

void game_music_player(void)
{
	int 	loop;
	char		*mus;
	if (!snd_volume_mus(-1))
		return;
	if (!opt_music)
		return;
	instead_eval("return get_music()");
	mus = instead_retval(0);
	loop = instead_iretval(1);
	unix_path(mus);
	instead_clear();

	if (mus && loop == -1) { /* disabled, 0 - forever, 1-n - loops */
		free(mus);
		mus = NULL;
	}
	if (!mus) {
		if (last_music) {
			game_stop_mus(500);
		}
	} else if (!last_music && mus) {
		game_stop_mus(500);
		last_music = mus;
		snd_play_mus(mus, 0, loop - 1);
	} else if (strcmp(last_music, mus)) {
		game_stop_mus(500);
		last_music = mus;
		snd_play_mus(mus, 0, loop - 1);
	} else
		free(mus);
}

#define MAX_WAVS 4

static struct {
	char *fname;
	wav_t wav;
} wavs[MAX_WAVS] = {
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
};

static int wavs_pos = 0;

static wav_t sound_add(const char *fname)
{
	wav_t w;
	w = snd_load_wav(fname);
	if (!w)
		return NULL;
	snd_free_wav(wavs[wavs_pos].wav);
	if (wavs[wavs_pos].fname)
		free(wavs[wavs_pos].fname);
	wavs[wavs_pos].wav = w;
	wavs[wavs_pos].fname = strdup(fname);
	wavs_pos ++;
	if (wavs_pos >= MAX_WAVS)
		wavs_pos = 0;
	return w;
}

static wav_t sound_find(const char *fname)
{
	int i;
	for (i = 0; i < MAX_WAVS; i++) {
		if (wavs[i].fname && !strcmp(wavs[i].fname, fname))
			return wavs[i].wav;
	}
	return NULL;
}

static void sounds_free(void)
{
	int i;
	for (i = 0; i < MAX_WAVS; i++) {
		if (wavs[i].fname)
			free(wavs[i].fname);
		snd_free_wav(wavs[i].wav);
		wavs[i].wav = wavs[i].fname = NULL;
	}
}

void game_sound_player(void)
{
	wav_t		w;
	char		*snd;
	int		chan = -1;
	int		loop = 1;
	
	if (!snd_volume_mus(-1))
		return;	
	instead_eval("return get_sound()");

	loop = instead_iretval(2);
	chan = instead_iretval(1);
	snd = instead_retval(0); 
	instead_clear();
	if (!snd) {
		if (chan != -1) {
			/* halt channel */
			snd_halt_chan(chan, 500);
			instead_eval("set_sound(nil, -1)"); instead_clear();
		}
		return;
	}	
	instead_eval("set_sound(nil, -1)"); instead_clear();
	
	unix_path(snd);
	w = sound_find(snd);
	if (!w)
		w = sound_add(snd);
	free(snd);
	if (!w)
		return;
	snd_play(w, chan, loop - 1);
}

char *horiz_inv(char *invstr)
{
	char *p = invstr;
	char *ns;
	char *np;
	int item = 0;
	if (!p || !(*p))
		return invstr;

	np = ns = malloc((strlen(p) + 2) * 3);

	if (!np)
		return invstr;
	while (*p) {
		if (*p == '\n') {
			if (p[strspn(p, " \n\t")]) {
				if (item) {
					*(np++) = ' ';
					*(np++) = '|';
				}
				*(np) = ' ';
				item = 0;
			} else
				break;
		} else {
			item = 1;
			*np = *p;
		}
		p ++;
		np ++;
	}
	*(np++) = '\n';
	*np = 0;
	free(invstr);
	invstr = ns;
	return invstr;
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

static void scroll_to_diff(const char *cmdstr, int cur_off)
{
	int off = 0;
	int pos = 0;
	int h = 0;
	pos = find_diff_pos(cmdstr, last_cmd);
	if (pos == -1)
		off = cur_off;
	else
		off = txt_layout_pos2off(txt_box_layout(el_box(el_scene)), pos);
	el_size(el_scene, NULL, &h);
	if (cur_off <= off && (cur_off + h) > off)
		off = cur_off;
	txt_box_scroll(el_box(el_scene), off);
}

int game_highlight(int x, int y, int on);



int game_cmd(char *cmd)
{
	int		old_off;
	int		fading = 0;
	int		new_pict = 0;
	int		title_h = 0, ways_h = 0, pict_h = 0;
	char 		buf[1024];
	char 		*cmdstr = NULL;
	char 		*invstr = NULL;
	char 		*waystr = NULL;
	char		*title = NULL;
	char 		*pict = NULL;
	img_t		oldscreen = NULL;
	if (menu_shown)
		return -1;
	cmdstr = instead_cmd(cmd); instead_clear();
//		goto err;

	game_music_player();	
	game_sound_player();

	if (!cmdstr) 
		goto inv; /* hackish? ok, yes  it is... */
	
	instead_eval("return get_title();"); 
	title = instead_retval(0); 
	instead_clear();

	if (title && *title) {
		snprintf(buf, sizeof(buf), "<b><c><a:look>%s</a></c></b>", title);
		txt_layout_set(el_layout(el_title), buf);
	} else
		txt_layout_set(el_layout(el_title), NULL);

	fading = check_fading();

	if (title && *title) {
		txt_layout_size(el_layout(el_title), NULL, &title_h);
		title_h += game_theme.font_size / 2; // todo?	
	}

	instead_eval("return get_picture();");
	pict = instead_retval(0);
	instead_clear();

	unix_path(pict);
	
	new_pict = check_new_pict(pict);

	if (pict) {
		int w, h, x;
		img_t img;

		if (new_pict) {
			img = gfx_load_image(pict);
			if (el_img(el_spic))
				gfx_free_image(el_img(el_spic));
			el(el_spic)->p.p = NULL;
			if (game_theme.gfx_mode != GFX_MODE_FLOAT) 
				img = game_pict_scale(img, game_theme.win_w, game_theme.max_scene_h);
			else
				img = game_pict_scale(img, game_theme.max_scene_w, game_theme.max_scene_h);
		} else
			img = el_img(el_spic);

		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
			if (game_theme.gfx_mode != GFX_MODE_FLOAT) {
				x = (game_theme.win_w - w)/2 + game_theme.win_x;
				el_set(el_spic, elt_image, x, game_theme.win_y + title_h, img);
			} else {
				x = (game_theme.max_scene_w - w)/2 + game_theme.gfx_x;
				el_set(el_spic, elt_image, x, game_theme.gfx_y/* + (game_theme.max_scene_h - h)/2*/, img);
			}
//			if (!game_theme.emb_gfx)
			pict_h = h;
		}
	} else if (el_img(el_spic)) {
		if (game_theme.gfx_mode != GFX_MODE_EMBEDDED)
			el_clear(el_spic);
		gfx_free_image(el_img(el_spic));
		el(el_spic)->p.p = NULL;
	}

	waystr = instead_cmd("way"); instead_clear();

	if (waystr) {
		waystr[strcspn(waystr,"\n")] = 0;
	}

	if (game_theme.gfx_mode != GFX_MODE_EMBEDDED) {
		txt_layout_set(el_layout(el_ways), waystr);
		txt_layout_size(el_layout(el_ways), NULL, &ways_h);
	} 
	old_off = txt_box_off(el_box(el_scene));
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		char *p;
		pict_h = 0; /* to fake code bellow */
		txt_layout_set(txt_box_layout(el_box(el_scene)), ""); /* hack, to null layout, but not images */
		if (el_img(el_spic)) {
			txt_layout_add_img(txt_box_layout(el_box(el_scene)),"scene", el_img(el_spic));
			txt_layout_add(txt_box_layout(el_box(el_scene)), "<c><g:scene></c>\n");
		}
		p = malloc(strlen(cmdstr) + ((waystr)?strlen(waystr):0) + 16);
		if (p) {
			*p = 0;
			if (waystr) {
				strcpy(p, waystr);
				strcat(p, "\n"); /* small hack */
			}
			strcat(p, cmdstr);
			free(cmdstr);
			cmdstr = p;
		} else { /* paranoia? Yes... */
			txt_layout_add(txt_box_layout(el_box(el_scene)), waystr);
			txt_layout_add(txt_box_layout(el_box(el_scene)), "<l></l>\n"); /* small hack */
		}
		txt_layout_add(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	} else {
		if (game_theme.gfx_mode == GFX_MODE_FLOAT) 
			pict_h = 0;	
		txt_layout_set(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	}
	if (!fading)
		scroll_to_diff(cmdstr, old_off);
	FREE(last_cmd);
	last_cmd = cmdstr;
	
	el(el_ways)->y = el(el_title)->y + title_h + pict_h;
	if (waystr)
		free(waystr);

	el(el_scene)->y = el(el_ways)->y + ways_h;
	
	/* draw title and ways */
	if (fading) {
		game_cursor(CURSOR_CLEAR);
		img_t offscreen = gfx_new(game_theme.w, game_theme.h);
		oldscreen = gfx_screen(offscreen);
		gfx_draw(oldscreen, 0, 0);
	}
	if (fading) {
		game_clear(game_theme.win_x, game_theme.win_y, game_theme.win_w, game_theme.win_h);
		if (game_theme.gfx_mode == GFX_MODE_FLOAT) {
			game_clear(game_theme.gfx_x, game_theme.gfx_y, game_theme.max_scene_w, game_theme.max_scene_h);
		}
//		el_draw(el_title);
	} else {
		game_clear(game_theme.win_x, game_theme.win_y + pict_h + title_h, 
			game_theme.win_w, game_theme.win_h - pict_h - title_h);
	}
	
	el_clear(el_title);
	el_draw(el_title);

	if (game_theme.gfx_mode != GFX_MODE_EMBEDDED) {
		el_draw(el_ways);
		if (fading) {
			gfx_dispose_gif(el_img(el_spic));
			el_draw(el_spic);
		}
	}

//	gfx_start_gif(el_img(el_spic));
	
	txt_box_resize(el_box(el_scene), game_theme.win_w, game_theme.win_h - title_h - ways_h - pict_h);
	el_draw(el_scene);
	
inv:
	if (inv_enabled()) {
		int off;

		invstr = instead_cmd("inv"); instead_clear();

		if (invstr && INV_MODE(game_theme.inv_mode) == INV_MODE_HORIZ)
			invstr = horiz_inv(invstr);

		off = txt_box_off(el_box(el_inv));
		txt_layout_set(txt_box_layout(el_box(el_inv)), invstr);
		txt_box_set(el_box(el_inv), txt_box_layout(el_box(el_inv)));
		txt_box_scroll(el_box(el_inv), off);
	
		if (invstr)
			free(invstr);
	
		el_clear(el_inv);
		el_draw(el_inv);
	}
//	scene_scrollbar();

	if (fading) {
		img_t offscreen;
		game_cursor(CURSOR_CLEAR);
		gfx_stop_gif(el_img(el_spic));
		offscreen = gfx_screen(oldscreen);
		gfx_change_screen(offscreen, fading);
		gfx_start_gif(el_img(el_spic));
		gfx_free_image(offscreen);
//		input_clear();
		goto err;
	}
	{ /* highlight new scene, to avoid flickering */
		int x, y;
		gfx_cursor(&x, &y, NULL, NULL);
		game_highlight(x, y, 1);
	}
	game_cursor(CURSOR_DRAW);
	gfx_flip();
//	input_clear();
err:
	game_autosave();
	if (err_msg) {
		mouse_reset(1);
		game_menu(menu_warning);
		return -1;
	}
	return 0;
}

void game_update(int x, int y, int w, int h)
{	
	game_cursor(CURSOR_DRAW);
	gfx_update(x, y, w, h);
}

void game_xref_update(xref_t xref, int x, int y)
{
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
		if (x >= el(i)->x && y >= el(i)->y && x <= el(i)->x + w && y <= el(i)->y + h)
			return el(i);
	}
	return NULL;
}

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
	return xref;
}

static xref_t old_xref = NULL;
static struct el *old_el = NULL;

int game_paused(void)
{
	return browse_dialog || menu_shown || use_xref || old_xref || gfx_fading() || minimized();
}

void menu_update(struct el *elem)
{
	gfx_draw(menubg, mx, my);
	gfx_draw(menu, mx, my);
	txt_layout_draw(elem->p.lay, elem->x, elem->y);
	gfx_update(mx, my, gfx_img_w(menu), gfx_img_h(menu));
//	gfx_fill(x, y, w, h, game_theme.menu_bg);
}


int game_highlight(int x, int y, int on)
{
	struct el 	 *elem = NULL;
	static struct el *oel = NULL;
	static xref_t 	 hxref = NULL;
	xref_t		 xref = NULL;
	if (on == 1) {
		xref = look_xref(x, y, &elem);
		if (xref && opt_hl && !xref_get_active(xref)) {
			xref_set_active(xref, 1);
			game_xref_update(xref, elem->x, elem->y);
		}
	}
	
	if (hxref != xref && oel) {
		if (hxref != use_xref && xref_get_active(hxref)) {
			xref_set_active(hxref, 0);
			if (on != -1)
				game_xref_update(hxref, oel->x, oel->y);
		}
		hxref = NULL;
	}
	hxref = xref;
	oel = elem;
	return 0;
}

void mouse_reset(int hl)
{
	if (hl)
		game_highlight(-1, -1, 0);
	else
		game_highlight(-1, -1, -1);
	disable_use();
	motion_mode = 0;
	old_xref = old_el = NULL;
}

static void menu_toggle(void)
{
	mouse_reset(1);
	menu_shown ^= 1;
	if (!menu_shown)
		cur_menu = menu_main;
	game_menu_box(menu_shown, game_menu_gen());
}

static int scroll_pup(int id)
{
	if (box_isscroll_up(id))
		return -1;		
//	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		int hh;
		el_size(el_scene, NULL, &hh);
		txt_box_scroll(el_box(id), -hh);
	} else
		txt_box_prev(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
	return 0;
}

static int scroll_pdown(int id)
{
	if (box_isscroll_down(id))
		return -1;
//	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		int hh;
		el_size(el_scene, NULL, &hh);
		txt_box_scroll(el_box(id), hh);
	} else
		txt_box_next(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
	return 0;
}

int mouse_filter(int filter)
{
	static unsigned int old_counter = 0;
	if (!opt_filter)
		return 0;
	if (filter && (abs(old_counter - timer_counter) <= (400 / HZ))) /* 400 ms */
		return -1;
	old_counter = timer_counter;
	return 0;
}

int game_click(int x, int y, int action, int filter)
{
	int menu_mode 	= 0;
	int use_mode	= 0;
	int go_mode	= 0;
	struct el	*elem = NULL;
	char 		buf[1024];
	xref_t 		xref = NULL;
	char		*xref_txt;
	
	if (action)
		motion_mode = 0;

	if (filter && opt_filter && action) {
		xref_t new_xref;
		struct el *new_elem;
		new_xref = look_xref(x, y, &new_elem);
		if (new_xref != old_xref || new_elem != old_el) {
			old_el = NULL;
			if (old_xref) {
				old_xref = NULL;
				return 0; /* just filtered */
			}
			old_xref = NULL;
		}
	}
	if (action) {
		xref = old_xref;
		elem = old_el;
		old_xref = NULL;
		old_el = NULL;
	} else  { /* just press */
		xref = look_xref(x, y, &elem);
		if (xref) {
			xref_set_active(xref, 1);
			game_xref_update(xref, elem->x, elem->y);
		} else if (elem && elem->type == elt_box && opt_motion &&
				(!box_isscroll_up(elem->id) || !box_isscroll_down(elem->id))) {
			motion_mode = 1;
			motion_id = elem->id;
			motion_y =y;
			return 0;
		}
		old_xref = xref;
		old_el = elem;
		return 0;
	}
	/* now look only second press */
	
	if (!xref) {
		if (elem) {
			if (elem->id == el_menu_button) {
				menu_toggle();
			} else if (elem->id == el_sdown) {
				scroll_pdown(el_scene);
			} else if (elem->id == el_sup) {
				scroll_pup(el_scene);
			} else if (elem->id == el_idown) {
				scroll_pdown(el_inv);
			} else if (elem->id == el_iup) {
				scroll_pup(el_inv);
			} else if (disable_use());
//				el_update(el_inv);
			motion_mode = 0;
		} else if (disable_use()) {
//			el_update(el_inv);
//			gfx_flip();
		}
		return 0;
	}
/* second xref */
	if (elem->id == el_menu) {
//		xref_set_active(xref, 0);
//		txt_layout_update_links(elem->p.lay, elem->x, elem->y, game_clear);
		if (game_menu_act(xref_get_text(xref))) {
			return -1;
		}
//		game_menu_box(menu_shown, game_menu_gen());
//		gfx_flip();
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
		xref_txt += 3;
	} else if (elem->id == el_inv) {
		use_mode = 1;
	}
	
	if (!use_xref) {
		if (use_mode) {
			enable_use(xref);
//			el_update(el_inv);
			return 0;
		}
		if (menu_mode) {
			if (elem->id == el_inv)
				snprintf(buf, sizeof(buf), "use %s", xref_txt);
			else
				snprintf(buf, sizeof(buf), "act %s", xref_txt);
		} else
			snprintf(buf, sizeof(buf), "%s", xref_get_text(xref));
		if (mouse_filter(filter))
			return 0;
		if (opt_click)
			snd_play(game_theme.click, -1, 0);
		game_cmd(buf);
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

	if (opt_click)
		snd_play(game_theme.click, -1, 0);
		
	game_cmd(buf);
	return 1;
}

void game_cursor(int on)
{
	static img_t	grab = NULL;
	static img_t 	cur = NULL;
	static int xc = 0, yc = 0, w = 0, h = 0; //, w, h;
	int xx, yy, ww, hh;
	
	gfx_getclip(&xx, &yy, &ww, &hh);
	gfx_noclip();
	if (on == CURSOR_OFF)	
		cur = NULL;

	if (grab) {
		gfx_draw(grab, xc, yc);
		gfx_free_image(grab);
		grab = NULL;
	}
	
	if (on == CURSOR_OFF) {
		gfx_update(xc, yc, w, h);
		goto out;
	}

	if (on == CURSOR_CLEAR || on == CURSOR_OFF)
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
			gfx_cursor(&xc, &yc, NULL, NULL);
			xc -= game_theme.cur_x;
			yc -= game_theme.cur_y;
		}
		
		w = gfx_img_w(cur);
		h = gfx_img_h(cur);

		grab = gfx_grab_screen(xc, yc, w, h);
		if (mouse_focus())
			gfx_draw(cur, xc, yc);
	
		if (on != CURSOR_DRAW) {
			gfx_update(xc, yc, w, h);
			gfx_update(ox, oy, ow, oh);
		} 
	} while (0);
out:
	gfx_clip(xx, yy, ww, hh);
	return;
}


static void scroll_up(int id, int count)
{
	int i;
	if (box_isscroll_up(id))
		return;		
//	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED)
		txt_box_scroll(el_box(id), -(FONT_SZ(game_theme.font_size)) * count);
	else
		for (i = 0; i < count; i++)
			txt_box_prev_line(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static void scroll_down(int id, int count)
{
	int i;
	if (box_isscroll_down(id))
		return;		
//	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED)
		txt_box_scroll(el_box(id), (FONT_SZ(game_theme.font_size)) * count);
	else
		for (i = 0; i < count; i++)
			txt_box_next_line(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static void scroll_motion(int id, int off)
{
	game_highlight(-1, -1, 0);
	if (!off || (off > 0 && box_isscroll_down(id)) ||
		(off < 0  && box_isscroll_up(id)))
		return;
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
		if (game_theme.gfx_mode != GFX_MODE_EMBEDDED && 
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
		if (game_theme.gfx_mode != GFX_MODE_EMBEDDED && 
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

static xref_t get_nearest_xref(int i, int mx, int my);

static void select_frame(int prev)
{
	struct el *elem = NULL;
	int x, y, w, h;
	
	gfx_cursor(&x, &y, NULL, NULL);
	
	elem = look_obj(x, y);
	
	if (elem)
		sel_el = elem->id;
	
	el(sel_el)->mx = x;
	el(sel_el)->my = y;

	if (menu_shown) {
		sel_el = el_menu;
	} else {
//		int old_sel;
//		if (!sel_el)
//			frame_next();
//		old_sel = sel_el;
//		do {
			if (prev) {
				frame_prev();
			} else {
				frame_next();
			}
//		} while (!get_xref(sel_el, 0) && sel_el != old_sel);
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
	struct el 	 *elem = NULL;
	xref_t		xref = NULL;
	gfx_cursor(&x, &y, NULL, NULL);
	
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
			if (!(xref = xref_prev(xref)) || xref_visible(xref, elem)) {
				if (!box_isscroll_up(elem->id) || !box_isscroll_down(elem->id))
					return -1;
				else
					xref = get_xref(elem->id, 1);
			}
		} else {
			if (!(xref = xref_next(xref)) || xref_visible(xref, elem)) {
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
	gfx_cursor(&xm, &ym, NULL, NULL);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		scroll_up(o->id, count);
	}
}

static void game_scroll_down(int count)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym, NULL, NULL);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		scroll_down(o->id, count);
	}
}

static int game_scroll_pup(void)
{
	int xm, ym;
	struct el *o;
	gfx_cursor(&xm, &ym, NULL, NULL);
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
	gfx_cursor(&xm, &ym, NULL, NULL);
	o = look_obj(xm, ym);
	if (o && (o->id == el_scene || o->id == el_inv)) {
		return scroll_pdown(o->id);
	}
	return -1;
}

static int is_key(struct inp_event *ev, const char *name)
{
	if (!ev->sym)
		return -1;
	return strcmp(ev->sym, name);
}

static int game_pic_coord(int x, int y, int *ox, int *oy)
{
	int xx, yy, ww, hh;
	img_t	img;
	word_t	word;
	img = el_img(el_spic);
	if (!img)
		return -1;
	if (game_theme.gfx_mode != GFX_MODE_EMBEDDED) {
		xx = el(el_spic)->x;
		yy = el(el_spic)->y;
		ww = gfx_img_w(img);
		hh = gfx_img_h(img);
		goto out;
	}
	el_size(el_scene, &ww, &hh);

	if (x < el(el_scene)->x || y < el(el_scene)->y || x >= el(el_scene)->x + ww ||
			y >= el(el_scene)->y + hh)
		return -1; /* no scene layout */

	for (word = NULL; (word = txt_layout_words(txt_box_layout(el_box(el_scene)), word)); ) { /* scene */
		if (word_image(word) != el_img(el_spic))
			continue;
		word_geom(word, &xx, &yy, &ww, &hh);
		yy -= txt_box_off(el_box(el_scene));
		xx += el(el_scene)->x;
		yy += el(el_scene)->y;
		goto out;
	}
	if (!word)
		return -1;
out:
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

static int game_bg_coord(int x, int y, int *ox, int *oy)
{
	struct el *o;
	struct game_theme *t = &game_theme;
	if (x < t->xoff || y < t->yoff || x >= (t->w - t->xoff) || y >= (t->h - t->yoff))
		return -1;
	o = look_obj(x, y);

	if (o && (o->id == el_sup || o->id == el_sdown ||
		o->id == el_iup || o->id == el_idown ||
		o->id == el_menu_button))
		return -1; /* ask Odyssey for that ;) */

	*ox = (int)((float)(x - t->xoff) / (float)t->scale);
	*oy = (int)((float)(y - t->yoff) / (float)t->scale);
	return 0;
}

static int game_input(int down, const char *key, int x, int y, int mb)
{
	char *p;
	char buf[1024];
	if (game_paused())
		return -1;
	p = encode_esc_string((key)?key:"unknown");
	if (!p)
		return -1;
	
	if (mb == -1)
		snprintf(buf, sizeof(buf), "return stead.input(\"kbd\", %s, \"%s\")", 
			((down)?"true":"false"), p);
	else {
		int px = -1;
		int py = -1;
		game_pic_coord(x, y, &px, &py); /* got picture coord */
		if (game_bg_coord(x, y, &x, &y)) /* no click on bg */
			return -1;
		if (px == -1) {
			snprintf(buf, sizeof(buf), "return stead.input(\"mouse\", %s, %d, %d, %d)", 
				((down)?"true":"false"), mb, x, y);
		} else {
			snprintf(buf, sizeof(buf), "return stead.input(\"mouse\", %s, %d, %d, %d, %d, %d)", 
				((down)?"true":"false"), mb, x, y, px, py);
		}
	}
	free(p);
	if (instead_eval(buf)) {
		instead_clear();
		return -1;
	}

	p = instead_retval(0); instead_clear();
	if (!p)
		return -1;
	mouse_reset(0);
	if (opt_click && mb != -1)
		snd_play(game_theme.click, -1, 0);
	game_cmd(p); free(p);
	return 0;
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
	if (opt_fs) {
		int old_menu = (menu_shown) ? cur_menu: -1;
		opt_fs ^= 1;
		game_restart();
		if (old_menu != -1)
			game_menu(old_menu);
	}
	mouse_cursor(1);
	game_cursor(CURSOR_OFF);
	browse_dialog = 1;
	g = p = open_file_dialog();
	browse_dialog = 0;
	game_cursor(CURSOR_ON);
	mouse_cursor(0);
	if (!p)
		return -1;
	strcpy(dir, p);
	strcpy(base, p);
	d = dir; b = base;
	
	i = strlen(d);
	if (i && d[i - 1] != '/') { /* file */
		d = dirname(d);
		strcpy(b, d);
	}
	d = dirname(d);
	b = basename(b);
/*	fprintf(stderr,"%s:%s\n", d, b); */
#ifdef _USE_UNPACK
	p = games_sw ? games_sw:game_local_games_path(1);
	fprintf(stderr,"Trying to install: %s\n", g);
	if (!unpack(g, p)) {
		if (!zip_game_dirname[0])
			return -1;
		if (games_replace(p, zip_game_dirname))
			goto clean;
		p = zip_game_dirname;
	} else if (zip_game_dirname[0]) { /* error, needs to clean */
		goto clean;
#else
	if (0) {
#endif
	} else if (games_replace(d, b)) {
		return -1;
	} else
		p = b;
	game_done(0);
	if (game_init(p)) {
		game_error(p);
	}
	return 0;
#ifdef _USE_UNPACK
clean:
	p = getpath(p, zip_game_dirname);
	fprintf(stderr, "Cleaning: '%s'...\n", p);
	remove_dir(p);
	free(p);
	return -1;
#endif
}
#endif
int game_loop(void)
{
	static int alt_pressed = 0;
	static int shift_pressed = 0;
	static int x = 0, y = 0;
	struct inp_event ev;
	memset(&ev, 0, sizeof(struct inp_event));
	while (game_running) {
		int rc;
		ev.x = -1;
//		game_cursor(CURSOR_CLEAR); /* release bg */
		while ((rc = input(&ev, 1)) == AGAIN);
		if (rc == -1) {/* close */
			break;
		} else if (curgame_dir && (ev.type == KEY_DOWN || ev.type == KEY_UP)
				&& !game_input((ev.type == KEY_DOWN), ev.sym, -1, -1, -1)) {
			; /* all is done in game_input */
		} else if (curgame_dir && (ev.type == MOUSE_DOWN || ev.type == MOUSE_UP)
				&& !game_input((ev.type == MOUSE_DOWN), "mouse", ev.x, ev.y, ev.code)) {
			; /* all is done in game_input */
		} else if (((ev.type ==  KEY_DOWN) || (ev.type == KEY_UP)) && 
			(!is_key(&ev, "left alt") || !is_key(&ev, "right alt"))) {
			alt_pressed = (ev.type == KEY_DOWN) ? 1:0;
		} else if (((ev.type ==  KEY_DOWN) || (ev.type == KEY_UP)) && 
			(!is_key(&ev,"left shift") || !is_key(&ev, "right shift"))) {
			shift_pressed = (ev.type == KEY_DOWN) ? 1:0;
		} else if (ev.type == KEY_DOWN) {
			if (!is_key(&ev, "f2") && curgame_dir) {
				mouse_reset(1);
				game_menu(menu_save);
			} else if (!is_key(&ev, "f3") && curgame_dir) {
				mouse_reset(1);
				game_menu(menu_load);
			} else if (!is_key(&ev, "f8") && curgame_dir && !menu_shown) {
				game_save(9);
			} else if (!is_key(&ev, "f9") && curgame_dir && !menu_shown) {
				if (!access(game_save_path(0, 9), R_OK)) {
					mouse_reset(1);
					game_select(curgame_dir);
					game_load(9);
				}	
			} else if (!is_key(&ev, "f5") && curgame_dir && !menu_shown) {
				mouse_reset(1);
				game_cmd("look");
			} else if (alt_pressed && !is_key(&ev, "r") && curgame_dir && !menu_shown && debug_sw) {
				mouse_reset(1);
				game_menu_act("/new");
				shift_pressed = alt_pressed = 0;
			} else if (!is_key(&ev, "f10")) {
				mouse_reset(1);
				game_menu(menu_askquit);
			} else if (!alt_pressed && (!is_key(&ev, "return") || !is_key(&ev, "enter"))) {
				gfx_cursor(&x, &y, NULL, NULL);
				game_highlight(-1, -1, 0); /* reset */

				game_click(x, y, 0, 0); 
				game_highlight(x, y, 1); /* hl on/off */
				game_highlight(x, y, 0);

				if (game_click(x, y, 1, 0) == -1) 
					break;
			} else if (!is_key(&ev, "f1")) {
				if (!menu_shown)
					menu_toggle();
/*
			} else if (!is_key(&ev, "f6")) {
				mouse_reset(1);
				game_menu(menu_games);
*/
			} else if (!is_key(&ev, "f4")) {
#ifdef _USE_UNPACK
#ifdef _USE_BROWSE
				mouse_reset(1);
				if (!game_from_disk()) {
					shift_pressed = alt_pressed = 0;
				}
#endif
#endif
			} else if (!is_key(&ev, "escape")) {
				if (use_xref)
					disable_use();
				else	
					menu_toggle();
			} else if (!is_key(&ev, "tab")) {
				select_frame(shift_pressed);
			} else if (!is_key(&ev, "up") || !is_key(&ev, "down") ||
					!is_key(&ev, "[8]") || !is_key(&ev, "[2]")) {
		
				int lm;
				int prev = !is_key(&ev, "up") || !is_key(&ev, "[8]");
				
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
			} else if (!is_key(&ev, "page up") || !is_key(&ev, "[9]") || 
					!is_key(&ev, "page down") || !is_key(&ev, "[3]")) {
				int lm;
				int prev = !is_key(&ev, "page up") || !is_key(&ev, "[9]");
				
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
			} else if (!is_key(&ev, "left") || !is_key(&ev, "[4]")) {
				select_ref(1, 0);
			} else if (!is_key(&ev, "right") || !is_key(&ev, "[6]")) {
				select_ref(0, 0);
			} else if (!is_key(&ev, "backspace") && !menu_shown) {
				scroll_pup(el_scene);
			} else if (!is_key(&ev, "space") && !menu_shown) {
				scroll_pdown(el_scene);
			} else if (alt_pressed && !is_key(&ev, "q")) {
				break;
			} else if (alt_pressed &&
				(!is_key(&ev, "enter") || !is_key(&ev, "return"))) {
				int old_menu = (menu_shown) ? cur_menu: -1;
				shift_pressed = alt_pressed = 0;
				opt_fs ^= 1;
				game_restart();
				if (old_menu != -1)
					game_menu(old_menu);
			}
		} else if (ev.type == MOUSE_DOWN) {
			if (ev.code != 1)
				disable_use();
			else {	
				game_highlight(-1, -1, 0);
				game_click(ev.x, ev.y, 0, 1);
				x = ev.x;
				y = ev.y;
			}
		} else if (ev.type == MOUSE_UP) {
			game_highlight(-1, -1, 0);
			if (game_click(ev.x, ev.y, 1, 1) == -1)
				break;
		} else if (ev.type == MOUSE_WHEEL_UP && !menu_shown) {
			game_scroll_up(ev.count);
		} else if (ev.type == MOUSE_WHEEL_DOWN && !menu_shown) {
			game_scroll_down(ev.count);
		} else if (ev.type == MOUSE_MOTION) {
			if (motion_mode) {
				motion_mode = 2;
				scroll_motion(motion_id, motion_y - ev.y);
				motion_y = ev.y;
			}
		//	game_highlight(ev.x, ev.y, 1);
		} 

		if (old_xref)
			game_highlight(x, y, 1);
		else {
			int x, y;
			gfx_cursor(&x, &y, NULL, NULL);
			game_highlight(x, y, 1);
		}
		game_cursor(CURSOR_ON);
	}
	return 0;
}
