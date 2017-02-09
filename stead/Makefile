include ../Rules.make
include ../config.make

clean:
all:	stead2/stead.lua stead3/stead.lua

install:
	$(INSTALLD) $(DESTDIR)$(STEADPATH)

	for api in stead2 stead3; do \
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
