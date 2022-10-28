#!/usr/bin/env bash
# build INSTEAD with emscripten

set -e
# export WORKSPACE="/home/peter/Devel/emsdk/env"

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

# some general flags
export PATH="$WORKSPACE/bin:$PATH"
export CFLAGS="-g0 -O2"
export CXXFLAGS="$CFLAGS"
export EM_CFLAGS="-Wno-warn-absolute-paths"
export EMCC_CFLAGS="$EM_CFLAGS"
export PKG_CONFIG_PATH="$WORKSPACE/lib/pkgconfig"
export MAKEFLAGS="-j2"

# flags to fake emconfigure and emmake
export CC="emcc"
export CXX="em++"
export LD="$CC"
export LDSHARED="$LD"
export RANLIB="emranlib"
export AR="emar"
export CC_BUILD=cc

deps()
{

# Lua
cd $WORKSPACE
if ! test -r .stamp_lua; then
rm -rf lua-5.1.5
[ -f lua-5.1.5.tar.gz ] || wget -nv 'https://www.lua.org/ftp/lua-5.1.5.tar.gz'
tar xf lua-5.1.5.tar.gz
cd lua-5.1.5
cat src/luaconf.h | sed -e 's/#define LUA_USE_POPEN//g' -e 's/#define LUA_USE_ULONGJMP//g'>src/luaconf.h.new
mv src/luaconf.h.new src/luaconf.h
emmake make posix CC=emcc 
emmake make install INSTALL_TOP=$WORKSPACE 
touch ../.stamp_lua
fi

# libiconv
cd $WORKSPACE
if ! test -r .stamp_iconv; then
rm -rf libiconv-1.15
[ -f libiconv-1.15.tar.gz ] || wget -nv 'https://ftp.gnu.org/gnu/libiconv/libiconv-1.15.tar.gz'
tar xf libiconv-1.15.tar.gz
cd libiconv-1.15
emconfigure ./configure --prefix=$WORKSPACE
emmake make install
touch ../.stamp_iconv
fi

# zlib
cd $WORKSPACE
if ! test -r .stamp_zlib; then
rm -rf zlib-1.2.13/
[ -f zlib-1.2.13.tar.gz ] || wget -nv 'http://zlib.net/zlib-1.2.13.tar.gz'
tar xf zlib-1.2.13.tar.gz
cd zlib-1.2.13
emconfigure ./configure --prefix=$WORKSPACE
emmake make install
touch ../.stamp_zlib
fi

# libmikmod
cd $WORKSPACE
if ! test -r .stamp_libmikmod; then
rm -rf libmikmod-3.1.12/
[ -f SDL2_mixer-2.0.1.tar.gz ] || wget -nv https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz
tar xf SDL2_mixer-2.0.1.tar.gz
mv SDL2_mixer-2.0.1/external/libmikmod-3.1.12/ libmikmod-3.1.12/
cd libmikmod-3.1.12/
emconfigure ./configure --prefix=$WORKSPACE --disable-shared --enable-static 
emmake make install SHELL="${SHELL}"
touch ../.stamp_libmikmod
fi

# freetype2

cd $WORKSPACE
if ! test -r .stamp_ft2; then
rm -rf freetype-2.8
wget --no-check-certificate https://download.savannah.gnu.org/releases/freetype/freetype-2.8.tar.gz
tar -xvf freetype-2.8.tar.gz
cd freetype-2.8
./autogen.sh

emconfigure ./configure --build=amd64-unknown-linux --host=asmjs-linux --prefix=$WORKSPACE  CPPFLAGS="-I$WORKSPACE/include" LDFLAGS="-L$WORKSPACE/lib" --disable-shared
emmake make install
touch ../.stamp_ft2
fi

# SDL2_ttf
cd $WORKSPACE
if ! test -r .stamp_sdl2_ttf; then
rm -rf SDL2_ttf
git clone https://github.com/emscripten-ports/SDL2_ttf.git
cd SDL2_ttf
git checkout master
git pull
sed -i -e 's/noinst_PROGRAMS = showfont glfont//' Makefile.am
./autogen.sh
emconfigure ./configure --build=amd64-unknown-linux --host=asmjs-unknown-linux --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -I$WORKSPACE/include/SDL2 -I$WORKSPACE/include/freetype2" LDFLAGS="-L$WORKSPACE/lib" --disable-sdltest --disable-shared CFLAGS="-sUSE_SDL=2"
emmake make install
touch ../.stamp_sdl2_ttf
fi


# SDL2_mixer
cd $WORKSPACE
if ! test -r .stamp_sdl2_mixer; then
rm -rf SDL_mixer-2.0.2
[ -f SDL2_mixer-2.0.2.tar.gz ] || wget -nv https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.2.tar.gz
tar xf SDL2_mixer-2.0.2.tar.gz && cd SDL2_mixer-2.0.2

cat configure.in | sed -e 's/AC_CHECK_LIB(\[modplug\], /AC_CHECK_LIB(\[modplug\], \[ModPlug_Load\], /' -e 's/have_libmikmod=no/have_libmikmod=yes/g' > configure.in.new
mv -f configure.in.new configure.in
./autogen.sh

export have_ogg_lib=yes
export have_ogg_h=yes
export have_libmikmod=yes
emconfigure ./configure --host=asmjs-unknown-emscripten --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -I$WORKSPACE/include/SDL2 -s USE_VORBIS=1 -s USE_OGG=1" LDFLAGS="-L$WORKSPACE/lib" --disable-sdltest --disable-shared \
   --disable-music-mp3-mad-gpl --enable-music-ogg --disable-music-ogg-shared --enable-music-mod-mikmod --disable-music-mod-mikmod-shared \
   --disable-music-midi-fluidsynth --disable-music-midi-fluidsynth-shared \
   --disable-music-mp3-smpeg --disable-music-mp3-smpeg-shared CFLAGS="-sUSE_SDL=2"

cat Makefile | sed -e 's| \$(objects)/playwave\$(EXE) \$(objects)/playmus\$(EXE)||g' > Makefile.new
mv -f Makefile.new Makefile
emmake make
emmake make install
touch ../.stamp_sdl2_mixer
fi

# jpeg lib
cd $WORKSPACE
if ! test -r .stamp_jpeg; then
rm -rf jpeg-9b
[ -f jpegsrc.v9b.tar.gz ] || wget -nv 'http://www.ijg.org/files/jpegsrc.v9b.tar.gz'
tar xf jpegsrc.v9b.tar.gz
cd jpeg-9b
emconfigure ./configure --prefix=$WORKSPACE --disable-shared
emmake make install
touch ../.stamp_jpeg
fi

# SDL_image
cd $WORKSPACE
if ! test -r .stamp_sdl2_image; then
rm -rf SDL2_image
[ -d SDL2_image/.git ] || git clone https://github.com/emscripten-ports/SDL2_image.git SDL2_image
cd SDL2_image
./autogen.sh
export ac_cv_lib_jpeg_jpeg_CreateDecompress=yes
export ac_cv_lib_png_png_create_read_struct=yes
emconfigure ./configure --host=asmjs-unknown-linux --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -I$WORKSPACE/include/SDL2 -s USE_LIBPNG=1" LDFLAGS="-L$WORKSPACE/lib -lpng -ljpeg" --disable-sdltest --disable-shared --enable-static --enable-png --disable-png-shared --enable-jpg --disable-jpg-shared CFLAGS="-sUSE_SDL=2"
emmake make install
touch ../.stamp_sdl2_image
fi

}

