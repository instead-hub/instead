/*
 * Copyright 2009-2016 Peter Kosyh <p.kosyh at gmail.com>
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


static int restart_needed = 0;
static int games_menu_from = 0;
static int themes_menu_page = 0;

static int cur_lang = 0;

int cur_menu = menu_main;
int top_menu = menu_main;

char *UNKNOWN_ERROR = NULL;
char *ERROR_MENU = NULL;
char *WARNING_MENU = NULL;
char *SAVE_SLOT_EMPTY = NULL;
char *SELECT_LOAD_MENU = NULL;
char *AUTOSAVE_SLOT = NULL;
char *BROKEN_SLOT = NULL;
char *SELECT_SAVE_MENU = NULL;
char *MAIN_MENU = NULL;
char *ABOUT_MENU = NULL;
char *BACK_MENU = NULL;
char *SETTINGS_SND_MENU = NULL;
char *SETTINGS_GFX_MENU = NULL;
char *SETTINGS_OTH_MENU = NULL;
char *CUSTOM_THEME_MENU = NULL;
char *OWN_THEME_MENU = NULL;
char *WAIT_MENU = NULL;
char *SELECT_GAME_MENU = NULL;
char *SELECT_THEME_MENU = NULL;
char *SAVED_MENU = NULL;
char *NOGAMES_MENU = NULL;
char *NOTHEMES_MENU = NULL;
char *BROWSE_MENU = NULL;
char *QUIT_MENU = NULL;
char *REMOVE_MENU = NULL;
char *ON = NULL;
char *OFF = NULL;

char *KBD_MODE_LINKS = NULL;
char *KBD_MODE_SMART = NULL;
char *KBD_MODE_SCROLL = NULL;
char *CANCEL_MENU = NULL;

char *FROM_THEME = NULL;

char *DISABLED_SAVE_MENU = NULL;

static char  menu_buff[8192];

static char *slot_name(const char *path)
{
	struct stat 	st;
	char *l;
	if (stat(path, &st))
		return NULL;
	l = lookup_tag(path, "Name", "--");
	if (l) {
		trunc_lines(l, 0);
		if (!is_empty(l) && game_tag_valid(l)) {
			int y;
			char *m;
			static char *months[] = {/* to work on every locale ;)*/
				"Jan", "Feb",
				"Mar", "Apr", "May",
				"Jun", "Jul", "Aug",
				"Sep", "Oct", "Nov",
				"Dec",
			};
			struct tm *tm;
			time_t t;

			char *s = instead_fromgame(l);
			free(l);
			if (!s)
				return s;
			time(&t);
			tm = localtime(&t);
			y = tm->tm_year;
			tm = localtime(&st.st_mtime);
			l = malloc(strlen(s) + 64);
			if (!l)
				return s;
			if (tm->tm_mon >=0 && tm->tm_mon < 12)
				m = months[tm->tm_mon];
			else
				m = "?";
			if (tm->tm_year == y)
				snprintf(l, 64, "%02d %s %02d:%02d - ", 
					tm->tm_mday, m, tm->tm_hour, tm->tm_min);
			else {
				if (tm->tm_year < 1900)
					tm->tm_year += 1900;
				snprintf(l, 64, "%02d %s %02d:%02d %04d - ", 
					tm->tm_mday, m, tm->tm_hour, tm->tm_min, tm->tm_year);
			}
			strcat(l, s);
			free(s);
			return l;
		}
		free(l);
	}
	l = ctime(&st.st_mtime);
	if (!l)
		return NULL;
	l[strcspn(l,"\n")] = 0;
	return strdup(l);
}

static void load_menu(void)
{
	int i;
	*menu_buff = 0;
	/*
	if (!game_saves_enabled()) {
		strcat(menu_buff, DISABLED_SAVE_MENU);
		strcat(menu_buff, CANCEL_MENU);
		return;
	} */
	strcpy(menu_buff, SELECT_LOAD_MENU);
	for (i = 0; i < MAX_SAVE_SLOTS; i ++) {
		char tmp[PATH_MAX];
		char *s = game_save_path(0, i);
		if (!s || access(s, R_OK)) {
			if (!i)
				continue;
			snprintf(tmp, sizeof(tmp), "<l>%d - %s\n</l>", i, SAVE_SLOT_EMPTY);
		} else {
			char *name;
			if (!i)
				name = strdup(AUTOSAVE_SLOT);
			else
				name = slot_name(s);
			if (!name)
				snprintf(tmp, sizeof(tmp), "<l>%d - %s</l>\n", i, BROKEN_SLOT);
			else {
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/load%d>%s</a></l>\n", i, i, name);
				free(name);
			}
		}
		strcat(menu_buff, tmp);
	}	
	strcat(menu_buff,"\n");
	strcat(menu_buff, CANCEL_MENU);
}

