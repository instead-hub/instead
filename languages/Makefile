include ../Rules.make
clean:
all:
install:
	$(INSTALLD) $(LANGPATH)
	for f in *.ini; do \
		$(INSTALL) $$f $(LANGPATH)/$$f;\
	done

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) -rf $(LANGPATH)
else
uninstall:
endif
