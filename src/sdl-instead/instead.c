#ifndef STEAD_PATH
#define STEAD_PATH 	"./stead"
#endif

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

typedef struct LoadF {
	int extraline;
	unsigned char byte;
	FILE *f;
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
	for (i = 0; i < *size; i ++) {
		unsigned char b = lf->buff[i];
		lf->buff[i] ^= lf->byte;
		lf->buff[i] = (lf->buff[i] >> 3) | (lf->buff[i] << 5);
		lf->byte = b;
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

static int loadfile (lua_State *L, const char *filename) {
	LoadF lf;
	int status, readstatus;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	lf.extraline = 0;
	lua_pushfstring(L, "@%s", filename);
	lf.f = fopen(filename, "r");
	lf.byte = 0xcc;
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
	if (loadfile(L, fname) != 0) lua_error(L);
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

static const luaL_Reg base_funcs[] = {
	{"doencfile", luaB_doencfile},
	{"print", luaB_print}, /* for some mystic, it is needed in win version (with -debug) */
	{NULL, NULL}
};

int instead_init(void)
{
	setlocale(LC_ALL,"");
//	strcpy(curcp, "UTF-8");
	/* initialize Lua */
	L = lua_open();
	if (!L)
		return -1;
	luaL_openlibs(L);
	luaL_register(L, "_G", base_funcs);
   	if (dofile(L,STEAD_PATH"/stead.lua")) {
		return -1;
	}

   	if (dofile(L,STEAD_PATH"/gui.lua")) {
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

int  instead_encode(const char *s, const char *d)
{
	FILE *src;
	FILE *dst;
	size_t size;
	int i = 0;
	unsigned char byte = 0xcc;
	unsigned char buff[4096];

	src = fopen(s, "r");
	if (!src) {
		fprintf(stderr,"Can't open on read: '%s'.\n", s);
		return -1;
	}
	dst = fopen(d, "w");
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

