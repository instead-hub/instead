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
static void footer(void)
{
	char *p;
	p = instead_cmd("way", NULL);
	if (p && *p) {
		printf(">> %s", p); free(p);
	}
	p = instead_cmd("inv", NULL);
	if (p && *p) {
		printf("** %s", p); free(p);
	}
}
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
	if (instead_load(&str)) {
		fprintf(stderr, "Can not load game: %s\n", instead_err());
		exit(1);
	}
	printf("%s\n", str);
#if 0 /* no autoload */
	str = instead_cmd("load autosave", &rc);
#else
	str = instead_cmd("look", &rc);
#endif
	if (!rc) {
		printf("%s", str);
		footer();
	}
	free(str);
	while (1) {
		char input[128], *p, cmd[64];
		p = fgets(input, sizeof(input), stdin);
		if (!p)
			break;
		printf("    * * *\n");
		p[strcspn(p, "\n\r")] = 0;
		if (!strcmp(p, "quit"))
			break;
		if (!strcmp(p, "log")) {
			log_opt = 1;
			continue;
		}
		snprintf(cmd, sizeof(cmd), "use %s", p);
		str = instead_cmd(cmd, &rc);
		if (rc) { /* try go */
			free(str);
			snprintf(cmd, sizeof(cmd), "go %s", p);
			str = instead_cmd(cmd, &rc);
		}
		if (rc) { /* try act */
			free(str);
			snprintf(cmd, sizeof(cmd), "%s", p);
			str = instead_cmd(cmd, &rc);
		}
		if (str)
			printf("%s", str);
		free(str);
		if (rc)
			printf("error!\n");
		else {
			footer();
			if (log_opt) fprintf(stderr, "%s\n", p);
		}
	}
	instead_cmd("save autosave", NULL);
	instead_done();
	exit(0);
}
