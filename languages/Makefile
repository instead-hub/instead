include ../Rules.make
clean:
all:
install:
	install -d -m0755 $(LANGPATH)
	for f in *.ini; do \
		install -m0644 $$f $(LANGPATH)/$$f;\
	done
