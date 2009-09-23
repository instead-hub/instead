include ../Rules.make
clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.jpg *.html *.txt $(DOCPATH)
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*
else
uninstall:
endif
