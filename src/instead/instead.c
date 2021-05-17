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

#include "system.h"
#include "instead.h"
#include "util.h"
#include "list.h"

#define DATA_IDF INSTEAD_IDF
#ifdef _USE_SDL
#ifndef __EMSCRIPTEN__
static SDL_mutex *sem;
#endif
void instead_lock(void) {
#ifndef __EMSCRIPTEN__
	SDL_LockMutex(sem);
#endif
}
void instead_unlock(void) {
#ifndef __EMSCRIPTEN__
	SDL_UnlockMutex(sem);
#endif
}
#else
void instead_lock(void) {
}
void instead_unlock(void) {
}
#endif
static char	instead_cwd_path[PATH_MAX];
static char	instead_game_path[PATH_MAX];

static int debug_sw = 0;
static int standalone_sw = 0;

static int busy = 0;
static idf_t data_idf = NULL;
/* the Lua interpreter */

char 		*instead_fromgame(const char *s);
char 		*togame(const char *s);
lua_State	*L = NULL;

static char *err_msg = NULL;
static char instead_api_path[PATH_MAX + 1];
static char instead_base_path[PATH_MAX] = STEAD_PATH;

static char *API = NULL;
static char *MAIN = NULL;

#define STEAD_API_PATH instead_api_path

#define ERR_MSG_MAX 512

static struct list_head extensions = LIST_HEAD_INIT(extensions);

#define for_each_extension(ext) list_for_each(&extensions, ext, list)

enum instead_hook {
	init,
	done,
	cmd,
	err,
};

#define HOOK_INIT 1
#define HOOK_DONE 2
#define HOOK_CMD  3
#define HOOK_ERR  4

static int extensions_hook(enum instead_hook nr)
{
	int rc = 0;
	struct instead_ext *ext = NULL;
	for_each_extension(ext) {
		switch (nr) {
		case init:
			if (ext->init)
				rc = ext->init();
			break;
		case done:
			if (ext->done)
				rc = ext->done();
			break;
		case cmd:
			if (ext->cmd)
				rc = ext->cmd();
			break;
		case err:
			if (ext->err)
				rc = ext->err();
			break;
		default:
			return -1;
		}
		if (rc)
			break;
	}
	return rc;
}

int instead_extension(struct instead_ext *ext)
{
	struct instead_ext *e = NULL;
	for_each_extension(e) {
		if (e == ext)
			return 0;
	}
	list_add(&extensions, &ext->list);
	return 0;
}

int instead_busy(void)
{
	return busy;
}

void instead_err_msg(const char *s)
{
	if (err_msg)
		free(err_msg);
	if (s) {
		err_msg = strdup(s);
		if (err_msg && strlen(err_msg) > ERR_MSG_MAX) {
			err_msg[ERR_MSG_MAX - 4] = 0;
			strcat(err_msg, "...");
		}
	} else
		err_msg = NULL;
}

const char *instead_err(void)
{
	return err_msg;
}

static int report (lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1)) {
		char *p;
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL)
			msg = "(error object is not a string)";
		fprintf(stderr,"Error: %s\n", msg);
		p = instead_fromgame(msg);
		instead_err_msg(p?p:msg);
		if (p)
			free(p);
		lua_pop(L, 1);
		status = -1;
		extensions_hook(err);
	}
	return status;
}

#if LUA_VERSION_NUM >= 502
static int traceback (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg)
    luaL_traceback(L, L, msg, 1);
  else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
    if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
      lua_pushliteral(L, "(no error message)");
  }
  return 1;
}
#else
static int traceback (lua_State *L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}
#endif
static int docall (lua_State *L, int narg)
{
	int status;
	int base = 0;
	if (debug_sw) {
		base = lua_gettop(L) - narg;  /* function index */
		lua_pushcfunction(L, traceback);  /* push traceback function */
		lua_insert(L, base);  /* put it under chunk and args */
	}
	busy ++;
	status = lua_pcall(L, narg, LUA_MULTRET, base);
	busy --;
	if (debug_sw)
		lua_remove(L, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0)
		lua_gc(L, LUA_GCCOLLECT, 0);
	return status;
}

int instead_pcall(lua_State *L, int nargs)
{
	int status;
	status = docall(L, nargs);
	status = report(L, status);
	return status;
}

