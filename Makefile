#SUBDIRS = src/sdl-instead stead games themes icon desktop doc
SUBDIRS = src/sdl-instead stead games themes icon desktop doc

all:	
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $(@) || exit 1; \
	done;

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir $(@) || exit 1; \
	done;

install: all
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install || exit 1; \
	done;

	
