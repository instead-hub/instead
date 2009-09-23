include ../Rules.make
clean:
all:
install:
	$(INSTALLD) $(ICONPATH)
	$(INSTALL) sdl_instead.png $(ICONPATH)

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) $(ICONPATH)/sdl_instead.png
else
uninstall:
endif
