#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/stat.h>
#include "graphics.h"
#include "sound.h"
#include "game.h"
#include "input.h"
#include "instead.h"

int opt_fs = 0;
int opt_hl = 1;
int opt_hz = 22050;
int opt_vol = 127;
int opt_autosave = 0;
char *opt_game = NULL;

char game_cwd[PATH_MAX];
char    *curgame;
int cfg_parse(const char *path);
char *game_cfg_path(void);

int game_save(void);

int cfg_load(void)
{
	char *p = game_cfg_path();
	if (!p)
		return -1;
	if (access(p, R_OK))
		return 0;
	return cfg_parse(p);
}

int cfg_save(void)
{
	FILE *fp;
	char *p = game_cfg_path();
	if (!p)
		return -1;
	fp = fopen(p, "w");
	if (!fp)
		return -1;
	fprintf(fp, "fs = %d\nhl = %d\nhz = %d\nvol = %d\nautosave = %d\ngame = %s", opt_fs, opt_hl, opt_hz, opt_vol, opt_autosave, curgame?curgame:"");
	fclose(fp);
	return 0;
}

void game_menu_box(int show, const char *txt);

void game_cursor(int on);

#ifndef THEMES_PATH
#define THEMES_PATH "./themes"
#endif

struct theme {
	int 	w;
	int 	h;
	color_t	bgcol;
	char	*bg_name;
	img_t	bg;
	char	*use_name;
	img_t	use;
	int 	pad;
	
	int 	win_x;
	int	win_y;
	int	win_w;
	int	win_h;

	char	*font_name;
	int	font_size;
	fnt_t	font;
	
	int 	max_scene_h;

	char	*a_up_name;
	char	*a_down_name;
	img_t	a_up;
	img_t	a_down;

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
	fnt_t	inv_font;
	
	char	*inv_a_up_name;
	char	*inv_a_down_name;
	img_t	inv_a_up;
	img_t	inv_a_down;
	
//	int	lstyle;
//	int	ilstyle;

	color_t menu_bg;
	color_t menu_fg;
	color_t border_col;
	color_t menu_link;
	color_t menu_alink;
	int 	menu_alpha;
	int 	border_w;
	char	*menu_font_name;
	int	menu_font_size;
	fnt_t	menu_font;
	
	char	*menu_button_name;
	img_t	menu_button;
	int	menu_button_x;
	int 	menu_button_y;
} game_theme = {
	.w = 800,
	.h = 480,
	.bg_name = NULL,
	.bg = NULL,
	.use_name = NULL,
	.use = NULL,
	.font_name = NULL,
	.font = NULL,
	.a_up_name = NULL,
	.a_down_name = NULL,
	.a_up = NULL,
	.a_down = NULL,
	.inv_font_name = NULL,
	.inv_font = NULL,
	.inv_a_up_name = NULL,
	.inv_a_down_name = NULL,
	.inv_a_up = NULL,
	.inv_a_down = NULL,
	.menu_font_name = NULL,
	.menu_font = NULL,
	.menu_button_name = NULL,
	.menu_button = NULL,
};

#define FREE(v) do { if ((v)) free((v)); v = NULL; } while(0)

void free_theme_strings()
{
	struct theme *t = &game_theme;
	FREE(t->use_name);
	FREE(t->bg_name);
	FREE(t->inv_a_up_name);
	FREE(t->inv_a_down_name);
	FREE(t->a_down_name);
	FREE(t->a_up_name);
	FREE(t->font_name);
	FREE(t->inv_font_name);
	FREE(t->menu_font_name);
	FREE(t->menu_button_name);
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

	if (game_theme.bg)
		gfx_free_image(game_theme.bg);

	if (game_theme.menu_button)
		gfx_free_image(game_theme.menu_button);

	game_theme.font = game_theme.inv_font = game_theme.menu_font = NULL;
	game_theme.a_up = game_theme.a_down = game_theme.use = NULL;
	game_theme.inv_a_up = game_theme.inv_a_down = NULL;
	game_theme.menu_button = NULL;
	game_theme.bg = NULL;
//	game_theme.slide = gfx_load_image("slide.png", 1);
	return 0;
}

int game_theme_init(void)
{
	struct theme *t = &game_theme;

	if (t->font_name) {
		fnt_free(t->font);
		if (!(t->font = fnt_load(t->font_name, t->font_size)))
			goto err;
	}
	
	if (t->inv_font_name) {
		fnt_free(t->inv_font);
		if (!(t->inv_font = fnt_load(t->inv_font_name, t->inv_font_size)))
			goto err;;
	}


	if (t->menu_font_name) {
		fnt_free(t->menu_font);
		if (!(t->menu_font = fnt_load(t->menu_font_name, t->menu_font_size)))
			goto err;
	}


	if (t->a_up_name) {
		gfx_free_image(t->a_up);
		if (!(t->a_up = gfx_load_image(t->a_up_name, 1)))
			goto err;
	}
	
	if (t->a_down_name) {
		gfx_free_image(t->a_down);
		if (!(t->a_down = gfx_load_image(t->a_down_name, 1)))
			goto err;
	}

	if (t->inv_a_up_name) {
		gfx_free_image(t->inv_a_up);
		if (!(t->inv_a_up = gfx_load_image(t->inv_a_up_name, 1)))
			goto err;
	}


	if (t->inv_a_down_name) {
		gfx_free_image(t->inv_a_down);
		if (!(t->inv_a_down = gfx_load_image(t->inv_a_down_name, 1)))
			goto err;
	}

	if (t->bg_name) {
		gfx_free_image(t->bg);
		t->bg = NULL;
		if (t->bg_name[0] && !(t->bg = gfx_load_image(t->bg_name, 0)))
			goto err;
	}

	if (t->use_name) {
		gfx_free_image(t->use);	
		if (!(t->use = gfx_load_image(t->use_name, 1)))
			goto err;
	}
	
	if (t->menu_button_name) {
		gfx_free_image(t->menu_button);
		if (!(t->menu_button = gfx_load_image(t->menu_button_name, 1)))
			goto err;
	}

	free_theme_strings();

	if (!t->use || !t->inv_a_up || !t->inv_a_down || !t->a_down || !t->a_up ||
		!t->font || !t->inv_font || !t->menu_font || !t->menu_button) {
		fprintf(stderr,"Can't init theme.\n");
		return -1;
	}
	return 0;
err:
	game_theme_free();
	return -1;
}

