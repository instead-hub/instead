include ../Rules.make
include ../config.make

all:
install:
	$(INSTALLD) $(GAMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		$(INSTALLD) $(GAMESPATH)/$$f; \
		tar --exclude=".svn" -c -C $$f . | tar -x -C $(GAMESPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(GAMESPATH)

clean:
