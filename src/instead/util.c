/*
 * Copyright 2009-2016 Peter Kosyh <p.kosyh at gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "system.h"
#include "util.h"
#include "tinymt32.h"

int get_utf8(const char *sp, unsigned long *sym_out)
{
	int i = 0, l = 0;
	unsigned long sym = 0;
	const unsigned char *p = (const unsigned char*)sp;

	if (sym_out)
		*sym_out = *p;

	if (!*p)
		return 0;

	if (!(*p & 0xc0))
		return 1;

	if ((*p & 0xe0) == 0xc0) {
		l = 1;
		sym = (*p & 0x1f);
	} else if ((*p & 0xf0) == 0xe0) {
		l = 2;
		sym = (*p & 0xf);
	} else if ((*p & 0xf8) == 0xf0) {
		l = 3;
		sym = (*p & 7);
	} else if ((*p & 0xfc) == 0xf8) {
		l = 4;
		sym = (*p & 3);
	} else if ((*p & 0xfe) == 0xfc) {
		l = 5;
		sym = (*p & 1);
	} else {
		return 1;
	}
	p ++;
	for (i = 0; i < l; i ++) {
		sym <<= 6;
		if ((*p & 0xc0) != 0x80) {
			return 1;
		}
		sym |= (*p++ & 0x3f);
	}
	if (sym_out)
		*sym_out = sym;
	return l + 1;
}

int is_cjk(unsigned long sym)
{
	if (sym >=0x2E80 && sym <= 0x2EFF)
		return 1;
	if (sym >=0x2F00 && sym <= 0x2FDF)
		return 1;
	if (sym >= 0x2FF0 && sym <= 0x2FFF)
		return 1;
	if (sym >= 0x3000 && sym <= 0x303F)
		return 1;
	if (sym >= 0x3040 && sym <= 0x309F)
		return 1;
	if (sym >= 0x30A0 && sym <=0x30FF)
		return 1;
	if (sym >= 0x3100 && sym <=0x312F)
		return 1;
	if (sym >= 0x3130 && sym <= 0x318F)
		return 1;
	if (sym >= 0x3190 && sym <= 0x319F)
		return 1;
	if (sym >= 0x31A0 && sym <= 0x31BF)
		return 1;
	if (sym >= 0x31F0 && sym <= 0x31FF)
		return 1;
	if (sym >= 0x3200 && sym <= 0x32FF)
		return 1;
	if (sym >= 0x3300 && sym <= 0x33FF)
		return 1;
	if (sym >= 0x3400 && sym <= 0x4DBF)
		return 1;
	if (sym >= 0x4DC0 && sym <= 0x4DFF)
		return 1;
	if (sym >= 0x4E00 && sym <= 0x9FFF)
		return 1;
	if (sym >= 0xA000 && sym <= 0xA48F)
		return 1;
	if (sym >= 0xA490 && sym <= 0xA4CF)
		return 1;
	if (sym >= 0xAC00 && sym <= 0xD7AF)
		return 1;
	if (sym >= 0xF900 && sym <= 0xFAFF)
		return 1;
	if (sym >= 0xFE30 && sym <= 0xFE4F)
		return 1;
	if (sym >= 0x1D300 && sym <= 0x1D35F)
		return 1;
	if (sym >= 0x20000 && sym <= 0x2A6DF)
		return 1;
	if (sym >= 0x2F800 && sym <= 0x2FA1F)
		return 1;
	return 0;
}

int is_space(int c)
{
	return (c == ' ' || c == '\t');
}

int is_empty(const char *str)
{
	if (!str || !*str)
		return 1;
	while (*str && !is_space(*str++))
		return 0;
	return 1;
}

char *strip(char *s)
{
	char *e;
	while (is_space(*s))
		s ++;
	if (!*s)
		return s;
	e = s + strlen(s) - 1;
	while (e != s && is_space(*e)) {
		*e = 0;
		e --;
	}
	return s;
}

int strlowcmp(const char *s, const char *d)
{
	int rc;
	char *ss = NULL;
	char *dd = NULL;
	ss = strdup(s);
	dd = strdup(d);
	if (!ss || !dd) {
		rc = strcmp(s, d);
		goto err;
	}
	tolow(ss);
	tolow(dd);
	rc = strcmp(ss, dd);
err:
	if (ss)
		free(ss);
	if (dd)
		free(dd);
	return rc; 
}

char *getpath(const char *d, const char *n)
{
	char *p = getfilepath(d, n);
	strcat(p, "/");
	return p;
}


void tolow(char *p)
{
	while (*p) {
		if (*p >=  'A' && *p <= 'Z')
			*p |= 0x20;
		p ++;
	}
}

char *getfilepath(const char *d, const char *n)
{
	int i = ((d)?strlen(d):0) + ((n)?strlen(n):0) + 3;
	char *p = malloc(i);
	if (p) {
		p[0] = 0;
		if (d && d[0]) { /* non empty string */
			strcpy(p, d);
			if (p[strlen(d) - 1] != '/')
				strcat(p, "/");
		}
		if (n)
			strcat(p, n);
		unix_path(p);
	}
	return p;
}

