#!/usr/bin/env bash
# build INSTEAD with emscripten

set -e
export WORKSPACE="" #"/home/peter/Devel/emsdk-portable/env"

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

# libmikmod
cd $WORKSPACE
rm -rf libmikmod-3.1.12/
[ -f SDL2_mixer-2.0.1.tar.gz ] || wget -nv https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz
tar xf SDL2_mixer-2.0.1.tar.gz
mv SDL2_mixer-2.0.1/external/libmikmod-3.1.12/ libmikmod-3.1.12/
cd libmikmod-3.1.12/
emconfigure ./configure --prefix=$WORKSPACE --disable-shared --enable-static 
emmake make install SHELL="${SHELL}"

# SDL2_mixer
cd $WORKSPACE
rm -rf SDL_mixer
[ -d SDL_mixer/.hg ] || hg clone https://hg.libsdl.org/SDL_mixer
cd SDL_mixer
hg pull -u
hg up -C
hg --config "extensions.purge=" purge --all

cat configure.in | sed -e 's/AC_CHECK_LIB(\[modplug\], /AC_CHECK_LIB(\[modplug\], \[ModPlug_Load\], /' -e 's/have_libmikmod=no/have_libmikmod=yes/g' > configure.in.new
mv -f configure.in.new configure.in
./autogen.sh
emconfigure ./configure --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -s USE_VORBIS=1 -s USE_OGG=1" LDFLAGS="-L$WORKSPACE/lib" --disable-sdltest --disable-shared \
   --disable-music-mp3-mad-gpl --enable-music-ogg --disable-music-ogg-shared --enable-music-mod-mikmod --disable-music-mod-mikmod-shared \
   --disable-music-midi-fluidsynth --disable-music-midi-fluidsynth-shared \
   --disable-music-mp3-smpeg --disable-music-mp3-smpeg-shared

cat Makefile | sed -e 's| \$(objects)/playwave\$(EXE) \$(objects)/playmus\$(EXE)||g' > Makefile.new
mv -f Makefile.new Makefile
emmake make install

# jpeg lib
cd $WORKSPACE
rm -rf jpeg-9b
[ -f jpegsrc.v9b.tar.gz ] || wget -nv 'http://www.ijg.org/files/jpegsrc.v9b.tar.gz'
tar xf jpegsrc.v9b.tar.gz
cd jpeg-9b
emconfigure ./configure --prefix=$WORKSPACE
emmake make install

# SDL_image
cd $WORKSPACE
rm -rf SDL2_image
[ -d SDL2_image/.git ] || git clone https://github.com/emscripten-ports/SDL2_image.git SDL2_image
cd SDL2_image
./autogen.sh
emconfigure ./configure --host=asmjs-unknown-linux --prefix=$WORKSPACE CPPFLAGS="-I$WORKSPACE/include -s USE_SDL=2 -s USE_LIBPNG=1 " LDFLAGS="-L$WORKSPACE/lib -lpng -ljpeg" --disable-sdltest --disable-shared --enable-static --enable-png --disable-png-shared --enable-jpg --disable-jpg-shared
emmake make install

# INSTEAD
echo "INSTEAD"
cd $WORKSPACE
[ -d instead-em ] || git clone https://github.com/instead-hub/instead.git instead-em
cd instead-em
git pull
[ -e Rules.make ] || ln -s Rules.standalone Rules.make
cat <<EOF > config.make
EXTRA_CFLAGS+= -DNOMAIN -D_HAVE_ICONV -I../../include
SDL_CFLAGS=-I../../include/SDL2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s SDL2_IMAGE_FORMATS='["png","jpeg","gif"]'
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
			var args = [];
			[ "instead-em", url, "-standalone" ].forEach(function(item) {
				args.push(allocate(intArrayFromString(item), 'i8', ALLOC_NORMAL));
				args.push(0); args.push(0); args.push(0);
			})
			args = allocate(args, 'i32', ALLOC_NORMAL);
			setTimeout(function() {
				Module.setStatus('');
			}, 3);
			Module.ccall('instead_main', 'number', ["number", "number"], [3, args ]);
		});
	}
	req.send(null);
});
EOF

emcc -O2 sdl-instead.bc lib/libz.a lib/libiconv.so lib/liblua.a lib/libSDL2_mixer.a lib/libmikmod.a lib/libSDL2_image.a lib/libjpeg.a  \
-s EXPORTED_FUNCTIONS="['_instead_main']" \
-s 'SDL2_IMAGE_FORMATS=["png","jpeg","gif"]' \
-s QUANTUM_SIZE=4 \
-s USE_OGG=1 -s USE_VORBIS=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_LIBPNG=1 \
-o instead-em.html -s SAFE_HEAP=0  -s TOTAL_MEMORY=167772160 -s ALLOW_MEMORY_GROWTH=0 \
--post-js post.js  \
--preload-file fs@/

echo "Happy hacking"
python2.7 -m SimpleHTTPServer 8000
