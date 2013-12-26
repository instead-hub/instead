include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(DESTDIR)$(LANGPATH)
	for f in *.ini; do \
		$(INSTALL) $$f $(DESTDIR)$(LANGPATH)/$$f;\
	done

uninstall:
	$(RM) -rf $(DESTDIR)$(LANGPATH)

