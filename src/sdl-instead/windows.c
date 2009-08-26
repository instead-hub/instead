#include <windows.h>
#include <shlobj.h>
#include <limits.h>
#include <sys/types.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

extern char *curgame;
extern char *curgame_dir;

static char save_path[PATH_MAX];

void	nsleep(int u)
{
	Sleep(u);
}

char *game_local_games_path(void)
{
	return NULL; /* TODO ? */
}

char *game_local_themes_path(void)
{
	return NULL; /* TODO ? */
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