void unix_path(char *path)
{
	char *p = path;
	if (!path)
		return;
	while (*p) { /* bad bad Windows!!! */
		if (*p == '\\')
			*p = '/';
		p ++;
	}
	return;
}

#ifdef _HAVE_ICONV
#define CHAR_MAX_LEN 4
char *decode(iconv_t hiconv, const char *s)
{
	size_t s_size, chs_size, outsz, insz;
	char *inbuf, *outbuf, *chs_buf;
	if (!s || hiconv == (iconv_t)(-1))
		return NULL;
	s_size = strlen(s) + 1; 
	chs_size = s_size * CHAR_MAX_LEN; 
	if ((chs_buf = malloc(chs_size + CHAR_MAX_LEN))==NULL)
		goto exitf; 
	outsz = chs_size; 
	outbuf = chs_buf; 
	insz = s_size; 
	inbuf = (char*)s; 
	while (insz) { 
		if (iconv(hiconv, &inbuf, &insz, &outbuf, &outsz) 
						== (size_t)(-1)) 
	   	 	goto exitf; 
	} 
	*outbuf++ = 0; 
	return chs_buf; 
exitf: 
	if(chs_buf) 
		free(chs_buf); 
	return NULL; 
}
#endif

static tinymt32_t trandom;

void mt_random_init(void) 
{
	tinymt32_init(&trandom, time(NULL));
}

void mt_random_seed(unsigned long seed) 
{
	tinymt32_init(&trandom, seed);
}

unsigned long mt_random(void) 
{
	return tinymt32_generate_uint32(&trandom);
}

double mt_random_double(void)
{
	return tinymt32_generate_32double(&trandom);
}

#if defined(S60) 
#include "system.h"
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <sys/unistd.h>

#include "snprintf.c"

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
	if (!path || !path[0])
		return 0;
	if (path[0] == '/' || path[0] == '\\')
		return 1;
	if (!path[1])
		return 0;
	return (path[1] == ':');
}
#elif defined(_WIN32_WCE) 

#include "system.h"
#include <windows.h>
#include <shlobj.h>
#include <limits.h>
#include <sys/types.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static char curdir[PATH_MAX];

int setdir(const char *path)
{
	strncpy(curdir, path, sizeof(curdir) - 1);
	return 0;
}

char *getdir(char *path, size_t size)
{
	strncpy(path, curdir, size - 1);
	return path;
}

char *dirpath(const char *path)
{
	static char fp[PATH_MAX * 4];
	if (path[0] == '/')
		return (char*)path;
	strcpy(fp, curdir);
	strcat(fp, "/");
	strcat(fp, path);
	unix_path(fp);
	return fp;
}

int is_absolute_path(const char *path)
{
	if (!path || !*path)
		return 0;
	return (*path == '/' || *path == '\\');
}

#elif defined(_WIN32)

#include <windows.h>
#include <shlobj.h>
#include <limits.h>
#include <libgen.h>
#include <sys/types.h>
#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

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
	if (!path || !path[0])
		return 0;
	if (path[0] == '/' || path[0] == '\\')
		return 1;
	if (!path[1])
		return 0;
	return (path[1] == ':');
}

#elif defined(__APPLE__)

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
#include "system.h"

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
#else

#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef ANDROID
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
#else
static char curdir[PATH_MAX];

int setdir(const char *path)
{
	strncpy(curdir, path, sizeof(curdir) - 1);
	return chdir(path);
}

char *getdir(char *path, size_t size)
{
	strncpy(path, curdir, size - 1);
	return path;
}

char *dirpath(const char *path)
{
	static char fp[PATH_MAX];
	if (path[0] == '/')
		return (char*)path;
	strcpy(fp, curdir);
	strcat(fp, "/");
	strcat(fp, path);
	unix_path(fp);
	return fp;
}
#endif

int is_absolute_path(const char *path)
{
	if (!path || !*path)
		return 0;
	return (*path == '/');
}

#endif
