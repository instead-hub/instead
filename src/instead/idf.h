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

#ifndef __IDF_H_INCLUDED
#define __IDF_H_INCLUDED

struct _idf_t;
struct _idff_t;
typedef struct _idf_t *idf_t;
typedef struct _idff_t *idff_t;

extern idf_t	idf_init(const char *path);
extern void	idf_shrink(idf_t idf);
extern void	idf_done(idf_t idf);

extern int idf_create(const char *file, const char *path);
extern idff_t idf_open(idf_t idf, const char *fname);
extern int idf_setdir(idf_t idf, const char *path);
extern char *idf_getdir(idf_t idf);

extern int idf_seek(idff_t fil, int offset, int whence);
extern int idf_read(idff_t fil, void *ptr, int size, int maxnum);
extern int idf_close(idff_t fil);

extern int idf_eof(idff_t idf);
extern int idf_error(idff_t idf);
extern int idf_access(idf_t idf, const char *fname);

extern idff_t idf_opendir(idf_t idf, const char *dname);
extern int idf_closedir(idff_t d);
extern char *idf_readdir(idff_t d);

extern char *idf_gets(idff_t idf, char *b, int size);
extern int idf_magic(const char *fname);
extern int idf_only(idf_t idf, int fl);

#ifdef _USE_SDL
#include <SDL3/SDL_iostream.h>
extern SDL_IOStream *RWFromIdf(idf_t idf, const char *fname);
#endif

#endif