static void save_menu(void)
{
	int i;
	*menu_buff = 0;
	if (!game_saves_enabled()) {
		strcat(menu_buff, DISABLED_SAVE_MENU);
		strcat(menu_buff, CANCEL_MENU);
		return;
	}
	strcpy(menu_buff, SELECT_SAVE_MENU);
	for (i = 1; i < MAX_SAVE_SLOTS; i ++) {
		char tmp[PATH_MAX];
		char *s = game_save_path(0, i);
		if (!s || access(s, R_OK))
			snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>%s</a></l>\n", i, i, SAVE_SLOT_EMPTY);
		else {
			char *name;
			if (!i)
				name = strdup(AUTOSAVE_SLOT);
			else
				name = slot_name(s);
			if (!name)
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>%s</a></l>\n", i, i, BROKEN_SLOT);
			else {
				snprintf(tmp, sizeof(tmp), "<l>%d - <a:/save%d>%s</a></l>\n", i, i, name);
				free(name);
			}
		}
		strcat(menu_buff, tmp);
	}	
	strcat(menu_buff,"\n");
	strcat(menu_buff, CANCEL_MENU);
}

static int pages_menu(char *res, int nr, int max, const char *menu, const char *append)
{
	static char buff[256];
	int k = MENU_PER_PAGER;
	int i = nr - MENU_PER_PAGER / 2;

	if (i < 0)
		i = 0;
	else if (max - i < MENU_PER_PAGER)
		i = max - MENU_PER_PAGER;
	if (i < 0)
		i = 0;
	if (nr)
		sprintf(buff, "<a:/%s prev><<</a> ", menu);
	else
		sprintf(buff, "<< ");
	strcat(res, buff);
	for (; i < max && k-- ; i ++) {
		if (i != nr)
			sprintf(buff, "<a:/%s %d>[%d]</a> ", menu, i, i + 1);
		else
			sprintf(buff, "[%d] ", i + 1);
		strcat(res, buff);
	}
	if ((nr + 1) != max)
		sprintf(buff, "<a:/%s next>>></a>", menu); 
	else
		sprintf(buff, ">>"); 
	strcat(res, buff);
	strcat(res, append);
	return 0;
}

static void games_menu(void)
{
	int i, n;
#ifdef _USE_BROWSE
	int append_browse = 0;
#endif
	char tmp[PATH_MAX];
#ifdef _USE_BROWSE
	snprintf(tmp, sizeof(tmp), " <u><a:/browse>%s</a></u>\n", BROWSE_MENU);
#else
	snprintf(tmp, sizeof(tmp), "\n");
#endif
	strcpy(menu_buff, SELECT_GAME_MENU);
	if ((games_nr - 1) / MENU_GAMES_MAX)
		pages_menu(menu_buff, games_menu_from / MENU_GAMES_MAX, (games_nr - 1) / MENU_GAMES_MAX + 1, "games", tmp);
#ifdef _USE_BROWSE
	else 
		append_browse = 1;
#endif
	for (i = games_menu_from, n = 0; i < games_nr && n < MENU_GAMES_MAX; i ++) {
		
		if (!games[i].name[0]) /* empty */
			continue;
		if (curgame_dir && !strcmp(games[i].dir, curgame_dir))
			snprintf(tmp, sizeof(tmp), "<l><a:/resume><b>%s</b></a></l>", games[i].name);
		else
			snprintf(tmp, sizeof(tmp), "<l><a:%s>%s</a></l>", games[i].dir, games[i].name);
		if (!strncmp(game_local_games_path(0), games[i].path, strlen(game_local_games_path(0))) &&
			!access(games[i].path, W_OK)) {
			snprintf(tmp + strlen(tmp), sizeof(tmp), " [<a:/remove_%d>X</a>]\n", i);
		} else
			strcat(tmp, "\n");

		strcat(menu_buff, tmp);
		n ++;
	}
	for(;n < MENU_GAMES_MAX && games_nr > MENU_GAMES_MAX; n++) /* align h */
		strcat(menu_buff, "\n");
#ifdef _USE_BROWSE
	if (append_browse) {
		snprintf(tmp, sizeof(tmp), "<u><a:/browse>%s</a></u>\n", BROWSE_MENU);
		strcat(menu_buff, tmp);
	}
#endif
	if (!games_nr) {
		sprintf(menu_buff, NOGAMES_MENU, GAMES_PATH);
#ifdef _USE_BROWSE
		strcat(menu_buff,"\n");
		snprintf(tmp, sizeof(tmp), "<u><a:/browse>%s</a></u>\n", BROWSE_MENU);
		strcat(menu_buff, tmp);
#endif
	}
	strcat(menu_buff,"\n");
	strcat(menu_buff, BACK_MENU); 
}

