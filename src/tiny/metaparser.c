#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instead/instead.h"
static int log_opt = 0;
static int tiny_init(void)
{
	int rc;
	rc = instead_loadfile("tiny.lua");
	if (rc)
		return rc;
	return 0;
}
static struct instead_ext ext = {
	.init = tiny_init,
};

int main(int argc, const char **argv)
{
	int rc; char *str; const char *game = NULL;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <game>\n", argv[0]);
		exit(1);
	}
	game = argv[1];
	if (instead_extension(&ext)) {
		fprintf(stderr, "Can't register tiny extension\n");
		exit(1);
	}
	instead_set_debug(1);

	if (instead_init(game)) {
		fprintf(stderr, "Can not init game: %s\n", game);
		exit(1);
	}
	if (instead_load(NULL)) {
		fprintf(stderr, "Can not load game: %s\n", instead_err());
		exit(1);
	}
#if 0 /* no autoload */
	str = instead_cmd("load autosave", &rc);
#else
	str = instead_cmd("look", &rc);
#endif
	if (!rc) {
		printf("%s\n", str); fflush(stdout);
	}
	free(str);

	while (1) {
		char input[256], *p, cmd[256 + 64];
		printf(">\n"); fflush(stdout);
		p = fgets(input, sizeof(input), stdin);
		if (!p)
			break;
		p[strcspn(p, "\n\r")] = 0;
		if (!strcmp(p, "quit"))
			break;
		if (!strcmp(p, "log")) {
			log_opt = 1;
			continue;
		}
		if (!strncmp(p, "load ", 5) || !strncmp(p, "save ", 5))
			snprintf(cmd, sizeof(cmd), "%s", p);
		else
			snprintf(cmd, sizeof(cmd), "@metaparser \"%s\"", p);
		str = instead_cmd(cmd, NULL);
		if (str) {
			printf("%s\n", str); fflush(stdout);
		}
		free(str);
		if (log_opt) fprintf(stderr, "%s\n", p);
	}
	instead_cmd("save autosave", NULL);
	instead_done();
	exit(0);
}
