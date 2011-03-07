include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(LANGPATH)
	for f in *.ini; do \
		$(INSTALL) $$f $(LANGPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(LANGPATH)

