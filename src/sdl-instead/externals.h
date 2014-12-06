#include <SDL_config.h>
#ifdef _WIN32_WCE
#include "wince.h"
#endif
#ifdef __IPHONEOS__
#include "ios.h"
#endif
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
/* #include <libintl.h> */
#include <unistd.h>

#include "sdl_iconv.h"
#include <limits.h>
/* #include <pwd.h> */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#ifndef S60
#include <libgen.h>
#endif
#include <math.h>