deps

# INSTEAD
echo "INSTEAD"
cd $WORKSPACE
[ -d instead-em ] || git clone https://github.com/instead-hub/instead.git instead-em
cd instead-em
git pull
[ -e Rules.make ] || ln -s Rules.standalone Rules.make
cat <<EOF > config.make
EXTRA_CFLAGS+= -DNOMAIN -D_HAVE_ICONV -I../../include
SDL_CFLAGS=-I../../include/SDL2 -sUSE_SDL=2
SDL_LFLAGS=
LUA_CFLAGS=
LUA_LFLAGS=
ZLIB_LFLAGS=
EOF
emmake make clean
sed -i -e 's/^EXE=$/EXE=.bc/' Rules.make.standalone
LDFLAGS=-r emmake make

cd $WORKSPACE
[ -d  instead-em-js ] ||  mkdir instead-em-js 
[ -d  instead-em-js/fs ] || mkdir instead-em-js/fs
cp -R instead-em/icon instead-em-js/fs/
cp -R instead-em/stead instead-em-js/fs/
cp -R instead-em/themes instead-em-js/fs/
cp -R instead-em/lang instead-em-js/fs/
cp -R instead-em/games instead-em-js/fs/
rm -rf instead-em-js/fs/games # without games
find instead-em-js/fs/ \( -name '*.svg' -o -name Makefile -o -name CMakeLists.txt \) -exec rm {} \;

