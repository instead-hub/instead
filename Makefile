include Rules.make
include config.make

VERTITLE := instead-$(VERSION)
ARCHIVE := instead_$(VERSION).tar.gz#

SUBDIRS += src/ stead games themes icon desktop doc lang

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $(@) || exit 1; \
	done;

Rules.make:
	ln -sf Rules.make.standalone Rules.make

config.make:
	echo "# you can define own flags here" > config.make

svnclean:
	{ test -d .svn && svn st; } | grep "^?" | awk '{ print $$2 }' | grep -v "config.make" | while read l; do $(RM) -rf $$l; done

gitclean:
	{ test -d .git && git status -s; } | grep "^??" | awk '{ print $$2 }' | grep -v "config.make" | while read l; do $(RM) -rf $$l; done

rules:
	$(RM) -f Rules.make
	ln -sf Rules.make.standalone Rules.make

docs: rules
	make docs -C doc/

PKGBUILD: PKGBUILD.in tarball
	cat PKGBUILD.in | sed -e s/MD5SUM/`md5sum $(ARCHIVE) | cut -f1 -d' '`/g > PKGBUILD

windows-sdl2.tar.gz:
	# wget "http://sourceforge.net/projects/instead/files/instead/build-bin-data/windows-sdl2-2.0.24.tar.gz/download" -O $(@)
	# wget "http://sourceforge.net/projects/instead/files/instead/build-bin-data/windows-sdl2-2.0.22.tar.gz/download" -O $(@)
	wget "http://sourceforge.net/projects/instead/files/instead/build-bin-data/windows-sdl2-2.0.14.tar.gz/download" -O $(@)

build-bin-data: windows-sdl2.tar.gz

tarball: clean svnclean gitclean docs rules
	echo "# you can define own flags here" > config.make
	ln -sf ./ $(VERTITLE)
	tar -cz --exclude $(VERTITLE)/$(VERTITLE) --exclude CJK.zip --exclude .git --exclude .svn --exclude $(ARCHIVE) --exclude windows-sdl2.tar.gz -f $(ARCHIVE) $(VERTITLE)/*
	$(RM) -f $(VERTITLE)

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir $(@) || exit 1; \
	done;
	$(RM) -f $(ARCHIVE)
	$(RM) -f PKGBUILD

install: all
ifeq ($(SYSTEMSETUP), yes)
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install || exit 1; \
	done;
else
	echo No install needed
endif

uninstall:
ifeq ($(SYSTEMSETUP), yes)
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir uninstall || exit 1; \
	done;
	$(RM) -rf $(STEADPATH)
else
	echo No uninstall needed
endif
