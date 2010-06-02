#!/bin/bash

echo -n "Checking pkg-config..."
if ! pkg-config --version >/dev/null 2>&1; then
	echo "error: no pkg-config in \$PATH."
	exit 1
fi
echo "ok"

echo -n "Checking pkg-config --cflags zlib..."
if ! pkg-config --cflags zlib >/dev/null 2>&1; then
	echo "internal"
	zlib_cflags=
	zlib_libs=
else
	echo "system"
	zlib_cflags="pkg-config --cflags zlib"	
	zlib_libs="pkg-config --libs zlib"	
fi

echo -n "Checking pkg-config --cflags gtk+-2.0..."
if ! pkg-config --cflags gtk+-2.0 >/dev/null 2>&1; then
	echo "no open file dialog"
	gtk_cflags=
	gtk_libs=
else
	echo "yes"
	gtk_cflags="pkg-config --cflags gtk+-2.0"
	gtk_libs="pkg-config --libs gtk+-2.0"
fi


echo -n "Checking pkg-config --cflags lua[5.1]..."
if ! pkg-config --cflags lua5.1 >/dev/null 2>&1; then
	if ! pkg-config --cflags lua >/dev/null 2>&1; then
		echo "failed: no package lua/lua5.1"
		echo "Please install lua development package."
		exit 1
	fi
	echo "lua"
	lua_cflags="pkg-config --cflags lua"
	lua_libs="pkg-config --libs lua"
else
	echo "lua5.1"
	lua_cflags="pkg-config --cflags lua5.1"	
	lua_libs="pkg-config --libs lua5.1"	
fi


echo -n "Checking sdl-config..."
if ! sdl-config --version >/dev/null 2>&1; then
	echo "error: no sdl-config in \$PATH."
	echo "Please install sdl, sdl_ttf, sdl_mixer and sdl_image development packages."
	exit 1
fi
echo "ok"

echo -n "Checking sdl-config --cflags..."
if ! sdl-config --cflags  >/dev/null 2>&1; then
	echo "failed."
	exit 1
fi
echo "ok"

ops=`$lua_cflags`
ops=$ops" "`$lua_libs`

echo -n "Looking for compiler..."
if ! cc --version >/dev/null 2>&1; then
	if ! gcc --version >/dev/null 2>&1; then
		if ! $(CC) --version >/dev/null 2>&1; then
			echo "cc, gcc, \$(CC) are not valid compilers... Please export CC for valid one...";
			exit 1;
		else
			cc=$CC;	
		fi
	else
		cc="gcc"	
	fi
else
	cc="cc"	
fi

cat << EOF >/tmp/sdl-test.c
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mutex.h>
#include <SDL_mixer.h>
int main(int argc, char **argv)
{
	return 0;
}
EOF
echo $cc
echo -n "Checking test build...("
echo -n $cc /tmp/sdl-test.c $ops `sdl-config --cflags` `sdl-config --libs` -lSDL_ttf -lSDL_mixer -lSDL_image -o /tmp/sdl-test ")..."
if ! $cc /tmp/sdl-test.c $ops `sdl-config --cflags` `sdl-config --libs` -lSDL_ttf -lSDL_mixer -lSDL_image -o /tmp/sdl-test; then
	echo "failed".
	echo "Please sure if these development packages are installed: sdl, sdl_ttf, sdl_mixer, sdl_image."
	rm -f /tmp/sdl-test.c /tmp/sdl-test
	exit 1
fi
echo "ok"
rm -f /tmp/sdl-test.c /tmp/sdl-test
if ! make clean >/dev/null 2>&1; then
	echo " * Warning!!! Can not do make clean..."
fi
echo -n "Generating config.make..."
echo "# autamatically generated by configure.sh" >config.make
if [[ ! -z "$gtk_cflags" ]]; then
	echo "EXTRA_CFLAGS+=-D_USE_GTK" >> config.make
	echo "EXTRA_CFLAGS+=\$(shell $gtk_cflags)" >> config.make
	echo "EXTRA_LDFLAGS+=\$(shell $gtk_libs)" >> config.make
fi
if [[ -z "$zlib_cflags" ]]; then
	echo "SUBDIRS=src/zlib" >> config.make
	echo "ZLIB_CFLAGS=-I../zlib" >> config.make
	echo "ZLIB_LFLAGS=../zlib/libz.a" >> config.make
else
	echo "ZLIB_CFLAGS=\$(shell $zlib_cflags)" >> config.make
	echo "ZLIB_LFLAGS=\$(shell $zlib_libs)" >> config.make
fi
echo "LUA_CFLAGS=\$(shell $lua_cflags)" >> config.make
echo "LUA_LFLAGS=\$(shell $lua_libs)" >> config.make
echo "SDL_CFLAGS=\$(shell sdl-config --cflags)" >> config.make
echo "SDL_LFLAGS=\$(shell sdl-config --libs) -lSDL_ttf -lSDL_mixer -lSDL_image" >> config.make
echo "ok"
echo -n "Choose installation mode. Standalone(1) or system(2) [1]: "
read ans
if [ "x$ans" = "x1" -o "x$ans" = "x" ]; then
	echo " * Standalone version"
	rm -f Rules.make
	ln -sf Rules.make.standalone Rules.make
	rm -f sdl-instead
	ln -sf src/sdl-instead/sdl-instead sdl-instead
	echo "Ok. We are ready to build. Use these commands:"
	echo "    \$ make"
	echo "    \$ ./sdl-instead"
elif [ "x$ans" = "x2" ]; then
	echo -n "Enter prefix path [/usr/local]: "
	read ans

	if [ "x$ans" = "x" ]; then
		prefix="/usr/local"
	else
		prefix="$ans"
	fi
	
	rm -f Rules.make
	ln -s Rules.make.system Rules.make

	echo " * System version with prefix: $prefix"

	echo "PREFIX=$prefix" >> config.make
	echo "BIN=\$(DESTDIR)\$(PREFIX)/bin/" >> config.make 
	echo "STEADPATH=\$(DESTDIR)\$(PREFIX)/share/instead" >> config.make
	echo "THEMESPATH=\$(STEADPATH)/themes" >> config.make
	echo "GAMESPATH=\$(STEADPATH)/games" >> config.make
	echo "ICONPATH=\$(DESTDIR)\$(PREFIX)/share/pixmaps" >> config.make
	echo "DOCPATH=\$(DESTDIR)\$(PREFIX)/share/doc/instead" >> config.make
	echo "LANGPATH=\$(STEADPATH)/languages" >> config.make
	echo "MANPATH=\$(DESTDIR)\$(PREFIX)/share/man/man6" >> config.make

	echo "Ok. We are ready to build and install. Use these commands:"
	echo "    \$ make"
	echo "    \$ sudo make install"
	echo "    \$ sdl-instead"
else
	echo "Huh!!! Wrong answer."
	exit 1
fi

echo " Enjoy..."


