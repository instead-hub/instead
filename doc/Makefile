include ../Rules.make
include ../config.make

pdf:
#	# try xetex here, and hope it works ;)
#	pdflatex reference.tex
#	makeindex reference.idx
#	pdflatex reference.tex
#	$(RM) -f reference.aux reference.log reference.toc reference.out reference.idx reference.ind reference.ilg
#to-do: interpret $PATH

wiki:
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:documentation&do=export_xhtml" -O instead-manual.html
	wget "http://instead.syscall.ru/wiki/doku.php?id=en:gamedev:documentation&do=export_xhtml" -O instead-manual-en.html
clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.txt *.html *.pdf $(DOCPATH)

	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*
