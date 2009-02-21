include ../Rules.make
all:
install:
	install -d -m0755 $(GAMESPATH)
	for f in *; do \
		if [ ! -d $$f ]; then continue; fi;\
		install -d -m0755 $(GAMESPATH)/$$f;\
		tar -c -C $$f . | tar -x -C $(GAMESPATH)/$$f;\
	done
clean:
