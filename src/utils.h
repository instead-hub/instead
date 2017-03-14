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

#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

#include "instead/util.h"
#include "instead/idf.h"
#include "instead/cache.h"
#include "instead/list.h"

typedef int (*parser_fn)(const char *v, void *data);

struct parser {
	const char 	*cmd;
	parser_fn	fn; 
	void		*p;
	long		aux;
};

extern int parse_ini(const char *path, struct parser *cmd_parser);
extern int parse_idff(idff_t idff, const char *path, struct parser *cmd_parser);

extern char *lookup_tag(const char *fname, const char *tag, const char *comm);
extern char *lookup_lang_tag(const char *fname, const char *tag, const char *comm, const char *lang);
extern char *lookup_lang_tag_idf(idf_t idf, const char *fname, const char *tag, const char *comm, const char *lang);

extern int parse_esc_string(const char *v, void *data);
extern int parse_string(const char *v, void *data);
extern int parse_int(const char *v, void *data);
extern int parse_float(const char *v, void *data);

extern int parse_full_path(const char *v, void *data);
extern int parse_path(const char *v, void *data);
extern int process_cmd(char *n, char *v, struct parser *cmd_parser);

extern char *encode_esc_string(const char *v);
extern char *find_in_esc(const char *l, const char *s);

extern char *sdl_path(char *path);

extern char *parse_tag(char *line, const char *tag, const char *comm, int *brk);
extern int remove_dir(const char *path);

extern void data2hex(const void *d, int len, void *o);
extern int hex2data(const void *d, void *o, int len);
extern void trunc_lines(char *pp, int max);

#endif
