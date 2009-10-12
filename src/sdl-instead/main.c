#include "externals.h"
#include "internals.h"

extern int debug_init(void);
extern void debug_done(void);

int debug_sw = 0;
int noauto_sw = 0;
char *game_sw = NULL;
char *games_sw = NULL;
char *theme_sw = NULL;
char *themes_sw = NULL;
char *encode_sw = NULL;
char *encode_output = NULL;

int main(int argc, char **argv)
{
	int err = 0;
	int i;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i],"-alsa")) 
			alsa_sw = 1;
		else if (!strcmp(argv[i], "-nosound"))
			nosound_sw = 1;
		else if (!strcmp(argv[i], "-fullscreen"))
			fullscreen_sw = 1;
		else if (!strcmp(argv[i], "-window"))
			window_sw = 1;
		else if (!strcmp(argv[i], "-debug"))
			debug_sw = 1;
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
		}

	}	
	
	if (debug_sw) {
		debug_init();
	}
	
	if (encode_sw) {
		err = instead_encode(encode_sw, encode_output);
		goto out;		
	}
	menu_langs_lookup(LANG_PATH);
	
	if (!langs_nr) {
		fprintf(stderr, "No languages found in: %s.\n", LANG_PATH);
		exit(1);
	}
	
	if (games_sw)
		games_lookup(games_sw);

	if (games_lookup(GAMES_PATH))
			fprintf(stderr, "No games found in: %s.\n", GAMES_PATH);
	if (themes_sw)
		themes_lookup(themes_sw);
	themes_lookup(THEMES_PATH);
	
	themes_lookup(game_local_themes_path());
	games_lookup(game_local_games_path());

	cfg_load();
	
	if (noauto_sw)
		opt_autosave = 0;
	if (window_sw)
		opt_fs = 0;
	if (fullscreen_sw)
		opt_fs = 1;
	
	
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

	if (game_init(opt_game)) {
		game_error(opt_game);
	}
	game_loop();
	cfg_save();
	game_done(0);
	gfx_done();
out:
	if (debug_sw)
		debug_done();
	return err;
}

