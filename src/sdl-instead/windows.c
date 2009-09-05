#include <windows.h>
#include <shlobj.h>
#include <limits.h>
#include <sys/types.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "internals.h"

extern char *curgame;
extern char *curgame_dir;

static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];

static char save_path[PATH_MAX];

void	nsleep(int u)
{
	Sleep(u);
}

char *game_locale(void)
{
	char buff[64];
	buff[0] = 0;
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME,
        	buff,sizeof(buff) - 1))
		return NULL;
	return strdup(buff);
}

char *app_dir( void );

char *game_local_games_path(void)
{
	snprintf(local_games_path, sizeof(local_games_path) - 1 , "%s/instead/games/", app_dir());
	return local_games_path;
}

char *game_local_themes_path(void)
{
	snprintf(local_themes_path, sizeof(local_themes_path) - 1 , "%s/instead/themes/", app_dir());
	return local_themes_path;
}

extern void unix_path(char *);

char *app_dir( void )
{
	static char appdir[PATH_MAX];
	SHGetFolderPath( NULL, 
		CSIDL_FLAG_CREATE | CSIDL_LOCAL_APPDATA,
		NULL,
		0, 
		(LPTSTR)appdir );
	unix_path(appdir);
	return appdir;
}

char *game_cfg_path( void )
{
	snprintf(save_path, sizeof(save_path) - 1 , "%s\\insteadrc", app_dir());
	if (!access(save_path, R_OK)) 
		return save_path;
/* no at home? Try in dir */
	snprintf(save_path, sizeof(save_path) - 1 , "%s\\instead", app_dir());
	if (mkdir(save_path) && errno != EEXIST)
		snprintf(save_path, sizeof(save_path) - 1 , "%s\\insteadrc", app_dir()); /* fallback to home */
	else
		snprintf(save_path, sizeof(save_path) - 1 , "%s\\instead\\insteadrc", app_dir());
	return save_path;
}

char *game_save_path( int cr, int nr )
{
	char appdir[PATH_MAX];
	strcpy( appdir, app_dir() );

	snprintf(save_path, sizeof(save_path) - 1 , "%s/instead", appdir);
	if (cr && mkdir(save_path) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1 , "%s/instead/saves", appdir);
	if (cr && mkdir(save_path) && errno != EEXIST)
		return NULL;
	snprintf(save_path, sizeof(save_path) - 1, "%s/instead/saves/%s", appdir, curgame_dir);
	if (cr && mkdir(save_path) && errno != EEXIST)
		return NULL;
	if (nr)
		snprintf(save_path, sizeof(save_path) - 1, "%s/instead/saves/%s/save%d", appdir, curgame_dir, nr);
	else
		snprintf(save_path, sizeof(save_path) - 1, "%s/instead/saves/%s/autosave", appdir, curgame_dir);
	return save_path; 
}

int debug_init(void)
{
	if (!AllocConsole())
		return -1;
	SetConsoleTitle("Debug");
	freopen("CON", "w", stdout); //Map stdout
	freopen("CON", "w", stderr); //Map stderr
	return 0;
}

void debug_done()
{
	FreeConsole();	
}

