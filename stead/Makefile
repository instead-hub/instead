include ../Rules.make
include ../config.make

clean:
all:	stead.lua

install:
	$(INSTALLD) $(STEADPATH) 
	$(INSTALL) stead.lua $(STEADPATH)/stead.lua
	$(INSTALL) gui.lua $(STEADPATH)/gui.lua
	$(INSTALL) goto.lua $(STEADPATH)/goto.lua
	$(INSTALL) xact.lua $(STEADPATH)/xact.lua
	$(INSTALL) input.lua $(STEADPATH)/input.lua
	$(INSTALL) click.lua $(STEADPATH)/click.lua
	$(INSTALL) dbg.lua $(STEADPATH)/dbg.lua
	$(INSTALL) vars.lua $(STEADPATH)/vars.lua
	$(INSTALL) prefs.lua $(STEADPATH)/prefs.lua
	$(INSTALL) snapshot.lua $(STEADPATH)/snapshot.lua
	$(INSTALL) object.lua $(STEADPATH)/object.lua
	$(INSTALL) para.lua $(STEADPATH)/para.lua

uninstall:
	$(RM) $(STEADPATH)/stead.lua
	$(RM) $(STEADPATH)/gui.lua