static int dofile (lua_State *L, const char *name) {
	int status = luaL_loadfile(L, name) || docall(L, 0);
	return report(L, status);
}

static const char *idf_reader(lua_State *L, void *data, size_t *size)
{
	static char buff[4096];
	int rc;
	rc = idf_read((idff_t)data, buff, 1, sizeof(buff));
	*size = rc;
	if (!rc)
		return NULL;
	return buff;
}

static int dofile_idf (lua_State *L, idff_t idf, const char *name) {
#if LUA_VERSION_NUM >= 502
	int status = lua_load(L, idf_reader, idf, name, "bt") || docall(L, 0);
#else
	int status = lua_load(L, idf_reader, idf, name) || docall(L, 0);
#endif
	return report(L, status);
}

static int dostring (lua_State *L, const char *s) {
	int status = luaL_loadstring(L, s) || docall(L, 0);
	return report(L, status);
}

char *getstring(char *cmd)
{
	char *s;
	int N;
	if (!L)
		return NULL;
	if (dostring(L, cmd))
		return NULL;
	N = lua_gettop(L);  /* number of arguments */
	if (-N >=0)
		return NULL;
	s = (char*)lua_tostring(L, -N);
	if (s)
		s = instead_fromgame(s);
	return s;
}

int instead_eval(char *s)
{
	if (!L)
		return -1;
	if (dostring(L, s))
		return -1;
	return 0;
}

int instead_clear(void)
{
	int N;
	if (!L)
		return -1;
	N = lua_gettop(L);  /* number of arguments */
	lua_pop(L, N);
	return 0;
}

char *instead_retval(int n)
{
	char *s;
	int N;
	if (!L)
		return NULL;
	N = lua_gettop(L);  /* number of arguments */
/*	fprintf(stderr,"%d\n", N); */
	if (n - N >= 0)
		return NULL;
	s = (char*)lua_tostring(L, n - N);
	if (s)
		s = instead_fromgame(s);
	return s;
}

int instead_bretval(int n)
{
	int N;
	if (!L)
		return 0;
	N = lua_gettop(L);  /* number of arguments */
	if (n - N >= 0)
		return 1;
	return lua_toboolean(L, n - N);
}

int instead_iretval(int n)
{
	int N;
	if (!L)
		return 0;
	N = lua_gettop(L);  /* number of arguments */
	if (n - N >= 0)
		return 0;
	return lua_tonumber(L, n - N);
}

char *instead_file_cmd(char *s, int *rc)
{
	struct instead_args args[] = {
		{ .val = NULL, .type = INSTEAD_STR },
		{ .val = NULL, },
	};
	if (!s)
		return NULL;
	args[0].val = s;
	instead_function("iface:cmd", args);
	s = instead_retval(0);
	if (rc)
		*rc = !instead_bretval(1);
	instead_clear();
	extensions_hook(cmd);
	return s;
}

char *instead_cmd(char *s, int *rc)
{
	struct instead_args args[] = {
		{ .val = NULL, .type = INSTEAD_STR },
		{ .val = NULL, },
	};
	if (!s)
		return NULL;
	s = togame(s);
	if (!s)
		return NULL;
	args[0].val = s;
	instead_function("iface:cmd", args);
	free(s);
	s = instead_retval(0);
	if (rc)
		*rc = !instead_bretval(1);
	instead_clear();
	extensions_hook(cmd);
	return s;
}

int instead_function(char *s, struct instead_args *args)
{
	int base = 0;
	int status = 0;
	int n = 0;
	char *p;
	char f[64];
	int method = 0;
	if (!L)
		return -1;
	strcpy(f, s);
	p = strchr(f, '.');
	if (!p)
		p = strchr(f, ':');
	if (p) {
		if (*p == ':')
			method = 1;
		*p = 0;
		p ++;
		lua_getglobal(L, f);
		lua_getfield(L, -1, p);
		lua_remove(L, -2);
		if (method)
			lua_getglobal(L, f);
	} else
		lua_getglobal(L, s);
	if (args) {
		while (args->val) {
			switch(args->type) {
			case INSTEAD_NIL:
				lua_pushnil(L);
				break;
			case INSTEAD_NUM:
				lua_pushnumber(L, atoi(args->val));
				break;
			case INSTEAD_BOOL:
				if (!strcmp(args->val, "true"))
					lua_pushboolean(L, 1);
				else
					lua_pushboolean(L, 0);
				break;
			default:
			case INSTEAD_STR:
				lua_pushstring(L, args->val);
			}
			args ++;
			n ++;
		}
	}
	if (debug_sw) {
		base = lua_gettop(L) - (method + n);  /* function index */
		lua_pushcfunction(L, traceback);  /* push traceback function */
		lua_insert(L, base);  /* put it under chunk and args */
	}
	busy ++;
	status = lua_pcall(L, method + n, LUA_MULTRET, base);
	busy --;
	if (debug_sw)
		lua_remove(L, base);  /* remove traceback function */
	if (status) {
		fprintf(stderr, "Error calling:%s\n", s);
		lua_gc(L, LUA_GCCOLLECT, 0);
	}
	return report(L, status);
}

#ifdef _HAVE_ICONV
static char *curcp = NULL;
static char *fromcp = NULL;
#endif

#ifdef _HAVE_ICONV
void instead_set_encoding(const char *cp)
{
	if (curcp)
		free(curcp);
	if (cp)
		curcp = strdup(cp);
	else
		curcp = NULL;
	return;
}
char *instead_fromgame(const char *s)
{
	iconv_t han;
	char *str;
	if (!s)
		return NULL;
	if (!fromcp)
		goto out0;
	han = iconv_open(curcp, fromcp);
	if (han == (iconv_t)-1)
		goto out0;
	if (!(str = decode(han, s)))
		goto out1;
	iconv_close(han);
	return str;
out1:
	iconv_close(han);
out0:
	return strdup(s);
}

char *togame(const char *s)
{
	iconv_t han;
	char *str;
	if (!s)
		return NULL;
	if (!fromcp)
		goto out0;
	han = iconv_open(fromcp, curcp);
	if (han == (iconv_t)-1)
		goto out0;
	if (!(str = decode(han, s)))
		goto out1;
	iconv_close(han);
	return str;
out1:
	iconv_close(han);
out0:
	return strdup(s);
}
#else
char *instead_fromgame(const char *s)
{
	if (!s)
		return NULL;
	return strdup(s);
}
char *togame(const char *s)
{
	if (!s)
		return NULL;
	return strdup(s);
}
void instead_set_encoding(const char *cp)
{
}
#endif

static int instead_getargs (char **argv, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		lua_pushstring(L, argv[i]);
	}

	lua_createtable(L, n, 0);
	for (i = 0; i < n; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_setglobal(L, "arg");
	return 0;
}

int instead_load(char **info)
{
	int rc;
	idff_t idf;

	rc = instead_function("stead:init", NULL); instead_clear();
	if (rc)
		goto err;

	idf = idf_open(data_idf, MAIN);

	if (idf) {
		int rc = dofile_idf(L, idf, MAIN);
		idf_close(idf);
		if (rc)
			goto err;
	} else if (dofile(L, dirpath(MAIN))) {
		goto err;
	}
	instead_clear();
#ifdef _HAVE_ICONV
	if (fromcp)
		free(fromcp);
	fromcp = getstring("return game.codepage;");
	instead_clear();
#endif
	rc = instead_function("game:ini", NULL);
	if (rc)
		goto err2;
	if (info) {
		*info = instead_retval(0);
		*info = instead_fromgame(*info);
	}
	instead_clear();
	return rc;
err2:
	instead_clear();
err:
	return -1;
}

int instead_loadfile(char *name)
{
	return instead_loadscript(name, -1, NULL, 1);
}

int instead_loadscript(char *name, int argc, char **argv, int exec)
{
	int status;
	if (exec && argc >= 0)
		instead_getargs(argv, argc);
	status = luaL_loadfile(L, name);
	if (!status) {
		if (exec) {
			if (argc >= 0)
				lua_insert(L, -(argc + 1));
			else
				argc = 0;
			status |= docall(L, argc);
		}
	}
	status = report(L, status);
	instead_clear();
	return status;
}

typedef struct LoadF {
	int extraline;
	unsigned char byte;
	FILE *f;
	idff_t idff;
	int enc;
	unsigned char buff[4096];
} LoadF;