int theme_load(const char *name);

int game_theme_load(const char *name)
{
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	if (chdir(THEMES_PATH) || chdir(name) || theme_load("theme.ini")) {
		chdir(cwd);
		return -1;
	}
	chdir(cwd);
	return 0;
}

int game_default_theme(void)
{
	return game_theme_load("default");
}

typedef int (*parser_fn)(const char *v, void *data);

int parse_string(const char *v, void *data)
{
	char **p = ((char **)data);
	if (*p)
		free(*p);
	*p = strdup(v);
	if (!*p)
		return -1;
	return 0;	
}

int parse_int(const char *v, void *data)
{
	int *i = (int *)data;
	char *eptr = NULL;
	*i = strtol(v, &eptr, 0);
	if (!eptr || *eptr)
		return -1;
	return 0;	
}

int parse_color(const char *v, void *data)
{
	color_t *c = (color_t *)data;
	return gfx_parse_color(v, c);
}

int parse_include(const char *v, void *data)
{
	int rc;
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	chdir(game_cwd);
	rc = game_theme_load(v);	
	chdir(cwd);
	return rc;
}

struct parser {
	const char 	*cmd;
	parser_fn	fn; 
	void		*p;
};

struct parser cfg_parser[] = {
	{ "hz", parse_int, &opt_hz },
	{ "fs", parse_int, &opt_fs },
	{ "vol", parse_int, &opt_vol }, 
	{ "hl", parse_int, &opt_hl },
	{ "game", parse_string, &opt_game },
	{ "autosave", parse_int, &opt_autosave },
	{ NULL, },
};

struct parser cmd_parser[] = {
	{ "scr.w", parse_int, &game_theme.w },
	{ "scr.h", parse_int, &game_theme.h },
	{ "scr.col.bg", parse_color, &game_theme.bgcol },
	{ "scr.gfx.bg", parse_string, &game_theme.bg_name },
	{ "scr.gfx.use", parse_string, &game_theme.use_name },
	{ "scr.gfx.pad", parse_int, &game_theme.pad  }, 

	{ "win.x", parse_int, &game_theme.win_x },
	{ "win.y", parse_int, &game_theme.win_y },
	{ "win.w", parse_int, &game_theme.win_w },	
	{ "win.h", parse_int, &game_theme.win_h },	
	{ "win.fnt.name", parse_string, &game_theme.font_name },
	{ "win.fnt.size", parse_int, &game_theme.font_size },
	{ "win.gfx.h", parse_int, &game_theme.max_scene_h },
	{ "win.gfx.up", parse_string, &game_theme.a_up_name },
	{ "win.gfx.down", parse_string, &game_theme.a_down_name },
	{ "win.col.fg", parse_color, &game_theme.fgcol }, 
	{ "win.col.link", parse_color, &game_theme.lcol },
	{ "win.col.alink", parse_color, &game_theme.acol }, 

	{ "inv.x", parse_int, &game_theme.inv_x },
	{ "inv.y", parse_int, &game_theme.inv_y },
	{ "inv.w", parse_int, &game_theme.inv_w },	
	{ "inv.h", parse_int, &game_theme.inv_h },	
	{ "inv.col.fg", parse_color, &game_theme.icol }, 
	{ "inv.col.link", parse_color, &game_theme.ilcol },
	{ "inv.col.alink", parse_color, &game_theme.iacol }, 
	{ "inv.fnt.name", parse_string, &game_theme.inv_font_name },
	{ "inv.fnt.size", parse_int, &game_theme.inv_font_size },
	{ "inv.gfx.up", parse_string, &game_theme.inv_a_up_name },
	{ "inv.gfx.down", parse_string, &game_theme.inv_a_down_name },

	{ "menu.col.bg", parse_color, &game_theme.menu_bg },
	{ "menu.col.fg", parse_color, &game_theme.menu_fg },
	{ "menu.col.link", parse_color, &game_theme.menu_link },
	{ "menu.col.alink", parse_color, &game_theme.menu_alink },
	{ "menu.col.alpha", parse_int, &game_theme.menu_alpha },
	{ "menu.col.border", parse_color, &game_theme.border_col },
	{ "menu.bw", parse_int, &game_theme.border_w },
	{ "menu.fnt.name", parse_string, &game_theme.menu_font_name },
	{ "menu.fnt.size", parse_int, &game_theme.menu_font_size },
	{ "menu.gfx.button", parse_string, &game_theme.menu_button_name },
	{ "menu.buttonx", parse_int, &game_theme.menu_button_x },
	{ "menu.buttony", parse_int, &game_theme.menu_button_y },
	{ "include", parse_include, NULL },
	{ NULL,  },
};

char *strip(char *s)
{
	char *e;
	while (isspace(*s))
		s ++;
	if (!*s)
		return s;
	e = s + strlen(s) - 1;
	while (e != s && isspace(*e)) {
		*e = 0;
		e --;
	}
	return s;
}

int process_cmd(char *n, char *v, struct parser *cmd_parser)
{
	int i;
	n = strip(n);
	v = strip(v);
	for (i = 0; cmd_parser[i].cmd; i++) {
		if (!strcmp(cmd_parser[i].cmd, n)) {
			return cmd_parser[i].fn(v, cmd_parser[i].p);
		}
	}
	return -1;
}

