include ../Rules.make
clean:
	rm -rf instead.desktop

all: instead.desktop

instead.desktop: instead.desktop.in
	cat instead.desktop.in | sed -e "s|@BIN|$(BIN)|g" > instead.desktop

install:
	install -d -m 0755 $(DESTDIR)$(PREFIX)/share/applications
	install -m 0644 instead.desktop $(DESTDIR)$(PREFIX)/share/applications/instead.desktop

