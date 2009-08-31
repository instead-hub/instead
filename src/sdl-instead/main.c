#include <unistd.h>
#include <stdlib.h>
#include "graphics.h"
#include "game.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <string.h>

extern int debug_init(void);
extern void debug_done(void);

int debug_sw = 0;
char *game_sw = NULL;

int main(int argc, char **argv)
{
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
		else if (!strcmp(argv[i], "-game")) {
			if ((i + 1) < argc)
				game_sw = argv[++i];
			else
				game_sw = "";
		}	
	}		

	if (debug_sw) {
		debug_init();
	}

	if (window_sw)
		opt_fs = 0;
	if (fullscreen_sw)
		opt_fs = 1;

	if (games_lookup(GAMES_PATH)) {
		fprintf(stderr, "No games found.\n");
	}
	
	themes_lookup(THEMES_PATH);
	themes_lookup(game_local_themes_path());
	
	games_lookup(game_local_games_path());
	
	cfg_load();
	
	if (game_sw)
		opt_game = game_sw;
		
	if (opt_theme)
		game_theme_select(opt_theme);
	if (!curtheme)
		game_theme_select(DEFAULT_THEME);

	if (game_init(opt_game)) {
		game_error(opt_game);
	}
	game_loop();
	cfg_save();
	game_done();
	if (debug_sw)
		debug_done();
	return 0;
}

