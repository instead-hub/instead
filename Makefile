include Rules.make
include config.make

VERTITLE := instead-$(VERSION)
ARCHIVE := instead_$(VERSION).tar.gz#

SUBDIRS += src/ stead games themes icon desktop doc lang

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $(@) || exit 1; \
	done;

test: all
	tests/run.sh

Rules.make:
	ln -sf Rules.make.standalone Rules.make

config.make:
	echo "# you can define own flags here" > config.make

gitclean:
	{ test -d .git && git status -s; } | grep "^??" | awk '{ print $$2 }' | grep -v "config.make" | grep -v "^doc/.*\.pdf$$" | while read l; do $(RM) -rf $$l; done

rules:
	$(RM) -f Rules.make
	ln -sf Rules.make.standalone Rules.make

docs: rules
	make docs -C doc/

tarball: clean gitclean rules
	echo "# you can define own flags here" > config.make
	ln -sf ./ $(VERTITLE)
	tar -cz --exclude $(VERTITLE)/$(VERTITLE) --exclude .git --exclude $(ARCHIVE) -f $(ARCHIVE) $(VERTITLE)/*
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