int parse_ini(const char *path, struct parser *cmd_parser)
{
	int rc = 0;
	int line_nr = 0;
	FILE *fp;
	char line[1024];
	fp = fopen(path, "r");
	if (!fp)
		return -1;
	while (fgets(line, sizeof(line), fp)) {
		char *p = line;
		char *val;
		int len;
		line_nr ++;
		p += strspn(p, " \t");
		if (*p == ';')
			continue;
		len = strcspn(p, "=");
		if (p[len] != '=')
			continue;
		p[len] = 0;
		val = p + len + 1;
		len = strcspn(p, " \t");
		p[len] = 0;
//		printf("%s\n", p);
		val += strspn(val, " \t");
		val[strcspn(val, "\n")] = 0;
		if (process_cmd(p, val, cmd_parser)) {
			rc = -1;
			fprintf(stderr, "Can't process cmd '%s' on line %d : %s\n", p, line_nr, strerror(errno));
		}
	}
	fclose(fp);
	return rc;
}

int theme_parse(const char *path)
{
	if (parse_ini(path, cmd_parser)) {
		game_theme_free();
		return -1;
	}
	return 0;
}

int theme_load(const char *name)
{
	if (theme_parse(name))
		return -1;
	if (game_theme_init()) 
		return -1;
	return 0;
}

int cfg_parse(const char *path)
{
	return parse_ini(path, cfg_parser);
}

#ifndef GAMES_PATH
#define GAMES_PATH "./games"
#endif

char *getpath(const char *d, const char *n)
{
	int i = strlen(d) + strlen(n) + 3;
	char *p = malloc(i);
	if (p) {
		strcpy(p, d);
		strcat(p, "/");
		strcat(p, n);
		strcat(p, "/");
	}
	return p;
}

#define MAIN_FILE	"main.lua"

int is_game(const char *n)
{
	int rc = 0;
	char *p = getpath(GAMES_PATH, n);
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

struct game {
	char *path;
	char *name;
};

struct	game *games;
int	games_nr = 0;

int game_select(const char *name)
{
	int i;
	if (!name || !*name) {
		if (games_nr == 1) 
			name = games[0].name;
		else
			return 0;
	} 
	chdir(game_cwd);
	for (i = 0; i<games_nr; i ++) {
		if (!strcmp(games[i].name, name)) {
			instead_done();
			if (instead_init())
				return -1;
			if (chdir(games[i].path))
				return -1;
			if (instead_load(MAIN_FILE))
				return -1;
			curgame = games[i].name;
			return 0;
		}
	}
	return 0;
}

static char save_path[PATH_MAX];
char *game_cfg_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.insteadrc", pw->pw_dir);
	return save_path;
	
}
char *game_save_path(int cr)
{
	struct passwd *pw;
	if (!curgame)
		return NULL;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/saves", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/", pw->pw_dir, curgame);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/autosave", pw->pw_dir, curgame);
	return save_path;
}

int games_lookup(void)
{
	char *p;
	int n = 0;
	DIR *d;
	struct dirent *de;
	d = opendir(GAMES_PATH);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (de->d_type != DT_DIR)
			continue;
		if (!is_game(de->d_name))
			continue;
		n ++;
	}
	rewinddir(d);
	if (!n)
		return 0;
	games = malloc(sizeof(struct game) * n);
	while ((de = readdir(d)) && games_nr < n) {
		if (de->d_type != DT_DIR)
			continue;
		if (!is_game(de->d_name))
			continue;
		p = getpath(GAMES_PATH, de->d_name);
		games[games_nr].path = p;
		games[games_nr].name = strdup(de->d_name);
		games_nr ++;
	}
	closedir(d);
	return 0;
}


static		char *last_pict = NULL;
static 		char *last_title = NULL;
static		char *last_music = NULL;
static int mx, my;
static img_t 	menubg = NULL;
static img_t	menu = NULL;
static int cur_menu = 0;
static int menu_shown = 0;
enum {
	menu_main = 0,
	menu_about, 
	menu_settings,
	menu_quit,
	menu_askquit,
	menu_saved, 
	menu_games, 
};
int game_cmd(char *cmd);
int change_vol(int d, int val);

void game_clear(int x, int y, int w, int h)
{
	if (game_theme.bg)
		gfx_draw_bg(game_theme.bg, x, y, w, h);
	else
		gfx_clear(x, y, w, h);

	if (menu_shown) {
		int xx = x - mx;
		int yy = y - my;
		gfx_draw_from(menubg, xx, yy, x, y, w, h); 
		gfx_draw_from(menu, xx, yy, x, y, w, h);
	//	gfx_update(mx, my, ww, hh);
		return;
	}
}



void game_clear(int x, int y, int w, int h);


