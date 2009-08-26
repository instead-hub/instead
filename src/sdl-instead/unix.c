#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

extern char *curgame;
extern char *curgame_dir;

static char save_path[PATH_MAX];
static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];


void	nsleep(int u)
{
	usleep(u);
}

char *game_local_games_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(local_games_path, sizeof(local_games_path) - 1 , "%s/.instead/games/", pw->pw_dir);
	return local_games_path;
	
}

char *game_local_themes_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(local_themes_path, sizeof(local_themes_path) - 1 , "%s/.instead/themes/", pw->pw_dir);
	return local_themes_path;
	
}

char *game_cfg_path(void)
{
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.insteadrc", pw->pw_dir);
	return save_path;
	
}
char *game_save_path(int cr, int nr)
{
	struct passwd *pw;
	if (!curgame)
		return NULL;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/.instead/saves", pw->pw_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/", pw->pw_dir, curgame_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;	
	if (nr)
		snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/save%d", pw->pw_dir, curgame_dir, nr);
	else
		snprintf(save_path, sizeof(save_path) - 1, "%s/.instead/saves/%s/autosave", pw->pw_dir, curgame_dir);
	return save_path;
}

int debug_init(void)
{
	return 0;
}

void debug_done()
{
	
}

