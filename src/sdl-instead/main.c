/*
 * Copyright 2009-2014 Peter Kosyh <p.kosyh at gmail.com>
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

#if defined(__APPLE__) || defined(S60) || defined(ANDROID)
#include <SDL.h>
#endif

#include "externals.h"
#include "internals.h"
#include "idf.h"

#ifdef _USE_GTK
#include <gtk/gtk.h>
#endif

#ifdef ANDROID
#include "android.h"
#endif

#ifdef _WIN32_WCE
extern void	libwince_init(const char* prog, int debug);
#endif

extern int debug_init(void);
extern void debug_done(void);

int debug_sw = 0;
int noauto_sw = 0;
int nostdgames_sw = 0;
int nostdthemes_sw = 0;
int version_sw = 0;
int owntheme_sw = 0;
int nopause_sw = 0;
int chunksize_sw = 0;
int software_sw = 0;
int hinting_sw = 1;
int vsync_sw = 0;

static int opt_index = 0;

char *game_sw = NULL;
char *games_sw = NULL;
char *theme_sw = NULL;
char *themes_sw = NULL;
char *encode_sw = NULL;
char *encode_output = NULL;
char *mode_sw = NULL;
char *appdata_sw = NULL;
char *idf_sw = NULL;
char *start_idf_sw = NULL;
char *lua_sw = NULL;
static int lua_exec = 1;

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
	game_sw = zip_game_dirname;
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
	start_idf_sw = file;
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
	char *p, *ep, *d;
	static char gp[PATH_MAX + 1];
	static char *cd = "./";
	if (!path)
		return -1;
	if (!path[0])
		return -1;
	if (strlen(path) >= PATH_MAX)
		return -1;
	strcpy(gp, path);
	p = gp;
	unix_path(p);
	ep = p + strlen(p) - 1;
	while (*ep == '/' && ep != p)
		*ep-- = 0;
	if (!p[0])
		return -1;
	ep = p + strlen(p) - 1;
	while (ep != p) {
		if (*ep == '/') {
			*ep ++ = 0;
			break;
		}
		ep --;
	}
	if (ep == p)
		d = cd;
	else
		d = p;

	if (!is_game(d, ep)) {
		fprintf(stderr, "%s/%s is not a game path.\n", d, ep);
		return -1;
	}
	game_sw = ep;
	games_sw = d;
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
	"    -window\n        Run the game in windowed mode.\n"
	"    -noautosave\n        Disable autosave/autoload.\n"
	"    -mode [WxH]\n        Use WxH resolution.\n"
	"    -software\n        Force software rendering.\n"
	"    -nopause\n        Do not pause the game on window minimize.\n"
	"    -hinting 0|1|2|3\n        Set the font hinting mode (helpful with infinality)\n"
	"    -install [game in zip]\n        Install game from zip archive\n"
	"    -appdata [fullpath]\n        Store saves and settings in appdata path. Path must exist!\n"
	"    -chunksize [size in bytes]\n        Size for audio buffer. Try this if sound lags.\n"
	"    -vsync\n        Enable vsync display output (SDL2 only)\n");
}
int main(int argc, char *argv[])
{
#ifdef _USE_UNPACK
	int clean_tmp = 0;
#endif
	int err = 0;
	int i;
#ifdef __APPLE__
	macosx_init();
#endif
#ifdef _USE_GTK
	gtk_init(&argc, &argv);
#endif
#ifndef S60
	putenv("SDL_MOUSE_RELATIVE=0"); /* test this! */
#endif

#ifdef _WIN32_WCE
	libwince_init(argv[0], 1);
	wince_init(argv[0]);
#else
#ifdef S60
	extern char s60_data[];
	strcpy(game_cwd, s60_data);
#else
#ifdef _WIN32
	strcpy(game_cwd, dirname(argv[0]));
#else
	if (!getcwd(game_cwd, sizeof(game_cwd)))
		fprintf(stderr,"Warning: can not get current dir\n.");
