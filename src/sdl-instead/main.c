#include <stdlib.h>
#include "graphics.h"
#include "game.h"
#include <stdio.h>
#include <sys/fcntl.h>

int main(int argc, char **argv)
{
	if (games_lookup()) {
		fprintf(stderr, "No games found.\n");
	//	exit(1);
	}

	if (game_init(NULL)) {
		exit(1);
	}
	game_loop();
	game_done();
	return 0;
}