int games_menu_maxw(void)
{
	int i = 0;
	int oldm = games_menu_from;
	int maxw = 0;
	for (i = 0; i < games_nr; i += MENU_GAMES_MAX) {
		int w;
		games_menu_from = i;
		games_menu();
		game_menu_box_wh(menu_buff, &w, NULL);
		if (w > maxw)
			maxw = w;
	}
	games_menu_from = oldm;
	return maxw;
}

static void themes_menu(void)
{
	int i, n, m;
	int type;
	int count = themes_max(&type);
	int pages = 0;

	if (count > 0)
		pages = (count - 1) / MENU_THEMES_MAX + 1;

	if (themes_menu_page >= pages)
		themes_menu_page = 0;

	strcpy(menu_buff, SELECT_THEME_MENU);

	if (pages > 1)
		pages_menu(menu_buff, themes_menu_page, pages, "themes", "\n");

	for (i = 0, n = 0, m = 0; i < themes_nr && n < MENU_THEMES_MAX; i ++) {
		char tmp[PATH_MAX];
		if (!themes[i].name[0]) /* empty */
			continue;
		if (themes[i].type != type)
			continue;
		m ++;
		if ((m - 1) / MENU_THEMES_MAX < themes_menu_page)
			continue;
		if (curtheme_dir[type] && !strcmp(themes[i].dir, curtheme_dir[type]))
			snprintf(tmp, sizeof(tmp), "<l><a:/resume><b>%s</b></a></l>\n", themes[i].name);
		else
			snprintf(tmp, sizeof(tmp), "<l><a:%s>%s</a></l>\n", themes[i].dir, themes[i].name);
		strcat(menu_buff, tmp);
		n ++;
	}

	for(;n < MENU_THEMES_MAX && count > MENU_THEMES_MAX; n++) /* align h */
		strcat(menu_buff, "\n");

	if (!count)
		sprintf(menu_buff, NOTHEMES_MENU, THEMES_PATH);
	strcat(menu_buff, "\n");
	strcat(menu_buff, BACK_MENU); 
}

int themes_menu_maxw(void)
{
	int i = 0;
	int oldm = themes_menu_page;
	int maxw = 0;
	int pages = 0;
	int count = themes_max(NULL);
	if (count > 0)
		pages = (count - 1) / MENU_THEMES_MAX + 1;
	else
		pages = 1;
	for (i = 0; i < pages; i ++) {
		int w;
		themes_menu_page = i;
		themes_menu();
		game_menu_box_wh(menu_buff, &w, NULL);
		if (w > maxw)
			maxw = w;
	}
	themes_menu_page = oldm;
	return maxw;
}

static char *opt_get_mode(void)
{
	static char buff[128];
	if (opt_mode[0] == -1 || opt_mode[1] == -1) {
		snprintf(buff, sizeof(buff), "%s", FROM_THEME);
		return buff;
	}
	snprintf(buff, sizeof(buff), "%dx%d", opt_mode[0], opt_mode[1]);
	return buff;
}

static int gtr = 0;
static int menu_settings_num = 0;

static void menu_strip_tag(const char *a, const char *b)
{
	char *p, *ep;
	size_t len;
	p = strstr(menu_buff, a);
	if (!p)
		return;
	ep = strstr(p, b);
	if (!ep)
		return;
/*	ep += strcspn(ep, "\n\r");
	ep += strspn(ep, "\n\r"); */
	ep += strlen(b);
	len = strlen(ep);
	memmove(p, ep, len);
	p[len] = 0;
}

static void menu_remove_tag(const char *a, const char *b)
{
	char *p, *ep;
	size_t len;
	p = strstr(menu_buff, a);
	if (!p)
		return;
	ep = p + strlen(a);
	len = strlen(ep);
	memmove(p, ep, len);
	p[len] = 0;

	ep = strstr(p, b);
	if (!ep)
		return;
	p = ep;
	ep += strlen(b);
	len = strlen(ep);

	memmove(p, ep, len);
	p[len] = 0;
}

