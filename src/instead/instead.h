/*
 * Copyright 2009-2021 Peter Kosyh <p.kosyh at gmail.com>
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

#include <lua.h>
#include <lauxlib.h>
#include "idf.h"
#include "list.h"

#define INSTEAD_NIL  0
#define INSTEAD_NUM  1
#define INSTEAD_STR  2
#define INSTEAD_BOOL 3

#define INSTEAD_MAIN	"main.lua"
#define INSTEAD_MAIN3	"main3.lua"
#define INSTEAD_IDF	"data.idf"

#ifndef STEAD_PATH
#define STEAD_PATH 	"./stead"
#endif

struct instead_args {
	int type;
	const char *val;
};

struct instead_ext {
	struct list_node list;
	int (*init)(void);
	int (*done)(void);
	int (*err)(void);
	int (*cmd)(void);
};

extern int  instead_init(const char *path);
extern int  instead_load(char **info);
extern void instead_done(void);
extern void instead_lock(void);
extern void instead_unlock(void);
extern char *instead_cmd(char *s, int *rc);
extern char *instead_file_cmd(char *s, int *rc);

extern int  instead_function(char *s, struct instead_args *args);
extern int  instead_eval(char *s);
extern int  instead_pcall(lua_State *L, int nargs);
extern int  instead_clear(void);
extern char *instead_retval(int n);
extern int  instead_bretval(int n);
extern int  instead_iretval(int n);
extern char *instead_fromgame(const char *s);
extern int  instead_encode(const char *s, const char *d);
extern int  instead_busy(void);

extern void instead_set_encoding(const char *cp); /* UTF-8 by default */

/* internal use */
extern int  instead_init_lua(const char *path, int detect);
extern int  instead_loadscript(char *name, int argc, char **argv, int exec);
extern int  instead_loadfile(char *name);

extern void 	instead_err_msg(const char *s);
extern const	char *instead_err(void);

extern int	instead_extension(struct instead_ext *ext);
extern int	instead_api_register(const luaL_Reg *api);

extern idf_t  instead_idf(void);
extern int  instead_set_lang(const char *lang);
extern int  instead_set_debug(int);
extern int  instead_set_standalone(int);
/* extern int  instead_set_api(const char *api); */
extern const char *instead_lua_path(const char *path); /* override STEAD_PATH */

extern const char *instead_get_api(void);

extern lua_State *instead_lua(void);

extern char	*instead_local_stead_path(void);
extern char	*instead_path(void);
extern char	*instead_stead_path(void);
extern char	*instead_cwd(void);

#endif
