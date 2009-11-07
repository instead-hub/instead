include ../Rules.make
clean:
	rm -f ./dev_manual/manual.toc ./dev_manual/manual.out ./dev_manual/manual.log ./dev_manual/manual.aux
all:
	pdflatex -output-directory ./dev_manual dev_manual/manual.tex
	pdflatex -output-directory ./dev_manual dev_manual/manual.tex
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.jpg *.html *.txt dev_manual/*.pdf $(DOCPATH)
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*

