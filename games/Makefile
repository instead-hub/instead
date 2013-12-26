include ../Rules.make
include ../config.make

all:
install:
	$(INSTALLD) $(DESTDIR)$(GAMESPATH)
	for f in *; do \
		echo $$f;\
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(DESTDIR)$(GAMESPATH)/$$f; \
		tar --exclude=".svn" -cf - -C $$f . | tar -xpf - -C $(DESTDIR)$(GAMESPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(DESTDIR)$(GAMESPATH)

clean:
