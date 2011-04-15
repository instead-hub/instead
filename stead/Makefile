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
	$(INSTALL) snapshots.lua $(STEADPATH)/snapshots.lua
	$(INSTALL) object.lua $(STEADPATH)/object.lua
	$(INSTALL) format.lua $(STEADPATH)/format.lua
	$(INSTALL) dash.lua $(STEADPATH)/dash.lua
	$(INSTALL) para.lua $(STEADPATH)/para.lua
	$(INSTALL) quotes.lua $(STEADPATH)/quotes.lua
	$(INSTALL) timer.lua $(STEADPATH)/timer.lua
	$(INSTALL) kbd.lua $(STEADPATH)/kbd.lua
	$(INSTALL) hotkeys.lua $(STEADPATH)/hotkeys.lua
	$(INSTALL) hideinv.lua $(STEADPATH)/hideinv.lua
	$(INSTALL) theme.lua $(STEADPATH)/theme.lua
	$(INSTALL) sprites.lua $(STEADPATH)/sprites.lua
	$(INSTALL) sound.lua $(STEADPATH)/sound.lua

uninstall:
	$(RM) $(STEADPATH)/stead.lua
	$(RM) $(STEADPATH)/gui.lua

