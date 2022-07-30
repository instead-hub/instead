#ifndef __INSTEAD_EXTERNAL_H_
#define __INSTEAD_EXTERNAL_H_

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
#if !defined(PLAN9)
 #include <libgen.h>
 #include <math.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

#endif
