include ../Rules.make
clean:
all:
install:
	install -d -m0755 $(THEMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		install -d -m0755 $(THEMESPATH)/$$f;\
		tar --exclude=".svn" -c -C $$f . | tar -x -C $(THEMESPATH)/$$f;\
	done

