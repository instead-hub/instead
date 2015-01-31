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

#include "externals.h"
#include "internals.h"
#include "list.h"
#include "idf.h"

#ifndef STEAD_PATH
#define STEAD_PATH 	"./stead"
#endif

static int busy = 0;

/* the Lua interpreter */
static gtimer_t instead_timer = NULL_TIMER;
static int instead_timer_nr = 0;

char 		*fromgame(const char *s);
char 		*togame(const char *s);
lua_State	*L = NULL;

int instead_busy(void)
{
	return busy;
}

static int report (lua_State *L, int status) 
{
	if (status && !lua_isnil(L, -1)) {
		char *p;
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) 
			msg = "(error object is not a string)";
		fprintf(stderr,"Error: %s\n", msg);
		p = fromgame(msg);
		game_err_msg(p?p:msg);
		if (p)
			free(p);
		lua_pop(L, 1);	
		status = -1;
		gfx_del_timer(instead_timer); /* to avoid error loops */
		instead_timer = NULL_TIMER;
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
		s = fromgame(s);
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
		s = fromgame(s);
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

char *instead_file_cmd(char *s)
{
	struct instead_args args[] = {
		{ .val = NULL, .type = INSTEAD_STR },
		{ .val = NULL, },
	};
	if (!s)
		return NULL;
	args[0].val = s;
	instead_function("iface:cmd", args);
	return instead_retval(0);
}

char *instead_cmd(char *s)
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
	return instead_retval(0);
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

int luacall(char *cmd)
{
	int rc;
	if (!L)
		return -1;
	if (dostring(L, cmd)) {
		return -1;
	}
	rc = lua_tonumber(L, -1);
	return rc;
}

#ifdef _HAVE_ICONV
static char *curcp = "UTF-8";
static char *fromcp = NULL;
#endif

#ifdef _HAVE_ICONV
char *fromgame(const char *s)
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
char *fromgame(const char *s) 
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

int instead_load(char *game)
{
	idff_t idf = idf_open(game_idf, game);
	if (idf) {
		int rc = dofile_idf(L, idf, game);
		idf_close(idf);
		if (rc)
			return -1;
	} else if (dofile(L, dirpath(game))) {
		return -1;
	}
	instead_clear();
#ifdef _HAVE_ICONV
	if (fromcp)
		free(fromcp);
	fromcp = getstring("return game.codepage;");
	instead_clear();
#endif
	return 0;
}

int instead_loadscript(char *name, int argc, char **argv, int exec)
{
	int status;
	if (exec)
		instead_getargs(argv, argc);
	status = luaL_loadfile(L, name);
	if (!status) {
		if (exec) {
			lua_insert(L, -(argc + 1));
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
	lf.idff = idf_open(game_idf, filename);
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

static int luaB_is_sound(lua_State *L) {
	const char *chan = luaL_optstring(L, 1, NULL);
	int c, r;
	if (!chan)
		c = -1;
	else
		c = atoi(chan);
	r = (sound_channel(c) != NULL);
	lua_pushboolean(L, r);  /* else not a number */
	return 1;
}

static int luaB_get_savepath(lua_State *L) {
	lua_pushstring(L, dirname(game_save_path(1, 0)));
	return 1;
}

static int luaB_get_gamepath(lua_State *L) {
	char path[PATH_MAX * 2];
	char *p = getdir(path, sizeof(path));
	if (!p)
		return 0;
	unix_path(p);
	if (game_idf) {
		strcat(p, "/");
		strcat(p, curgame_dir);
	}
	lua_pushstring(L, p);
	return 1;
}

static int luaB_get_steadpath(lua_State *L) {
	char stead_path[PATH_MAX];

	if (STEAD_PATH[0] != '/') {
		strcpy(stead_path, game_cwd);
		strcat(stead_path, "/");
	} else
		stead_path[0] = 0;
	strcat(stead_path, STEAD_PATH);
	unix_path(stead_path);
	lua_pushstring(L, stead_path);
	return 1;
}

static int luaB_get_themespath(lua_State *L) {
	char themes_path[PATH_MAX];

	if (THEMES_PATH[0] != '/') {
		strcpy(themes_path, game_cwd);
		strcat(themes_path, "/");
	} else
		themes_path[0] = 0;
	strcat(themes_path, THEMES_PATH);
	unix_path(themes_path);
	lua_pushstring(L, themes_path);
	return 1;
}

static int luaB_get_gamespath(lua_State *L) {
	char games_path[PATH_MAX];
	if (GAMES_PATH[0] != '/') {
		strcpy(games_path, game_cwd);
		strcat(games_path, "/");
	} else
		games_path[0] = 0;
	strcat(games_path, GAMES_PATH);
	unix_path(games_path);
	lua_pushstring(L, games_path);
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

static void instead_timer_do(void *data)
{
	char *p;
	if (game_paused() || !curgame_dir)
		goto out;
	if (instead_function("stead.timer", NULL)) {
		instead_clear();
		goto out;
	}
	p = instead_retval(0); instead_clear();
	if (!p)
		goto out;

	game_cmd(p, 0); free(p);

	game_cursor(CURSOR_ON);
out:
	instead_timer_nr = 0;
}

static int instead_fn(int interval, void *p)
{
	if (instead_timer_nr)
		return interval; /* framedrop */
	instead_timer_nr ++;
	push_user_event(instead_timer_do, NULL);
	return interval;
}

static int luaB_set_timer(lua_State *L) {
	const char *delay = luaL_optstring(L, 1, NULL);
	int d;
	gfx_del_timer(instead_timer);
	instead_timer = NULL_TIMER;
	if (!delay)
		d = 0;
	else	
		d = atoi(delay);
	if (!d)
		return 0;
	instead_timer_nr = 0;
	instead_timer = gfx_add_timer(d, instead_fn, NULL);
	return 0;
}

extern int theme_setvar(char *name, char *val);
extern char *theme_getvar(const char *name);

static int luaB_theme_var(lua_State *L) {
	const char *var = luaL_optstring(L, 1, NULL);
	const char *val = luaL_optstring(L, 2, NULL);
	if (var && !val) { /* get */
		char *p = theme_getvar(var);
		if (p) {
			lua_pushstring(L, p);
			free(p);
			return 1;
		}
		return 0;
	}
	if (!val || !var)
		return 0;
	game_own_theme = 1;
	if (!opt_owntheme)
		return 0;
	if (!strcmp(var, "scr.w") ||
		!strcmp(var, "scr.h")) /* filter resolution */
		return 0;
	if (!theme_setvar((char*)var, (char*)val)) {
		if (strcmp(var, "win.scroll.mode")) /* let change scroll mode w/o theme reload */
			game_theme_changed = 2;
	}
	return 0;
}

static int luaB_theme_name(lua_State *L) {
	if (game_own_theme && opt_owntheme)
		lua_pushstring(L, ".");
	else
		lua_pushstring(L, curtheme_dir);
	return 1;
}

extern int dir_iter_factory (lua_State *L);
extern int luaopen_lfs (lua_State *L);

static LIST_HEAD(sprites);

static LIST_HEAD(fonts);

typedef struct {
	struct list_node list;
	char	*name;
	fnt_t	fnt;
} _fnt_t;

typedef struct {
	struct list_node list;
	char	*name;
	img_t	img;
} _spr_t;

static void sprites_free(void)
{
/*	fprintf(stderr, "sprites free \n"); */
	while (!list_empty(&sprites)) {
		_spr_t *sp = list_top(&sprites, _spr_t, list);
		free(sp->name);
		cache_forget(gfx_image_cache(), sp->img);
		list_del(&sp->list);
		free(sp);
	}
	while (!list_empty(&fonts)) {
		_fnt_t *fn = list_top(&fonts, _fnt_t, list);
		fnt_free(fn->fnt);
		free(fn->name);
		list_del(&fn->list);
		free(fn);
	}
	game_pict_modify(NULL);
	cache_shrink(gfx_image_cache());
}

static _spr_t *sprite_lookup(const char *name)
{
	_spr_t *pos = NULL;
	_spr_t *sp;
	list_for_each(&sprites, pos, list) {
		sp = (_spr_t*)pos;
		if (!strcmp(name, sp->name)) {
			list_del(&sp->list);
			list_add(&sprites, &sp->list); /* move it on head */
			return sp;
		}
	}
	return NULL;
}

static _fnt_t *font_lookup(const char *name)
{
	_fnt_t *pos = NULL;
	_fnt_t *fn;
	list_for_each(&fonts, pos, list) {
		fn = (_fnt_t*)pos;
		if (!strcmp(name, fn->name)) {
			list_del(&fn->list);
			list_add(&fonts, &fn->list); /* move it on head */
			return fn;
		}
	}
	return NULL;
}

static _spr_t *sprite_new(const char *name, img_t img)
{
	_spr_t *sp;
	sp = malloc(sizeof(_spr_t));
	if (!sp)
		return NULL;
/*	INIT_LIST_HEAD(&sp->list); */
	sp->name = strdup(name);
	if (!sp->name) {
		free(sp);
		return NULL;
	}
	sp->img = img;
	if (cache_add(gfx_image_cache(), name, img)) {
		free(sp->name);
		free(sp);
		return NULL;
	}
/*	fprintf(stderr, "added: %s\n", name); */
	list_add(&sprites, &sp->list);
	return sp;
}

static _fnt_t *font_new(const char *name, fnt_t fnt)
{
	_fnt_t *fn;
	fn = malloc(sizeof(_fnt_t));
	if (!fn)
		return NULL;
/*	INIT_LIST_HEAD(&fn->list); */
	fn->name = strdup(name);
	if (!fn->name) {
		free(fn);
		return NULL;
	}
	fn->fnt = fnt;
	list_add(&fonts, &fn->list);
	return fn;
}

static void sprite_name(const char *name, char *sname, int size)
{
	unsigned long h = 0;
	if (!sname || !size)
		return;
	h = hash_string(name);
	do { /* new uniq name */
		snprintf(sname, size, "spr:%lx", h);
		h ++;
	} while (sprite_lookup(sname) || cache_lookup(gfx_image_cache(), sname));
	sname[size - 1] = 0;
}

static void font_name(const char *name, char *sname, int size)
{
	unsigned long h = 0;
	if (!sname || !size)
		return;
	h = hash_string(name);
	do { /* new uniq name */
		snprintf(sname, size, "fnt:%lx", h);
		h ++;
	} while (font_lookup(sname));
	sname[size - 1] = 0;
}

static int _free_sprite(const char *key);

static int luaB_free_sprites(lua_State *L) {
	sprites_free();
	return 0;
}

static int luaB_load_sprite(lua_State *L) {
	img_t img = NULL;
	_spr_t *sp;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *fname = luaL_optstring(L, 1, NULL);
	const char *desc = luaL_optstring(L, 2, NULL);

	if (!fname)
		return 0;

	img = gfx_load_image((char*)fname);
	if (img)
		img = gfx_display_alpha(img); /*speed up */
	if (img)
		theme_img_scale(&img);

	if (!img)
		goto err;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(fname, sname, sizeof(sname));
	} else
		key = desc;
	sp = sprite_new(key, img);
	if (!sp)
		goto err;

	lua_pushstring(L, key);
	return 1;
err:
	gfx_free_image(img);
	return 0;
}

static int luaB_load_font(lua_State *L) {
	fnt_t fnt = NULL;
	_fnt_t *fn;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];
	struct game_theme *t = &game_theme;

	const char *fname = luaL_optstring(L, 1, NULL);
	int sz = luaL_optnumber(L, 2, t->font_size) * game_theme.scale;
	const char *desc = luaL_optstring(L, 3, NULL);
	if (!fname)
		return 0;

	fnt = fnt_load((char*)fname, sz);

	if (!fnt)
		return 0;

	if (!desc || font_lookup(desc)) {
		key = sname;
		font_name(fname, sname, sizeof(sname));
	} else
		key = desc;

	fn = font_new(key, fnt);
	if (!fn)
		goto err;

	lua_pushstring(L, key);
	return 1;
err:
	fnt_free(fnt);
	return 0;
}

static int luaB_text_size(lua_State *L) {
	_fnt_t *fn;
	int w = 0, h = 0;

	const char *font = luaL_optstring(L, 1, NULL);
	const char *text = luaL_optstring(L, 2, NULL);

	if (!font)
		return 0;

	fn = font_lookup(font);
	
	if (!fn)
		return 0;
	if (!text) {
		w = 0;
		h = ceil((float)fnt_height(fn->fnt) / game_theme.scale);
	} else {
		txt_size(fn->fnt, text, &w, &h);
		w = ceil((float)w / game_theme.scale);
		h = ceil((float)h / game_theme.scale);
	}
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

static int luaB_font_size_scaled(lua_State *L) {
	int sz = luaL_optnumber(L, 1, game_theme.font_size);
	lua_pushnumber(L, FONT_SZ(sz));
	return 1;
}

static int luaB_text_sprite(lua_State *L) {
	img_t img = NULL;
	_spr_t *sp;
	_fnt_t *fn;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *font = luaL_optstring(L, 1, NULL);
	const char *text = luaL_optstring(L, 2, NULL);
	char txtkey[32];
	const char *color = luaL_optstring(L, 3, NULL);
	int style = luaL_optnumber(L, 4, 0);
	const char *desc = luaL_optstring(L, 5, NULL);

	color_t col = { .r = game_theme.fgcol.r, .g = game_theme.fgcol.g, .b = game_theme.fgcol.b };

	if (!font)
		return 0;
	if (color)
		gfx_parse_color (color, &col);
	if (!text)
		text = "";

	fn = font_lookup(font);

	if (!fn)
		return 0;

	fnt_style(fn->fnt, style);

	img = fnt_render(fn->fnt, text, col);
	if (img)
		img = gfx_display_alpha(img); /*speed up */

	if (!img)
		return 0;
	
	if (!desc || sprite_lookup(desc)) {
		key = sname;
		strncpy(txtkey, text, sizeof(txtkey));
		txtkey[sizeof(txtkey) - 1] = 0;
		sprite_name(txtkey, sname, sizeof(sname));
	} else
		key = desc;
	
	sp = sprite_new(key, img);

	if (!sp)
		goto err;

	lua_pushstring(L, key);
	return 1;
err:
	gfx_free_image(img);
	return 0;
}

static img_t grab_sprite(const char *dst, int *xoff, int *yoff)
{
	img_t d;
	if (DIRECT_MODE && !strcmp(dst, "screen")) {
		d = gfx_screen(NULL);
		*xoff = game_theme.xoff;
		*yoff = game_theme.yoff;
	} else {
		*xoff = 0;
		*yoff = 0;
		d = cache_lookup(gfx_image_cache(), dst);
	}
	return d;
}


static int luaB_sprite_size(lua_State *L) {
	img_t s = NULL;
	float v;
	int w, h;
	int xoff, yoff;
	const char *src = luaL_optstring(L, 1, NULL);
	if (!src)
		return 0;
	s = grab_sprite(src, &xoff, &yoff);
	if (!s)
		return 0;
	
	v = game_theme.scale;

	w = ceil ((float)(gfx_img_w(s) - xoff * 2) / v);
	h = ceil ((float)(gfx_img_h(s) - yoff * 2) / v);

	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

#define BLIT_COPY 0
#define BLIT_DRAW 1
#define BLIT_COMPOSE 2

static int luaB_blit_sprite(lua_State *L, int mode) {
	img_t s, d;
	img_t img2 = NULL;
	float v;
	const char *src = luaL_optstring(L, 1, NULL);
	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	int w = luaL_optnumber(L, 4, -1);
	int h = luaL_optnumber(L, 5, -1);
	const char *dst = luaL_optstring(L, 6, NULL);
	int xx = luaL_optnumber(L, 7, 0);
	int yy = luaL_optnumber(L, 8, 0);
	int alpha = luaL_optnumber(L, 9, 255);
	int xoff = 0, yoff = 0;
	int xoff0 = 0, yoff0 = 0;
	if (!src || !dst)
		return 0;

	s = grab_sprite(src, &xoff0, &yoff0);

	d = grab_sprite(dst, &xoff, &yoff);	

	if (!s || !d)
		return 0;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
		if (w != -1)
			w = ceil(w * v);
		if (h != -1)
			h = ceil(h * v);
		xx *= v;
		yy *= v;
	}

	if (w == -1)
		w = gfx_img_w(s) - 2 * xoff0;
	if (h == -1)
		h = gfx_img_h(s) - 2 * yoff0;

	game_pict_modify(d);

	if (alpha != 255) {
		img2 = gfx_alpha_img(s, alpha);
		if (img2)
			s = img2;
	}
	gfx_clip(game_theme.xoff, game_theme.yoff, game_theme.w - 2*game_theme.xoff, game_theme.h - 2*game_theme.yoff);

	switch (mode) {
	case BLIT_DRAW:
		gfx_draw_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	case BLIT_COPY:
		gfx_copy_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	case BLIT_COMPOSE:
		gfx_compose_from(s, x + xoff0, y + yoff0, w, h, d, xx + xoff, yy + yoff);
		break;
	default:
		break;
	}

	gfx_noclip();
	gfx_free_image(img2);
	lua_pushboolean(L, 1);
	return 1;
}


static int luaB_draw_sprite(lua_State *L) 
{
	return luaB_blit_sprite(L, BLIT_DRAW);
}

static int luaB_copy_sprite(lua_State *L) 
{
	return luaB_blit_sprite(L, BLIT_COPY);
}

static int luaB_compose_sprite(lua_State *L) 
{
	return luaB_blit_sprite(L, BLIT_COMPOSE);
}

static int luaB_alpha_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	int alpha = luaL_optnumber(L, 2, 255);
	const char *desc = luaL_optstring(L, 3, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;
	
	img2 = gfx_alpha_img(s, alpha);
	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_colorkey_sprite(lua_State *L) {
	img_t s;
	color_t  col;

	const char *src = luaL_optstring(L, 1, NULL);
	const char *color = luaL_optstring(L, 2, NULL);

	if (color)
		gfx_parse_color(color, &col);

	if (!src)
		return 0;
	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;
	if (color)
		gfx_set_colorkey(s, col);
	else 
		gfx_unset_colorkey(s);
	return 0;
}

static int luaB_dup_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];
	const char *src = luaL_optstring(L, 1, NULL);
	const char *desc = luaL_optstring(L, 2, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;

	img2 = gfx_alpha_img(s, 255);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_scale_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	float xs = luaL_optnumber(L, 2, 0);
	float ys = luaL_optnumber(L, 3, 0);
	int smooth = lua_toboolean(L, 4);
	const char *desc = luaL_optstring(L, 5, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;

	if (xs == 0)
		xs = 1.0f;

	if (ys == 0)
		ys = xs;

	img2 = gfx_scale(s, xs, ys, smooth);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}


static int luaB_rotate_sprite(lua_State *L) {
	_spr_t *sp;
	img_t s;
	img_t img2 = NULL;
	const char *key;
	char sname[sizeof(unsigned long) * 2 + 16];

	const char *src = luaL_optstring(L, 1, NULL);
	float angle = luaL_optnumber(L, 2, 1.0f);
	int smooth = lua_toboolean(L, 3);
	const char *desc = luaL_optstring(L, 4, NULL);

	if (!src)
		return 0;

	s = cache_lookup(gfx_image_cache(), src);
	if (!s)
		return 0;
	img2 = gfx_rotate(s, angle, smooth);

	if (!img2)
		return 0;

	if (!desc || sprite_lookup(desc)) {
		key = sname;
		sprite_name(src, sname, sizeof(sname));
	} else
		key = desc;

	sp = sprite_new(key, img2);
	if (!sp)
		goto err;
	lua_pushstring(L, sname);
	return 1;
err:
	gfx_free_image(img2);
	return 0;
}

static int luaB_fill_sprite(lua_State *L) {
	img_t d;
	float v;
	const char *dst = luaL_optstring(L, 1, NULL);
	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	int w = luaL_optnumber(L, 4, -1);
	int h = luaL_optnumber(L, 5, -1);
	const char *color = luaL_optstring(L, 6, NULL);
	int xoff = 0, yoff = 0;
	color_t  col = { .r = game_theme.bgcol.r, .g = game_theme.bgcol.g, .b = game_theme.bgcol.b };
	if (!dst)
		return 0;

	d = grab_sprite(dst, &xoff, &yoff);

	if (color)
		gfx_parse_color(color, &col);

	if (!d)
		return 0;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
		if (w != -1)
			w = ceil(w * v);
		if (h != -1)
			h = ceil(h * v);
	}
	if (w == -1)
		w = gfx_img_w(d) - 2 * xoff;
	if (h == -1)
		h = gfx_img_h(d) - 2 * yoff;
	game_pict_modify(d);
	gfx_clip(game_theme.xoff, game_theme.yoff, game_theme.w - 2*game_theme.xoff, game_theme.h - 2*game_theme.yoff);
	gfx_img_fill(d, x + xoff, y + yoff, w, h, col);
	gfx_noclip();
	lua_pushboolean(L, 1);
	return 1;
}

static int luaB_pixel_sprite(lua_State *L) {
	img_t d;
	float v;
	int rc, w, h;
	color_t  col = { .r = game_theme.bgcol.r, .g = game_theme.bgcol.g, .b = game_theme.bgcol.b, .a = 255 };
	const char *dst = luaL_optstring(L, 1, NULL);
	int x = luaL_optnumber(L, 2, 0);
	int y = luaL_optnumber(L, 3, 0);
	const char *color = luaL_optstring(L, 4, NULL);
	int alpha = luaL_optnumber(L, 5, 255);
	int xoff = 0, yoff = 0;

	if (!dst)
		return 0;

	d = grab_sprite(dst, &xoff, &yoff);

	if (color)
		gfx_parse_color(color, &col);

	if (!d)
		return 0;

	w = gfx_img_w(d) - 2 * xoff;
	h = gfx_img_h(d) - 2 * yoff;

	v = game_theme.scale;

	if (v != 1.0f) {
		x *= v;
		y *= v;
	}

	if (color) {
		if (x < 0 || y < 0 || x >= w || y >= h)
			return 0;
		game_pict_modify(d);
		col.a = alpha;
		rc = gfx_set_pixel(d, x + xoff, y + yoff, col);
	} else {
		rc = gfx_get_pixel(d, x + xoff, y + yoff, &col);
	}

	if (rc)
		return 0;

	lua_pushnumber(L, col.r);
	lua_pushnumber(L, col.g);
	lua_pushnumber(L, col.b);
	lua_pushnumber(L, col.a);
	return 4;
}

static int _free_sprite(const char *key)
{
	_spr_t *sp;
	if (!key)
		return -1;
	sp = sprite_lookup(key);

	if (!sp)
		return -1;

	cache_forget(gfx_image_cache(), sp->img);
	cache_shrink(gfx_image_cache());

	list_del(&sp->list);
	free(sp->name); free(sp);
	return 0;
}

static int luaB_free_sprite(lua_State *L) {
	const char *key = luaL_optstring(L, 1, NULL);
	if (_free_sprite(key))
		return 0;
	lua_pushboolean(L, 1);
	return 1;
}

static int luaB_show_menu(lua_State *L) {
	const char *menu = luaL_optstring(L, 1, NULL);
	if (menu) {
		if (!strcmp(menu, "save"))
			menu_toggle(menu_save);
		else if (!strcmp(menu, "load"))
			menu_toggle(menu_load);
		else if (!strcmp(menu, "quit") || !strcmp(menu, "exit"))
			menu_toggle(menu_askquit);
		else
			menu_toggle(-1);
		return 0;
	}
	menu_toggle(menu_main);
	return 0;
}

static int luaB_stead_busy(lua_State *L) {
	static unsigned long busy_time = 0;
	int busy = lua_toboolean(L, 1);
	if (busy) {
		struct inp_event ev;
		memset(&ev, 0, sizeof(ev));
		while (input(&ev, 0) == AGAIN);
		if (ev.type == MOUSE_MOTION)
			game_cursor(CURSOR_ON); /* to make all happy */
		if (!busy_time)
			busy_time = gfx_ticks();
		if (gfx_ticks() - busy_time >= 750 && menu_visible() != menu_wait)
			game_menu(menu_wait);
		game_gfx_commit(0);
		return 0;
	}
	if (menu_visible() == menu_wait) {
		menu_toggle(-1);
	}
	busy_time = 0;
	return 0;
}

static int luaB_free_font(lua_State *L) {
	const char *key = luaL_optstring(L, 1, NULL);
	_fnt_t *fn;
	if (!key)
		return 0;

	fn = font_lookup(key);
	if (!fn)
		return 0;

	list_del(&fn->list);
	free(fn->name); free(fn);
	lua_pushboolean(L, 1);
	return 1;
}

static int luaB_load_sound(lua_State *L) {
	int rc;
	const char *fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	rc = sound_load(fname);
	if (rc)
		return 0;
	lua_pushstring(L, fname);
	return 1;
}


static int luaB_free_sound(lua_State *L) {
	const char *fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	sound_unload(fname);
	return 0;
}

static int luaB_free_sounds(lua_State *L) {
	sounds_free();
	return 0;
}

static int luaB_panning_sound(lua_State *L) {
	int chan = luaL_optnumber(L, 1, -1);
	int left = luaL_optnumber(L, 2, 255);
	int right = luaL_optnumber(L, 3, 255);
	snd_panning(chan, left, right);
	return 0;
}

static int luaB_volume_sound(lua_State *L) {
	int vol = luaL_optnumber(L, 1, -1);
	vol = snd_volume_mus(vol);
	lua_pushnumber(L, vol);
	return 1;
}

static int luaB_channel_sound(lua_State *L) {
	const char *s;
	int ch = luaL_optnumber(L, 1, 0);
	ch = ch % SND_CHANNELS;
	s = sound_channel(ch);
	if (s) {
		lua_pushstring(L, s);
		return 1;
	}
	return 0;
}

static int luaB_mouse_pos(lua_State *L) {
	int x = luaL_optnumber(L, 1, -1);
	int y = luaL_optnumber(L, 2, -1);
	int m;
	float v = game_theme.scale;
	if (x != -1 && y != -1) {
		x *= v;
		y *= v;
		gfx_warp_cursor(x + game_theme.xoff, y + game_theme.yoff);
		x = -1;
		y = -1;
	}
	m = gfx_cursor(&x, &y);
	x = (x - game_theme.xoff) / v;
	y = (y - game_theme.yoff) / v;
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, m);
	return 3;
}

static int luaB_finger_pos(lua_State *L) {
	int x, y;
	float pressure;
	float v = game_theme.scale;
	const char *finger = luaL_optstring(L, 1, NULL);
	if (!finger)
		return 0;
	if (finger_pos(finger, &x, &y, &pressure)) /* no finger */
		return 0;
	x = (x - game_theme.xoff) / v;
	y = (y - game_theme.yoff) / v;
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, pressure);
	return 3;
}

extern int mouse_filter_delay;

static int luaB_mouse_filter(lua_State *L) {
	int d = luaL_optnumber(L, 1, -1);
	int ov = mouse_filter_delay;
	if (d != -1)
		mouse_filter_delay = d;
	lua_pushnumber(L, ov);
	return 1;
}

static int luaB_get_ticks(lua_State *L) {
	lua_pushnumber(L, gfx_ticks());
	return 1;
}

static int luaB_bit_and(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a & b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_or(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a | b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_xor(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a ^ b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_shl(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a << b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_shr(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a >> b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_not(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int r = ~a;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_div(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 1);
	unsigned int r;
	if (b) {
		r = a / b;
		lua_pushnumber(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_idiv(lua_State *L) {
	int a = luaL_optnumber(L, 1, 0);
	int b = luaL_optnumber(L, 2, 1);
	int r;
	if (b) {
		r = a / b;
		lua_pushnumber(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_mod(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 1);
	unsigned int r;
	if (b) {
		r = a % b;
		lua_pushnumber(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_mul(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a * b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_imul(lua_State *L) {
	int a = luaL_optnumber(L, 1, 0);
	int b = luaL_optnumber(L, 2, 0);
	int r = a * b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_sub(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a - b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_add(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	unsigned int b = luaL_optnumber(L, 2, 0);
	unsigned int r = a + b;
	lua_pushnumber(L, r);
	return 1;
}

static int luaB_bit_unsigned(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	lua_pushnumber(L, a);
	return 1;
}

static int luaB_bit_signed(lua_State *L) {
	unsigned int a = luaL_optnumber(L, 1, 0);
	lua_pushnumber(L, (int)a);
	return 1;
}

static int luaB_maxn (lua_State *L) {
	lua_Number max = 0;
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushnil(L);  /* first key */
	while (lua_next(L, 1)) {
		lua_pop(L, 1);  /* remove value */
		if (lua_type(L, -1) == LUA_TNUMBER) {
			lua_Number v = lua_tonumber(L, -1);
			if (v > max) max = v;
		}
	}
	lua_pushnumber(L, max);
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
	if (a >=0 && b >= a) {
		rt = a + (r % (b - a + 1));
	} else if (a >=0 && b == -1) {
		rt = (r % a) + 1;
	} else {
		rt = mt_random_double();
	}
	lua_pushnumber(L, rt);
	return 1;
}

static const luaL_Reg base_funcs[] = {
	{"doencfile", luaB_doencfile},
	{"dofile", luaB_dofile},
	{"print", luaB_print}, /* for some mystic, it is needed in win version (with -debug) */
	{"table_get_maxn", luaB_maxn},
	{"instead_random", luaB_random},
	{"instead_srandom", luaB_srandom},

	{"instead_readdir", dir_iter_factory},
	{"instead_sound", luaB_is_sound},
	{"instead_savepath", luaB_get_savepath},
	{"instead_gamepath", luaB_get_gamepath},
	{"instead_steadpath", luaB_get_steadpath},
	{"instead_themespath", luaB_get_themespath},
	{"instead_gamespath", luaB_get_gamespath},
	{"instead_realpath", luaB_get_realpath},
	{"instead_timer", luaB_set_timer},
	{"instead_theme_var", luaB_theme_var},
	{"instead_theme_name", luaB_theme_name},
	{"instead_menu_toggle", luaB_show_menu},
	{"instead_busy", luaB_stead_busy},
	{"instead_sound_load", luaB_load_sound},
	{"instead_sound_free", luaB_free_sound},
	{"instead_sound_channel", luaB_channel_sound},
	{"instead_sound_panning", luaB_panning_sound},
	{"instead_sound_volume", luaB_volume_sound},
	{"instead_sounds_free", luaB_free_sounds},

	{"instead_mouse_pos", luaB_mouse_pos},
	{"instead_mouse_filter", luaB_mouse_filter},
	{"instead_finger_pos", luaB_finger_pos},

	{"instead_font_load", luaB_load_font},
	{"instead_font_free", luaB_free_font},
	{"instead_font_scaled_size", luaB_font_size_scaled},
	{"instead_ticks", luaB_get_ticks},
	{"instead_sprite_load", luaB_load_sprite},
	{"instead_sprite_text", luaB_text_sprite},
	{"instead_sprite_free", luaB_free_sprite},
	{"instead_sprites_free", luaB_free_sprites},
	{"instead_sprite_draw", luaB_draw_sprite},
	{"instead_sprite_copy", luaB_copy_sprite},
	{"instead_sprite_compose", luaB_compose_sprite},
	{"instead_sprite_fill", luaB_fill_sprite},
	{"instead_sprite_dup", luaB_dup_sprite},
	{"instead_sprite_alpha", luaB_alpha_sprite},
	{"instead_sprite_colorkey", luaB_colorkey_sprite},
	{"instead_sprite_size", luaB_sprite_size},
	{"instead_sprite_scale", luaB_scale_sprite},
	{"instead_sprite_rotate", luaB_rotate_sprite},
	{"instead_sprite_text_size", luaB_text_size},
	{"instead_sprite_pixel", luaB_pixel_sprite},
	
	{"bit_or", luaB_bit_or},
	{"bit_and", luaB_bit_and},
	{"bit_xor", luaB_bit_xor},
	{"bit_shl", luaB_bit_shl},
	{"bit_shr", luaB_bit_shr},
	{"bit_not", luaB_bit_not},
	{"bit_div", luaB_bit_div},
	{"bit_idiv", luaB_bit_idiv},
	{"bit_mod", luaB_bit_mod},
	{"bit_mul", luaB_bit_mul},
	{"bit_imul", luaB_bit_imul},
	{"bit_sub", luaB_bit_sub},
	{"bit_add", luaB_bit_add},
	{"bit_signed", luaB_bit_signed},
	{"bit_unsigned", luaB_bit_unsigned},

	{NULL, NULL}
};

int instead_lang(void)
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
#elif defined(S60)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='S60'");
#elif defined(ANDROID)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='ANDROID'");
#elif defined(_WIN32)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='WIN32'");
#elif defined(MAEMO)
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='MAEMO'");
#else
	snprintf(plat, sizeof(plat) - 1, "PLATFORM='UNIX'");
#endif

	plat[sizeof(plat) - 1] = 0;
	instead_eval(plat); instead_clear();
	return 0;
}

static int instead_package(const char *path)
{
	char *p;
#if defined(_WIN32_WCE)
	char stead_path[PATH_MAX * 4] = "package.path=\""; /* wince have not cwd :) */
	if (path) {
		strcat(stead_path, path);
		strcat(stead_path, "/?.lua;");
	}
#else
	char stead_path[PATH_MAX] = "package.path=\"";
	if (path)
		strcat(stead_path, "./?.lua;");
#endif
	p = game_local_stead_path();
	if (p) {
		strcat(stead_path, p);
		strcat(stead_path, "/?.lua;");
	}

	if (STEAD_PATH[0] != '/') {
		strcat(stead_path, game_cwd);
		strcat(stead_path, "/");
		strcat(stead_path, STEAD_PATH);
	} else {
		strcat(stead_path, STEAD_PATH);
	}
	strcat(stead_path, "/?.lua");
	strcat(stead_path, "\"");

	instead_eval(stead_path); instead_clear();
/*	putenv(stead_path); */
	return 0;
}

int instead_init_lua(const char *path)
{
	busy = 0;
	setlocale(LC_ALL,"");
	setlocale(LC_NUMERIC,"C"); /* to avoid . -> , in numbers */	
/*	strcpy(curcp, "UTF-8"); */

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
	luaopen_lfs (L);

	instead_package(path);
	instead_platform();
	instead_lang();
	if (debug_sw)
		instead_eval("DEBUG=true"); instead_clear();
	return 0;
}

int instead_init(const char *path)
{
	if (instead_init_lua(path))
		return -1;

	if (dofile(L, dirpath(STEAD_PATH"/stead.lua"))) {
		return -1;
	}

	if (dofile(L, dirpath(STEAD_PATH"/gui.lua"))) {
		instead_clear();
		return -1;
	}
	/* cleanup Lua */
	instead_clear();
	srand(time(NULL));
	mt_random_init();
	return 0;
}

void instead_done(void)
{
	gfx_del_timer(instead_timer);
	instead_timer = NULL_TIMER;
#ifdef _HAVE_ICONV
	if (fromcp)
		free(fromcp);
#endif
	if (L)
		lua_close(L);
	L = NULL;
#ifdef _HAVE_ICONV
	fromcp = NULL;
#endif
	sprites_free();
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
