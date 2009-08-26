#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>
#include "graphics.h"
#include "sound.h"
#include "game.h"
#include "input.h"
#include "instead.h"

#ifdef RUSSIAN
#include "menu.h"
#else
#include "menu-en.h"
#endif

int opt_fsize = 0;
int opt_fs = 0;
int opt_hl = 1;
int opt_hz = 22050;
int opt_vol = 127;
int opt_motion = 1;
int opt_click = 1;
int opt_music = 1;
int opt_autosave = 1;
int opt_filter = 1;
int opt_owntheme = 1;
char *opt_game = NULL;
char *opt_theme = NULL;
char *err_msg = NULL;

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

#define ERR_MSG_MAX 512

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

char game_cwd[PATH_MAX];
char    *curgame = NULL;
char	*curgame_dir = NULL;
static int own_theme = 0;
char    *curtheme = NULL;
char	*curtheme_dir = NULL;
int cfg_parse(const char *path);

extern char *game_cfg_path(void);
extern char *game_save_path(int rc, int nr);

int game_save(int nr);

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
	fprintf(fp, "fs = %d\nhl = %d\nhz = %d\nvol = %d\nautosave = %d\n\
game = %s\nfscale = %d\nmotion = %d\n\
click = %d\nmusic = %d\ntheme = %s\n\
filter = %d\nowntheme = %d", 
		opt_fs, opt_hl, opt_hz, opt_vol, opt_autosave, 
		curgame_dir?curgame_dir:"", opt_fsize, opt_motion, 
		opt_click, opt_music, curtheme_dir?curtheme_dir:DEFAULT_THEME, 
		opt_filter, opt_owntheme);
	fclose(fp);
	return 0;
}

void game_menu_box(int show, const char *txt);

void game_cursor(int on);

#define GFX_MODE_FLOAT 0
#define GFX_MODE_FIXED 1
#define GFX_MODE_EMBEDDED 2

#define INV_MODE_VERT 0
#define INV_MODE_HORIZ 1

struct game_theme {
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

	int	gfx_x;
	int	gfx_y;
	int	max_scene_w;
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
	int	gfx_mode;
	int	inv_mode;
	char	*click_name;
	void	*click;
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
	.gfx_mode = GFX_MODE_EMBEDDED,
	.inv_mode = INV_MODE_VERT,
	.click_name = NULL,
	.click = NULL,
};

#define FREE(v) do { if ((v)) free((v)); v = NULL; } while(0)

void free_theme_strings()
{
	struct game_theme *t = &game_theme;
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
/*	FREE(t->click_name); must be reloaded, ugly :(*/
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

	if (game_theme.click)
		snd_free_wav(game_theme.click);

	game_theme.font = game_theme.inv_font = game_theme.menu_font = NULL;
	game_theme.a_up = game_theme.a_down = game_theme.use = NULL;
	game_theme.inv_a_up = game_theme.inv_a_down = NULL;
	game_theme.menu_button = NULL;
	game_theme.bg = NULL;
	game_theme.click = NULL;
//	game_theme.slide = gfx_load_image("slide.png", 1);
	return 0;
}

#define FONT_SZ(v) ((v) * (1.0f + ((0.1f * opt_fsize))))

int game_theme_init(void)
{
	struct game_theme *t = &game_theme;

	if (t->font_name) {
		fnt_free(t->font);
		if (!(t->font = fnt_load(t->font_name, FONT_SZ(t->font_size))))
			goto err;
	}
	
	if (t->inv_font_name) {
		fnt_free(t->inv_font);
		if (!(t->inv_font = fnt_load(t->inv_font_name, FONT_SZ(t->inv_font_size))))
			goto err;
	}


	if (t->menu_font_name) {
		fnt_free(t->menu_font);
		if (!(t->menu_font = fnt_load(t->menu_font_name, t->menu_font_size))) /* do not scale menu!!! */
			goto err;
	}


	if (t->a_up_name) {
		gfx_free_image(t->a_up);
		if (!(t->a_up = gfx_load_image(t->a_up_name)))
			goto err;
	}
	
	if (t->a_down_name) {
		gfx_free_image(t->a_down);
		if (!(t->a_down = gfx_load_image(t->a_down_name)))
			goto err;
	}

	if (t->inv_a_up_name) {
		gfx_free_image(t->inv_a_up);
		if (!(t->inv_a_up = gfx_load_image(t->inv_a_up_name)))
			goto err;
	}


	if (t->inv_a_down_name) {
		gfx_free_image(t->inv_a_down);
		if (!(t->inv_a_down = gfx_load_image(t->inv_a_down_name)))
			goto err;
	}

	if (t->bg_name) {
		gfx_free_image(t->bg);
		t->bg = NULL;
		if (t->bg_name[0] && !(t->bg = gfx_load_image(t->bg_name)))
			goto err;
	}

	if (t->use_name) {
		gfx_free_image(t->use);	
		if (!(t->use = gfx_load_image(t->use_name)))
			goto err;
	}
	
	if (t->menu_button_name) {
		gfx_free_image(t->menu_button);
		if (!(t->menu_button = gfx_load_image(t->menu_button_name)))
			goto err;
	}
	
	if (t->click_name) {
		snd_free_wav(t->click);
		t->click = snd_load_wav(t->click_name);
	}

	free_theme_strings();

	if (!t->use || !t->inv_a_up || !t->inv_a_down || !t->a_down || !t->a_up ||
		!t->font || !t->inv_font || !t->menu_font || !t->menu_button) {
		fprintf(stderr,"Can't init theme.\n");
		return -1;
	}
	return 0;
err:
	fprintf(stderr, "Can not init theme!\n");
	game_theme_free();
	return -1;
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

int parse_gfx_mode(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "fixed"))
		*i = GFX_MODE_FIXED;	
	else if (!strcmp(v, "embedded"))
		*i = GFX_MODE_EMBEDDED;
	else if (!strcmp(v, "float"))
		*i = GFX_MODE_FLOAT;	
	else
		return -1;
	return 0;	
}

