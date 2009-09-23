include ../Rules.make
clean:
	rm -rf instead.desktop

all: instead.desktop

instead.desktop: instead.desktop.in
	cat instead.desktop.in | sed -e "s|@BIN|$(BIN)|g" > instead.desktop

install:
	$(INSTALLD) $(DESTDIR)$(PREFIX)/share/applications
	$(INSTALL) instead.desktop $(DESTDIR)$(PREFIX)/share/applications/instead.desktop

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/share/applications/instead.desktop
else
uninstall:
endif
