#ifndef STEAD_PATH
#define STEAD_PATH 	"./stead"
#endif

#include "gui.h"
#include "externals.h"
#include "internals.h"
/* the Lua interpreter */

char 		*fromgame(const char *s);
char 		*togame(const char *s);
lua_State	*L = NULL;

static int report (lua_State *L, int status) 
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) 
			msg = "(error object is not a string)";
		fprintf(stderr,"Error: %s\n", msg);
		game_err_msg(msg);
		lua_pop(L, 1);	
		status = -1;
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

extern int debug_sw;

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
	if (!L)
		return NULL;
	if (dostring(L, cmd))
		return NULL;
	s = (char*)lua_tostring(L, -1);
	if (s)
		s = fromgame(s);
	return s;
}


char *instead_eval(char *s)
{
	char *p;
	p = getstring(s);
	return p;
}

char *instead_cmd(char *s)
{
	char buf[1024];
	char *p = s;
	while (*p) {
		if (*p == '\\' || *p == '\'' || *p == '\"' || *p == '[' || *p == ']')
			return NULL;
		p ++;
	}
	s = togame(s);
	snprintf(buf, sizeof(buf), "return iface:cmd('%s')", s);
	free(s);
	p = getstring(buf);
	return p;
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
#define CHAR_MAX_LEN 4
static char *decode(iconv_t hiconv, const char *s)
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
#ifdef _HAVE_ICONV
	if (fromcp)
		free(fromcp);
	fromcp = getstring("return game.codepage;");
#endif
	return 0;
}

int instead_init(void)
{
	setlocale(LC_ALL,"");
//	strcpy(curcp, "UTF-8");
	/* initialize Lua */
	L = lua_open();
	luaL_openlibs(L);
   	if (dofile(L,STEAD_PATH"/stead.lua")) {
		return -1;
	}
	if (luacall(instead_gui_lua)) {
		return -1;
	}
	/* cleanup Lua */
	return 0;
}

void instead_done(void)
{
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

