echo -n "Checking pkg-config..."
if ! pkg-config --version >/dev/null 2>&1; then
	echo "no pkg-config in PATH."
	exit 1
fi
echo "ok"
echo -n "Checking pkg-config --cflags lua..."
if ! pkg-config --cflags lua >/dev/null 2>&1; then
	if ! pkg-config --cflags lua5.1 >/dev/null 2>&1; then
		echo "failed"
		exit 1
	fi
	echo "lua5.1"
	lua_cflags="pkg-config --cflags lua5.1"
	lua_libs="pkg-config --libs lua5.1"
else
	echo "lua"
	lua_cflags="pkg-config --cflags lua"	
	lua_libs="pkg-config --libs lua"	
fi

echo -n "Checking sdl-config..."
if ! sdl-config --version >/dev/null 2>&1; then
	echo "no sdl-config in PATH."
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
			echo "cc, gcc, \$(CC) are not valid compilers... Please, export CC for valid one...";
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
	return 1;
}
EOF
echo $cc
echo -n "Checking test build...("
echo $cc /tmp/sdl-test.c $ops `sdl-config --cflags` `sdl-config --libs` -lSDL_ttf -lSDL_mixer -lSDL_image
if ! $cc /tmp/sdl-test.c $ops `sdl-config --cflags` `sdl-config --libs` -lSDL_ttf -lSDL_mixer -lSDL_image; then
	echo ") failed".
	echo "Please, check those dev packages installed: SDL_ttf SDL_mixer SDL_image."
	rm /tmp/sdl-test.c
	exit 1
fi
echo ") ok"
rm /tmp/sdl-test.c
echo -n "Generating config.make..."
echo "LUA_CFLAGS=\$(shell $lua_cflags)" > config.make
echo "LUA_LFLAGS=\$(shell $lua_libs)" >> config.make
echo "SDL_CFLAGS=\$(shell sdl-config --cflags)" >> config.make
echo "SDL_LFLAGS=\$(shell sdl-config --libs) -lSDL_ttf -lSDL_mixer -lSDL_image" >> config.make
echo "ok"
echo -n "Do you want game will run standalone(1) or will be installed in system(2): "
read ans
if [ "$ans" == "1" ]; then
	rm -f Rules.make
	ln -s Rules.make.standalone Rules.make
	if ! make clean >/dev/null 2>&1; then
		echo "* Warning! Can not do make clean..."
	fi
	echo "Ok, now do:"
	echo "    make; ./sdl-instead"
elif [ "$ans" == "2" ]; then
	rm -f Rules.make
	ln -s Rules.make.system Rules.make
	if ! make clean >/dev/null 2>&1; then
		echo "* Warning! Can not do make clean..."
	fi
	echo "Ok, now do:"
	echo "    make; make install; sdl-instead"
else
	echo "Huh!!! Wrong answer."
	exit 1
fi
echo "Bye..."