static const char *getF (lua_State *L, void *ud, size_t *size) {
	unsigned int i = 0;
	LoadF *lf = (LoadF *)ud;
	(void)L;
	if (lf->extraline) {
		lf->extraline = 0;
		*size = 1;
		return "\n";
	}

	if (lf->f && feof(lf->f))
		return NULL;
	if (lf->idff && idf_eof(lf->idff))
		return NULL;

	if (lf->idff)
		*size = idf_read(lf->idff, lf->buff, 1, sizeof(lf->buff));
	else
		*size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);

	if (lf->enc) {
		for (i = 0; i < *size; i ++) {
			unsigned char b = lf->buff[i];
			lf->buff[i] ^= lf->byte;
			lf->buff[i] = (lf->buff[i] >> 3) | (lf->buff[i] << 5);
			lf->byte = b;
		}
	}
	return (*size > 0) ? (char*)lf->buff : NULL;
}

static int errfile (lua_State *L, const char *what, int fnameindex) {
	const char *serr = strerror(errno);
	const char *filename = lua_tostring(L, fnameindex) + 1;
	lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
	lua_remove(L, fnameindex);
	return LUA_ERRFILE;
}

static int loadfile (lua_State *L, const char *filename, int enc) {
	LoadF lf;
	int status, readstatus;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	lf.extraline = 0;
	lua_pushfstring(L, "@%s", filename);
	lf.idff = idf_open(data_idf, filename);
	if (!lf.idff)
		lf.f = fopen(dirpath(filename), "rb");
	else
		lf.f = NULL;
	lf.byte = 0xcc;
	lf.enc = enc;
	if (lf.f == NULL && lf.idff == NULL) return errfile(L, "open", fnameindex);

#if LUA_VERSION_NUM >= 502
	status = lua_load(L, getF, &lf, lua_tostring(L, -1), "bt");
#else
	status = lua_load(L, getF, &lf, lua_tostring(L, -1));
#endif
	if (lf.f)
		readstatus = ferror(lf.f);
	else
		readstatus = idf_error(lf.idff);

	if (lf.f)
		fclose(lf.f);  /* close file (even in case of errors) */
	else
		idf_close(lf.idff);

	if (readstatus) {
		lua_settop(L, fnameindex);  /* ignore results from `lua_load' */
		return errfile(L, "read", fnameindex);
	}
	lua_remove(L, fnameindex);
	return status;
}


static int luaB_doencfile (lua_State *L) {
	const char *fname = luaL_optstring(L, 1, NULL);
	int n = lua_gettop(L);
	if (loadfile(L, fname, 1) != 0) lua_error(L);
	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - n;
}

static int luaB_dofile (lua_State *L) {
	const char *fname = luaL_optstring(L, 1, NULL);
	int n = lua_gettop(L);
	if (loadfile(L, fname, 0) != 0) lua_error(L);
	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - n;
}

#if LUA_VERSION_NUM <= 503
/* is this hack still needed? */
static int luaB_print (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "LUA_QL("print"));
		if (i>1) fputs("\t", stdout);
		fputs(s, stdout);
		lua_pop(L, 1);  /* pop result */
	}
	fputs("\n", stdout);
	return 0;
}
#else
static int luaB_print (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  for (i = 1; i <= n; i++) {  /* for each argument */
    size_t l;
    const char *s = luaL_tolstring(L, i, &l);  /* convert it to string */
    if (i > 1)  /* not the first element? */
      lua_writestring("\t", 1);  /* add a tab before it */
    lua_writestring(s, l);  /* print it */
    lua_pop(L, 1);  /* pop result */
  }
  lua_writeline();
  return 0;
}
#endif

static int luaB_maxn (lua_State *L) {
	lua_Integer max = 0;
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushnil(L);  /* first key */
	while (lua_next(L, 1)) {
		lua_pop(L, 1);  /* remove value */
		if (lua_type(L, -1) == LUA_TNUMBER) {
			lua_Number v = lua_tonumber(L, -1);
			if (v > max) max = v;
		}
	}
	lua_pushinteger(L, max);
	return 1;
}

