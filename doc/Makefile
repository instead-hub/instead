include ../Rules.make
pdf:
	pdflatex manual.tex
	pdflatex manual.tex
	$(RM) manual.aux manual.log manual.toc manual.out
clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.jpg *.html *.txt *.pdf $(DOCPATH)
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*