char *game_menu_gen(void)
{
	if (cur_menu == menu_main) {
		strcpy(menu_buff, MAIN_MENU);
		if (standalone_sw) {
			int count = 0, n;
			menu_strip_tag("<?:select>", "</?>");
			count = themes_count(THEME_GLOBAL);
			if (curgame_dir && opt_owntheme) {
				if ((n = themes_count(THEME_GAME)) > 0)
					count = n;
				else if (game_own_theme)
					count = 1;
			}
			if (count <= 1)
				menu_strip_tag("<?:themes>", "</?>");
			else
				menu_remove_tag("<?:themes>", "</?>");
		} else {
			menu_remove_tag("<?:select>", "</?>");
			menu_remove_tag("<?:themes>", "</?>");
		}
	} else if (cur_menu == menu_about || cur_menu == menu_about_instead) {
		struct game *g;
		if (cur_menu == menu_about && curgame_dir && (g = game_lookup(curgame_dir))) {
			char version[32];
			char author[64];
			char info[192];
			char instead[64];

			if (g->version)
				snprintf(version, sizeof(version), "%s", g->version);
			else
				strcpy(version, "1.0");

			if (g->author)
				snprintf(author, sizeof(author), "\n%s", g->author);
			else
				strcpy(author, "");

			if (g->info)
				snprintf(info, sizeof(info), "\n\n%s", g->info);
			else
				strcpy(info, "");

			if (!standalone_sw)
				snprintf(instead, sizeof(instead), "<a:/about-instead>INSTEAD</a> | ");
			else
				strcpy(instead, "");
			author[sizeof(author) - 1] = 0;
			version[sizeof(version) - 1] = 0;
			info[sizeof(info) - 1] = 0;
			snprintf(menu_buff, sizeof(menu_buff), "%s - %s%s%s\n\n%s%s", 
				g->name, version,
				author, info,
				instead, BACK_MENU);
		} else {
			snprintf(menu_buff, sizeof(menu_buff), ABOUT_MENU, VERSION);
		}
	} else if (cur_menu == menu_settings) {
		char *just[JUST_MAX] = { FROM_THEME, OFF, ON };
		char *kbd [KBD_MAX] = { KBD_MODE_SMART, KBD_MODE_LINKS, KBD_MODE_SCROLL };
		int fsize = 100 + (10 * opt_fsize);
		opt_kbd = (unsigned int)opt_kbd % KBD_MAX;
		opt_justify = (unsigned int)opt_justify % JUST_MAX;
		switch (menu_settings_num) {
		case 0:
			snprintf(menu_buff, sizeof(menu_buff), SETTINGS_GFX_MENU, 
			opt_get_mode(), opt_fs?ON:OFF, opt_hires?ON:OFF, fsize, just[opt_justify],
				opt_hl?ON:OFF, opt_fading?ON:OFF, opt_owntheme?ON:OFF);
			if (standalone_sw)
				menu_strip_tag("<?:owntheme>", "</?>");
			else
				menu_remove_tag("<?:owntheme>", "</?>");
			break;
		case 1:
			snprintf(menu_buff, sizeof(menu_buff), SETTINGS_SND_MENU, 
			snd_vol_to_pcn(snd_volume_mus(-1)), snd_hz(), opt_music?ON:OFF, opt_click?ON:OFF);
			break;
		case 2:
			snprintf(menu_buff, sizeof(menu_buff), SETTINGS_OTH_MENU, 
			opt_motion?ON:OFF, opt_filter?ON:OFF, kbd[opt_kbd],
			langs[cur_lang].name,  (opt_autosave & 1)?ON:OFF);
			break;
		}
	} else if (cur_menu == menu_askquit) {
		strcpy(menu_buff, QUIT_MENU);
	} else if (cur_menu == menu_saved) {
		strcpy(menu_buff, SAVED_MENU);
	} else if (cur_menu == menu_games) {
		games_menu();
	} else if (cur_menu == menu_themes) {
		themes_menu();
	} else if (cur_menu == menu_own_theme) {
		strcpy(menu_buff, OWN_THEME_MENU);
	} else if (cur_menu == menu_wait) {
		strcpy(menu_buff, WAIT_MENU);
	} else if (cur_menu == menu_custom_theme) {
		strcpy(menu_buff, CUSTOM_THEME_MENU);
	} else if (cur_menu == menu_load) {
		load_menu();
	} else if (cur_menu == menu_save) {
		save_menu();
	} else if (cur_menu == menu_error) {
		snprintf(menu_buff, sizeof(menu_buff),
		ERROR_MENU, instead_err()?instead_err():UNKNOWN_ERROR);
		instead_err_msg(NULL);
	} else if (cur_menu == menu_warning) {
		snprintf(menu_buff, sizeof(menu_buff),
		WARNING_MENU, instead_err()?instead_err():UNKNOWN_ERROR);
		instead_err_msg(NULL);
	} else if (cur_menu == menu_remove) {
		const char *sname = games[gtr].path;
		if (strlen(games[gtr].path) >= 48) {
			sname = games[gtr].name;
		}
		snprintf(menu_buff, sizeof(menu_buff), REMOVE_MENU, sname);
	}
	return menu_buff;
}