static int luaB_srandom(lua_State *L) {
	mt_random_seed(luaL_optnumber(L, 1, time(NULL)));
	return 0;
}

static int luaB_random(lua_State *L) {
	lua_Number rt;
	unsigned long r = 0;
	long a = luaL_optnumber(L, 1, -1);
	long b = luaL_optnumber(L, 2, -1);
	r = mt_random();
	if (a >=0 && b > a) {
		r = a + (r % (b - a + 1));
		lua_pushinteger(L, r);
	} else if (a > 0 && b == -1) {
		r = (r % a) + 1;
		lua_pushinteger(L, r);
	} else {
		rt = mt_random_double();
		lua_pushnumber(L, rt);
	}
	return 1;
}

static int luaB_get_realpath(lua_State *L) {
	char realpath[PATH_MAX];
	char outpath[PATH_MAX];
	const char *path = luaL_optstring(L, 1, NULL);
	if (!path)
		return 0;
	strncpy(realpath, path, sizeof(realpath));
	realpath[sizeof(realpath) - 1] = 0;
	unix_path(realpath);
	path = getrealpath(realpath, outpath);
	if (!path)
		return 0;
	lua_pushstring(L, outpath);
	return 1;
}

static int luaB_get_gamepath(lua_State *L) {
	char path[PATH_MAX * 2];
	char *p = getdir(path, sizeof(path));
	if (!p)
		return 0;
	unix_path(p);

	if (idf_only(instead_idf(), -1) == 1) { /* no gamepath */
		strcpy(path, instead_game_path);
	}

	lua_pushstring(L, p);
	return 1;
}

static int luaB_get_steadpath(lua_State *L) {
	char stead_path[PATH_MAX];

	if (STEAD_API_PATH[0] != '/') {
		strcpy(stead_path, instead_cwd());
		strcat(stead_path, "/");
	} else
		stead_path[0] = 0;
	strcat(stead_path, STEAD_API_PATH);
	unix_path(stead_path);
	lua_pushstring(L, stead_path);
	return 1;
}

#define utf_cont(p) ((*(p) & 0xc0) == 0x80)

static int utf_ff(const char *s, const char *e)
{
	int l = 0;
	if (!s || !e)
		return 0;
	if (s > e)
		return 0;
	if ((*s & 0x80) == 0) /* ascii */
		return 1;
	l = 1;
	while (s < e && utf_cont(s + 1)) {
		s ++;
		l ++;
	}
	return l;
}

static int utf_bb(const char *s, const char *e)
{
	int l = 0;
	if (!s || !e)
		return 0;
	if (s > e)
		return 0;
	if ((*e & 0x80) == 0) /* ascii */
		return 1;
	l = 1;
	while (s < e && utf_cont(e)) {
		e --;
		l ++;
	}
	return l;
}

static int luaB_utf_next(lua_State *L) {
	int l = 0;
	const char *s = luaL_optstring(L, 1, NULL);
	int idx = luaL_optnumber(L, 2, 1) - 1;
	if (s && idx >= 0) {
		int len = strlen(s);
		if (idx < len)
			l = utf_ff(s + idx, s + len - 1);
	}
	lua_pushnumber(L, l);
	return 1;
}

static int luaB_utf_prev(lua_State *L) {
	int l = 0;
	const char *s = luaL_optstring(L, 1, NULL);
	int idx = luaL_optnumber(L, 2, 0) - 1;
	int len = 0;
	if (s) {
		len = strlen(s);
		if (idx < 0)
			idx += len;
		if (idx >= 0) {
			if (idx < len)
				l = utf_bb(s, s + idx);
		}
	}
	lua_pushnumber(L, l);
	return 1;
}

static int luaB_utf_char(lua_State *L) {
	int len, l = 0;
	char *rs;
	const char *s = luaL_optstring(L, 1, NULL);
	int idx = luaL_optnumber(L, 2, 1) - 1;
	if (!s || idx < 0)
		return 0;
	len = strlen(s) - 1;
	while (idx >= 0 && len >= 0) {
		s += l;
		l = utf_ff(s, s + len);
		if (l <= 0)
			return 0;
		idx --;
		len -= l;
	}
	rs = malloc(l + 1);
	if (!rs)
		return 0;
	if (l)
		memcpy(rs, s, l);
	rs[l] = 0;
	lua_pushstring(L, rs);
	free(rs);
	return 1;
}