int parse_inv_mode(const char *v, void *data)
{
	int *i = (int *)data;
	if (!strcmp(v, "vertical") || !strcmp(v, "0"))
		*i = INV_MODE_VERT;	
	else if (!strcmp(v, "horizontal") || !strcmp(v, "1"))
		*i = INV_MODE_HORIZ;
	else
		return -1;
	return 0;	
}

int parse_full_path(const char *v, void *data)
{
	char cwd[PATH_MAX];
	char **p = ((char **)data);
	if (*p)
		free(*p);
	getcwd(cwd, sizeof(cwd));
	*p = malloc(strlen(v) + strlen(cwd) + 2);
	if (!*p)
		return -1;
	strcpy(*p, cwd);
	strcat(*p,"/");
	strcat(*p, v);
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
int game_theme_load(const char *name);
int game_theme_select(const char *name);

int parse_include(const char *v, void *data)
{
	int rc;
	char cwd[PATH_MAX];
	if (!strcmp(v, DEFAULT_THEME))
		return 0;
	getcwd(cwd, sizeof(cwd));
	chdir(game_cwd);
	rc = game_theme_load(v);
//	if (!rc)
//		game_theme_select(v);
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
	{ "theme", parse_string, &opt_theme }, 
	{ "autosave", parse_int, &opt_autosave },
	{ "motion", parse_int, &opt_motion }, 
	{ "click", parse_int, &opt_click }, 
	{ "music", parse_int, &opt_music }, 
	{ "fscale", parse_int, &opt_fsize },
	{ "filter", parse_int, &opt_filter },
	{ "owntheme", parse_int, &opt_owntheme },
	{ NULL, },
};

struct parser cmd_parser[] = {
	{ "scr.w", parse_int, &game_theme.w },
	{ "scr.h", parse_int, &game_theme.h },
	{ "scr.col.bg", parse_color, &game_theme.bgcol },
	{ "scr.gfx.bg", parse_string, &game_theme.bg_name },
	{ "scr.gfx.use", parse_string, &game_theme.use_name },
	{ "scr.gfx.pad", parse_int, &game_theme.pad  }, 
	{ "scr.gfx.x", parse_int, &game_theme.gfx_x },
	{ "scr.gfx.y", parse_int, &game_theme.gfx_y },
	{ "scr.gfx.w", parse_int, &game_theme.max_scene_w },
	{ "scr.gfx.h", parse_int, &game_theme.max_scene_h },
	{ "scr.gfx.mode", parse_gfx_mode, &game_theme.gfx_mode },

	{ "win.x", parse_int, &game_theme.win_x },
	{ "win.y", parse_int, &game_theme.win_y },
	{ "win.w", parse_int, &game_theme.win_w },	
	{ "win.h", parse_int, &game_theme.win_h },	
	{ "win.fnt.name", parse_string, &game_theme.font_name },
	{ "win.fnt.size", parse_int, &game_theme.font_size },
/* compat mode directive */
	{ "win.gfx.h", parse_int, &game_theme.max_scene_h },
/* here it was */
	{ "win.gfx.up", parse_string, &game_theme.a_up_name },
	{ "win.gfx.down", parse_string, &game_theme.a_down_name },
	{ "win.col.fg", parse_color, &game_theme.fgcol }, 
	{ "win.col.link", parse_color, &game_theme.lcol },
	{ "win.col.alink", parse_color, &game_theme.acol }, 

	{ "inv.x", parse_int, &game_theme.inv_x },
	{ "inv.y", parse_int, &game_theme.inv_y },
	{ "inv.w", parse_int, &game_theme.inv_w },	
	{ "inv.h", parse_int, &game_theme.inv_h },	
	{ "inv.mode", parse_inv_mode, &game_theme.inv_mode },
	{ "inv.horiz", parse_inv_mode, &game_theme.inv_mode },	

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
	{ "menu.button.x", parse_int, &game_theme.menu_button_x },
	{ "menu.button.y", parse_int, &game_theme.menu_button_y },
/* compat */
	{ "menu.buttonx", parse_int, &game_theme.menu_button_x },
	{ "menu.buttony", parse_int, &game_theme.menu_button_y },

	{ "snd.click", parse_full_path, &game_theme.click_name },
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
		if (p[len] != '=') /* just ignore it */
			continue;
		p[len] = 0;
		val = p + len + 1;
		len = strcspn(p, " \t");
		p[len] = 0;
//		printf("%s\n", p);
		val += strspn(val, " \t");
		val[strcspn(val, ";\n")] = 0;
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
		fprintf(stderr, "Theme parsed with errors!\n");
//		game_theme_free();
		return -1;
	}
	return 0;
}

int theme_load(const char *name)
{
	if (theme_parse(name))
		return 0; /* no theme loaded if error in parsing */
	if (game_theme_init()) 
		return -1;
	return 0;
}

int cfg_parse(const char *path)
{
	return parse_ini(path, cfg_parser);
}

char *getfilepath(const char *d, const char *n)
{
	int i = strlen(d) + strlen(n) + 3;
	char *p = malloc(i);
	if (p) {
		strcpy(p, d);
		strcat(p, "/");
		strcat(p, n);
	}
	return p;
}