cat <<EOF > post.js
var Module;
FS.mkdir('/appdata');
FS.mount(IDBFS,{},'/appdata');

Module['postRun'].push(function() {
	var argv = []
	var req
	if (typeof window === "object") {
		argv = window.location.search.substr(1).trim().split('&');
		if (!argv[0])
			argv = [];
	}
	var url = argv[0];
	if (!url) {
		FS.syncfs(true, function (error) {
			if (error) {
				console.log("Error while syncing: ", error);
			};
			console.log("Running...");
			Module.ccall('instead_main', 'number');
		});
		return;
	}

	req = new XMLHttpRequest();
	req.open("GET", url, true);
	req.responseType = "arraybuffer";
	console.log("Get: ", url);

	setTimeout(function() {
		var spinnerElement = document.getElementById('spinner');
		spinnerElement.style.display = 'inline-block';
		Module['setStatus']('Downloading data file...');
	}, 3);
	req.onload = function() {
		var basename = function(path) {
			parts = path.split( '/' );
			return parts[parts.length - 1];
		}
		var data = req.response;
		console.log("Data loaded...");
		FS.syncfs(true, function (error) {
			if (error) {
				console.log("Error while syncing: ", error);
			}
			url = basename(url);
			console.log("Writing: ", url);
			FS.writeFile(url, new Int8Array(data), { encoding: 'binary' }, "w");
			console.log("Running...");
			var args = stackAlloc(7 * 4);
			args[6] = 0;
			[ "instead-em", url, "-standalone", "-window", "-resizable", "-mode" ].forEach(function(item, i) {
				HEAP32[(args >> 2) + i] = allocateUTF8OnStack(item);
			})
			setTimeout(function() {
				Module.setStatus('');
				document.getElementById('status').style.display = 'none';
			}, 3);
			window.onclick = function(){ window.focus() };
			Module.ccall('instead_main', 'number', ["number", "number"], [6, args ]);
		});
	}
	req.send(null);
});
EOF

unzip -o -j instead-em/contrib/instead-em.zip -d instead-em-js/

cd instead-em-js
ln -f -s ../instead-em/src/sdl-instead.bc sdl-instead.bc
ln -f -s ../lib lib

emcc -O2 sdl-instead.bc lib/libz.a lib/libiconv.so lib/liblua.a lib/libSDL2_ttf.a  lib/libfreetype.a lib/libSDL2_mixer.a lib/libmikmod.a  lib/libSDL2_image.a lib/libjpeg.a  \
-lidbfs.js \
-s EXPORTED_FUNCTIONS="['_instead_main']" \
-s 'SDL2_IMAGE_FORMATS=["png","jpeg","gif"]' \
-s 'EXPORTED_RUNTIME_METHODS=["ccall", "Pointer_stringify"]' \
-s 'DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=["$autoResumeAudioContext", "$dynCall"]' \
-s QUANTUM_SIZE=4 \
-s WASM=1 \
-s PRECISE_F32=1 \
-s USE_OGG=1 -s USE_VORBIS=1 -s USE_LIBPNG=1 -s USE_SDL=2 \
-o instead-em.html -s SAFE_HEAP=0  -s TOTAL_MEMORY=167772160 -s ALLOW_MEMORY_GROWTH=1 \
--post-js post.js  \
--preload-file fs@/

echo "Happy hacking"
# python2.7 -m SimpleHTTPServer 8000
