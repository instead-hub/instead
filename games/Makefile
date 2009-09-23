include ../Rules.make
all:
install:
ifeq ($(SYSTEMSETUP), yes)
	$(INSTALLD) $(GAMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(GAMESPATH)/$$f; \
		tar --exclude=".svn" -c -C $$f . | tar -x -C $(GAMESPATH)/$$f;\
	done
endif

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) -rf $(GAMESPATH)
else
uninstall:
endif

clean:
