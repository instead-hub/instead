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

#ifndef __INSTEAD_H__
#define __INSTEAD_H__

#define INSTEAD_NIL  0
#define INSTEAD_NUM  1
#define INSTEAD_STR  2
#define INSTEAD_BOOL 3

struct instead_args {
	int type;
	const char *val;
};

extern int  instead_init(const char *path);
extern int  instead_load(char *game);
extern void instead_done(void);
extern char *instead_cmd(char *s);
extern char *instead_file_cmd(char *s);
extern int  instead_function(char *s, struct instead_args *args);
extern int  instead_eval(char *s);
extern int  instead_clear(void);
extern char *instead_retval(int n);
extern int  instead_bretval(int n);
extern int  instead_iretval(int n);
extern int	instead_lang(void);
char 		*fromgame(const char *s);
extern int  instead_encode(const char *s, const char *d);
extern int  instead_busy(void);

/* internal use */
extern int  instead_init_lua(const char *path);
extern int  instead_loadscript(char *name, int argc, char **argv, int exec);

#endif
