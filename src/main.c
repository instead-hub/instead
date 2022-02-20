/*
 * Copyright 2009-2022 Peter Kosyh <p.kosyh at gmail.com>
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

#if defined(__APPLE__) || defined(S60) || defined(ANDROID) || defined(WINRT)
#include <SDL.h>
#endif

#include "externals.h"
#include "internals.h"

#ifdef _USE_GTK
#include <gtk/gtk.h>
#endif

#ifdef ANDROID
#include "android.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32_WCE
extern void	libwince_init(const char* prog, int debug);
#endif

extern int debug_init(void);
extern void debug_done(void);
extern luaL_Reg paths_funcs[];
extern luaL_Reg bits_funcs[];

int debug_sw = 0;
int noauto_sw = 0;
int nostdgames_sw = 0;
int nostdthemes_sw = 0;
int version_sw = 0;
int owntheme_sw = 0;
int noowntheme_sw = 0;
int nopause_sw = 0;
int chunksize_sw = 0;
int software_sw = 0;
int hinting_sw = 1;
int vsync_sw = 0;
int resizable_sw = 0;
int scale_sw = 1;
int standalone_sw = 0;
int nocursor_sw = 0;
int glhack_sw = 0;
int dpi_sw = 0;

static int opt_index = 1;

static int nohires_sw = 0;
static char *fsize_sw = NULL;

char *game_sw = NULL;
char *games_sw = NULL;
char *theme_sw = NULL;
char *themes_sw = NULL;
char *encode_sw = NULL;
char *encode_output = NULL;
char *mode_sw = NULL;
char *modes_sw = NULL;
char *appdata_sw = NULL;
char *idf_sw = NULL;
char *start_idf_sw = NULL;
char *lua_sw = NULL;
char *render_sw = NULL;
char *lang_sw = NULL;
static int lua_exec = 1;
static int nocfg_sw = 0;

char *instead_exec = NULL;

#ifdef _USE_UNPACK
extern int unpack(const char *zipfilename, const char *where);
extern char zip_game_dirname[];

int setup_zip(const char *file, char *p)
{
	if (!p)
		return -1;
	fprintf(stderr,"Trying to install: %s\n", file);
	if (unpack(file, p)) {
		if (zip_game_dirname[0]) {
			p = getpath(p, zip_game_dirname);
			fprintf(stderr, "Cleaning: '%s'...\n", p);
			remove_dir(p);
			free(p);
		}
		return -1;
	}
	p = strdup(p);
	FREE(game_sw);
	FREE(games_sw);
	game_sw = strdup(zip_game_dirname);
	games_sw = p;
	return 0;
}
#endif

static int start_idf(char *file)
{
	if (!file)
		return -1;
	if (!idf_magic(file))
		return -1;
	FREE(start_idf_sw);
	start_idf_sw = strdup(file);
	return 0;
}

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
void macosx_init(void) {
	char resourcePath[PATH_MAX];
	CFBundleRef mainBundle;
	CFURLRef resourcesDirectoryURL;
	mainBundle = CFBundleGetMainBundle();
	if (!mainBundle)
		return;
	resourcesDirectoryURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	if (!resourcesDirectoryURL)
		return;
	CFURLGetFileSystemRepresentation(resourcesDirectoryURL, true, (UInt8 *) resourcePath, PATH_MAX);
	CFRelease(resourcesDirectoryURL);
	chdir(resourcePath);
#ifdef IOS
	setup_inbox();
#endif
	return;
}
#endif

#ifdef _WIN32_WCE
char *getcurdir(char *path)
{
	char *p;
	if (path == NULL || *path == '\0')
		return ".";
	p = path + strlen(path) - 1;
	while (*p == '/') {
		if (p == path)
			return path;
		*p-- = '\0';
	}
	while (p >= path && *p != '/')
		p--;
	return p < path ? "." : p == path ? "/" : (*p = '\0', path);
}
void wince_init(char *path)
{
	unix_path(path);
	strcpy(game_cwd, getcurdir(path));
}
#endif
static int run_game(const char *path)
{
	char *b, *d;
	struct stat path_stat;
	static char dir[PATH_MAX + 1];
	static char base[PATH_MAX + 1];
	if (!path)
		return -1;
	if (!path[0])
		return -1;
	if (strlen(path) >= PATH_MAX)
		return -1;
	strcpy(dir, getrealpath(path, base)); /* always get full path */
	unix_path(dir);
	stat(dir, &path_stat);
	if (S_ISREG(path_stat.st_mode) &&
		(!strlowcmp(basename(dir), INSTEAD_MAIN) ||
		!strlowcmp(basename(dir), INSTEAD_MAIN3))) {
		d = dirname(dir);
		strcpy(base, d);
		d = dirname(d);
	} else {
		d = dirname(dir);
		strcpy(base, path);
		unix_path(base);
	}
	b = basename(base);
	if (!is_game(d, b)) {
		fprintf(stderr, "%s/%s is not a game path.\n", d, b);
		return -1;
	}
	FREE(game_sw);
	FREE(games_sw);
	game_sw = strdup(b);
	games_sw = strdup(d);
	return 0;
}
static void usage(void)
{
	fprintf(stderr,
	"INSTEAD "VERSION" - Simple Text Adventure Engine, The Interpreter\n"
	"Usage:\n"
	"    sdl-instead [options] [game.zip or game.idf or path to game]\n"
	"Some options:\n"
	"    -debug Debug mode\n        (for game developers).\n"
	"    -nosound\n        Run the game without sound.\n"
	"    -gamespath\n        <path> Add path with games.\n"
	"    -game <basename>\n        Select game in gamespath.\n"
	"    -owntheme\n        Force game to use own theme.\n"
	"    -fullscreen\n        Run the game in fullscreen mode.\n"
	"    -hires\n        Set the high resolution if run in fullscreen mode.\n"
	"    -window\n        Run the game in windowed mode.\n"
	"    -noautosave\n        Disable autosave load.\n"
	"    -mode [WxH]\n        Use WxH resolution.\n"
	"    -software\n        Force software rendering.\n"
	"    -nopause\n        Do not pause the game on window minimize.\n"
	"    -hinting 0|1|2|3\n        Set the font hinting mode (helpful with infinality)\n"
	"    -install [game in zip]\n        Install game from zip archive\n"
	"    -appdata [fullpath]\n        Store saves and settings in appdata path. Path must exist!\n"
	"    -chunksize [size in bytes]\n        Size for audio buffer. Try this if sound lags.\n"
	"    -vsync\n        Enable vsync display output (SDL2 only)\n");
}
static int profile_load(const char *path);
extern int game_instead_extensions(void);