struct el {
	int		id;
	int 		x;
	int 		y;
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
	el_menu = 0,
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


void el_draw(int n);
int game_init(const char *name)
{
	char *s;
	layout_t lay;
	textbox_t box;
	getcwd(game_cwd, sizeof(game_cwd));
	cfg_load();
	if (!name && opt_game)
		name = opt_game;
	if (gfx_init(game_theme.w, game_theme.h, opt_fs))
		return -1;	
	snd_init(opt_hz);
	change_vol(0, opt_vol);
	if (game_default_theme())
		return -1;
	if (game_select(name)) {
		name = NULL;
	}
	if (!access("theme.ini", R_OK)) {
		if (theme_load("theme.ini"))
			return -1;
	}
	gfx_bg(game_theme.bgcol);
	game_clear(0, 0, game_theme.w, game_theme.h);
//	if (instead_init("cat.lua"))
//		return -1;
//	gfx_update(0, 0, game_theme.w, game_theme.h);
	lay = txt_layout(game_theme.font, ALIGN_JUSTIFY, game_theme.win_w, game_theme.win_h);
	if (!lay)
		return -1;
	box = txt_box(game_theme.win_w, game_theme.win_h);
	if (!box)
		return -1;
	txt_layout_color(lay, game_theme.fgcol);
	txt_layout_link_color(lay, game_theme.lcol);
	txt_layout_active_color(lay, game_theme.acol);
//	txt_layout_link_style(lay, game_theme.lstyle);

	txt_box_set(box, lay);
	el_set(el_scene, elt_box, game_theme.win_x, 0, box);

	lay = txt_layout(game_theme.inv_font, ALIGN_LEFT, game_theme.inv_w, game_theme.inv_h);
	if (!lay)
		return -1;
	txt_layout_link_color(lay, game_theme.ilcol);
	txt_layout_active_color(lay, game_theme.iacol);
//	txt_layout_link_style(lay, game_theme.ilstyle);
	box = txt_box(game_theme.inv_w, game_theme.inv_h);
	if (!box)
		return -1;

	txt_box_set(box, lay);
	el_set(el_inv, elt_box, game_theme.inv_x, game_theme.inv_y, box);

	lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
	if (!lay)
		return -1;

	el_set(el_title, elt_layout, game_theme.win_x, game_theme.win_y, lay);

	lay = txt_layout(game_theme.font, ALIGN_CENTER, game_theme.win_w, 0);
	if (!lay)
		return -1;
	el_set(el_ways, elt_layout, game_theme.win_x, 0, lay);

	el_set(el_sdown, elt_image, 0, 0, game_theme.a_down);
	el_set(el_sup, elt_image, 0, 0,  game_theme.a_up);
//	el_set(el_sslide, elt_image, 0, 0,  game_theme.slide);
	el_set(el_idown, elt_image, 0, 0, game_theme.inv_a_down);
	el_set(el_iup, elt_image, 0, 0, game_theme.inv_a_up);
//	el_set(el_islide, elt_image, 0, 0,  game_theme.slide);

	el_set(el_spic, elt_image, game_theme.win_x, game_theme.win_y, NULL);
	el_set(el_menu, elt_layout, 0, 0, NULL);
	el_set(el_menu_button, elt_image, game_theme.menu_button_x, game_theme.menu_button_y, game_theme.menu_button);
	
	el_draw(el_menu_button);

	s = game_save_path(0);

	if (s && !access(s, R_OK)) {
		char cmd[256];
		snprintf(cmd, sizeof(cmd) - 1, "load %s", s);
		game_cmd(cmd);
		return 0;
	}
	if (!curgame) {
		menu_shown = 1;
		cur_menu = menu_games;
		game_menu_box(menu_shown, game_menu_gen());
		gfx_flip();
	} else
		game_cmd("look");
	return 0;
}

void free_last(void)
{
	if (last_pict)
		free(last_pict);
	if (last_title)
		free(last_title);
	if (last_music)
		free(last_music);
	last_pict = last_title = last_music = NULL;
}

void game_done(void)
{
	int i;
	if (opt_autosave)
		game_save();
	chdir(game_cwd);
	cfg_save();
	menu_shown = 0;
	game_menu_box(0, NULL);
	cur_menu = menu_main;
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
	instead_done();
	snd_done();
	gfx_done();
	curgame = NULL;
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
	gfx_update(x, y, w, h);
	return;
}

void box_update_scrollbar(int n)
{
	struct el *elup;
	struct el *eldown;
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
	txt_layout_size(l, NULL, &hh);
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

	if (hh - off >= h)
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
	int w, h;
	float scale1, scale2, scale;
	w = gfx_img_w(img);
	h = gfx_img_h(img);
	if (w <= ww && h <= hh)
		return img;
	scale1 = (float)ww / (float)w;
	scale2 = (float)hh / (float)h;
	scale = (scale1<scale2)?scale1:scale2;
	img2 = gfx_scale(img, scale, scale);
	gfx_free_image(img);
	return img2;
}


int vol_from_pcn(int v)
{
	return (v * 127) / 100;
}

int vol_to_pcn(int v)
{
	return (v * 100) / 127;
}
void music_player(void);

int static old_vol = 0;
int static cur_vol = 0;

int change_vol(int d, int val)
{
	int v = snd_volume_mus(-1);
	int pc = vol_to_pcn(v);
	int opc = pc;
	if (d) {
		pc += d;
		if (pc < 0)
			pc = 0;
		if (pc > 100)
			pc = 100;
		while (vol_to_pcn(v) != pc)
			v += (d<0)?-1:1;
	} else {
		v = val;
		pc = vol_to_pcn(v);
	}
	if (!pc)
		v = 0;
	snd_volume_mus(v);
	if (opc && !pc) {
		snd_stop_mus(0);
		if (last_music)
			free(last_music);
		last_music = NULL;
	} 
	if (!opc && pc) {
		music_player();
	}
	cur_vol = snd_volume_mus(-1);
	opt_vol = cur_vol;
	return 0;
}

int change_hz(int hz)
{
	if (!hz)
		return -1;
	snd_done();
	if (last_music)
		free(last_music);
	last_music = NULL;
	snd_init(hz);
	snd_volume_mus(cur_vol);
	music_player();
	opt_hz = snd_hz();
	return 0;
}
#define MENU_GAMES_MAX 4
static int games_menu_from = 0;
int game_save(void)
{
	char *s = game_save_path(1);
	char cmd[256];
	char *p;
	if (s) {
		snprintf(cmd, sizeof(cmd) - 1, "save %s", s); 
		p = instead_cmd(cmd);
		if (p)
			free(p);
		return 0;
	}
	return -1;
}
int game_menu_act(const char *a)
{
	if (!strcmp(a, "/autosave")) {
		opt_autosave ^= 1;
	} else if (!strcmp(a, "/hl")) {
		opt_hl ^= 1;
	} else if (!strcmp(a, "/fs")) {
		char *og = curgame;
		opt_fs ^= 1;
		game_save();
		game_done();
		game_init(og);
	} else if (!strcmp(a, "/games_prev")) {
		games_menu_from -= MENU_GAMES_MAX;
		if (games_menu_from < 0)
			games_menu_from = 0;
	} else if (!strcmp(a, "/games_next")) {
		if (games_menu_from + MENU_GAMES_MAX < games_nr)
			games_menu_from += MENU_GAMES_MAX;
	} else if (!strcmp(a, "/select")) {
		cur_menu = menu_games;
	} else if (!strcmp(a, "/save")) {
		if (!game_save()) {
			cur_menu = menu_saved;
		}
	}
	else if (!strcmp(a, "/new")) {
		if (!curgame)
			return 0;
		free_last();
		game_select(curgame);
//		instead_done();
//		instead_init();
//		instead_load(MAIN_FILE);
		menu_shown = 0;
		game_menu_box(0, NULL);
		game_cmd("look");
		game_save();
//		game_done();
//		game_init();
//		game_cmd("look");
	}
	else if (!strcmp(a,"/main")) {
		cur_menu = menu_main;
	}
	else if (!strcmp(a,"/ask_quit")) {
		cur_menu = menu_askquit;
	}
	else if (!strcmp(a,"/about")) {
		cur_menu = menu_about;
	}
	else if (!strcmp(a,"/mtoggle")) {
		if (!old_vol) {
			old_vol = snd_volume_mus(-1);
			change_vol(0, 0);
		} else {
			change_vol(0, old_vol);
			old_vol = 0;
		}
	}
	else if (!strcmp(a,"/resume")) {
		menu_shown = 0;
		game_menu_box(0, NULL);
	}
	else if (!strcmp(a, "/settings")) {
		cur_menu = menu_settings;
	}
	else if (!strcmp(a, "/vol--")) {
		change_vol(-10, 0);
	}
	else if (!strcmp(a, "/vol++")) {
		change_vol(+10, 0);
	}
	else if (!strcmp(a, "/vol-")) {
		change_vol(-1, 0);
	}
	else if (!strcmp(a, "/vol+")) {
		change_vol(+1, 0);
	}
	else if (!strcmp(a, "/hz-")) {
		int hz = snd_hz();
		if (hz == 44100)
			hz = 22050;
		else if (hz == 22050)
			hz = 11025;
		else
			hz = 0;
		change_hz(hz);
	}
	else if (!strcmp(a, "/hz+")) {
		int hz = snd_hz();
		if (hz == 11025)
			hz = 22050;
		else if (hz == 22050)
			hz = 44100;
		else
			hz = 0;
		change_hz(hz);
	}
	else if (!strcmp(a,"/quit")) {
		return -1;
	} else if (cur_menu == menu_games) {
		char *p;
		p = strdup(a);
		if (p) {
			char *og = curgame;
			game_done();
			if (game_init(p)) {
				fprintf(stderr, "Can't init game:%s:%s\n", p, strerror(errno));
			//	exit(1);
				game_done();
				game_init(og);
			}
			free(p);
		}
	}
	return 0;
}
#ifdef RUSSIAN
#define MAIN_MENU "<a:/resume>Вернуться в игру</a>\n<a:/select>Выбор игры</a>\n<a:/new>Новая игра</a>\n<a:/save>Сохранить игру</a>\n<a:/about>Информация</a>\n<a:/settings>Настройки</a>\n<a:/ask_quit>Выход</a>"
#define ABOUT_MENU "IN S.T.E.A.D SDL - "VERSION"\n\nИнтерпретатор простых\nтекстовых приключений.\n\nКосых П.А. '2009\n\n<a:/main>Назад</a>"

#define BACK_MENU	"<a:/main>Назад</a>"
#define ON	   "Да"
#define OFF	   "Нет"

#define SETTINGS_MENU "Громкость\n<a:/vol--><<</a><a:/vol-><</a> <a:/mtoggle>%d%%</a> <a:/vol+>></a><a:/vol++>>></a>\n\n\
Качество звука\n<a:/hz-><<</a> %dГц <a:/hz+>>></a>\n\nПодсветка ссылок: <a:/hl>%s</a>\n\nПолный экран: <a:/fs>%s</a>\n\n\
Автосохранение: <a:/autosave>%s</a>\n\n\
<a:/main>Назад</a>"

#define QUIT_MENU "На самом деле выйти?\n\n<a:/quit>Да</a> | <a:/main>Нет</a>"
#define SELECT_GAME_MENU "Выбор игры\n\n"
#define SAVED_MENU "Игра сохранена!\n\n<a:/main>Ок</a>"
#define NOGAMES_MENU "Не найдена ни одна игра. \nПожалуйста, скопируйте хотя бы одну игру в каталог:\n'%s'"
#else
#define MAIN_MENU "<a:/resume>Resume</a>\n<a:/select>Select Game</a>\n<a:/new>New</a>\n<a:/save>Save</a>\n<a:/about>About</a>\n<a:/settings>Settings</a>\n<a:/ask_quit>Quit</a>"
#define ABOUT_MENU "Written by Peter Kosyh '2009\n\n<a:/main>Back</a>"
#define BACK_MENU	"<a:/main>Back</a>"

#define ON	   "on"
#define OFF	   "off"
#define SELECT_GAME_MENU "Select game to play\n\n"
#define SETTINGS_MENU "Volume\n<a:/vol--><<</a><a:/vol-><</a> <a:/mtoggle>%d%%</a> <a:/vol+>></a><a:/vol++>>></a>\n\n\
Quality\n<a:/hz-><<</a> %dHz <a:/hz+>>></a>Refs highlighting: <a:/hl>%s</a>\n\nFull Screen: <a:/fs>%s</a>\n\n\
Autosave: <a:/autosave>%s</a>\n\n\
<a:/main>Back</a>"

#define QUIT_MENU "Really quit?\n\n<a:/quit>Yes</a> | <a:/main>No</a>"

#define SAVED_MENU "Current Game saved!\n\n<a:/main>Ok</a>"
#define NOGAMES_MENU "No games found. \nPlease, write any game in the this directory:\n'%s'"
#endif

char  menu_buff[4096];
void games_menu(void)
{
	int i;
	*menu_buff = 0;
	sprintf(menu_buff, SELECT_GAME_MENU);
	for (i = games_menu_from; i < games_nr && i - games_menu_from < MENU_GAMES_MAX; i ++) {
		char tmp[PATH_MAX];
		if (curgame && !strcmp(games[i].name, curgame))
			snprintf(tmp, sizeof(tmp), "<a:/resume><b>%s</b></a>\n", games[i].name);
		else
			snprintf(tmp, sizeof(tmp), "<a:%s>%s</a>\n", games[i].name, games[i].name);
		strcat(menu_buff, tmp);
	}	
	if (!games_nr)
		sprintf(menu_buff, NOGAMES_MENU, GAMES_PATH);
	strcat(menu_buff,"\n");
	if (games_menu_from)
		strcat(menu_buff,"<a:/games_prev><<</a> ");
	strcat(menu_buff, BACK_MENU); 
	if (games_menu_from + MENU_GAMES_MAX < games_nr)
		strcat(menu_buff," <a:/games_next>>></a>");
}

char *game_menu_gen(void)
{
	if (cur_menu == menu_main) {
		snprintf(menu_buff, sizeof(menu_buff), MAIN_MENU);
	} else if (cur_menu == menu_about) {
		snprintf(menu_buff, sizeof(menu_buff), ABOUT_MENU);
	} else if (cur_menu == menu_settings) {
		snprintf(menu_buff, sizeof(menu_buff), 
		SETTINGS_MENU, vol_to_pcn(snd_volume_mus(-1)), snd_hz(), opt_hl?ON:OFF, opt_fs?ON:OFF, opt_autosave?ON:OFF);
	} else if (cur_menu == menu_askquit) {
		snprintf(menu_buff, sizeof(menu_buff), QUIT_MENU);
	} else if (cur_menu == menu_saved) {
		snprintf(menu_buff, sizeof(menu_buff),
		SAVED_MENU);
	} else if (cur_menu == menu_games) {
		games_menu();
	}
	return menu_buff;
}

void game_menu_box(int show, const char *txt)
{	
//	img_t	menu;
	int w, h, mw, mh;
	int x, y;
	int b = game_theme.border_w;
	int pad = game_theme.pad;
	layout_t lay;
	el(el_menu)->drawn = 0;
	if (el_layout(el_menu)) {
		txt_layout_free(el_layout(el_menu));
		el(el_menu)->p.p = NULL;
	}

	if (menubg) {
		gfx_draw(menubg, mx, my);
		gfx_free_image(menubg);
		menubg = NULL;
	}
	el_clear(el_menu_button);

	if (!show)
		el_draw(el_menu_button);

	el_update(el_menu_button);

	if (!show) {
		gfx_flip();
		return;
	}
	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, game_theme.win_w, 0);
	txt_layout_color(lay, game_theme.menu_fg);
	txt_layout_link_color(lay, game_theme.menu_link);
	txt_layout_active_color(lay, game_theme.menu_alink);
	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);	
	if (menu) {
		gfx_free_image(menu);
		menu = NULL;
	}
	menu = gfx_new(w + (b + pad)*2, h + (b + pad)*2);
	gfx_img_fill(menu, 0, 0, w + (b + pad)*2, h + (b + pad)*2, game_theme.border_col);
	gfx_img_fill(menu, b, b, w + pad*2, h + pad*2, game_theme.menu_bg);
	gfx_set_alpha(menu, game_theme.menu_alpha);
	x = (game_theme.win_w - w)/2 + game_theme.win_x; // (game_theme.w - w)/2;
	y = (game_theme.win_h - h)/2 + game_theme.win_y; //(game_theme.h - h)/2;
	mx = x - b - pad;
	my = y - b - pad;
	mw = w + (b + pad) * 2;
	mh = h + (b + pad) * 2;
	menubg = gfx_grab_screen(mx, my, mw, mh);
	gfx_draw(menu, mx, my);
	el_set(el_menu, elt_layout, game_theme.win_x, y, lay);
	el_draw(el_menu);
	gfx_flip();
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

