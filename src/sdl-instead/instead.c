#include "externals.h"
#include "internals.h"

#ifndef STEAD_PATH
#define STEAD_PATH 	"./stead"
#endif

/* the Lua interpreter */

static gtimer_t instead_timer = NULL_TIMER;
static int instead_timer_nr = 0;

char 		*fromgame(const char *s);
char 		*togame(const char *s);
lua_State	*L = NULL;

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

static int docall (lua_State *L) 
{
	int status;
	int base = 0; 	
	if (debug_sw) {
		base = lua_gettop(L);  /* function index */
		lua_pushcfunction(L, traceback);  /* push traceback function */
		lua_insert(L, base);  /* put it under chunk and args */
	}
	status = lua_pcall(L, 0, LUA_MULTRET, base);
	if (debug_sw)
		lua_remove(L, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0) 
		lua_gc(L, LUA_GCCOLLECT, 0);
	return status;
}

static int dofile (lua_State *L, const char *name) {
	int status = luaL_loadfile(L, name) || docall(L);
	return report(L, status);
}

static int dostring (lua_State *L, const char *s) {
	int status = luaL_loadstring(L, s) || docall(L);
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
	status = lua_pcall(L, method + n, LUA_MULTRET, base);
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

int instead_load(char *game)
{
   	if (dofile(L, game)) {
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

typedef struct LoadF {
	int extraline;
	unsigned char byte;
	FILE *f;
	int enc;
	unsigned char buff[4096];
} LoadF;

static const char *getF (lua_State *L, void *ud, size_t *size) {
	int i = 0;
	LoadF *lf = (LoadF *)ud;
	(void)L;
	if (lf->extraline) {
		lf->extraline = 0;
		*size = 1;
		return "\n";
	}
	if (feof(lf->f)) return NULL;
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
	lf.f = fopen(filename, "rb");
	lf.byte = 0xcc;
	lf.enc = enc;
	if (lf.f == NULL) return errfile(L, "open", fnameindex);
	status = lua_load(L, getF, &lf, lua_tostring(L, -1));
	readstatus = ferror(lf.f);
	if (filename) fclose(lf.f);  /* close file (even in case of errors) */
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
	if (loadfile(L, dirpath(fname), 1) != 0) lua_error(L);
	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - n;
}

static int luaB_dofile (lua_State *L) {
	const char *fname = luaL_optstring(L, 1, NULL);
	int n = lua_gettop(L);
	if (loadfile(L, dirpath(fname), 0) != 0) lua_error(L);
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
	r = snd_playing(c);
	lua_pushboolean(L, (r != 0));  /* else not a number */
	return 1;
}

static int luaB_get_savepath(lua_State *L) {
	lua_pushstring(L, dirname(game_save_path(1, 0)));
	return 1;
}

static int luaB_get_gamepath(lua_State *L) {
	char path[PATH_MAX];
	char *p = getdir(path, sizeof(path));
	if (p)
		unix_path(p);
	lua_pushstring(L, p);
	return 1;
}

static int luaB_get_steadpath(lua_State *L) {
	char stead_path[PATH_MAX];
	strcpy(stead_path, game_cwd);
	strcat(stead_path, "/");
	strcat(stead_path, STEAD_PATH);
	unix_path(stead_path);
	lua_pushstring(L, stead_path);
	return 1;
}


extern void mouse_reset(int hl); /* too bad */

static void instead_timer_do(void *data)
{
	char *p;
	if (game_paused())
		goto out;
	if (instead_function("stead.timer", NULL)) {
		instead_clear();
		goto out;
	}
	p = instead_retval(0); instead_clear();
	if (!p)
		goto out;
	mouse_reset(0);
	game_cmd(p); free(p);
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
	if (!theme_setvar((char*)var, (char*)val))
		game_theme_changed = 2;
	return 0;
}

extern int dir_iter_factory (lua_State *L);
extern int luaopen_lfs (lua_State *L);

static const luaL_Reg base_funcs[] = {
	{"doencfile", luaB_doencfile},
	{"dofile", luaB_dofile},
	{"print", luaB_print}, /* for some mystic, it is needed in win version (with -debug) */
	{"is_sound", luaB_is_sound},
	{"get_savepath", luaB_get_savepath},
	{"get_gamepath", luaB_get_gamepath},
	{"get_steadpath", luaB_get_steadpath},
	{"set_timer", luaB_set_timer},
	{"theme_var", luaB_theme_var},
	{"readdir", dir_iter_factory},
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

static int instead_package(void)
{
	char *p;
	char stead_path[PATH_MAX] = "package.path=\"./?.lua;";

	p = game_local_stead_path();
	if (p) {
		strcat(stead_path, p);
		strcat(stead_path, "/?.lua");
		strcat(stead_path, ";");
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
int instead_init(void)
{
	setlocale(LC_ALL,"");
	setlocale(LC_NUMERIC,"C"); /* to avoid . -> , in numbers */	
//	strcpy(curcp, "UTF-8");

	/* initialize Lua */

	L = lua_open();
	if (!L)
		return -1;

	luaL_openlibs(L);
	luaL_register(L, "_G", base_funcs);
	luaopen_lfs (L);

	instead_package();
	instead_lang();

	if (dofile(L, dirpath(STEAD_PATH"/stead.lua"))) {
		return -1;
	}

	if (dofile(L, dirpath(STEAD_PATH"/gui.lua"))) {
		instead_clear();
		return -1;
	}
	/* cleanup Lua */
	instead_clear();
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
}

int  instead_encode(const char *s, const char *d)
{
	FILE *src;
	FILE *dst;
	size_t size;
	int i = 0;
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
			return -1;
		}
	}
	fclose(src);
	fclose(dst);
	return 0;
}

