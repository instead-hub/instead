include ../Rules.make
include ../config.make

clean:
all:	stead.lua

install:
	$(INSTALLD) $(STEADPATH) 
	$(INSTALL) stead.lua $(STEADPATH)/stead.lua
	$(INSTALL) gui.lua $(STEADPATH)/gui.lua
	$(INSTALL) goto.lua $(STEADPATH)/goto.lua
	$(INSTALL) xobj.lua $(STEADPATH)/xobj.lua
	$(INSTALL) input.lua $(STEADPATH)/input.lua
	$(INSTALL) click.lua $(STEADPATH)/click.lua

uninstall:
	$(RM) $(STEADPATH)/stead.lua
	$(RM) $(STEADPATH)/gui.lua