int game_menu_act(const char *a)
{
	static int old_vol = 0;

	if (!strcmp(a, "/autosave")) {
		opt_autosave = !(opt_autosave & 1);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/kbd")) {
		opt_kbd += 1;
		if (opt_kbd == KBD_MAX)
			opt_kbd = 0;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/owntheme")) {
		opt_owntheme = !opt_owntheme;
		if (game_own_theme)
			restart_needed = 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/motion")) {
		opt_motion ^= 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/filter")) {
		opt_filter ^= 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/click")) {
		opt_click ^= 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/mode++")) {
		if (gfx_next_mode(&opt_mode[0], &opt_mode[1]))
			opt_mode[0] = opt_mode[1] = -1;
		restart_needed = 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/mode--")) {	
		if (gfx_prev_mode(&opt_mode[0], &opt_mode[1]))
			opt_mode[0] = opt_mode[1] = -1;
		restart_needed = 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/just++")) {
		restart_needed = 1;
		opt_justify ++;
		if (opt_justify >= JUST_MAX)
			opt_justify = 0;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/just--")) {	
		restart_needed = 1;
		opt_justify --;
		if (opt_justify < 0)
			opt_justify = JUST_MAX - 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/fs--")) {
		opt_fsize --;
		if (opt_fsize >= FONT_MIN_SZ) {
			restart_needed = 1;
		} else
			opt_fsize ++;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/fs++")) {
		opt_fsize ++;
		if (opt_fsize <= FONT_MAX_SZ) {
			restart_needed = 1;
		} else
			opt_fsize --;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/hl")) {
		opt_hl ^= 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/fading")) {
		opt_fading ^= 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/fs")) {
#if !defined(ANDROID) && !defined(IOS)
		restart_needed = 1;
		opt_fs ^= 1;
		game_menu_box(1, game_menu_gen());
#endif
	} else if (!strcmp(a, "/hires")) {
		opt_hires ^= 1;
		if (opt_fs)
			restart_needed = 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strncmp(a, "/games ", 7)) {
		if (!strcmp(a + 7, "prev")) {
			games_menu_from -= MENU_GAMES_MAX;
			if (games_menu_from < 0)
				games_menu_from = 0;
		} else if (!strcmp(a + 7, "next")) {
			if (games_menu_from + MENU_GAMES_MAX < games_nr)
				games_menu_from += MENU_GAMES_MAX;
		} else {
			int nr = atoi(a + 7);
			games_menu_from = nr * MENU_GAMES_MAX;
		}
		game_menu_box(1, game_menu_gen());
	} else if (!strncmp(a, "/themes ", 8)) {
		if (!strcmp(a + 8, "prev")) {
			themes_menu_page --;
			if (themes_menu_page < 0)
				themes_menu_page = 0;
		} else if (!strcmp(a + 8, "next")) {
			int count = themes_max(NULL); int pages = 0;
			if (count > 0)
				pages = (count - 1) / MENU_THEMES_MAX + 1;
			if (themes_menu_page + 1 < pages)
				themes_menu_page ++;
		} else {
			int nr = atoi(a + 8);
			themes_menu_page = nr;
		}
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/select")) {
		game_menu(menu_games);
	} else if (!strcmp(a, "/remove")) {
		fprintf(stderr,"Removing '%s'\n", games[gtr].path);
		if (curgame_dir && !strcmp(curgame_dir, games[gtr].dir)) {
			game_done(0);
			if (game_init(NULL)) {
				game_error();
				return 0;
			}
		}
		games_remove(gtr);
		if (games_menu_from >= games_nr)
			games_menu_from -= MENU_GAMES_MAX;
		if (games_menu_from < 0)
			games_menu_from = 0;
		game_menu(menu_games);
	} else if (!strcmp(a, "/themes")) {
		game_menu(menu_themes);
	} else if (!strcmp(a, "/save_menu")) {
		if (curgame_dir)
			game_menu(menu_save);
	} else if (!strncmp(a, "/save", 5)) {
		if (!game_save(atoi(a + 5))) {
			game_menu(menu_saved);
		}
	} else if (!strcmp(a, "/load_menu")) {
		if (curgame_dir)
			game_menu(menu_load);
	} else if (!strncmp(a, "/load", 5)) {
		int nr = atoi(a + 5);
		if (!curgame_dir)
			return 0;
		game_menu_box(0, NULL);
		if (!game_reset()) {
			game_load(nr);
			cur_menu = menu_main;
		}
/*		game_menu_box(0, NULL); */
	} else if (!strcmp(a, "/new")) {
		char *s;
		if (!curgame_dir)
			return 0;

/* remove autlosave */
		s = game_save_path(0, 0);
		if (s && !access(s, R_OK) && (opt_autosave & 1))
			unlink (s);
		game_menu_box(0, NULL);
		if (!game_reset()) {
			game_cmd("look", 0);
			custom_theme_warn();
		}
	} else if (!strcmp(a,"/main")) {
		if (top_menu != menu_main) {
			cur_menu = menu_main;
			game_menu_box(0, NULL);
		} else
			game_menu(menu_main);
		if (restart_needed) {
			game_restart();
			restart_needed = 0;
		}
	} else if (!strcmp(a,"/ask_quit")) {
		game_menu(menu_askquit);
	} else if (!strncmp(a, "/remove_", 8)) {
		gtr = atoi(a + 8);
		game_menu(menu_remove);
	} else if (!strcmp(a,"/about")) {
		game_menu(menu_about);
	} else if (!strcmp(a,"/about-instead")) {
		game_menu(menu_about_instead);
	} else if (!strcmp(a,"/mtoggle")) {
		if (!old_vol) {
			old_vol = snd_volume_mus(-1);
			game_change_vol(0, 0);
		} else {
			game_change_vol(0, old_vol);
			old_vol = 0;
		}
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a,"/music")) {
		opt_music ^= 1;
		if (!opt_music) {
			game_stop_mus(0);
		} else
			game_music_player();
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a,"/resume")) {
		cur_menu = menu_main;
		game_menu_box(0, NULL);
	} else if (!strcmp(a, "/settings")) {
		game_menu(menu_settings);
	} else if (!strcmp(a, "/settings-gfx")) {
		menu_settings_num = 0;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/settings-snd")) {
		menu_settings_num = 1;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/settings-oth")) {
		menu_settings_num = 2;
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/vol--")) {
		game_change_vol(-10, 0);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/vol++")) {
		game_change_vol(+10, 0);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/vol-")) {
		game_change_vol(-1, 0);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/vol+")) {
		game_change_vol(+1, 0);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/hz-")) {
		int hz = snd_hz();
		if (hz == 44100)
			hz = 22050;
		else if (hz == 22050)
			hz = 11025;
		else if (hz == 11025)
			hz = 0;
		else
			hz = 22050;
		game_change_hz(hz);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/hz+")) {
		int hz = snd_hz();
		if (hz == 11025)
			hz = 22050;
		else if (hz == 22050)
			hz = 44100;
		else if (hz == 44100)
			hz = 0;
		else
			hz = 22050;
		game_change_hz(hz);
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/lang++")) {
		do {
			cur_lang ++;
			if (cur_lang >= langs_nr)
				cur_lang = 0;
		} while (menu_lang_select(langs[cur_lang].file));
		if (curgame_dir)
			instead_set_lang(opt_lang);
		themes_rename();
		games_rename();
		game_reset_name();
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a, "/lang--")) {
		do {
			cur_lang --;
			if (cur_lang < 0)
			cur_lang = langs_nr - 1;
		} while (menu_lang_select(langs[cur_lang].file));
		if (curgame_dir)
			instead_set_lang(opt_lang);
		themes_rename();
		games_rename();
		game_reset_name();
		game_menu_box(1, game_menu_gen());
	} else if (!strcmp(a,"/quit")) {
		return -1;
#ifdef _USE_BROWSE
	} else if (!strcmp(a,"/browse")) {
		game_from_disk();
		return 0;
#endif
	} else if (cur_menu == menu_games) {
		char *p;
		p = strdup(a);
		if (p) {
			game_done(0);
			if (game_init(p)) {
				game_error();
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
			game_done(0);
			if (game_init(og)) {
				game_error();
			} else if (curgame_dir && game_own_theme && opt_owntheme && !curtheme_dir[THEME_GAME]) {
				game_menu(menu_own_theme);
			}
			free(p);
		}
	}
	return 0;
}

