include Rules.make
include config.make

VERTITLE := instead-$(VERSION)
ARCHIVE := instead_$(VERSION).tar.gz

SUBDIRS = src/sdl-instead stead games themes icon desktop doc languages 

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $(@) || exit 1; \
	done;

Rules.make:
	ln -sf Rules.make.standalone Rules.make

config.make:
	echo "# you can define own flags here" > config.make

svnclean:
	svn st | grep "^?" | awk '{ print $$2 }' | grep -v "config.make" | while read l; do $(RM) -rf $$l; done

tarball: clean
	echo "# you can define own flags here" > config.make
	$(RM) -f Rules.make
	ln -sf Rules.make.standalone Rules.make
	ln -sf ./ $(VERTITLE)
	tar -cz --exclude fantasy --exclude $(VERTITLE)/$(VERTITLE) --exclude .svn --exclude $(ARCHIVE) -f $(ARCHIVE) $(VERTITLE)/*
	$(RM) -f $(VERTITLE)

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir $(@) || exit 1; \
	done;
	$(RM) -f $(ARCHIVE)

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