void music_player(void)
{
	char		*mus;
	if (!snd_volume_mus(-1))
		return;
	mus = instead_eval("return get_music()");
	if (mus) {
		if (!last_music && mus) {
			last_music = mus;
			snd_stop_mus(500);
			snd_play_mus(mus, 0);
		} else if (strcmp(last_music, mus)) {
			free(last_music);
			last_music = mus;
			snd_stop_mus(500);
			snd_play_mus(mus, 0);
		} else
			free(mus);
	}
}

int game_cmd(char *cmd)
{
	int		new_pict = 0, new_place = 0;
	int		title_h = 0, ways_h = 0, pict_h = 0;
	char 		buf[256];
	char 		*cmdstr;
	char 		*invstr;
	char 		*waystr;
	char		*title;
	char 		*pict;
	img_t		oldscreen;

	cmdstr = instead_cmd(cmd);
	if (!cmdstr) {
		return -1;
	}
	music_player();
	title = instead_eval("return get_title();");
	if (title) {
		snprintf(buf, sizeof(buf), "<b><c><a:look>%s</a></c></b>", title);
		txt_layout_set(el_layout(el_title), buf);
	} else
		txt_layout_set(el_layout(el_title), NULL);

	new_place = check_new_place(title);

	txt_layout_size(el_layout(el_title), NULL, &title_h);
//	title_h += 12; // todo	
	pict = instead_eval("return get_picture();");

	new_pict = check_new_pict(pict);

	if (pict) {
		int w, h, x;
		img_t img;

		if (new_pict) {
			img = gfx_load_image(pict, 1);
			if (el_img(el_spic))
				gfx_free_image(el_img(el_spic));
			el(el_spic)->p.p = NULL;
			img = game_pict_scale(img, game_theme.win_w, game_theme.max_scene_h);
		} else
			img = el_img(el_spic);

		if (img) {
			w = gfx_img_w(img);
			h = gfx_img_h(img);
			x = (game_theme.win_w - w)/2 + game_theme.win_x;
			el_set(el_spic, elt_image, x, game_theme.win_y + title_h, img);
			pict_h = h;
		}
	} else if (el_img(el_spic)) {
		el_clear(el_spic);
		gfx_free_image(el_img(el_spic));
		el(el_spic)->p.p = NULL;
	}

	waystr = instead_cmd("way");
	invstr = instead_cmd("inv");

	if (waystr) {
		waystr[strcspn(waystr,"\n")] = 0;
	}

	txt_layout_set(el_layout(el_ways), waystr);
	if (waystr)
		free(waystr);
	txt_layout_size(el_layout(el_ways), NULL, &ways_h);
	el(el_ways)->y = el(el_title)->y + title_h + pict_h;
	txt_layout_set(txt_box_layout(el_box(el_scene)), cmdstr);
	txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	free(cmdstr);

	el(el_scene)->y = el(el_ways)->y + ways_h;
	/* draw title and ways */
	if (new_pict || new_place) {
		img_t offscreen = gfx_new(game_theme.w, game_theme.h);
		oldscreen = gfx_screen(offscreen);
		gfx_draw(oldscreen, 0, 0);
	}
	if (new_pict || new_place) {
		game_clear(game_theme.win_x, game_theme.win_y, game_theme.win_w, game_theme.win_h);
//		el_draw(el_title);
	} else {
		game_clear(game_theme.win_x, game_theme.win_y + pict_h + title_h, 
			game_theme.win_w, game_theme.win_h - pict_h - title_h);
	}
	el_clear(el_title);
	el_draw(el_title);

	el_draw(el_ways);
	if (new_pict || new_place)
		el_draw(el_spic);
	
	txt_box_resize(el_box(el_scene), game_theme.win_w, game_theme.win_h - title_h - ways_h - pict_h);
	el_draw(el_scene);

	txt_layout_set(txt_box_layout(el_box(el_inv)), invstr);
	txt_box_set(el_box(el_inv), txt_box_layout(el_box(el_inv)));
	if (invstr)
		free(invstr);
	
	el_clear(el_inv);
	el_draw(el_inv);
//	scene_scrollbar();

	if (new_pict || new_place) {
		img_t offscreen;
		offscreen = gfx_screen(oldscreen);
		gfx_change_screen(offscreen);
		gfx_free_image(offscreen);
		return 0;
	}
	gfx_flip();	
	return 0;
}

