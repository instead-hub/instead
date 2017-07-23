#!/usr/bin/env bash

# build INSTEAD with emscripten

set -e
export WORKSPACE="$WORKSPACE"
# /home/peter/Devel/emsdk-portable/env"
if [ ! -f ./emsdk_set_env.sh ]; then
	echo "Run this script in emsdk directory"
	exit 1
fi
if [ -z "$WORKSPACE" ]; then
	echo "Define WORKSPACE path in $0"
	exit 1
fi

if [ ! -d "$WORKSPACE" ]; then
	echo "Please, create build directory $WORKSPACE"
	exit 1
fi
. ./emsdk_set_env.sh

# some general flags
export CFLAGS="-g0 -O2"
export CXXFLAGS="$CFLAGS"
export EM_CFLAGS="-Wno-warn-absolute-paths"
export EMMAKEN_CFLAGS="$EM_CFLAGS"
export PKG_CONFIG_PATH="$WORKSPACE/lib/pkgconfig"
export MAKEFLAGS="-j2"

# flags to fake emconfigure and emmake
export CC="emcc"
export CXX="em++"
export LD="$CC"
export LDSHARED="$LD"
export RANLIB="emranlib"
export AR="emar"

# Lua
cd $WORKSPACE
rm -rf lua-5.1.5
[ -f lua-5.1.5.tar.gz ] || wget -nv 'https://www.lua.org/ftp/lua-5.1.5.tar.gz'
tar xf lua-5.1.5.tar.gz
cd lua-5.1.5
cat src/luaconf.h | sed -e 's/#define LUA_USE_POPEN//g' -e 's/#define LUA_USE_ULONGJMP//g'>src/luaconf.h.new
mv src/luaconf.h.new src/luaconf.h
emmake make posix CC=emcc 
emmake make install INSTALL_TOP=$WORKSPACE 

# libiconv
cd $WORKSPACE
rm -rf libiconv-1.15
[ -f libiconv-1.15.tar.gz ] || wget -nv 'https://ftp.gnu.org/gnu/libiconv/libiconv-1.15.tar.gz'
tar xf libiconv-1.15.tar.gz
cd libiconv-1.15
emconfigure ./configure --prefix=$WORKSPACE
emmake make install

# zlib
cd $WORKSPACE
rm -rf zlib-1.2.11/
[ -f zlib-1.2.11.tar.gz ] || wget -nv 'http://zlib.net/zlib-1.2.11.tar.gz'
tar xf zlib-1.2.11.tar.gz
cd zlib-1.2.11
emconfigure ./configure --prefix=$WORKSPACE
emmake make install

# libmad (with some patches from debian+arch)

cd $WORKSPACE
[ -d libmad-0.15.1b ] || PATCH_MAD=1
wget -nv -N http://download.sourceforge.net/mad/libmad-0.15.1b.tar.gz
wget -nv -N http://http.debian.net/debian/pool/main/libm/libmad/libmad_0.15.1b-8.diff.gz
wget -nv -N https://projects.archlinux.org/svntogit/packages.git/plain/trunk/libmad.patch?h=packages/libmad -O libmad-pkgconfig.patch
tar xf libmad-0.15.1b.tar.gz
cd libmad-0.15.1b
if [ ! -z ${PATCH_MAD+x} ]; then
  zcat ../libmad_0.15.1b-8.diff.gz | patch -p1
  patch -p1 -i ../libmad-pkgconfig.patch
  patch -p1 -i debian/patches/frame_length.diff
  patch -p1 -i debian/patches/amd64-64bit.diff
  patch -p1 -i debian/patches/optimize.diff
fi
emconfigure ./configure --prefix=$WORKSPACE --disable-shared --enable-static 
emmake make install

# libmodplug
cd $WORKSPACE
rm -rf libmodplug-0.8.9.0/
[ -f libmodplug-0.8.9.0.tar.gz ] || wget -nv http://downloads.sourceforge.net/project/modplug-xmms/libmodplug/0.8.9.0/libmodplug-0.8.9.0.tar.gz
tar xf libmodplug-0.8.9.0.tar.gz
cd libmodplug-0.8.9.0
emconfigure ./configure --prefix=$WORKSPACE --disable-shared --enable-static 
emmake make install

