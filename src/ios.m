#include <Foundation/NSString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <UIKit/UIKit.h>
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
#include <dirent.h>
#include <SDL.h>

#include "externals.h"
#include "internals.h"

static char save_path[PATH_MAX];
static char cfg_path[PATH_MAX];
static char local_games_path[PATH_MAX];
static char local_themes_path[PATH_MAX];
static char local_stead_path[PATH_MAX];

extern int setup_zip(const char *file, char *p);

char *game_locale(void)
{
	NSString * language = [[NSLocale preferredLanguages] objectAtIndex:0];
	return strdup([language UTF8String]);
}

char *game_tmp_path(void)
{
	static char tmp[PATH_MAX];
	NSString * tmpdir = NSTemporaryDirectory();
	snprintf(tmp, sizeof(tmp), "%s/instead-games", [tmpdir UTF8String]); 
	tmp[sizeof(tmp) - 1] = 0;
	if (mkdir(tmp, S_IRWXU) && errno != EEXIST)
		return NULL;
	return tmp;
}

static char *inbox(void)
{
	static char dir[PATH_MAX];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	snprintf(dir, sizeof(dir) - 1 , "%s/Inbox", [basePath UTF8String]);
	return dir;
}

int setup_inbox(void)
{
	char path[PATH_MAX];
	DIR *d;
	struct dirent *de;
    char *p;

	if (!path)
		return 0;

	d = opendir(inbox());
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		snprintf(path, sizeof(path), "%s/%s", inbox(), de->d_name);
		path[sizeof(path) - 1] = 0;
		p = game_local_games_path(1);
		fprintf(stderr, "Install zip: %s\n", path);
		setup_zip(path, game_local_games_path(1));
		unlink(path);
	}
	closedir(d);
	return 0;
}

char *appdir(void)
{
	static char dir[PATH_MAX];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	snprintf(dir, sizeof(dir) - 1 , "%s/.instead", [basePath UTF8String]);
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
	snprintf(cfg_path, sizeof(cfg_path) - 1 , "%s/", app);
	mkdir(cfg_path, S_IRWXU);
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
#if 0
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
#endif
	return NULL;
}

void rotate_landscape(void)
{
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
	if (UIDeviceOrientationIsPortrait([UIApplication sharedApplication].statusBarOrientation)) {
		[[UIDevice currentDevice] setValue:@(UIDeviceOrientationLandscapeRight) forKey:@"orientation"];
	}
}

void rotate_portrait(void)
{
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait PortraitUpsideDown");
	if (UIDeviceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation)) {
		[[UIDevice currentDevice] setValue:@(UIDeviceOrientationPortrait) forKey:@"orientation"];
	}
}

void unlock_rotation(void)
{
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait PortraitUpsideDown LandscapeLeft LandscapeRight");
}

void ios_cfg_init(void)
{
	BOOL is_iPhone = [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone;
	opt_fsize = (is_iPhone) ? 5 : 0;
}
