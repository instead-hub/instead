INSTEAD 1.4.4
=============

WARNING! For successfull building you must install these development packages (names may vary in your distribution):

	sdl, sdl-mixer, sdl-image, sdl-ttf, lua5.1

0) Prepare for building
=======================

Unpack source package with this command:
	$ tar xzvf instead_<version>.tar.gz

Change current dir to project's build dir:
	$ cd instead-<version>

There are several ways to build package.

1) On Debian based systems you can build deb package
====================================================
	$ sudo apt-get install liblua5.1-dev libreadline-dev libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-mixer1.2-dev
	$ sudo apt-get install build-essential debhelper
	$ debuild
	$ cd ..
	$ sudo dpkg -i instead_<version>.deb

2) On gentoo or arch linux 
==========================
Install stable version from AUR:
	$ yaourt -S instead

Or build from svn:
	$ make PKGBUILD
	$ makepkg -s

And, I think, you have enough skills to build package yourself. ;)

3) For other distributions 
==========================
You have a two choices:
	a) try run ./configure.sh and follow instructions
or
	b) do all things manually

4) Manual build
===============

To build game, that runs just from build dir use Rules.make.standalone
----------------------------------------------------------------------
	$ rm -f Rules.make
	$ ln -s Rules.make.standalone Rules.make
	$ make clean
	$ make

If build process has failed check Rules.make.standalone and make changes if needed.

	$ ./sdl-instead

To build game for system install
--------------------------------
	$ rm -f Rules.make
	$ ln -s Rules.make.system Rules.make
	$ make clean
	$ make

If build process has failed check Rules.make.system and make changes if needed.

	$ sudo make install

	$ sdl-instead

Enjoy.
