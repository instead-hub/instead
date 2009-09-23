include ../Rules.make
clean:
all:
install:
ifeq ($(SYSTEMSETUP), yes)
	$(INSTALLD) $(THEMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(THEMESPATH)/$$f;\
		tar --exclude=".svn" -c -C $$f . | tar -x -C $(THEMESPATH)/$$f;\
	done
endif

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) -rf $(THEMESPATH)
else
uninstall:
endif