void custom_theme_warn(void)
{
	if (game_own_theme && !opt_owntheme && cur_menu != menu_warning) {
		game_menu(menu_custom_theme);
	}
}



struct	lang *langs = NULL;
int	langs_nr = 0;


static void lang_free(void)
{
	FREE(UNKNOWN_ERROR);
	FREE(ERROR_MENU);
	FREE(WARNING_MENU);
	FREE(SAVE_SLOT_EMPTY);
	FREE(SELECT_LOAD_MENU);
	FREE(AUTOSAVE_SLOT);
	FREE(BROKEN_SLOT);
	FREE(SELECT_SAVE_MENU);
	FREE(MAIN_MENU);
	FREE(ABOUT_MENU);
	FREE(BACK_MENU);
	FREE(SETTINGS_SND_MENU);
	FREE(SETTINGS_GFX_MENU);
	FREE(SETTINGS_OTH_MENU);
	FREE(CUSTOM_THEME_MENU);
	FREE(OWN_THEME_MENU);
	FREE(WAIT_MENU);
	FREE(SELECT_GAME_MENU);
	FREE(SELECT_THEME_MENU);
	FREE(SAVED_MENU);
	FREE(NOGAMES_MENU);
	FREE(NOTHEMES_MENU);
	FREE(QUIT_MENU);
	FREE(REMOVE_MENU);
	FREE(ON);
	FREE(OFF);
	FREE(KBD_MODE_LINKS);
	FREE(KBD_MODE_SMART);
	FREE(KBD_MODE_SCROLL);
	FREE(CANCEL_MENU);
	FREE(FROM_THEME);
	FREE(DISABLED_SAVE_MENU);
	FREE(BROWSE_MENU);
}