static int luaB_utf_len(lua_State *L) {
	int l = 0;
	int sym = 0;
	const char *s = luaL_optstring(L, 1, NULL);
	if (s) {
		int len = strlen(s) - 1;
		while (len >= 0) {
			l = utf_ff(s, s + len);
			if (!l)
				break;
			s += l;
			len -= l;
			sym ++;
		}
	}
	lua_pushnumber(L, sym);
	return 1;
}

extern int dir_iter_factory (lua_State *L);
extern int luaopen_lfs (lua_State *L);

static const luaL_Reg base_funcs[] = {
	{"print", luaB_print}, /* for some mystic, it is needed in win version (with -debug) */

	{"doencfile", luaB_doencfile},
	{"dofile", luaB_dofile},

	{"table_get_maxn", luaB_maxn},

	{"instead_random", luaB_random},
	{"instead_srandom", luaB_srandom},

	{"instead_realpath", luaB_get_realpath},
	{"instead_gamepath", luaB_get_gamepath},
	{"instead_steadpath", luaB_get_steadpath},

	{"instead_readdir", dir_iter_factory},

	{"utf8_next", luaB_utf_next},
	{"utf8_prev", luaB_utf_prev},
	{"utf8_char", luaB_utf_char},
	{"utf8_len", luaB_utf_len},
	{ NULL, NULL }
};


static int instead_platform(void)
{
	char plat[64];
	if (!L)
		return 0;

#if defined(IOS)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='IOS'");
#elif defined(__APPLE__)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='MACOSX'");
#elif defined(_WIN32_WCE)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='WINCE'");
#elif defined(WINRT)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='WINRT'");
#elif defined(S60)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='S60'");
#elif defined(ANDROID)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='ANDROID'");
#elif defined(_WIN32)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='WIN32'");
#elif defined(MAEMO)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='MAEMO'");
#elif defined(SAILFISHOS)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='SFOS'");
#else
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='UNIX'");
#endif

	plat[sizeof(plat) - 1] = 0;
	instead_eval(plat); instead_clear();
	return 0;
}

static int instead_package(const char *path)
{
	char *stead_path;
	stead_path = malloc(PATH_MAX * 5); /* instead_cwd + STEAD_API_PATH and so on... */
	if (!stead_path)
		return -1;
	strcpy(stead_path, "package.path=\"");
#if defined(_WIN32_WCE) || defined(WINRT)
	if (path) {
		strcat(stead_path, path); /* wince have not cwd :) */
		strcat(stead_path, "/?.lua;");
	}
#else
	if (path)
		strcat(stead_path, "./?.lua;");
#endif

#ifdef INSTEAD_LEGACY
	p = instead_local_stead_path(wd);
	if (p) {
		strcat(stead_path, p);
		strcat(stead_path, "/?.lua;");
	}
#endif

	if (!is_absolute_path(STEAD_API_PATH)) {
		strcat(stead_path, instead_cwd());
		strcat(stead_path, "/");
		strcat(stead_path, STEAD_API_PATH);
	} else {
		strcat(stead_path, STEAD_API_PATH);
	}
	strcat(stead_path, "/?.lua");
	strcat(stead_path, "\"");

	if (standalone_sw) {
		strcat(stead_path, "..';'..(package.path or '')");
	}
	instead_eval(stead_path); instead_clear();
	free(stead_path);
/*	putenv(stead_path); */
	return 0;
}

const char *instead_get_api(void)
{
	return API;
}

const char *instead_lua_path(const char *path)
{
	if (!path)
		return instead_base_path;
	if (!*path) {
		strncpy(instead_base_path, STEAD_PATH, sizeof(instead_base_path) - 1);
		return instead_base_path;
	}
	strncpy(instead_base_path, path, sizeof(instead_base_path) - 1);
	return instead_base_path;
}

