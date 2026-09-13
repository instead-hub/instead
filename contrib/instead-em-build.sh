#!/usr/bin/env bash
# build INSTEAD with emscripten (SDL3)

set -e
# export WORKSPACE="/home/peter/Devel/emsdk/env"
# export INSTEAD_SRC="/path/to/instead/source" (optional)

if [ ! -f ./emsdk_env.sh ]; then
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
. ./emsdk_env.sh

SDL3_IMAGE_VER=release-3.4.6
SDL3_MIXER_VER=release-3.2.4
LIBXMP_VER=libxmp-4.6.3

# some general flags
export CFLAGS="-g0 -O2"
export CXXFLAGS="$CFLAGS"
export MAKEFLAGS="-j$(nproc)"

deps()
{
# Lua
cd $WORKSPACE
if ! test -r .stamp_lua; then
	rm -rf lua-5.1.5
	[ -f lua-5.1.5.tar.gz ] || wget -nv 'https://www.lua.org/ftp/lua-5.1.5.tar.gz' || wget -nv 'https://www.tecgraf.puc-rio.br/lua/mirror/ftp/lua-5.1.5.tar.gz'
	tar xf lua-5.1.5.tar.gz
	cd lua-5.1.5
	cat src/luaconf.h | sed -e 's/#define LUA_USE_POPEN//g' -e 's/#define LUA_USE_ULONGJMP//g' > src/luaconf.h.new
	mv src/luaconf.h.new src/luaconf.h
	emmake make posix CC=emcc
	emmake make install INSTALL_TOP=$WORKSPACE
	touch ../.stamp_lua
fi

# emscripten ports
cd $WORKSPACE
if ! test -r .stamp_ports; then
	embuilder build sdl3 sdl3_ttf zlib
	touch .stamp_ports
fi

# cmake shim for SDL3 ports
mkdir -p $WORKSPACE/sdl3-cmake
cat > $WORKSPACE/sdl3-cmake/SDL3Config.cmake <<EOF
set(SDL3_VERSION "3.4.10")
set(SDL3_VERSION_STRING "3.4.10")
set(SDL3_FOUND TRUE)
set(SDL3_Headers_FOUND TRUE)
if(NOT TARGET SDL3::Headers)
  add_library(SDL3::Headers INTERFACE IMPORTED)
  set_target_properties(SDL3::Headers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "\$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include")
endif()
if(NOT TARGET SDL3::SDL3)
  add_library(SDL3::SDL3 INTERFACE IMPORTED)
  set_target_properties(SDL3::SDL3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "\$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include")
endif()
EOF
cat > $WORKSPACE/sdl3-cmake/SDL3ConfigVersion.cmake <<EOF
set(PACKAGE_VERSION "3.4.10")
if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)
  set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
  set(PACKAGE_VERSION_COMPATIBLE TRUE)
  if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
    set(PACKAGE_VERSION_EXACT TRUE)
  endif()
endif()
EOF

# SDL3_image
cd $WORKSPACE
if ! test -r .stamp_sdl3_image; then
	rm -rf SDL_image
	git clone --depth 1 -b $SDL3_IMAGE_VER https://github.com/libsdl-org/SDL_image.git SDL_image
	git -C SDL_image submodule update --init --depth 1 external/zlib external/libpng
	emcmake cmake -S SDL_image -B SDL_image/build -DSDL3_DIR=$WORKSPACE/sdl3-cmake \
		-DBUILD_SHARED_LIBS=OFF -DSDLIMAGE_VENDORED=ON -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF \
		-DSDLIMAGE_AVIF=OFF -DSDLIMAGE_JXL=OFF -DSDLIMAGE_SVG=OFF -DSDLIMAGE_TIF=OFF -DSDLIMAGE_WEBP=OFF
	cmake --build SDL_image/build
	cmake --install SDL_image/build --prefix $WORKSPACE
	touch .stamp_sdl3_image
fi

# libxmp
cd $WORKSPACE
if ! test -r .stamp_libxmp; then
	rm -rf libxmp
	git clone --depth 1 -b $LIBXMP_VER https://github.com/libxmp/libxmp.git libxmp
	emcmake cmake -S libxmp -B libxmp/build -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF
	cmake --build libxmp/build
	cmake --install libxmp/build --prefix $WORKSPACE
	touch .stamp_libxmp
