include ../Rules.make
include ../config.make

pdf:
	pdflatex manual.tex
	makeindex manual.idx
	pdflatex manual.tex
	$(RM) manual.aux manual.log manual.toc manual.out manual.idx manual.ind
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
