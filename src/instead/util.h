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

#ifndef __INSTEAD_UTIL_H_INCLUDED
#define __INSTEAD_UTIL_H_INCLUDED

extern char *strip(char *s);

extern int is_cjk(unsigned long sym);
extern int get_utf8(const char *p, unsigned long *sym_out);

extern int is_space(int c);
extern int is_empty(const char *str);

extern char *getfilepath(const char *d, const char *n);
extern int strlowcmp(const char *s, const char *d);

extern void unix_path(char *path);
extern int setdir(const char *path);
extern char *getpath(const char *d, const char *n);
extern char *getdir(char *path, size_t size);
extern char *dirpath(const char *path);
extern void tolow(char *p);
extern int is_absolute_path(const char *p);

extern void mt_random_init(void);
extern void mt_random_seed(unsigned long seed);
extern unsigned long mt_random(void);
extern double mt_random_double(void);

#define FREE(v) do { if ((v)) free((v)); v = NULL; } while(0)

#ifdef _USE_SDL
 #include <SDL_config.h>
 #include <SDL_mutex.h>
 #ifdef _USE_SDL_ICONV
  #include <SDL_stdinc.h>
  #define iconv SDL_iconv
  #define iconv_t SDL_iconv_t
  #define iconv_open SDL_iconv_open
  #define iconv_close SDL_iconv_close
 #else
  #ifdef _HAVE_ICONV
   #include <iconv.h>
  #endif
 #endif
#else
  #ifdef _HAVE_ICONV
   #include <iconv.h>
  #endif
#endif

#ifdef _HAVE_ICONV
extern char *decode(iconv_t hiconv, const char *s);
#endif

extern char *getrealpath(const char *path, char *resolved);

#endif