static int instead_set_api(const char *api)
{
	int i, c = 0;
	ssize_t s;
	char *oa;
	if (!api || !*api) {
		FREE(API);
		snprintf(instead_api_path, sizeof(instead_api_path), "%s", instead_lua_path(NULL));
	} else {
		s = strlen(api);
		for (i = 0; i < s; i ++) {
			if (api[i] == '.') {
				if (c > 0) {
					instead_err_msg("Wrong API.");
					fprintf(stderr, "Wrong API.\n");
					return -1;
				}
				c ++;
			} else
				c = 0;
		}
		oa = API;
		API = strdup(api);
		FREE(oa);
		snprintf(instead_api_path, sizeof(instead_api_path), "%s/%s", instead_lua_path(NULL), API);
	}
	return 0;
}


static int instead_detect_api(const char *path)
{
	int api = 0;
	char *p;
	if (data_idf && idf_only(data_idf, -1) == 1) {
		if (!idf_access(data_idf, INSTEAD_MAIN3))
			api = 3;
		else if (!idf_access(data_idf, INSTEAD_MAIN))
			api = 2;
	} else {
		p = getfilepath(path, INSTEAD_MAIN3);
		if (!p)
			return -1;
		if (!access(dirpath(p), R_OK))
			api = 3;
		free(p);
		if (api)
			goto out;
		p = getfilepath(path, INSTEAD_MAIN);
		if (!access(dirpath(p), R_OK))
			api = 2;
		free(p);
	}
out:
	switch (api){
	case 2:
		if (instead_set_api("stead2") < 0)
			return -1;
		MAIN = INSTEAD_MAIN;
		break;
	case 3:
		if (instead_set_api("stead3") < 0)
			return -1;
		MAIN = INSTEAD_MAIN3;
		break;
	default:
		return -1;
	}
	return api;
}

int instead_init_lua(const char *path, int detect)
{
	int api = 0;
	busy = 0;
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC, "C"); /* to avoid . -> , in numbers */
	setlocale(LC_CTYPE, "C"); /* to avoid lower/upper problems */
#ifdef LC_MESSAGES
	setlocale(LC_MESSAGES, "C");
#endif
#ifdef LC_COLLATE
	setlocale(LC_COLLATE, "C");
#endif
/*	strcpy(curcp, "UTF-8"); */
	instead_set_encoding("UTF-8");
	getdir(instead_cwd_path, sizeof(instead_cwd_path));
	unix_path(instead_cwd_path);
	instead_cwd_path[sizeof(instead_cwd_path) - 1] = 0;
	strncpy(instead_game_path, path, sizeof(instead_game_path) - 1);
	instead_cwd_path[sizeof(instead_game_path) - 1] = 0;

	if (detect && (api = instead_detect_api(path)) < 0) {
		fprintf(stderr, "Can not detect game format: %s\n", path);
		instead_err_msg("Can not detect game format.");
		return -1;
	}

	/* initialize Lua */
#if LUA_VERSION_NUM >= 502
	L = luaL_newstate();
#else
	L = lua_open();
#endif
	if (!L)
		return -1;

	luaL_openlibs(L);
#if LUA_VERSION_NUM >= 502
	lua_pushglobaltable(L);
	lua_pushglobaltable(L);
	lua_setfield(L, -2, "_G");
	/* open lib into global table */
	luaL_setfuncs(L, base_funcs, 0);
#else
	luaL_register(L, "_G", base_funcs);
#endif
	instead_package(path);
	instead_platform();
/*	instead_set_lang(opt_lang); */
	if (api == 3)
		instead_eval("API='stead3'");
	else if (api == 2)
		instead_eval("API='stead2'");
	if (debug_sw)
		instead_eval("DEBUG=true");
	else
		instead_eval("DEBUG=false");
	instead_clear();
	if (standalone_sw)
		instead_eval("STANDALONE=true");
	else
		instead_eval("STANDALONE=false");
	instead_clear();
	srand(time(NULL));
	mt_random_init();
	luaopen_lfs(L);
	return 0;
}

