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

#include "externals.h"
#include "internals.h"

#include "util.h"

extern char	instead_cwd[PATH_MAX];

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
		strcpy(stead_path, instead_cwd);
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
		strcpy(themes_path, instead_cwd);
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
		strcpy(games_path, instead_cwd);
		strcat(games_path, "/");
	} else
		games_path[0] = 0;
	strcat(games_path, GAMES_PATH);
	unix_path(games_path);
	lua_pushstring(L, games_path);
	return 1;
}

extern char *instead_exec;

static int luaB_get_exepath(lua_State *L) {
	char instead_path[PATH_MAX];
	if (instead_exec) {
		strcpy(instead_path, instead_exec);
		unix_path(instead_path);
		lua_pushstring(L, instead_exec);
		return 1;
	}
	return 0;
}

extern int dir_iter_factory (lua_State *L);
extern int luaopen_lfs (lua_State *L);

static const luaL_Reg paths_funcs[] = {
	{"instead_readdir", dir_iter_factory},
	{"instead_savepath", luaB_get_savepath},
	{"instead_gamepath", luaB_get_gamepath},
	{"instead_steadpath", luaB_get_steadpath},
	{"instead_themespath", luaB_get_themespath},
	{"instead_gamespath", luaB_get_gamespath},
	{"instead_exepath", luaB_get_exepath},

	{ NULL, NULL }
};

int instead_paths_init(void)
{
	luaopen_lfs(instead_lua());
	return instead_api_register(paths_funcs);
}
