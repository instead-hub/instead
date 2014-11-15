/*
 * Copyright 2009-2014 Peter Kosyh <p.kosyh at gmail.com>
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

#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED
#include "idf.h"
#include "tinymt32.h"

typedef int (*parser_fn)(const char *v, void *data);

struct parser {
	const char 	*cmd;
	parser_fn	fn; 
	void		*p;
	long		aux;
};
extern int is_cjk(unsigned long sym);
extern int get_utf8(const char *p, unsigned long *sym_out);

extern int is_space(int c);
extern int is_empty(const char *str);

extern int parse_ini(const char *path, struct parser *cmd_parser);
extern int parse_idff(idff_t idff, const char *path, struct parser *cmd_parser);

extern char *getpath(const char *d, const char *n);
extern char *strip(char *s);
char *getfilepath(const char *d, const char *n);

extern char *lookup_tag(const char *fname, const char *tag, const char *comm);
extern char *lookup_lang_tag(const char *fname, const char *tag, const char *comm);
extern char *lookup_lang_tag_idf(idff_t idf, const char *tag, const char *comm);

extern int parse_esc_string(const char *v, void *data);
extern int parse_string(const char *v, void *data);
extern int parse_int(const char *v, void *data);
extern int parse_float(const char *v, void *data);

extern int parse_full_path(const char *v, void *data);
extern int process_cmd(char *n, char *v, struct parser *cmd_parser);

extern char *encode_esc_string(const char *v);
extern char *find_in_esc(const char *l, const char *s);
#ifdef _HAVE_ICONV
extern char *decode(iconv_t hiconv, const char *s);
#endif

extern void unix_path(char *path);
extern char *sdl_path(char *path);
extern int setdir(const char *path);
extern char *getdir(char *path, size_t size);
extern char *dirpath(const char *path);

extern char *parse_tag(char *line, const char *tag, const char *comm, int *brk);
extern int remove_dir(const char *path);

extern void tolow(char *p);
extern int strlowcmp(const char *s, const char *d);
extern char *getrealpath(const char *path, char *resolved);
extern int is_absolute_path(const char *p);

extern void mt_random_init(void);
extern void mt_random_seed(unsigned long seed);
extern unsigned long mt_random(void);
extern double mt_random_double(void);

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

#define FREE(v) do { if ((v)) free((v)); v = NULL; } while(0)

#endif