fi

# SDL3_mixer
cd $WORKSPACE
if ! test -r .stamp_sdl3_mixer; then
	rm -rf SDL_mixer
	git clone --depth 1 -b $SDL3_MIXER_VER https://github.com/libsdl-org/SDL_mixer.git SDL_mixer
	emcmake cmake -S SDL_mixer -B SDL_mixer/build -DSDL3_DIR=$WORKSPACE/sdl3-cmake \
		-DBUILD_SHARED_LIBS=OFF -DSDLMIXER_VENDORED=OFF -DSDLMIXER_TESTS=OFF -DSDLMIXER_EXAMPLES=OFF \
		-DSDLMIXER_GME=OFF -DSDLMIXER_MOD=ON -DSDLMIXER_MOD_XMP_SHARED=OFF -Dlibxmp_DIR=$WORKSPACE/lib/cmake/libxmp \
		-DSDLMIXER_MIDI=OFF -DSDLMIXER_WAVPACK=OFF \
		-DSDLMIXER_FLAC_LIBFLAC=OFF -DSDLMIXER_VORBIS_VORBISFILE=OFF -DSDLMIXER_OPUS=OFF -DSDLMIXER_MP3_MPG123=OFF
	cmake --build SDL_mixer/build
	cmake --install SDL_mixer/build --prefix $WORKSPACE
	touch .stamp_sdl3_mixer
fi
}

deps

# INSTEAD
echo "INSTEAD"
SRC="$INSTEAD_SRC"
if [ -z "$SRC" ]; then
	cd $WORKSPACE
	[ -d instead-em ] || git clone https://github.com/instead-hub/instead.git instead-em
	cd instead-em
	git checkout .
	git pull
	SRC=$WORKSPACE/instead-em
fi

cd "$SRC"
[ -e Rules.make ] || ln -s Rules.make.standalone Rules.make
cat <<EOF > config.make
EXTRA_CFLAGS = -DNOMAIN -D_HAVE_ICONV -I$WORKSPACE/include
SDL_CFLAGS = -I$WORKSPACE/include -sUSE_SDL=3 -sUSE_SDL_TTF=3
SDL_LFLAGS =
LUA_CFLAGS = -I$WORKSPACE/include
LUA_LFLAGS =
ZLIB_CFLAGS = -sUSE_ZLIB=1
ZLIB_LFLAGS =
EOF
emmake make clean
emmake make EXE=.bc LDFLAGS=-r

cd $WORKSPACE
[ -d instead-em-js ] || mkdir instead-em-js
[ -d instead-em-js/fs ] || mkdir instead-em-js/fs
cp -R "$SRC"/icon instead-em-js/fs/
cp -R "$SRC"/stead instead-em-js/fs/
cp -R "$SRC"/themes instead-em-js/fs/
cp -R "$SRC"/lang instead-em-js/fs/
rm -rf instead-em-js/fs/games
find instead-em-js/fs/ \( -name '*.svg' -o -name Makefile -o -name CMakeLists.txt \) -exec rm {} \;

unzip -o -j "$SRC"/contrib/instead-em.zip -d instead-em-js/

cd instead-em-js
cp "$SRC"/src/sdl-instead.bc sdl-instead.o
emcc -O2 sdl-instead.o $WORKSPACE/lib/liblua.a $WORKSPACE/lib/libSDL3_image.a $WORKSPACE/lib/libpng16.a $WORKSPACE/lib/libSDL3_mixer.a $WORKSPACE/lib/libxmp.a \
	-sUSE_SDL=3 \
	-sUSE_SDL_TTF=3 \
	-sUSE_ZLIB=1 \
	-lidbfs.js \
	-sEXPORTED_FUNCTIONS='["_instead_main"]' \
	-sEXPORTED_RUNTIME_METHODS='["ccall"]' \
	-sWASM=1 \
	-sALLOW_MEMORY_GROWTH=1 \
	-s TOTAL_MEMORY=167772160 \
	--post-js post.js \
	--preload-file fs@/ \
	-o instead-em.js

echo "Happy hacking"