int instead_init(const char *path)
{
	char stead_path[PATH_MAX];
	int idf = 0;

	if (data_idf)
		idf_done(data_idf);

	data_idf = idf_init(path);

	if (data_idf) {
		idf_only(data_idf, 1);
		idf = 1;
	}

	if (instead_init_lua(path, 1))
		goto err;

	if (snprintf(stead_path, sizeof(stead_path), "%s/stead.lua", STEAD_API_PATH) >=
		(int)sizeof(stead_path))
		fprintf(stderr, "Path is too long.\n");

	if (dofile(L, dirpath(stead_path)))
		goto err;

	if (extensions_hook(init) < 0) {
		fprintf(stderr, "Can't init instead engine.\n");
		goto err;
	}
#ifdef _USE_SDL
#ifndef __EMSCRIPTEN__
	sem = SDL_CreateMutex();
	if (!sem)
		goto err;
#endif
#endif

	if ((!idf && setdir(path))) {
		instead_clear();
		goto err;
	}

	if (!idf)
		data_idf = idf_init(DATA_IDF);

	/* cleanup Lua */
	instead_clear();
	instead_err_msg(NULL);
	return 0;
err:
	if (data_idf) {
		idf_done(data_idf);
		data_idf = NULL;
	}
	if (L)
		lua_close(L);
	L = NULL;
	return -1;
}

int instead_api_register(const luaL_Reg *api)
{
	if (!L)
		return -1;
#if LUA_VERSION_NUM >= 502
	lua_pushglobaltable(L);
	luaL_setfuncs(L, api, 0);
#else
	lua_getfield(L, LUA_GLOBALSINDEX, "_G");
	luaL_register(L, NULL, api);
#endif
	lua_pop(L, 1);
	return 0;
}

void instead_done(void)
{
	int wasL = !!L;
	if (wasL)
		extensions_hook(done);
#ifdef _USE_SDL
#ifndef __EMSCRIPTEN__
	if (sem)
		SDL_DestroyMutex(sem);
	sem = NULL;
#endif
#endif
#ifdef _HAVE_ICONV
	FREE(fromcp);
	FREE(curcp);
#endif
	if (L)
		lua_close(L);
	L = NULL;
	if (data_idf)
		idf_done(data_idf);
	data_idf = NULL;
	if (wasL)
		setdir(instead_cwd_path);
	FREE(API);
}

int  instead_encode(const char *s, const char *d)
{
	FILE *src;
	FILE *dst;
	size_t size;
	unsigned int i = 0;
	unsigned char byte = 0xcc;
	unsigned char buff[4096];

	src = fopen(s, "rb");
	if (!src) {
		fprintf(stderr,"Can't open on read: '%s'.\n", s);
		return -1;
	}
	dst = fopen(d, "wb");
	if (!dst) {
		fprintf(stderr,"Can't open on write: '%s'.\n", s);
		fclose(src);
		return -1;
	}
	while ((size = fread(buff, 1, sizeof(buff), src))) {
		for (i = 0; i < size; i++) {
			buff[i] = (buff[i] << 3) | (buff[i] >> 5);
			buff[i] ^= byte;
			byte = buff[i];
		}
		if (fwrite(buff, 1, size, dst) != size) {
			fprintf(stderr, "Error while writing file: '%s'.\n", d);
			fclose(src);
			fclose(dst);
			return -1;
		}
	}
	fclose(src);
	fclose(dst);
	return 0;
}

idf_t  instead_idf(void)
{
	return data_idf;
}

char *instead_stead_path(void)
{
	return instead_api_path;
}

char *instead_path(void)
{
	return instead_game_path;
}

char *instead_cwd(void)
{
	return instead_cwd_path;
}

int instead_set_debug(int sw)
{
	int ov = debug_sw;
	debug_sw = sw;
	if (L) {
		if (sw)
			instead_eval("DEBUG=true");
		else
			instead_eval("DEBUG=false");
		instead_clear();
	}
	return ov;
}

int instead_set_standalone(int sw)
{
	int ov = standalone_sw;
	standalone_sw = sw;
	if (L) {
		if (sw)
			instead_eval("STANDALONE=true");
		else
			instead_eval("STANDALONE=false");
		instead_clear();
	}
	return ov;
}

int instead_set_lang(const char *opt_lang)
{
	char lang[64];
	if (!L)
		return 0;
	if (opt_lang && *opt_lang)
		snprintf(lang, sizeof(lang) - 1, "LANG='%s'", opt_lang);
	else
		snprintf(lang, sizeof(lang) - 1, "LANG='en'");
	instead_eval(lang); instead_clear();
	return 0;
}

lua_State *instead_lua(void)
{
	return L;
}