char *getpath(const char *d, const char *n)
{
	char *p = getfilepath(d, n);
	strcat(p, "/");
	return p;
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

struct game {
	char *path;
	char *name;
	char *dir;
};

struct	game *games = NULL;
int	games_nr = 0;



int game_select(const char *name)
{
	int i;
	if (!name || !*name) {
		if (games_nr == 1) 
			name = games[0].dir;
		else
			return 0;
	} 
	chdir(game_cwd);
	for (i = 0; i<games_nr; i ++) {
		if (!strcmp(games[i].dir, name)) {
			instead_done();
			if (instead_init())
				return -1;
			if (chdir(games[i].path))
				return -1;
			if (instead_load(MAIN_FILE))
				return -1;
			curgame = games[i].name;
			curgame_dir = games[i].dir;
			return 0;
		}
	}
	return 0;
}

static char *parse_tag(char *line, const char *tag, const char *comm, int *brk)
{
	char *l = line;
	l += strspn(l, " \t");
	if (strncmp(l, comm, strlen(comm))) { /* non coment block */
		*brk = 1;
		return NULL;
	}
	l += strlen(comm); l += strspn(l, " \t");
	if (strncmp(l, tag, strlen(tag)))
		return NULL;
	l += strlen(tag);
	l += strspn(l, " \t");
	l[strcspn(l, "$\n\r")] = 0;
	return strdup(l);
}

static char *game_name(const char *path, const char *d_name)
{
	int brk = 0;
	char *p = getfilepath(path, MAIN_FILE);
	if (p) {
		char *l; char line[1024];
		FILE *fd = fopen(p, "r");
		free(p);
		if (!fd)
			goto err;

		while ((l = fgets(line, sizeof(line), fd)) && !brk) {
			l = parse_tag(l, "$Name:", "--", &brk);
			if (l)
				return l;
		}
		fclose(fd);
	}
err:
	return strdup(d_name);
}

int games_lookup(const char *path)
{
	char *p;
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
		if (!is_game(path, de->d_name))
			continue;
		n ++;
	}
		
	rewinddir(d);
	if (!n)
		return 0;
	games = realloc(games, sizeof(struct game) * (n + games_nr));
	while ((de = readdir(d)) && i < n) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (!is_game(path, de->d_name))
			continue;
		p = getpath(path, de->d_name);
		games[games_nr].path = p;
		games[games_nr].dir = strdup(de->d_name);
		games[games_nr].name = game_name(p, de->d_name);
		games_nr ++;
		i ++;
	}
	closedir(d);
	return 0;
}

struct	theme *themes = NULL;
int	themes_nr = 0;

struct theme {
	char *path;
	char *name;
	char *dir;
};

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
	int brk = 0;
	char *p = getfilepath(path, THEME_FILE);
	if (p) {
		char *l; char line[1024];
		FILE *fd = fopen(p, "r");
		free(p);
		if (!fd)
			goto err;

		while ((l = fgets(line, sizeof(line), fd)) && !brk) {
			l = parse_tag(l, "$Name:", ";", &brk);
			if (l)
				return l;
		}
		fclose(fd);
	}
err:
	return strdup(d_name);
}

int themes_lookup(const char *path)
{
	char *p;
	int n = 0, i = 0;
	DIR *d;
	struct dirent *de;

	if (!path)
		return 0;

	d = opendir(path);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (!is_theme(path, de->d_name))
			continue;
		n ++;
	}
		
	rewinddir(d);
	if (!n)
		return 0;
	themes = realloc(themes, sizeof(struct theme) * (n + themes_nr));
	while ((de = readdir(d)) && i < n) {
		/*if (de->d_type != DT_DIR)
			continue;*/
		if (!is_theme(path, de->d_name))
			continue;
		p = getpath(path, de->d_name);
		themes[themes_nr].path = p;
		themes[themes_nr].dir = strdup(de->d_name);
		themes[themes_nr].name = theme_name(p, de->d_name);
		themes_nr ++;
		i ++;
	}
	closedir(d);
	return 0;
}

int theme_load(const char *name);

struct theme *theme_lookup(const char *name)
{
	int i;
	if (!name || !*name) {
		if (themes_nr == 1) 
			return &themes[0];
	}
	for (i = 0; i<themes_nr; i ++) {
		if (!strcmp(themes[i].dir, name)) {
			return &themes[i];
		}
	}
	return NULL;
}

int game_theme_load(const char *name)
{
	struct theme *theme;
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	chdir(game_cwd);
	theme = theme_lookup(name);
	if (!theme || chdir(theme->path) || theme_load(THEME_FILE)) {
		chdir(cwd);
		return -1;
	}
	chdir(cwd);
	return 0;
}

int game_theme_select(const char *name)
{
	struct theme *theme;
	theme = theme_lookup(name);
	if (!theme)
		return -1;
	curtheme = theme->name;
	curtheme_dir = theme->dir;
	return 0;
}

int game_default_theme(void)
{
	return game_theme_load(DEFAULT_THEME);
}

static int motion_mode = 0;
static int motion_id = 0;
static int motion_y = 0;

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
	menu_themes, 
	menu_own_theme,
	menu_custom_theme,
	menu_load, 
	menu_save, 
	menu_error, 
	menu_warning,
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

void game_menu(int nr)
{
	cur_menu = nr;
	menu_shown = 1;
	game_menu_box(menu_shown, game_menu_gen());
}

int game_error(const char *name)
{
	game_done();
	if (game_init(NULL)) {
		fprintf(stderr,"Fatal error! Can't init anything!!!\n");
		exit(1);
	}
	game_menu(menu_error);
	return 0;
}

void el_draw(int n);

