Deps:
	sdl, sdl-mixer, sdl-image, sdl-ttf, lua5.1

To run just from build dir use:

	rm -f Rules.make
	ln -s Rules.make.standalone Rules.make
	make clean
	make
	./sdl-instead

To install into system:

	# as root
	rm -f Rules.make
	ln -s Rules.make.system Rules.make
	make clean
	make install
	sdl-instead

On debian systems:
	apt-get install liblua5.1-dev libreadline-dev libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libreadline5-dev

building deb package:
	apt-get install build-essential
	debuild

***

Or you can try ./configure.sh.
