#include "externals.h"
#include "internals.h"

char	*curtheme_dir = NULL;

static int parse_gfx_mode(const char *v, void *data)
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

static int parse_inv_mode(const char *v, void *data)
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

static int parse_color(const char *v, void *data)
{
	color_t *c = (color_t *)data;
	return gfx_parse_color(v, c);
}

static int parse_include(const char *v, void *data)
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

struct parser cmd_parser[] = {
	{ "scr.w", parse_int, &game_theme.w },
	{ "scr.h", parse_int, &game_theme.h },
	{ "scr.col.bg", parse_color, &game_theme.bgcol },
	{ "scr.gfx.bg", parse_string, &game_theme.bg_name },
	{ "scr.gfx.cursor.normal", parse_string, &game_theme.cursor_name },
	{ "scr.gfx.cursor.x", parse_int, &game_theme.cur_x },
	{ "scr.gfx.cursor.y", parse_int, &game_theme.cur_y },
	{ "scr.gfx.use", parse_string, &game_theme.use_name }, /* compat */
	{ "scr.gfx.cursor.use", parse_string, &game_theme.use_name },
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

struct game_theme game_theme = {
	.w = 800,
	.h = 480,
	.bg_name = NULL,
	.bg = NULL,
	.use_name = NULL,
	.cursor_name = NULL,
	.use = NULL,
	.cursor = NULL,
	.cur_x = 0,
	.cur_y = 0,
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
	game_theme.cur_x = game_theme.cur_y = 0;
//	game_theme.slide = gfx_load_image("slide.png", 1);
	return 0;
}


static int game_theme_init(void)
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

	if (t->cursor_name) {
		gfx_free_image(t->cursor);	
		if (!(t->cursor = gfx_load_image(t->cursor_name)))
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

	if (!t->cursor || !t->use || !t->inv_a_up || !t->inv_a_down || !t->a_down || !t->a_up ||
		!t->font || !t->inv_font || !t->menu_font || !t->menu_button) {
		fprintf(stderr,"Can't init theme. Not all required elements are defined.\n");
		return -1;
	}
	return 0;
err:
	fprintf(stderr, "Can not init theme!\n");
	game_theme_free();
	return -1;
}

static int theme_parse(const char *path)
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

static struct theme *theme_lookup(const char *name)
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
	curtheme_dir = theme->dir;
	return 0;
}

int game_default_theme(void)
{
	return game_theme_load(DEFAULT_THEME);
}
