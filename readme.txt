Deps:
sdl, sdl-mixer, sdl-image, sdl-ttf, lua5.1

On debian systems:
apt-get install liblua5.1-dev libreadline-dev libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libreadline5-dev

make 
make install
sdl-instead

To run just from build dir use:

rm Rules.make
ln -s Rules.make.standalone Rules.make
make clean
make
./sdl-instead


