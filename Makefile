include Rules.make

SUBDIRS = src/sdl-instead stead games themes icon desktop doc languages 

all:	config.make
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $(@) || exit 1; \
	done;

config.make:
	echo "# you can define own flags here" > config.make

include config.make

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir $(@) || exit 1; \
	done;

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