static int luaB_clipboard(lua_State *L) {
	char *buf = NULL;
	const char *text = luaL_optstring(L, 1, NULL);
	if (!text) { /* get */
		if (system_clipboard(NULL, &buf) == 0) {
			lua_pushstring(L, buf);
			free(buf);
		} else
			lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, system_clipboard(text, NULL) == 0);
	return 1;
}

static int luaB_wait_use(lua_State *L) {
	int v = -1;
	int old = game_wait_use;

	if (lua_isboolean(L, 1))
		v = lua_toboolean(L, 1);

	if (v == -1) {
		lua_pushboolean(L, old);
		return 1;
	}

	if (!opt_owntheme) {
		lua_pushboolean(L, 0);
		return 1;
	}
	game_wait_use = v;
	lua_pushboolean(L, old);
	return 1;
}

static int luaB_grab_events(lua_State *L) {
	int grab = lua_toboolean(L, 1);
	int ov = game_grab_events;
	if (lua_isboolean(L, 1))
		game_grab_events = grab;
	lua_pushboolean(L, ov);
	return 1;
}

static int luaB_text_input(lua_State *L) {
	int rc;
	int inp = lua_toboolean(L, 1);

	if (!lua_isboolean(L, 1)) {
		rc = input_text(-1);
		lua_pushboolean(L, rc > 0);
		return 1;
	}
	rc = input_text(inp);
	lua_pushboolean(L, rc >= 0);
	return 1;
}