xref_t	inv_xref = NULL;

int disable_inv(void)
{
	if (inv_xref) {
		xref_set_active(inv_xref, 0);
		xref_update(inv_xref, el(el_inv)->x, el(el_inv)->y, game_clear);
//		txt_box_update_links(el_box(el_inv), el(el_inv)->x, el(el_inv)->y, game_clear);
		inv_xref = NULL;
		return 1;
	}
	return 0;
}

void enable_inv(xref_t xref)
{
	inv_xref = xref;
	xref_set_active(xref, 1);
	//txt_box_update_links(el_box(el_inv), el(el_inv)->x, el(el_inv)->y, game_clear);
	xref_update(inv_xref, el(el_inv)->x, el(el_inv)->y, game_clear);
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
	xref_t		xref = NULL;
	int up = 0;
	if (!opt_hl)
		return 0;
	if (on) {
		xref = look_xref(x, y, &elem);
		if (xref) {
			game_cursor(-1);
			xref_set_active(xref, 1);
			xref_update(xref, elem->x, elem->y, game_clear);
		}
	}
	
	if (hxref != xref && oel) {
		if (hxref != inv_xref) {
			xref_set_active(hxref, 0);
			game_cursor(-1);
			xref_update(hxref, oel->x, oel->y, game_clear);
			up = 1;
		}
		hxref = NULL;
	}
	hxref = xref;
//	if (!up || !elem)
//		return 0;
//	xref_update(hxref, elem->x, elem->y, game_clear);
//	game_cursor(2);
//	game_cursor(0);

//	if (elem->id == el_menu) {
//		menu_update(elem);
//	}
// else if (elem->type == elt_layout)
//		txt_layout_update_links(elem->p.lay, elem->x, elem->y, game_clear);
//	else if (elem->type == elt_box)
//		txt_box_update_links(elem->p.box, elem->x, elem->y, game_clear);
//	el_update(elem->id);
	oel = elem;
	return 0;
}

