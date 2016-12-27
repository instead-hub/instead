include ../Rules.make
include ../config.make

clean:
all:	classic/stead.lua

install:
	$(INSTALLD) $(DESTDIR)$(STEADPATH)

	for api in classic; do \
		$(INSTALLD) $(DESTDIR)$(STEADPATH)/$$api; \
		$(INSTALLD) $(DESTDIR)$(STEADPATH)/$$api/ext; \
		for f in $$api/*.lua; do \
			$(INSTALL) $$f $(DESTDIR)$(STEADPATH)/$$f || exit 1; \
		done; \
		for f in $$api/ext/*.lua; do \
			$(INSTALL) $$f $(DESTDIR)$(STEADPATH)/$$f || exit 1; \
		done; \
	done

uninstall:

