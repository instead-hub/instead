/* From:
** LuaFileSystem
** Copyright Kepler Project 2003 (http://www.keplerproject.org/luafilesystem)
** $Id: lfs.c,v 1.61 2009/07/04 02:10:16 mascarenhas Exp $
*/
#include "system.h"
#include "util.h"

#define DIR_METATABLE "directory metatable"
typedef struct dir_data {
	int  closed;
#ifdef _WIN32
	long hFile;
	char pattern[MAX_PATH+1];
#else
	DIR *dir;
#endif
} dir_data;

/*
** Directory iterator
*/
static int dir_iter (lua_State *L) {
#ifdef _WIN32
	struct _finddata_t c_file;
#else
	struct dirent *entry;
#endif
	dir_data *d = (dir_data *)luaL_checkudata (L, 1, DIR_METATABLE);
	luaL_argcheck (L, d->closed == 0, 1, "closed directory");
#ifdef _WIN32
	if (d->hFile == 0L) { /* first entry */
		if ((d->hFile = _findfirst (d->pattern, &c_file)) == -1L) {
			lua_pushnil (L);
			lua_pushstring (L, "Error while iterating dir.");
			d->closed = 1;
			return 2;
		} else {
			lua_pushstring (L, c_file.name);
			return 1;
		}
	} else { /* next entry */
		if (_findnext (d->hFile, &c_file) == -1L) {
			/* no more entries => close directory */
			_findclose (d->hFile);
			d->closed = 1;
			return 0;
		} else {
			lua_pushstring (L, c_file.name);
			return 1;
		}
	}
#else
	if (!d->dir)
		return 0; /* no directory opened */
	if ((entry = readdir (d->dir)) != NULL) {
		lua_pushstring (L, entry->d_name);
		return 1;
	} else {
		/* no more entries => close directory */
		closedir (d->dir);
		d->closed = 1;
		return 0;
	}
#endif
}


/*
** Closes directory iterators
*/
static int dir_close (lua_State *L) {
	dir_data *d = (dir_data *)lua_touserdata (L, 1);
#ifdef _WIN32
	if (!d->closed && d->hFile) {
		_findclose (d->hFile);
	}
#else
	if (!d->closed && d->dir) {
		closedir (d->dir);
	}
#endif
	d->closed = 1;
	return 0;
}


/*
** Factory of directory iterators
*/
int dir_iter_factory (lua_State *L) {
	const char *path = luaL_checkstring (L, 1);
	dir_data *d;
	lua_pushcfunction (L, dir_iter);
	d = (dir_data *) lua_newuserdata (L, sizeof(dir_data));
	d->closed = 0;
#ifdef _WIN32
	d->hFile = 0L;
	luaL_getmetatable (L, DIR_METATABLE);
	lua_setmetatable (L, -2);
	if (strlen(path) > MAX_PATH-2)
	  luaL_error (L, "path too long: %s", path);
	else
	  sprintf (d->pattern, "%s/*", path);
#else
	luaL_getmetatable (L, DIR_METATABLE);
	lua_setmetatable (L, -2);
	d->dir = opendir (dirpath(path));
	if (d->dir == NULL)
		luaL_error (L, "cannot open %s.", path);
#endif
	return 2;
}


/*
** Creates directory metatable.
*/
static int dir_create_meta (lua_State *L) {
	luaL_newmetatable (L, DIR_METATABLE);
	/* set its __gc field */
	lua_pushstring (L, "__index");
	lua_newtable(L);
	lua_pushstring (L, "next");
	lua_pushcfunction (L, dir_iter);
	lua_settable(L, -3);
	lua_pushstring (L, "close");
	lua_pushcfunction (L, dir_close);
	lua_settable(L, -3);
	lua_settable (L, -3);
	lua_pushstring (L, "__gc");
	lua_pushcfunction (L, dir_close);
	lua_settable (L, -3);
	return 1;
}

int luaopen_lfs (lua_State *L) {
	dir_create_meta (L);
	return 1;
}
