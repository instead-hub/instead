include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(THEMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(THEMESPATH)/$$f;\
		tar --exclude=".svn" -cf - -C $$f . | tar -xpf - -C $(THEMESPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(THEMESPATH)

