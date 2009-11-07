include ../Rules.make
clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.jpg *.html *.txt $(DOCPATH)
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*