void menu_toggle(void)
{
	game_cursor(0);
	game_highlight(-1, -1, 0);
	disable_inv();
	menu_shown ^= 1;
	if (!menu_shown)
		cur_menu = menu_main;
	game_menu_box(menu_shown, game_menu_gen());
}

int game_click(int x, int y, int action)
{
	struct el	*elem = NULL;
	char buf[256];
	xref_t 	xref;
	
	if (old_xref) {
		xref = old_xref;
		elem = old_el;
	} else {
		xref = look_xref(x, y, &elem);
	}
	
	if (!xref) {
		if (action && elem) {
			if (elem->id == el_menu_button) {
				menu_toggle();
			} else if (elem->id == el_sdown) {
				txt_box_next(el_box(el_scene));
				el_clear(el_scene);
				el_draw(el_scene);
				el_update(el_scene);
			}
			else if (elem->id == el_sup) {
				txt_box_prev(el_box(el_scene));
				el_clear(el_scene);
				el_draw(el_scene);
				el_update(el_scene);
			}
			else if (elem->id == el_idown) {
				txt_box_next(el_box(el_inv));
				el_clear(el_inv);
				el_draw(el_inv);
				el_update(el_inv);
			}
			else if (elem->id == el_iup) {
				txt_box_prev(el_box(el_inv));
				el_clear(el_inv);
				el_draw(el_inv);
				el_update(el_inv);
			}
		}
		if (disable_inv()) {
			el_update(el_inv);
//			gfx_flip();
		}
		return 0;
	}
	
	if (!action) {
		xref_set_active(xref, 1);
		xref_update(xref, elem->x, elem->y, game_clear);
/*		
		if (elem->id == el_menu) {
			menu_update(elem);
		} else if (elem->type == elt_layout)
			txt_layout_update_links(elem->p.lay, elem->x, elem->y, game_clear);
		else if (elem->type == elt_box)
			txt_box_update_links(elem->p.box, elem->x, elem->y, game_clear);
		el_update(elem->id); */
		old_xref = xref;
		old_el = elem;
		return 0;
	} 

	old_el = NULL;
	old_xref = NULL;

	if (elem->id == el_menu) {
//		xref_set_active(xref, 0);
//		txt_layout_update_links(elem->p.lay, elem->x, elem->y, game_clear);
		if (game_menu_act(xref_get_text(xref))) {
			return -1;
		}
		game_menu_box(menu_shown, game_menu_gen());
//		gfx_flip();
		return 1;
	}

	if (elem->id == el_ways ||
		elem->id == el_title) {
		strcpy(buf, xref_get_text(xref));
		disable_inv();
		game_cmd(buf);
		return 1;
	}

	if (elem->id == el_scene) {
		if (inv_xref) {
			snprintf(buf,sizeof(buf), "use %s,%s", xref_get_text(inv_xref), xref_get_text(xref));
			disable_inv();
		} else	
			strcpy(buf, xref_get_text(xref));
		game_cmd(buf);
		return 1;
	}
	
	if (elem->id == el_inv) {
		if (!inv_xref) {
			enable_inv(xref);
			el_update(el_inv);
			return 0;
		}	
		if (xref == inv_xref)
			snprintf(buf,sizeof(buf), "use %s", xref_get_text(xref));
		else
			snprintf(buf,sizeof(buf), "use %s,%s", xref_get_text(inv_xref), xref_get_text(xref));
		disable_inv();
		game_cmd(buf);
		return 1;
	}
	return 0;
}

