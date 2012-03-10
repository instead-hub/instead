include ../Rules.make
include ../config.make

all:
install:
	$(INSTALLD) $(GAMESPATH)
	for f in *; do \
		echo $$f;\
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(GAMESPATH)/$$f; \
		tar --exclude=".svn" -cf - -C $$f . | tar -xpf - -C $(GAMESPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(GAMESPATH)

clean:
