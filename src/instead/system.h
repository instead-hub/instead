#ifndef __INSTEAD_EXTERNAL_H_
#define __INSTEAD_EXTERNAL_H_

#if defined (_WIN32_WCE) || defined(WINRT)
 #define errno 0
 #define putenv(a) ;
 #ifndef WINRT
 #define strerror(a) ""
 #define setlocale(a, b) ;
 #endif
#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>

#include <limits.h>

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
#ifdef S60
 #include "snprintf.h"
 typedef long ssize_t;
#endif
#include <math.h>

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

#endif
