include ../Rules.make
include ../config.make

all:
clean:

pdf:
	./makepdfs
#	# try xetex here, and hope it works ;)
#	pdflatex reference.tex
#	makeindex reference.idx
#	pdflatex reference.tex
#	$(RM) -f reference.aux reference.log reference.toc reference.out reference.idx reference.ind reference.ilg
#to-do: interpret $PATH
man:
	man ./instead.6 | col -b > instead.txt

wiki:
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:documentation&do=export_xhtml" -O instead-manual.html
	wget "http://instead.syscall.ru/wiki/doku.php?id=en:gamedev:documentation&do=export_xhtml" -O instead-manual-en.html

docs:	wiki man pdf

install:
	$(INSTALLD) $(DESTDIR)$(DOCPATH)
	-$(INSTALL) instead.txt *.html *.pdf $(DESTDIR)$(DOCPATH)

	$(INSTALLD) $(DESTDIR)$(MANPATH)
	$(INSTALL) *.6 $(DESTDIR)$(MANPATH)

uninstall:
	$(RM) -rf $(DESTDIR)$(DOCPATH)
	$(RM) $(DESTDIR)$(MANPATH)/instead.*
