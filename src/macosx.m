#include <AppKit/NSOpenPanel.h>
#include <Foundation/NSString.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "externals.h"
#include "internals.h"

static char save_path[PATH_MAX];
static char cfg_path[PATH_MAX];
static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];
static char local_stead_path[PATH_MAX];

char *game_locale(void)
{
	char *p;
	char *s;
	p = getenv("LANG");
	if (!p || !(s = strdup(p)))
		return NULL;
	if ((p = strchr(s, '_')))
		*p = 0;
	return s;
}

char *game_tmp_path(void)
{
	static time_t t = 0;
	static char tmp[PATH_MAX]="/tmp/instead-games";
	if (!t) {
		t = time(NULL);
		sprintf(tmp, "/tmp/instead-games-%ld", (unsigned long)t);
	}
	if (mkdir(tmp, S_IRWXU) && errno != EEXIST)
		return NULL;
	return tmp;
}

char *appdir(void)
{
	static char dir[PATH_MAX];
	struct passwd *pw;
#ifdef _LOCAL_APPDATA
	strcpy(dir, game_cwd);
	strcat(dir, "/appdata");
	if (!access(dir, W_OK))
		return dir;
#endif
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(dir, sizeof(dir) - 1 , "%s/.instead", pw->pw_dir);
	return dir;
}

char *game_local_games_path(int cr)
{
	char *app = appdir();
	if (!app)
		return NULL;
	strcpy(local_games_path, app);
	if (cr) {
		if (mkdir(local_games_path, S_IRWXU) && errno != EEXIST)
        		return NULL;
        }
        strcat(local_games_path,"/games");
        if (cr) { 
		if (mkdir(local_games_path, S_IRWXU) && errno != EEXIST)
        		return NULL;
        }
	return local_games_path;
}

char *game_local_themes_path(void)
{
	char *app = appdir();
	if (!app)
		return NULL;
	snprintf(local_themes_path, sizeof(local_themes_path) - 1 , "%s/themes", app);
	return local_themes_path;
}

char *game_local_stead_path(void)
{
	char *app = appdir();
	if (!app)
		return NULL;
	snprintf(local_stead_path, sizeof(local_stead_path) - 1 , "%s/stead", app);
	return local_stead_path;
}

char *game_cfg_path(void)
{
	char *app = appdir();
	struct passwd *pw;
	pw = getpwuid(getuid());
	if (!pw) 
		return NULL;
	snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/.insteadrc", pw->pw_dir); /* at home */
	if (!access(cfg_path, R_OK)) 
		return cfg_path;
/* no at home? Try in dir */
	if (app)
		snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/", app);
	if (!app || (mkdir(cfg_path, S_IRWXU) && errno != EEXIST))
		snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/.insteadrc", pw->pw_dir); /* fallback to home */
	else
		snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/insteadrc", app);
	return cfg_path;
}

char *game_save_path(int cr, int nr)
{
	char *app = appdir();
	if (!curgame_dir)
		return NULL;
	if (!access("saves", R_OK)) {
		if (nr)
			snprintf(save_path, sizeof(save_path) - 1, "saves/save%d", nr);
		else
			snprintf(save_path, sizeof(save_path) - 1, "saves/autosave");
		return save_path;
	}	
	if (!app) 
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/", app);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/saves", app);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/", app, curgame_dir);
	if (cr && mkdir(save_path, S_IRWXU) && errno != EEXIST)
		return NULL;	
	if (nr)
		snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/save%d", app, curgame_dir, nr);
	else
		snprintf(save_path, sizeof(save_path) - 1, "%s/saves/%s/autosave", app, curgame_dir);
	return save_path;
}

int debug_init(void)
{
	return 0;
}

void debug_done()
{
	
}

char *sdl_path(char *p)
{
	unix_path(p);
	return p;
}


char *open_file_dialog(void) 
{
	const char *filename;
	static char *file_name[PATH_MAX];
	NSArray* fileTypes = [NSArray  arrayWithObjects: @"zip", @"lua", @"idf", nil];

	NSOpenPanel * panel = [NSOpenPanel openPanel];
	[panel setCanChooseDirectories:NO];
	[panel setCanChooseFiles:YES];
	[panel setAllowsMultipleSelection:NO];

	if ([panel runModalForTypes:fileTypes] == NSOKButton) {
#ifdef __POWERPC__
		filename = [[panel filename] cString];
#else
		filename = [[panel filename] cStringUsingEncoding:NSUTF8StringEncoding];
#endif
		strcpy(file_name, filename);
		return file_name;
	}
	return NULL;
}
#if 0
int setdir(const char *path)
{
	return chdir(path);
}

char *getdir(char *path, size_t size)
{
	return getcwd(path, size);
}

char *dirpath(const char *path)
{
	return (char*)path;
}

int is_absolute_path(const char *path)
{
	if (!path || !*path)
		return 0;
	return (*path == '/');
}
#endif