static void custom_theme_warn(void)
{
	if (own_theme && !opt_owntheme) {
		game_menu(menu_custom_theme);
	}
}

int window_sw = 0;
int fullscreen_sw = 0;

static int game_load(int nr)
{
	char *s;
	s = game_save_path(0, nr);

	if (s && !access(s, R_OK)) {
		char cmd[PATH_MAX];
		snprintf(cmd, sizeof(cmd) - 1, "load %s", s);
		game_cmd(cmd);
		if (nr == -1)
			unlink(s);
		return 0;
	}
	return -1;
}

int game_apply_theme(void)
{
	layout_t lay;
	textbox_t box;

	memset(objs, 0, sizeof(struct el) * el_max);

	if (gfx_setmode(game_theme.w, game_theme.h, opt_fs))
		return -1;	
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
	txt_layout_active_color(lay, game_theme.acol);

	txt_box_set(box, lay);
	el_set(el_scene, elt_box, game_theme.win_x, 0, box);

	lay = txt_layout(game_theme.inv_font, (game_theme.inv_mode == INV_MODE_HORIZ)?
			ALIGN_CENTER:ALIGN_LEFT, game_theme.inv_w, game_theme.inv_h);
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

int game_init(const char *name)
{
	getcwd(game_cwd, sizeof(game_cwd));
	if (name)
		game_err_msg(NULL);

	if (gfx_init() || input_init())
		return -1;	

	snd_init(opt_hz);
	change_vol(0, opt_vol);

	if (game_default_theme()) {
		fprintf(stderr, "Can't load default theme.\n");
		return -1;
	}

	if (game_select(name))
		return -1;
	
	if (curgame && !access(THEME_FILE, R_OK)) {
		own_theme = 1;
	}
	
	if (own_theme && opt_owntheme) {
		if (theme_load(THEME_FILE))
			return -1;
	} else if (curtheme_dir && strcmp(DEFAULT_THEME, curtheme_dir)) {
		game_theme_load(curtheme_dir);
	}

	if (game_apply_theme())
		return -1;

	if (!curgame) {
		game_menu(menu_games);
	} else {
		if (!game_load(-1)) /* tmp save */
			return 0;
		if (opt_autosave && !game_load(0))  /* autosave */
			return 0;
		instead_eval("game:ini()");
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

void free_last(void)
{
	if (last_pict)
		free(last_pict);
	if (last_title)
		free(last_title);
	free_last_music();
	last_pict = last_title = NULL;
	snd_stop_mus(500);
}

void game_done(void)
{
	int i;
	if (opt_autosave && curgame)
		game_save(0);
	chdir(game_cwd);
//	cfg_save();
	
	if (menu_shown) {
		menu_shown = 0;
		game_menu_box(0, NULL);
	}
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
	input_clear();
	snd_done();
	instead_done();
	gfx_done();
	curgame = NULL;
	curgame_dir = NULL;
	own_theme = 0;
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
		free_last_music();
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
	free_last_music();
	snd_init(hz);
	snd_volume_mus(cur_vol);
	snd_free_wav(game_theme.click);
	game_theme.click = snd_load_wav(game_theme.click_name);
	music_player();
	opt_hz = snd_hz();
	return 0;
}
static int games_menu_from = 0;

static int themes_menu_from = 0;

int game_save(int nr)
{
	char *s = game_save_path(1, nr);
	char cmd[PATH_MAX];
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
static int restart_needed = 0;

static int game_restart(void)
{
	if (restart_needed) {
		restart_needed = 0;
		char *og = curgame_dir;
		game_save(-1);
		game_done();
		if (game_init(og)) {
			game_error(og);
			return 0;
		}
	}
	return 0;
}

int game_menu_act(const char *a)
{
	if (!strcmp(a, "/autosave")) {
		opt_autosave ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/owntheme")) {
		opt_owntheme ^= 1;
		if (own_theme)
			restart_needed = 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/motion")) {
		opt_motion ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/filter")) {
		opt_filter ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/click")) {
		opt_click ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/fs--")) {
		opt_fsize --;
		if (FONT_SZ(game_theme.font_size) > FONT_MIN_SZ) {
			restart_needed = 1;
		} else
			opt_fsize ++;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/fs++")) {
		opt_fsize ++;
		if (FONT_SZ(game_theme.font_size) < FONT_MAX_SZ) {
			restart_needed = 1;
		} else
			opt_fsize --;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/hl")) {
		opt_hl ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/fs")) {
		restart_needed = 1;
		opt_fs ^= 1;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/games_prev")) {
		games_menu_from -= MENU_GAMES_MAX;
		if (games_menu_from < 0)
			games_menu_from = 0;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/games_next")) {
		if (games_menu_from + MENU_GAMES_MAX < games_nr)
			games_menu_from += MENU_GAMES_MAX;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/themes_prev")) {
		themes_menu_from -= MENU_THEMES_MAX;
		if (themes_menu_from < 0)
			themes_menu_from = 0;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/themes_next")) {
		if (themes_menu_from + MENU_THEMES_MAX < themes_nr)
			themes_menu_from += MENU_THEMES_MAX;
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/select")) {
		game_menu(menu_games);
	} else if (!strcmp(a, "/themes")) {
		game_menu(menu_themes);
	} else if (!strcmp(a, "/save_menu")) {
		if (curgame)
			game_menu(menu_save);
	} else if (!strncmp(a, "/save", 5)) {
		if (!game_save(atoi(a + 5))) {
			game_menu(menu_saved);
		}
	} else if (!strcmp(a, "/load_menu")) {
		if (curgame)
			game_menu(menu_load);
	} else if (!strncmp(a, "/load", 5)) {
		int nr = atoi(a + 5);
		if (!curgame_dir)
			return 0;
		
		free_last();
		game_select(curgame_dir);
		menu_shown = 0;
		game_menu_box(0, NULL);
		game_load(nr);
		cur_menu = menu_main;
//		game_menu_box(0, NULL);
	} else if (!strcmp(a, "/new")) {
		char *s;
		if (!curgame_dir)
			return 0;
		free_last();
		game_select(curgame_dir);
		menu_shown = 0;
		game_menu_box(0, NULL);
		instead_eval("game:ini()");
		game_cmd("look");
		s = game_save_path(0, 0);
		if (s && !access(s, R_OK) && opt_autosave)
			unlink (s);
		custom_theme_warn();
	} else if (!strcmp(a,"/main")) {
		game_restart();
		game_menu(menu_main);
	} else if (!strcmp(a,"/ask_quit")) {
		game_menu(menu_askquit);
	} else if (!strcmp(a,"/about")) {
		game_menu(menu_about);
	} else if (!strcmp(a,"/mtoggle")) {
		if (!old_vol) {
			old_vol = snd_volume_mus(-1);
			change_vol(0, 0);
		} else {
			change_vol(0, old_vol);
			old_vol = 0;
		}
	} else if (!strcmp(a,"/music")) {
		opt_music ^= 1;
		if (!opt_music) {
			snd_stop_mus(0);
			free_last_music();
		} else
			music_player();
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a,"/resume")) {
		menu_shown = 0;
		cur_menu = menu_main;
		game_menu_box(0, NULL);
	} else if (!strcmp(a, "/settings")) {
		game_menu(menu_settings);
	} else if (!strcmp(a, "/vol--")) {
		change_vol(-10, 0);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/vol++")) {
		change_vol(+10, 0);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/vol-")) {
		change_vol(-1, 0);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/vol+")) {
		change_vol(+1, 0);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/hz-")) {
		int hz = snd_hz();
		if (hz == 48000)
			hz = 44100;
		else if (hz == 44100)
			hz = 22050;
		else if (hz == 22050)
			hz = 11025;
		else
			hz = 0;
		change_hz(hz);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a, "/hz+")) {
		int hz = snd_hz();
		if (hz == 11025)
			hz = 22050;
		else if (hz == 22050)
			hz = 44100;
		else if (hz == 44100)
			hz = 48000;	
		else
			hz = 0;
		change_hz(hz);
		game_menu_box(menu_shown, game_menu_gen());
	} else if (!strcmp(a,"/quit")) {
		return -1;
	} else if (cur_menu == menu_games) {
		char *p;
		p = strdup(a);
		if (p) {
			game_done();
			if (game_init(p)) {
				game_error(p);
			}
			free(p);
		}
	} else if (cur_menu == menu_themes) {
		char *p;
		p = strdup(a);
		if (p) {
			if (game_theme_select(p))
				fprintf(stderr, "Can't select theme:%s:%s\n", p, strerror(errno));
			char *og = curgame_dir;
			game_save(-1);
			game_done();
			if (game_init(og))
				game_error(og);
			else if (curgame && own_theme && opt_owntheme) {
				game_menu(menu_own_theme);
			}
			free(p);
		}
	}
	return 0;
}

char  menu_buff[4096];

char *slot_name(const char *path)
{
	struct stat 	st;
	int brk = 0;
	char *l; char line[1024];
	FILE *fd = fopen(path, "r");
	if (!fd)
		return NULL;

	while ((l = fgets(line, sizeof(line), fd)) && !brk) {
		l = parse_tag(l, "$Name:", "--", &brk);
		if (l) {
			char *s = fromgame(l);
			free(l);
			return s;
		}
	}
	fclose(fd);
	if (stat(path, &st))
		return NULL;
	l = ctime(&st.st_ctime);
	if (!l)
		return NULL;
	l[strcspn(l,"\n")] = 0;
	return strdup(l);
}

void load_menu(void)
{
	int i;
	*menu_buff = 0;
	sprintf(menu_buff, SELECT_LOAD_MENU);
	for (i = 0; i < MAX_SAVE_SLOTS; i ++) {
		char tmp[PATH_MAX];
		char *s = game_save_path(0, i);
		if (!s || access(s, R_OK)) {
			if (!i)
				continue;
			snprintf(tmp, sizeof(tmp), "<l>%d - "SAVE_SLOT_EMPTY"\n</l>", i);
		} else {
			char *name;
			if (!i)
				name = strdup(AUTOSAVE_SLOT);
			else
				name = slot_name(s);
			if (!name)
				snprintf(tmp, sizeof(tmp), "<l>%d - "BROKEN_SLOT"</l>\n", i);
			else {
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/load%d>%s</a></l>\n", i, i, name);
				free(name);
			}
		}
		strcat(menu_buff, tmp);
	}	
	strcat(menu_buff,"\n<a:/resume>Отмена</a>");
}

void save_menu(void)
{
	int i;
	*menu_buff = 0;
	sprintf(menu_buff, SELECT_SAVE_MENU);
	for (i = 1; i < MAX_SAVE_SLOTS; i ++) {
		char tmp[PATH_MAX];
		char *s = game_save_path(0, i);
		if (!s || access(s, R_OK))
			snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>"SAVE_SLOT_EMPTY"</a></l>\n", i, i);
		else {
			char *name;
			if (!i)
				name = strdup(AUTOSAVE_SLOT);
			else
				name = slot_name(s);
			if (!name)
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>"BROKEN_SLOT"</a></l>\n", i, i);
			else {
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>%s</a></l>\n", i, i, name);
				free(name);
			}
		}
		strcat(menu_buff, tmp);
	}	
	strcat(menu_buff,"\n<a:/resume>Отмена</a>");
}

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
			snprintf(tmp, sizeof(tmp), "<a:%s>%s</a>\n", games[i].dir, games[i].name);
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

void themes_menu(void)
{
	int i;
	*menu_buff = 0;
	sprintf(menu_buff, SELECT_THEME_MENU);
	for (i = themes_menu_from; i < themes_nr && i - themes_menu_from < MENU_THEMES_MAX; i ++) {
		char tmp[PATH_MAX];
		if (curtheme && !strcmp(themes[i].name, curtheme))
			snprintf(tmp, sizeof(tmp), "<a:/resume><b>%s</b></a>\n", themes[i].name);
		else
			snprintf(tmp, sizeof(tmp), "<a:%s>%s</a>\n", themes[i].dir, themes[i].name);
		strcat(menu_buff, tmp);
	}	
	if (!themes_nr)
		sprintf(menu_buff, NOTHEMES_MENU, THEMES_PATH);
	strcat(menu_buff,"\n");
	if (themes_menu_from)
		strcat(menu_buff,"<a:/themes_prev><<</a> ");
	strcat(menu_buff, BACK_MENU); 
	if (themes_menu_from + MENU_THEMES_MAX < themes_nr)
		strcat(menu_buff," <a:/themes_next>>></a>");
}

char *game_menu_gen(void)
{
	if (cur_menu == menu_main) {
		snprintf(menu_buff, sizeof(menu_buff), MAIN_MENU);
	} else if (cur_menu == menu_about) {
		snprintf(menu_buff, sizeof(menu_buff), ABOUT_MENU);
	} else if (cur_menu == menu_settings) {
		snprintf(menu_buff, sizeof(menu_buff), SETTINGS_MENU, 
		vol_to_pcn(snd_volume_mus(-1)), snd_hz(), opt_music?ON:OFF, opt_click?ON:OFF,
		opt_fs?ON:OFF, opt_fsize, opt_hl?ON:OFF, opt_motion?ON:OFF, opt_filter?ON:OFF,
		opt_owntheme?ON:OFF, opt_autosave?ON:OFF);
	} else if (cur_menu == menu_askquit) {
		snprintf(menu_buff, sizeof(menu_buff), QUIT_MENU);
	} else if (cur_menu == menu_saved) {
		snprintf(menu_buff, sizeof(menu_buff),
		SAVED_MENU);
	} else if (cur_menu == menu_games) {
		games_menu();
	} else if (cur_menu == menu_themes) {
		themes_menu();
	} else if (cur_menu == menu_own_theme) {
		snprintf(menu_buff, sizeof(menu_buff),
		OWN_THEME_MENU);
	} else if (cur_menu == menu_custom_theme) {
		snprintf(menu_buff, sizeof(menu_buff),
		CUSTOM_THEME_MENU);
	} else if (cur_menu == menu_load) {
		load_menu();
	} else if (cur_menu == menu_save) {
		save_menu();
	} else if (cur_menu == menu_error) {
		snprintf(menu_buff, sizeof(menu_buff),
		ERROR_MENU, err_msg?err_msg:UNKNOWN_ERROR);
		game_err_msg(NULL);
	} else if (cur_menu == menu_warning) {
		snprintf(menu_buff, sizeof(menu_buff),
		WARNING_MENU, err_msg?err_msg:UNKNOWN_ERROR);
		game_err_msg(NULL);
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
	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, game_theme.win_w - 2 * (b + pad), 0);
	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);	
	txt_layout_free(lay);

	lay = txt_layout(game_theme.menu_font, ALIGN_CENTER, w, 0);

	txt_layout_set(lay, (char*)txt);
	txt_layout_real_size(lay, &w, &h);	

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
	x = (game_theme.win_w - w)/2 + game_theme.win_x; //(game_theme.w - w)/2;
	y = (game_theme.win_h - h)/2 + game_theme.win_y; //(game_theme.h - h)/2;
	mx = x - b - pad;
	my = y - b - pad;
	mw = w + (b + pad) * 2;
	mh = h + (b + pad) * 2;
	menubg = gfx_grab_screen(mx, my, mw, mh);
	gfx_draw(menu, mx, my);
	el_set(el_menu, elt_layout, /*game_theme.win_x*/  x, y, lay);
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


static void dec_music(void *data)
{
	char *mus;
	if (!curgame)
		return;
	mus = instead_eval("return dec_music_loop()");
	if (!mus)
		return;
	if (atoi(mus) == -1)
		free_last_music();
	free(mus);
}

void game_music_finished(void)
{
	push_user_event(&dec_music, NULL);
}

void unix_path(char *path)
{
	char *p = path;
	if (!path)
		return;
	while (*p) { /* bad bad Windows!!! */
		if (*p == '\\')
			*p = '/';
		p ++;
	}
	return;
}

void music_player(void)
{
	int 	loop;
	char		*mus;
	if (!snd_volume_mus(-1))
		return;
	if (!opt_music)
		return;
		
	mus = instead_eval("return get_music_loop()");

	if (mus) {
		loop = atoi(mus);
		free(mus);
	} else
		loop = -1;
		
	mus = instead_eval("return get_music()");
	unix_path(mus);
	
	if (mus && loop == -1) { /* disabled, 0 - forever, 1-n - loops */
		free(mus);
		mus = NULL;
	}
	
	if (!mus) {
		if (last_music) {
			free(last_music);
			snd_stop_mus(500);
			last_music = NULL;
		}
	} else if (!last_music && mus) {
		last_music = mus;
		snd_stop_mus(500);
		snd_play_mus(mus, 0, loop - 1);
	} else if (strcmp(last_music, mus)) {
		free(last_music);
		last_music = mus;
		snd_stop_mus(500);
		snd_play_mus(mus, 0, loop - 1);
	} else
		free(mus);
}

char *horiz_inv(char *invstr)
{
	char *p = invstr;
	char *ns = malloc(strlen(p) * 3);
	char *np = ns;
	if (!np)
		return invstr;
	while (*p) {
		if (*p == '\n') {
			if (p[strspn(p, " \n\t")]) {
				*(np++) = ' ';
				*(np++) = '|';
				*(np) = ' ';
			} else
				break;
		} else
			*np = *p;
		p ++;
		np ++;
	}
	*(np++) = '\n';
	*np = 0;
	free(invstr);
	invstr = ns;
	return invstr;
}

int game_cmd(char *cmd)
{
	int		new_pict = 0, new_place = 0;
	int		title_h = 0, ways_h = 0, pict_h = 0;
	char 		buf[512];
	char 		*cmdstr;
	char 		*invstr;
	char 		*waystr;
	char		*title;
	char 		*pict;
	img_t		oldscreen = NULL;

	cmdstr = instead_cmd(cmd);
	if (!cmdstr) 
		goto err;
	music_player();
//	sound_player(); /* TODO */
	title = instead_eval("return get_title();");
	unix_path(title);
	if (title) {
		snprintf(buf, sizeof(buf), "<b><c><a:look>%s</a></c></b>", title);
		txt_layout_set(el_layout(el_title), buf);
	} else
		txt_layout_set(el_layout(el_title), NULL);

	new_place = check_new_place(title);

	txt_layout_size(el_layout(el_title), NULL, &title_h);
	title_h += game_theme.font_size / 2; // todo?	
	pict = instead_eval("return get_picture();");

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

	waystr = instead_cmd("way");
	invstr = instead_cmd("inv");

	if (invstr && game_theme.inv_mode == INV_MODE_HORIZ) {
		invstr = horiz_inv(invstr);
	}

	if (waystr) {
		waystr[strcspn(waystr,"\n")] = 0;
	}

	if (game_theme.gfx_mode != GFX_MODE_EMBEDDED) {
		txt_layout_set(el_layout(el_ways), waystr);
		txt_layout_size(el_layout(el_ways), NULL, &ways_h);
	} 


	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		int off = 0;
		if (!new_pict && !new_place) {
			off = txt_box_off(el_box(el_scene));
			if (off > pict_h)
				off = pict_h;
		}
		pict_h = 0; /* to fake code bellow */
		txt_layout_set(txt_box_layout(el_box(el_scene)), ""); /* hack, to null layout, but not images */
		if (el_img(el_spic)) {
			txt_layout_add_img(txt_box_layout(el_box(el_scene)),"scene", el_img(el_spic));
			txt_layout_add(txt_box_layout(el_box(el_scene)), "<c><g:scene></c>\n");
		}
		txt_layout_add(txt_box_layout(el_box(el_scene)), waystr);
		txt_layout_add(txt_box_layout(el_box(el_scene)), "<l>\n"); /* small hack */
		txt_layout_add(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
		if (!new_pict && !new_place) 
			txt_box_scroll(el_box(el_scene), off);
	} else {
		if (game_theme.gfx_mode == GFX_MODE_FLOAT) 
			pict_h = 0;	
		txt_layout_set(txt_box_layout(el_box(el_scene)), cmdstr);
		txt_box_set(el_box(el_scene), txt_box_layout(el_box(el_scene)));
	}
	free(cmdstr);
	el(el_ways)->y = el(el_title)->y + title_h + pict_h;
	if (waystr)
		free(waystr);

	el(el_scene)->y = el(el_ways)->y + ways_h;
	/* draw title and ways */
	if (new_pict || new_place) {
		img_t offscreen = gfx_new(game_theme.w, game_theme.h);
		oldscreen = gfx_screen(offscreen);
		gfx_draw(oldscreen, 0, 0);
	}
	if (new_pict || new_place) {
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
		if ((new_pict || new_place))
			el_draw(el_spic);
	}
	
	txt_box_resize(el_box(el_scene), game_theme.win_w, game_theme.win_h - title_h - ways_h - pict_h);
	el_draw(el_scene);

	do {
		int off = txt_box_off(el_box(el_inv));
		txt_layout_set(txt_box_layout(el_box(el_inv)), invstr);
		txt_box_set(el_box(el_inv), txt_box_layout(el_box(el_inv)));
		txt_box_scroll(el_box(el_inv), off);
	} while(0);
	
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
//		input_clear();
		goto err;
	}
	gfx_flip();
//	input_clear();
err:
	if (err_msg) {
		game_menu(menu_warning);
		return -1;
	}
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
	oel = elem;
	return 0;
}

void menu_toggle(void)
{
	game_cursor(0);
	game_highlight(-1, -1, 0);
	disable_inv();
	menu_shown ^= 1;
	motion_mode = 0;
	old_xref = old_el = NULL;
	if (!menu_shown)
		cur_menu = menu_main;
	game_menu_box(menu_shown, game_menu_gen());
}

static void scroll_pup(int id)
{
	game_cursor(0);
	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		int hh;
		el_size(el_scene, NULL, &hh);
		txt_box_scroll(el_box(id), -hh);
	} else
		txt_box_prev(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static void scroll_pdown(int id)
{
	game_cursor(0);
	game_highlight(-1, -1, 0);
	if (game_theme.gfx_mode == GFX_MODE_EMBEDDED) {
		int hh;
		el_size(el_scene, NULL, &hh);
		txt_box_scroll(el_box(id), hh);
	} else
		txt_box_next(el_box(id));
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static unsigned int old_counter = 0;
extern unsigned int timer_counter;
int mouse_filter(void)
{
	if (!opt_filter)
		return 0;
	if (abs(old_counter - timer_counter) <= 4) /* 400 ms */
		return -1;
	old_counter = timer_counter;
	return 0;
}

int game_click(int x, int y, int action)
{
	struct el	*elem = NULL;
	char buf[512];
	xref_t 	xref = NULL;

	if (action)
		motion_mode = 0;

	if (opt_filter && action) {
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
			xref_update(xref, elem->x, elem->y, game_clear);
		} else if (elem && elem->type == elt_box && opt_motion) {
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
			} else if (disable_inv())
				el_update(el_inv);
			motion_mode = 0;
		} else if (disable_inv()) {
			el_update(el_inv);
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

	if (elem->id == el_ways ||
		elem->id == el_title) {
		strcpy(buf, xref_get_text(xref));
		if (mouse_filter())
			return 0;
		if (opt_click)
			snd_play(game_theme.click);
		if (disable_inv()) {
			el_update(el_inv);
			return 0;
		}
		game_cmd(buf);
		return 1;
	}

	if (elem->id == el_scene) {
		if (inv_xref) {
			snprintf(buf,sizeof(buf), "use %s,%s", xref_get_text(inv_xref), xref_get_text(xref));
			disable_inv();
		} else	
			strcpy(buf, xref_get_text(xref));
		if (mouse_filter())
			return 0;
		if (opt_click)
			snd_play(game_theme.click);
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
		if (mouse_filter())
			return 0;
		if (opt_click)
			snd_play(game_theme.click);
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
		if (!on) {
			gfx_update(xc, yc, gfx_img_w(game_theme.use), gfx_img_h(game_theme.use));
			return;
		}
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


static void scroll_up(int id, int count)
{
	int i;
	game_cursor(0);
	game_highlight(-1, -1, 0);
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
	game_cursor(0);
	game_highlight(-1, -1, 0);
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
	game_cursor(0);
	game_highlight(-1, -1, 0);
	txt_box_scroll(el_box(id), off);
	el_clear(id);
	el_draw(id);
	el_update(id);
}

static int alt_pressed = 0;
int game_loop(void)
{
	static int x = 0, y = 0;
	struct inp_event ev;
	while (1) {
		int rc;
		ev.x = -1;
		while ((rc = input(&ev, 1)) == AGAIN);
		if (rc == -1) /* close */
			break;
		else if (((ev.type ==  KEY_DOWN) || (ev.type == KEY_UP)) && ev.sym && 
			(!strcmp(ev.sym,"left alt") || !strcmp(ev.sym, "right alt"))) {
			alt_pressed = (ev.type == KEY_DOWN) ? 1:0;
		} else if (ev.type == KEY_DOWN && ev.sym) {
			if (!strcmp(ev.sym,"escape")) {
				menu_toggle();
			} else if (!strcmp(ev.sym,"up") && !menu_shown) {
				scroll_up(el_scene, 1);
			} else if (!strcmp(ev.sym,"down") && !menu_shown) {
				scroll_down(el_scene, 1);
			} else if ((!strcmp(ev.sym,"page up") || !strcmp(ev.sym, "backspace")) && !menu_shown) {
				scroll_pup(el_scene);
			} else if ((!strcmp(ev.sym,"page down") || !strcmp(ev.sym, "space")) && !menu_shown) {
				scroll_pdown(el_scene);
			} else if (alt_pressed && !strcmp(ev.sym, "q")) {
				break;
			} else if (alt_pressed &&
				(!strcmp(ev.sym,"enter") || !strcmp(ev.sym, "return"))) {
				int old_menu = -1;
				game_menu_act("/fs");
				game_highlight(-1, -1, 0);
				disable_inv();
				old_xref = old_el = NULL;
				if (menu_shown)
					old_menu = cur_menu;
				game_restart();
				if (old_menu != -1)
					game_menu(old_menu);
//				game_menu_act("/main");
			}
		} else if (ev.type == MOUSE_DOWN) {
			game_cursor(0);
			game_highlight(-1, -1, 0);
			game_click(ev.x, ev.y, 0);
			x = ev.x;
			y = ev.y;
		} else if (ev.type == MOUSE_UP) {
			game_cursor(0);
			game_highlight(-1, -1, 0);
			if (game_click(ev.x, ev.y, 1) == -1)
				break;
		} else if (ev.type == MOUSE_WHEEL_UP && !menu_shown) {
			int xm, ym;
			struct el *o;
			gfx_cursor(&xm, &ym, NULL, NULL);
			o = look_obj(xm, ym);
			if (o && (o->id == el_scene || o->id == el_inv)) {
				scroll_up(o->id, ev.count);
			}
		} else if (ev.type == MOUSE_WHEEL_DOWN && !menu_shown) {
			int xm, ym;
			struct el *o;
			gfx_cursor(&xm, &ym, NULL, NULL);
			o = look_obj(xm, ym);
			if (o && (o->id == el_scene || o->id == el_inv)) {
				scroll_down(o->id, ev.count);
			}
		} else if (ev.type == MOUSE_MOTION) {
			if (motion_mode) {
				scroll_motion(motion_id, motion_y - ev.y);
				motion_y = ev.y;
			}
		//	game_highlight(ev.x, ev.y, 1);
		}
		if (old_xref)
			game_highlight(x, y, 1);
		else if (ev.x >= 0)
			game_highlight(ev.x, ev.y, 1);
		game_cursor(1);
	}
	return 0;
}

