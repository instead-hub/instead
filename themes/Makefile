include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(DESTDIR)$(THEMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(DESTDIR)$(THEMESPATH)/$$f;\
		tar --exclude=".svn" -cf - -C $$f . | tar -xpf - -C $(DESTDIR)$(THEMESPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(DESTDIR)$(THEMESPATH)

