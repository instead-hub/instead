include ../Rules.make
include ../config.make

clean:
all:
install:
	$(INSTALLD) $(ICONPATH)
	$(INSTALL) sdl_instead.png $(ICONPATH)
ifeq ($(WINDOWS), yes)
	$(INSTALL) sdl_instead_1x32.png $(ICONPATH)/sdl_instead_1.png
else
	$(INSTALL) sdl_instead_1.png $(ICONPATH)
endif
	$(INSTALL) sdl_instead.ico $(ICONPATH)

uninstall:
	$(RM) $(ICONPATH)/sdl_instead.png
	$(RM) $(ICONPATH)/sdl_instead_1.png
	$(RM) $(ICONPATH)/sdl_instead.ico
