#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include <unistd.h>
/* the Lua interpreter */

lua_State* L;

extern char *fromgame(char *s);

char *getstring(char *cmd)
{
	char *s;
	int N;
	if (luaL_dostring(L, cmd)) {
		fprintf(stderr,"Error: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	N = lua_gettop(L); 
	if (N <= 0)
		return NULL;
	s = (char*)lua_tostring(L, -N);
	if (s)
		s = fromgame(s);
	lua_pop(L, N);
	return s;
}

int luacall(char *cmd)
{
	int rc, N;
	if (luaL_dostring(L, cmd)) {
		fprintf(stderr,"Error: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	N = lua_gettop(L); 
	if (N <= 0)
		return 0;
	rc = lua_tonumber(L, -1);
	lua_pop(L, N);
	return rc;
}

extern int loop(void);
extern int width;
extern int height;
int main (int argc, char *argv[])
{
	int opt;
	setlocale(LC_ALL,"");
	/* initialize Lua */
	
	while ((opt = getopt(argc, argv, "w:h:")) != -1) {
               switch (opt) {
               case 'w':
                   width = atoi(optarg);
                   break;
               case 'h':
                   height = atoi(optarg);
                   break;
               }
        }
	if (argc < 2 || argc - optind < 1) {
		fprintf(stderr,"Usage: %s <game.lua> [-w<width>] [-h<height>]\n", argv[0]);
		return 1;
	}
	if (width == 0 || height == 0) {
		fprintf(stderr,"Wrong geometry.\n");
		return 1;
	}

	L = lua_open();
	luaL_openlibs(L);
   	if (luaL_loadfile(L,STEAD_PATH"stead.lua") || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr,"Error:%s\n", lua_tostring(L, -1));
		return 1;
	}

   	if (luaL_loadfile(L,argv[optind]) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr,"Error:%s\n", lua_tostring(L, -1));
		return 1;
	}
	loop();
	/* cleanup Lua */
	lua_close(L);
	return 0;
}

