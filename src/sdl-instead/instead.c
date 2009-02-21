#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include <unistd.h>
#include "gui.h"
#ifdef HAVE_ICONV
#include <iconv.h>
#endif
/* the Lua interpreter */

char 		*fromgame(const char *s);
char 		*togame(const char *s);
lua_State	*L = NULL;

char *getstring(char *cmd)
{
	char *s;
	if (!L)
		return NULL;
	if (luaL_dostring(L, cmd)) {
		fprintf(stderr,"Error: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	s = (char*)lua_tostring(L, -1);
	if (s)
		s = fromgame(s);
	return s;
}

char *instead_eval(char *s)
{
	char *p;
//	s = togame(s);
	p = getstring(s);
//	free(s);
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
	if (luaL_dostring(L, cmd)) {
		fprintf(stderr,"Error: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	rc = lua_tonumber(L, -1);
	return rc;
}

#ifdef HAVE_ICONV
static char *curcp = "UTF-8";
static char *fromcp = NULL;
#endif

#ifdef HAVE_ICONV
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
   	if (luaL_loadfile(L, game) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr,"Error:%s\n", lua_tostring(L, -1));
		return -1;
	}
#ifdef HAVE_ICONV
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
   	if (luaL_loadfile(L,STEAD_PATH"stead.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr,"Error:%s\n", lua_tostring(L, -1));
		return -1;
	}
	if (luacall(instead_gui_lua)) {
		fprintf(stderr,"Error while registering lua gui functions\n");
		return -1;
		
	}
#if 0
   	if (luaL_loadfile(L,"gui.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr,"Error:%s\n", lua_tostring(L, -1));
		return -1;
	}
#endif
	/* cleanup Lua */
	return 0;
}

void instead_done(void)
{
#ifdef HAVE_ICONV
	if (fromcp)
		free(fromcp);
#endif
	if (L)
		lua_close(L);
	L = NULL;
	fromcp = NULL;
}

