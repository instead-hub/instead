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
#ifdef _USE_UNPACK
extern int unpack(const char *zipfilename, const char *where);
extern char zip_game_dirname[];

static int setup_zip(const char *file, char *p)
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
		if (!strcmp(argv[i],"-alsa")) 
			alsa_sw = 1;
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
		else if (!strcmp(argv[i], "-debug"))
			debug_sw = 1;
		else if (!strcmp(argv[i], "-owntheme"))
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
				exit(1);
			}
		} else if (!strcmp(argv[i], "-encode")) {	
			if ((i + 1) < argc)
				encode_sw = argv[++i];
			else {
				fprintf(stderr,"No lua file specified.\n");
				exit(1);	
			}
			if ((i + 1) < argc)
				encode_output = argv[++i];
			else
				encode_output = "lua.enc";
		} else if (!strcmp(argv[i], "-version")) {
			version_sw = 1;
		} else if (!strcmp(argv[i], "-nopause")) {
			nopause_sw = 1;
#ifdef _USE_UNPACK
		} else if (!strcmp(argv[i], "-install")) {
			if ((i + 1) < argc) {
				char *file = argv[++i];
				char *p;
				if (games_sw)
					p = games_sw;
				else
					p = game_local_games_path(1);
				if (setup_zip(file, p))
					exit(1);
			}
#endif
		} else if (!strcmp(argv[i], "-quit")) {
			exit(0);
		} else if (argv[i][0] == '-') {
			fprintf(stderr,"Unknown option: %s\n", argv[i]);
			exit(1);
		}
		else if (!start_idf(argv[i])) {
			fprintf(stderr, "Adding idf: %s\n", argv[i]);
		}
#ifdef _USE_UNPACK
		else {
			char *p;
			if (games_sw)
				p = games_sw;
			else
				p = game_tmp_path();
			if (setup_zip(argv[i], p))
				exit(1);
			clean_tmp = 1;
		}
#endif
	}

	if (debug_sw) {
		debug_init();
	}

	if (version_sw) {
		fprintf(stdout, VERSION"\n");
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
		exit(1);
	}
	
	cfg_load();
	
	if (!opt_lang || !opt_lang[0])
		opt_lang = game_locale();
	
	if (menu_lang_select(opt_lang) && menu_lang_select(LANG_DEF)) {
		fprintf(stderr, "Can not load default language.\n");
		exit(1);
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
	
	// Initialize SDL
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
#ifdef ANDROID
	exit(err);
#endif
	return err;
}