# SDL2_mixer
cd $WORKSPACE
rm -rf SDL_mixer
[ -d SDL_mixer/.hg ] || hg clone https://hg.libsdl.org/SDL_mixer
cd SDL_mixer
hg pull -u
hg up -C
hg --config "extensions.purge=" purge --all

cat <<EOF > sdl_mixer_mad.patch
diff -Nur SDL_mixer/music_mad.c SDL_mixer.mad/music_mad.c
--- SDL_mixer/music_mad.c	2017-07-23 16:10:08.569368777 +0300
+++ SDL_mixer.mad/music_mad.c	2017-07-23 16:13:06.322936929 +0300
@@ -265,7 +265,7 @@
       SDL_memcpy(out, mp3_mad->output_buffer + mp3_mad->output_begin, num_bytes);
     } else {
       SDL_MixAudioFormat(out, mp3_mad->output_buffer + mp3_mad->output_begin,
-                         mixer.format, num_bytes, mp3_mad->volume);
+                         mp3_mad->mixer.format, num_bytes, mp3_mad->volume);
     }
     out += num_bytes;
     mp3_mad->output_begin += num_bytes;
EOF
patch -p1 -i ./sdl_mixer_mad.patch
cat configure.in | sed -e 's/AC_CHECK_LIB(\[modplug\], /AC_CHECK_LIB(\[modplug\], \[ModPlug_Load\], /' > configure.in.new
mv -f configure.in.new configure.in
autoconf
emconfigure ./configure --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -s USE_VORBIS=1 -s USE_OGG=1" LDFLAGS="-L$WORKSPACE/lib" --disable-sdltest --disable-shared \
   --enable-music-mp3-mad-gpl --enable-music-ogg --disable-music-ogg-shared --enable-music-mod-modplug --disable-music-mod-modplug-shared \
   --disable-music-midi-fluidsynth --disable-music-midi-fluidsynth-shared
emmake make install

# INSTEAD
echo "INSTEAD"
cd $WORKSPACE
[ -d instead-em ] || git clone https://github.com/instead-hub/instead.git instead-em
cd instead-em
git pull
[ -e Rules.make ] || ln -s Rules.standalone Rules.make
cat <<EOF > config.make
EXTRA_CFLAGS+= -D_HAVE_ICONV -I../../include
SDL_CFLAGS=-I../../include/SDL2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2
SDL_LFLAGS=
LUA_CFLAGS=
LUA_LFLAGS=
ZLIB_LFLAGS=
EOF
emmake make clean
emmake make

cd $WORKSPACE
[ -d  instead-em-js ] ||  mkdir instead-em-js 
[ -d  instead-em-js/fs ] || mkdir instead-em-js/fs
cp -R instead-em/icon instead-em-js/fs/
cp -R instead-em/stead instead-em-js/fs/
cp -R instead-em/themes instead-em-js/fs/
cp -R instead-em/lang instead-em-js/fs/
cp -R instead-em/games instead-em-js/fs/
find instead-em-js/fs/ \( -name '*.svg' -o -name Makefile -o -name CMakeLists.txt \) -exec rm {} \;
cd instead-em-js
ln -f -s ../instead-em/src/sdl-instead sdl-instead.bc
ln -f -s ../lib lib

emcc -O2 sdl-instead.bc lib/libz.a lib/libiconv.so lib/liblua.a lib/libSDL2_mixer.a lib/libmodplug.a lib/libmad.a -s USE_OGG=1 -s USE_VORBIS=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2  -o project.html -s SAFE_HEAP=0  -s TOTAL_MEMORY=167772160 -s ALLOW_MEMORY_GROWTH=0 --use-preload-plugins --preload-file fs@/
echo "Happy hacking"
python2.7 -m SimpleHTTPServer 8000