static const luaL_Reg sdl_funcs[] = {
	{ "instead_clipboard", luaB_clipboard },
	{ "instead_wait_use", luaB_wait_use },
	{ "instead_grab_events", luaB_grab_events},
	{ "instead_text_input", luaB_text_input},
	{NULL, NULL}
};

static int input_text_state = 0;

static int sdl_ext_init(void)
{
	char path[PATH_MAX];
	instead_api_register(sdl_funcs);
	input_text_state = input_text(-1);
	game_wait_use = 1;
	game_grab_events = 0;
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/gui.lua");
	return instead_loadfile(dirpath(path));
}

static int sdl_ext_done(void)
{
	input_text(input_text_state);
	return 0;
}

static struct instead_ext sdl_ext = {
	.init = sdl_ext_init,
	.done = sdl_ext_done,
};

static int sdl_extensions(void)
{
	if (game_instead_extensions())
		return -1;
	return instead_extension(&sdl_ext);
}

int instead_main(int argc, char *argv[])
{
#ifdef _USE_UNPACK
	int clean_tmp = 0;
#endif
	int err = 0;
	int i;
#ifdef _WIN32
	char *exe_path;
	HINSTANCE lib = LoadLibrary("user32.dll");
	int (*SetProcessDPIAware)() = (void*) GetProcAddress(lib, "SetProcessDPIAware");
	if (SetProcessDPIAware)
		SetProcessDPIAware();
#endif
#ifdef __APPLE__
	macosx_init();
#endif
#ifndef S60
	putenv("SDL_MOUSE_RELATIVE=0"); /* test this! */
#if GTK_MAJOR_VERSION == 4 /* fix crash when SDL2 uses gl */
	putenv("GDK_DEBUG=gl-disable");
#endif
#endif

#ifdef _WIN32_WCE
	libwince_init(argv[0], 1);
	wince_init(argv[0]);
#elif defined(APPIMAGE)
	unix_path(argv[0]);
	strcpy(game_cwd, dirname(argv[0]));
#elif defined(WINRT)
	unix_path(argv[0]);
	strcpy(game_cwd, argv[0]);
#elif defined(S60)
	extern char s60_data[];
	strcpy(game_cwd, s60_data);
#elif defined(_WIN32)
	exe_path = malloc(PATH_MAX + 1);
	if (exe_path) {
		i = GetModuleFileName(NULL, exe_path, PATH_MAX);
		exe_path[i] = 0;
		strcpy(game_cwd, dirname(exe_path));
		free(exe_path);
	} else /* Sorry, kernel mode programming practice. Useless here. */
		strcpy(game_cwd, dirname(argv[0]));
#else
	if (!getcwd(game_cwd, sizeof(game_cwd)))
		fprintf(stderr,"Warning: can not get current dir\n.");
#endif
	if (sdl_extensions() < 0) {
		fprintf(stderr, "Fatal: can not init SDL extensions\n");
		return 1;
	}

	if (argc > 0)
		instead_exec = strdup(argv[0]);

	unix_path(game_cwd);
	setdir(game_cwd);
	profile_load(NULL);

	for (i = 1; i < argc; i++) {
		if (lua_sw) /* during load profile */
			break;
		if (!strcmp(argv[i], "-vsync"))
			vsync_sw = 1;
		else if (!strcmp(argv[i], "-nosound"))
			nosound_sw = 1;
		else if (!strcmp(argv[i], "-dpi"))
			if ((i + 1) < argc)
				dpi_sw = atoi(argv[++i]);
			else
				dpi_sw = 96;
		else if (!strcmp(argv[i], "-fullscreen"))
			fullscreen_sw = 1;
		else if (!strcmp(argv[i], "-hires"))
			hires_sw = 1;
		else if (!strcmp(argv[i], "-nohires"))
			nohires_sw = 1;
		else if (!strcmp(argv[i], "-mode")) {
			FREE(mode_sw);
			if ((i + 1) < argc)
				mode_sw = strdup(argv[++i]);
			else
				mode_sw = strdup("-1x-1");
		} else if (!strcmp(argv[i], "-modes")) {
			FREE(modes_sw);
			if ((i + 1) < argc)
				modes_sw = strdup(argv[++i]);
		} else if (!strcmp(argv[i], "-fontscale")) {
			FREE(fsize_sw);
			if (i + 1 < argc)
				fsize_sw = strdup(argv[++i]);
			else
				fsize_sw = NULL;
		} else if (!strcmp(argv[i], "-window"))
			window_sw = 1;
		else if (!strcmp(argv[i], "-debug")) {
			if (!debug_sw)
				debug_init();
			debug_sw = 1;
		} else if (!strcmp(argv[i], "-owntheme"))
			owntheme_sw = 1;
		else if (!strcmp(argv[i], "-notheme"))
			noowntheme_sw = 1;
		else if (!strcmp(argv[i], "-noautosave"))
			noauto_sw = 1;
		else if (!strcmp(argv[i], "-game")) {
			FREE(game_sw);
			if ((i + 1) < argc)
				game_sw = strdup(argv[++i]);
			else
				game_sw = strdup("");
		} else if (!strcmp(argv[i], "-theme")) {
			FREE(theme_sw);
			if ((i + 1) < argc)
				theme_sw = strdup(argv[++i]);
			else
				theme_sw = strdup("");
		} else if (!strcmp(argv[i], "-nostdgames")) {
			nostdgames_sw = 1;
		} else if (!strcmp(argv[i], "-appdata")) {
			FREE(appdata_sw);
			if ((i + 1) < argc)
				appdata_sw = strdup(argv[++i]);
			else
				appdata_sw = strdup("");
		} else if (!strcmp(argv[i], "-chunksize")) {
			if ((i + 1) < argc)
				chunksize_sw = atoi(argv[++i]);
			else
				chunksize_sw = DEFAULT_CHUNKSIZE;
		} else if (!strcmp(argv[i], "-gamespath")) {
			FREE(games_sw);
			if ((i + 1) < argc)
				games_sw = strdup(argv[++i]);
			else
				games_sw = strdup("");
		} else if (!strcmp(argv[i], "-themespath")) {
			FREE(themes_sw);
			if ((i + 1) < argc)
				themes_sw = strdup(argv[++i]);
			else
				themes_sw = strdup("");
		} else if (!strcmp(argv[i], "-idf")) {
			FREE(idf_sw);
			if ((i + 1) < argc)
				idf_sw = strdup(argv[++i]);
			else {
				fprintf(stderr,"No data directory specified.\n");
				err = 1;
				goto out;
			}
		} else if (!strcmp(argv[i], "-encode")) {
			FREE(encode_sw);
			if ((i + 1) < argc)
				encode_sw = strdup(argv[++i]);
			else {
				fprintf(stderr,"No lua file specified.\n");
				err = 1;
				goto out;
			}
			FREE(encode_output);
			if ((i + 1) < argc)
				encode_output = strdup(argv[++i]);
			else
				encode_output = strdup("lua.enc");
		} else if (!strcmp(argv[i], "-version")) {
			version_sw = 1;
		} else if (!strcmp(argv[i], "-nopause")) {
			nopause_sw = 1;
		} else if (!strcmp(argv[i], "-nocursor")) {
			nocursor_sw = 1;
		} else if (!strcmp(argv[i], "-software")) {
			software_sw = 1;
		} else if (!strcmp(argv[i], "-glhack")) {
			if ((i + 1) < argc)
				glhack_sw = atoi(argv[++i]);
			else
				glhack_sw = 565; /* some samsung devices */
		} else if (!strcmp(argv[i], "-resizable")) {
			resizable_sw = 1;
		} else if (!strcmp(argv[i], "-scale")) {
			if ((i + 1) < argc)
				scale_sw = atoi(argv[++i]);
			else
				scale_sw = 2;
		} else if (!strcmp(argv[i], "-standalone")) {
			standalone_sw = 1;
			owntheme_sw = 1;
		} else if (!strcmp(argv[i], "-noconfig")) {
			nocfg_sw = 1;
		} else if (!strcmp(argv[i], "-profile")) {
			if ((i + 1) < argc) {
				profile_load(argv[++i]);
				i ++;
			}
#ifdef _USE_UNPACK
		} else if (!strcmp(argv[i], "-install")) {
			if ((i + 1) < argc) {
				char *file = argv[++i];
				char *p;
				if (games_sw)
					p = games_sw;
				else
					p = game_local_games_path(1);
				if (setup_zip(file, p)) {
					err = 1;
					goto out;
				}
			}
#endif
		} else if (!strcmp(argv[i], "-quit")) {
			exit(0);
		} else if (!strcmp(argv[i], "-hinting")) {
			if ((i + 1) < argc)
				hinting_sw = atoi(argv[++i]);
			else
				hinting_sw = 1;
		} else if (!strcmp(argv[i], "-lua") || !strcmp(argv[i], "-luac")) {
			if ((i + 1) < argc) {
				lua_exec = !strcmp(argv[i], "-lua");
				FREE(lua_sw);
				lua_sw = strdup(argv[++ i]);
				opt_index = i + 1;
				break;
			} else {
				fprintf(stderr, "No lua script.\n");
				err = 1;
				goto out;
			}
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help")
			|| !strcmp(argv[i], "--help")) {
			usage();
			goto out;
		} else if (!strcmp(argv[i], "-renderer")) {
			FREE(render_sw);
			if ((i + 1) < argc)
				render_sw = strdup(argv[++i]);
			else {
				fprintf(stderr, "Parameter required: %s\n", argv[i]);
				fprintf(stderr, "opengl, opengles2, opengles, software, direct3d\n");
				err = 1;
				goto out;
			}
		} else if (!strcmp(argv[i], "-lang")) {
			if ((i + 1) < argc)
				lang_sw = strdup(argv[++i]);
			else
				lang_sw = strdup("en");
		} else if (argv[i][0] == '-') {
			fprintf(stderr,"Unknown option: %s\n", argv[i]);
			usage();
			err = 1;
			goto out;
		} else if (!start_idf(argv[i])) {
			fprintf(stderr, "Adding idf: %s\n", argv[i]);
		} else if (!run_game(argv[i])) {
			fprintf(stderr, "Opening game: %s\n", argv[i]);
		}
#ifdef _USE_UNPACK
		else {
			char *p;
			if (games_sw)
				p = games_sw;
			else
				p = game_tmp_path();
			if (setup_zip(argv[i], p)) {
				err = 1;
				goto out;
			}
			clean_tmp = 1;
		}
#endif
		opt_index = i;
	}

	if (scale_sw <= 0)
		scale_sw = 1;
	else if (scale_sw > 8)
		scale_sw = 8;

	if (nocfg_sw || cfg_load()) { /* no config */
		cfg_init();
	}

	if (fsize_sw) {
		int i = atoi(fsize_sw);
		if (i > 0)
			opt_fsize = (i - 100) / 10;
	}

	if (opt_fsize < FONT_MIN_SZ)
		opt_fsize = FONT_MIN_SZ;
	else if (opt_fsize > FONT_MAX_SZ)
		opt_fsize = FONT_MAX_SZ;

	if (opt_debug == 1 && debug_sw == 0) {
		debug_sw = 1;
		debug_init();
	}

	if (opt_vsync == 1 && vsync_sw == 0)
		vsync_sw = 1;

	if (opt_resizable == 1 && resizable_sw == 0)
		resizable_sw = 1;

	if (version_sw) {
		fprintf(stdout, VERSION"\n");
		goto out;
	}

	if (lua_sw) {
		char *script_dir = strdup(lua_sw);
		instead_set_standalone(1);
		instead_set_debug(debug_sw);
		err = instead_init_lua(dirname(script_dir), 0);
		free(script_dir);
		if (err)
			goto out;
		instead_api_register(paths_funcs);
		instead_api_register(bits_funcs);
		if (!err)
			err = instead_loadscript(lua_sw,
				argc - opt_index,
				argv + opt_index,
				lua_exec);
		instead_done();
		goto out;
	}

	if (encode_sw) {
		err = instead_encode(encode_sw, encode_output);
		goto out;
	}

	if (idf_sw) {
		char *p = malloc(strlen(idf_sw) + 5);
		if (p) {
			char *b;
			strcpy(p, idf_sw);
			b = basename(p);
			strcat(b, ".idf");
			idf_create(b, idf_sw);
			free(p);
		} else
			idf_create("data.idf", idf_sw);
		goto out;
	}
	menu_langs_lookup(dirpath(LANG_PATH));

	if (!langs_nr) {
		fprintf(stderr, "No languages found in: %s.\n", dirpath(LANG_PATH));
		err = 1;
		goto out;
	}
	if (lang_sw)
		opt_lang = strdup(lang_sw);
	else if (!opt_lang || !opt_lang[0])
		opt_lang = game_locale();

	if (menu_lang_select(opt_lang) && menu_lang_select(LANG_DEF)) {
		fprintf(stderr, "Can not load default language.\n");
		err = 1;
		goto out;
	}

	if (games_sw)
		games_lookup(games_sw);

	if (!nostdgames_sw && games_lookup(dirpath(GAMES_PATH)))
		fprintf(stderr, "No games found in: %s.\n", GAMES_PATH);

	if (themes_sw)
		themes_lookup(themes_sw, THEME_GLOBAL);

	if (!nostdthemes_sw) {
		themes_lookup(dirpath(THEMES_PATH), THEME_GLOBAL);
		themes_lookup(game_local_themes_path(), THEME_GLOBAL);
	}

	if (!nostdgames_sw)
		games_lookup(game_local_games_path(0));


	if (start_idf_sw) {
		char *d, *b;
		char *dd, *bb;
		static char idf_game[255];
		d = strdup(start_idf_sw);
		b = strdup(start_idf_sw);
		if (d && b) {
			dd = dirname(d);
			bb = basename(b);
			if (!games_replace(dirpath(dd), bb)) {
				strncpy(idf_game, bb, sizeof(idf_game) - 1);
				idf_game[sizeof(idf_game) - 1] = 0;
				FREE(game_sw);
				game_sw = strdup(idf_game);
			}
		}
		if (d)
			free(d);
		if (b)
			free(b);
	}
/* too dangerous to be in release
	if (games_nr == 1) {
		if (strncmp(GAMES_PATH, games[0].path, strlen(GAMES_PATH))) {
			standalone_sw = 1;
		}
	}
*/
	if (standalone_sw) {
		fprintf(stderr, "Standalone mode...\n");
		owntheme_sw = 1;
	}

	if (owntheme_sw && !opt_owntheme) {
		opt_owntheme = 2;
	}

	if (noowntheme_sw && opt_owntheme) {
		opt_owntheme = 0;
	} else
		noowntheme_sw = 0;

	if (noauto_sw && opt_autosave)
		opt_autosave = 2;
	if (window_sw)
		opt_fs = 0;
	if (fullscreen_sw)
		opt_fs = 1;

	if (nohires_sw)
		opt_hires = 0;

	if (hires_sw != -1)
		opt_hires = hires_sw;

	if (mode_sw)
		parse_mode(mode_sw, opt_mode);

	if (game_sw) {
		FREE(opt_game);
		opt_game = game_sw;
	}

	if (theme_sw) {
		FREE(opt_theme);
		opt_theme = theme_sw;
	}

	if (opt_theme)
		game_theme_select(opt_theme);
	if (!curtheme_dir[THEME_GLOBAL])
		game_theme_select(DEFAULT_THEME);

	/* Initialize SDL */
	if (gfx_init() < 0)
		return -1;
	/* Initialize Sound */
	snd_init(opt_hz);
	snd_volume_mus(opt_vol);
#ifdef _USE_GTK
#if GTK_MAJOR_VERSION == 4
	gtk_init();
#else
	gtk_init(&argc, &argv); /* must be called AFTER SDL_Init when using SDL2 */
#endif
#endif
	if (gfx_video_init() || input_init())
		return -1;

	if (game_init(opt_game?opt_game:"")) {
		game_error();
	}

	game_loop();
#ifdef __EMSCRIPTEN__
	return 0;
#endif
	cfg_save();
	game_done(0);
	snd_done();
	input_done();
	gfx_video_done();
	gfx_done();
out:
	if (debug_sw)
		debug_done();
#ifdef _USE_GTK
/*	gtk_main_quit (); */
#endif
#ifdef _USE_UNPACK
	if (clean_tmp)
		remove_dir(game_tmp_path());
#endif
	return err;
}