#endif
#endif
#endif
	unix_path(game_cwd);
	setdir(game_cwd);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-vsync"))
			vsync_sw = 1;
		else if (!strcmp(argv[i], "-nosound"))
			nosound_sw = 1;
		else if (!strcmp(argv[i], "-fullscreen"))
			fullscreen_sw = 1;
		else if (!strcmp(argv[i], "-mode")) {	
			if ((i + 1) < argc)
				mode_sw = argv[++i];
			else
				mode_sw = "-1x-1";
		} else if (!strcmp(argv[i], "-window"))
			window_sw = 1;
		else if (!strcmp(argv[i], "-debug")) {
			if (!debug_sw)
				debug_init();
			debug_sw = 1;
		} else if (!strcmp(argv[i], "-owntheme"))
			owntheme_sw = 1;
		else if (!strcmp(argv[i], "-noautosave"))
			noauto_sw = 1;
		else if (!strcmp(argv[i], "-game")) {
			if ((i + 1) < argc)
				game_sw = argv[++i];
			else
				game_sw = "";
		} else if (!strcmp(argv[i], "-theme")) {
			if ((i + 1) < argc)
				theme_sw = argv[++i];
			else
				theme_sw = "";
		} else if (!strcmp(argv[i], "-nostdgames")) {
			nostdgames_sw = 1;
#ifdef _LOCAL_APPDATA
		} else if (!strcmp(argv[i], "-appdata")) {
			if ((i + 1) < argc)
				appdata_sw = argv[++i];
			else
				appdata_sw = "";
#endif
		} else if (!strcmp(argv[i], "-chunksize")) {
			if ((i + 1) < argc)
				chunksize_sw = atoi(argv[++i]);
			else
				chunksize_sw = DEFAULT_CHUNKSIZE;
		} else if (!strcmp(argv[i], "-gamespath")) {
			if ((i + 1) < argc)
				games_sw = argv[++i];
			else
				games_sw = "";
		} else if (!strcmp(argv[i], "-themespath")) {
			if ((i + 1) < argc)
				themes_sw = argv[++i];
			else
				themes_sw = "";
		} else if (!strcmp(argv[i], "-idf")) {	
			if ((i + 1) < argc)
				idf_sw = argv[++i];
			else {
				fprintf(stderr,"No data directory specified.\n");
				err = 1;
				goto out;
			}
		} else if (!strcmp(argv[i], "-encode")) {	
			if ((i + 1) < argc)
				encode_sw = argv[++i];
			else {
				fprintf(stderr,"No lua file specified.\n");
				err = 1;
				goto out;
			}
			if ((i + 1) < argc)
				encode_output = argv[++i];
			else
				encode_output = "lua.enc";
		} else if (!strcmp(argv[i], "-version")) {
			version_sw = 1;
		} else if (!strcmp(argv[i], "-nopause")) {
			nopause_sw = 1;
		} else if (!strcmp(argv[i], "-software")) {
			software_sw = 1;
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
				lua_sw = argv[++ i];
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
	}
	cfg_load();

	if (opt_debug == 1 && debug_sw == 0) {
		debug_sw = 1;
		debug_init();
	}

	if (opt_vsync == 1 && vsync_sw == 0)
		vsync_sw = 1;

	if (version_sw) {
#ifdef IOS
        printf("INSTEAD v%s\n", VERSION);
        
        printf("%s\n", LUA_RELEASE);
        
        SDL_version ver;
        SDL_VERSION(&ver);
        printf("SDL v%u.%u.%u\n", ver.major, ver.minor, ver.patch);
        
        #include <SDL_image.h>
        SDL_IMAGE_VERSION(&ver)
        printf("SDL_image v%u.%u.%u\n", ver.major, ver.minor, ver.patch);
        
        #include <SDL_mixer.h>
        SDL_MIXER_VERSION(&ver)
        printf("SDL_mixer v%u.%u.%u\n", ver.major, ver.minor, ver.patch);
        
        #include <SDL_ttf.h>
        SDL_TTF_VERSION(&ver)
        printf("SDL_ttf v%u.%u.%u\n", ver.major, ver.minor, ver.patch);
#else
        fprintf(stdout, VERSION"\n");
        goto out;
#endif
	}

	if (lua_sw) {
		err = instead_init_lua(dirname(lua_sw));
		if (err)
			goto out;
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
	if (!opt_lang || !opt_lang[0])
		opt_lang = game_locale();
	
	if (menu_lang_select(opt_lang) && menu_lang_select(LANG_DEF)) {
		fprintf(stderr, "Can not load default language.\n");
		err = 1;
		goto out;
	}
	
	if (games_sw)
		games_lookup(games_sw);

	if (owntheme_sw && !opt_owntheme) {
		opt_owntheme = 2;
	}

	if (!nostdgames_sw && games_lookup(dirpath(GAMES_PATH)))
		fprintf(stderr, "No games found in: %s.\n", GAMES_PATH);

	if (themes_sw)
		themes_lookup(themes_sw);

	if (!nostdthemes_sw) {
		themes_lookup(dirpath(THEMES_PATH));
		themes_lookup(game_local_themes_path());
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
				game_sw = idf_game;
				strncpy(idf_game, bb, sizeof(idf_game) - 1);
				idf_game[sizeof(idf_game) - 1] = 0;
			}
		}
		if (d)
			free(d); 
		if (b)
			free(b);
	}

	if (noauto_sw && opt_autosave)
		opt_autosave = 2;
	if (window_sw)
		opt_fs = 0;
	if (fullscreen_sw)
		opt_fs = 1;
	
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
	if (!curtheme_dir)
		game_theme_select(DEFAULT_THEME);
	
#ifdef IOS
    correct_font_size();
#endif
    
	/* Initialize SDL */
	if (gfx_init() < 0)
		return -1;

	if (gfx_video_init() || input_init())
		return -1;

	if (game_init(opt_game)) {
		game_error();
	}

	game_loop();
	cfg_save();
	game_done(0);

	gfx_video_done();

#ifndef ANDROID
	gfx_done();
#endif
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
#if defined(ANDROID) || defined(IOS)
	exit(err);
#endif
	return err;
}

