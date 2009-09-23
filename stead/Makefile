include ../Rules.make
clean:
all:	stead.lua

install:
	$(INSTALLD) $(STEADPATH) 
	$(INSTALL) stead.lua $(STEADPATH)/stead.lua
	$(INSTALL) gui.lua $(STEADPATH)/gui.lua

uninstall:
	$(RM) $(STEADPATH)/stead.lua
	$(RM) $(STEADPATH)/gui.lua