static struct parser profile_parser[] = {
	{ "standalone", parse_int, &standalone_sw, 0 },
	{ "vsync", parse_int, &vsync_sw, 0 },
	{ "debug", parse_int, &debug_sw, 0 },
	{ "lua", parse_string, &lua_sw, 0 },
	{ "nopause", parse_int, &nopause_sw, 0 },
	{ "noconfig", parse_int, &nocfg_sw, 0 },
	{ "noautosave", parse_int, &noauto_sw, 0 },
	{ "nostdgames", parse_int, &nostdgames_sw, 0 },
	{ "nostdthemes", parse_int, &nostdthemes_sw, 0 },
	{ "chunksize", parse_int, &chunksize_sw, 0 },
	{ "software", parse_int, &software_sw, 0 },
	{ "hinting", parse_int, &hinting_sw, 0 },
	{ "resizable", parse_int, &resizable_sw, 0 },
	{ "scale", parse_int, &scale_sw, 0 },
	{ "gamespath", parse_string, &games_sw, 0 },
	{ "themespath", parse_string, &themes_sw, 0 },
	{ "game", parse_string, &game_sw, 0 },
	{ "owntheme", parse_int, &owntheme_sw, 0 },
	{ "notheme", parse_int, &noowntheme_sw, 0 },
	{ "lang", parse_string, &lang_sw, 0 },
	{ "appdata", parse_string, &appdata_sw, 0 },
	{ "fullscreen", parse_int, &fullscreen_sw, 0 },
	{ "hires", parse_int, &hires_sw, 0 },
	{ "nohires", parse_int, &nohires_sw, 0 },
	{ "window", parse_int, &window_sw, 0 },
	{ "mode", parse_string, &mode_sw, 0 },
	{ "modes", parse_string, &modes_sw, 0 },
	{ "fontscale", parse_string, &fsize_sw, 0 },
	{ "renderer", parse_string, &render_sw, 0 },
	{ "nocursor", parse_int, &nocursor_sw, 0 },
	{ "dpi", parse_int, &dpi_sw, 0 },
	{ NULL, NULL, NULL, 0 },
};

static int profile_parse(const char *path)
{
	return parse_ini(path, profile_parser);
}

static int profile_load(const char *prof)
{
	char path[PATH_MAX];
	const char *p = (prof)?prof:appdir();
	if (!p)
		return -1;
	if (!prof)
		snprintf(path, sizeof(path), "%s/profile", p);
	else
		snprintf(path, sizeof(path), "%s", p);
	if (access(path, R_OK))
		return 0;
	if (prof)
		fprintf(stderr, "Using profile...'%s'\n", path);
	return profile_parse(path);
}
#ifndef NOMAIN
int main(int argc, char *argv[])
{
	int err;
	err = instead_main(argc, argv);
#if defined(ANDROID) || defined(IOS)
	exit(err);
#endif
	return err;
}
#endif