static int lang_ok(void)
{
	if (UNKNOWN_ERROR && ERROR_MENU && WARNING_MENU && SAVE_SLOT_EMPTY &&
		SELECT_LOAD_MENU && AUTOSAVE_SLOT && BROKEN_SLOT && SELECT_SAVE_MENU &&
		MAIN_MENU && ABOUT_MENU && BACK_MENU && SETTINGS_SND_MENU && SETTINGS_GFX_MENU && SETTINGS_OTH_MENU &&
		CUSTOM_THEME_MENU && OWN_THEME_MENU && SELECT_GAME_MENU && SELECT_THEME_MENU && WAIT_MENU &&
		SAVED_MENU && NOGAMES_MENU && NOTHEMES_MENU && QUIT_MENU && REMOVE_MENU &&
		ON && OFF && KBD_MODE_LINKS && KBD_MODE_SMART && KBD_MODE_SCROLL && CANCEL_MENU &&
		FROM_THEME && DISABLED_SAVE_MENU && BROWSE_MENU)
		return 0;
	return -1;
}	

struct parser lang_parser[] = {
	{ "UNKNOWN_ERROR", parse_esc_string, &UNKNOWN_ERROR, 0 },
	{ "ERROR_MENU", parse_esc_string, &ERROR_MENU, 0 },
	{ "WARNING_MENU", parse_esc_string, &WARNING_MENU, 0 },
	{ "SAVE_SLOT_EMPTY", parse_esc_string, &SAVE_SLOT_EMPTY, 0 },
	{ "SELECT_LOAD_MENU", parse_esc_string, &SELECT_LOAD_MENU, 0 },
	{ "AUTOSAVE_SLOT", parse_esc_string, &AUTOSAVE_SLOT, 0 },
	{ "BROKEN_SLOT", parse_esc_string, &BROKEN_SLOT, 0 },
	{ "SELECT_SAVE_MENU", parse_esc_string, &SELECT_SAVE_MENU, 0 },
	{ "MAIN_MENU", parse_esc_string, &MAIN_MENU, 0 },
	{ "ABOUT_MENU", parse_esc_string, &ABOUT_MENU, 0 },
	{ "BACK_MENU", parse_esc_string, &BACK_MENU, 0 },
	{ "SETTINGS_GFX_MENU", parse_esc_string, &SETTINGS_GFX_MENU, 0 },
	{ "SETTINGS_SND_MENU", parse_esc_string, &SETTINGS_SND_MENU, 0 },
	{ "SETTINGS_OTH_MENU", parse_esc_string, &SETTINGS_OTH_MENU, 0 },
	{ "CUSTOM_THEME_MENU", parse_esc_string, &CUSTOM_THEME_MENU, 0 },
	{ "OWN_THEME_MENU", parse_esc_string, &OWN_THEME_MENU, 0 },
	{ "WAIT_MENU", parse_esc_string, &WAIT_MENU, 0 },
	{ "SELECT_GAME_MENU", parse_esc_string, &SELECT_GAME_MENU, 0 },
	{ "SELECT_THEME_MENU", parse_esc_string, &SELECT_THEME_MENU, 0 },
	{ "SAVED_MENU", parse_esc_string, &SAVED_MENU, 0 },
	{ "NOGAMES_MENU", parse_esc_string, &NOGAMES_MENU, 0 },
	{ "NOTHEMES_MENU", parse_esc_string, &NOTHEMES_MENU, 0 },
	{ "QUIT_MENU", parse_esc_string, &QUIT_MENU, 0 },
	{ "REMOVE_MENU", parse_esc_string, &REMOVE_MENU, 0 },
	{ "ON", parse_esc_string, &ON, 0 },
	{ "OFF", parse_esc_string, &OFF, 0 },
	{ "KBD_MODE_LINKS", parse_esc_string, &KBD_MODE_LINKS, 0 },
	{ "KBD_MODE_SMART", parse_esc_string, &KBD_MODE_SMART, 0 },
	{ "KBD_MODE_SCROLL", parse_esc_string, &KBD_MODE_SCROLL, 0 },
	{ "CANCEL_MENU", parse_esc_string, &CANCEL_MENU, 0 },
	{ "FROM_THEME", parse_esc_string, &FROM_THEME, 0 },
	{ "DISABLED_SAVE_MENU", parse_esc_string, &DISABLED_SAVE_MENU, 0 },
	{ "BROWSE_MENU", parse_esc_string, &BROWSE_MENU, 0 },
	{ NULL,  NULL, NULL, 0 },
};