void game_cursor(int on)
{
	static img_t	grab = NULL;
	static int xc, yc, w, h;
	if (grab) {
		gfx_draw(grab, xc, yc);
		gfx_free_image(grab);
		grab = NULL;
	}
	if (!on) {
		gfx_update(xc, yc, gfx_img_w(game_theme.use), gfx_img_h(game_theme.use));
		return;
	}
	if (on == -1)
		return;
	if (inv_xref) {
		int ox = xc;
		int oy = yc;
		gfx_cursor(&xc, &yc, &w, &h);
		xc += w/2;
		yc += h/2;
		grab = gfx_grab_screen(xc, yc, gfx_img_w(game_theme.use), gfx_img_h(game_theme.use));
		gfx_draw(game_theme.use, xc, yc);
		gfx_update(xc, yc, gfx_img_w(game_theme.use), gfx_img_h(game_theme.use));
		gfx_update(ox, oy, gfx_img_w(game_theme.use), gfx_img_h(game_theme.use));
	}
}

int game_loop(void)
{
	static int x = 0, y = 0;
	struct inp_event ev;
	while (1) {
		if (input(&ev, 1) == -1) /* close */
			break;
		if (ev.type == KEY_DOWN && ev.sym && !strcmp(ev.sym,"escape")) {
			menu_toggle();
		} else if (ev.type == MOUSE_DOWN) {
			game_cursor(0);
			game_highlight(-1, -1, 0);
			game_click(ev.x, ev.y, 0);
			x = ev.x;
			y = ev.y;
		} else if (ev.type == MOUSE_UP) {
			game_cursor(0);
			game_highlight(-1, -1, 0);
			if (game_click(x, y, 1) == -1)
				break;
		} else if (ev.type == MOUSE_WHEEL_UP && !menu_shown) {
			int xm, ym;
			struct el *o;
			gfx_cursor(&xm, &ym, NULL, NULL);
			o = look_obj(xm, ym);
			if (o && (o->id == el_scene || o->id == el_inv)) {
				game_cursor(0);
				game_highlight(-1, -1, 0);
				txt_box_prev_line(el_box(o->id));
				el_clear(o->id);
				el_draw(o->id);
				el_update(o->id);
			}
		} else if (ev.type == MOUSE_WHEEL_DOWN && !menu_shown) {
			int xm, ym;
			struct el *o;
			gfx_cursor(&xm, &ym, NULL, NULL);
			o = look_obj(xm, ym);
			if (o && (o->id == el_scene || o->id == el_inv)) {
				game_cursor(0);
				game_highlight(-1, -1, 0);
				txt_box_next_line(el_box(o->id));
				el_clear(o->id);
				el_draw(o->id);
				el_update(o->id);
			}
		} else if (ev.type == MOUSE_MOTION) {
		//	game_highlight(ev.x, ev.y, 1);
		}
		game_highlight(ev.x, ev.y, 1);
		game_cursor(1);
	}
	return 0;
}

