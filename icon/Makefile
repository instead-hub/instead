include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(DESTDIR)$(ICONPATH)
	$(INSTALL) sdl_instead.png $(DESTDIR)$(ICONPATH)
ifeq ($(WINDOWS), yes)
	$(INSTALL) sdl_instead_1x32.png $(DESTDIR)$(ICONPATH)/sdl_instead_1.png
	$(INSTALL) sdl_instead.ico $(DESTDIR)$(ICONPATH)
else
	$(INSTALL) sdl_instead_1.png $(DESTDIR)$(ICONPATH)
endif

uninstall:
	$(RM) $(DESTDIR)$(ICONPATH)/sdl_instead.png
	$(RM) $(DESTDIR)$(ICONPATH)/sdl_instead_1.png
	$(RM) $(DESTDIR)$(ICONPATH)/sdl_instead.ico