static int lang_parse(const char *path)
{
	return parse_ini(path, lang_parser);
}

static int is_lang(const char *path, const char *n)
{
	char *p = getfilepath(path, n);
	if (!p)
		return 0;
	if (access(p, F_OK))
		return 0;
	free(p);
	if (!(p = strstr(n, ".ini")) && !(p = strstr(n, ".INI")))
		return 0;
	return 1;
}

static char *lang_code(const char *str)
{
	char *p = strdup(str);
	if (!p)
		return NULL;
	p[strcspn(p, ".")] = 0;
	tolow(p);
	return p;
}


static char *lang_name(const char *path, const char *file)
{
	char *l;
	l = lookup_tag(path, "Name", ";");
	if (l)
		return l;
	return lang_code(file);
}


static int cmp_lang(const void *p1, const void *p2)
{
	const struct lang *l1 = (const struct lang*)p1;
	const struct lang *l2 = (const struct lang*)p2;
	return strcmp(l1->name, l2->name);
}

static void langs_sort()
{
	qsort(langs, langs_nr, sizeof(struct lang), cmp_lang);
}

int menu_langs_lookup(const char *path)
{
	char *p;
	int n = 0, i = 0;
	DIR *d;
	struct dirent *de;
	struct lang *new_langs;

	if (!path)
		return 0;

	d = opendir(path);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (!is_lang(path, de->d_name))
			continue;
		n ++;
	}
	if (!n)
		goto out;
	closedir(d); d = opendir(path);
	if (!d)
		return -1;
	new_langs = realloc(langs, sizeof(struct lang) * (n + langs_nr));
	if (!new_langs) {
		closedir(d);
		return -1;
	}
	langs = new_langs;

	while ((de = readdir(d)) && i < n) {
		if (!is_lang(path, de->d_name))
			continue;
		p = getfilepath(path, de->d_name);
		langs[langs_nr].path = p;
		langs[langs_nr].file = lang_code(de->d_name);
		langs[langs_nr].name = lang_name(p, de->d_name);
		langs_nr ++;
		i ++;
	}
out:
	langs_sort();
	closedir(d);
	return 0;
}

int menu_lang_select(const char *name)
{
	int i;
	char cwd[PATH_MAX];
	if (!name)
		return -1;
	getdir(cwd, sizeof(cwd));
	setdir(game_cwd);
	for (i = 0; i<langs_nr; i ++) {
		if (!strcmp(langs[i].file, name)) {
			lang_free();
			if (lang_parse(langs[i].path) || lang_ok()) {
				fprintf(stderr,"Error while loading language: %s\n", langs[i].file);
				setdir(cwd);
				return -1;
			}
			cur_lang = i;
			FREE(opt_lang); opt_lang = strdup(langs[i].file);
			setdir(cwd);
			return 0;
		}
	}
	setdir(cwd);
	return -1;
}
