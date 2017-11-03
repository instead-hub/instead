include ../Rules.make
include ../config.make

all:

clean:
	rm -f *.pdf *.aux *.log *.out *.toc stead3-manual.tex modules3-manual.tex stead2.html stead2-en.html instead.txt

stead3-manual.tex: stead3.md
	multimarkdown stead3.md -t latex -o stead3-manual.tex

stead3.pdf: stead3.tex stead3-manual.tex
	xelatex stead3.tex
	xelatex stead3.tex
	xelatex stead3.tex

modules3-manual.tex: modules3.md
	multimarkdown modules3.md -t latex -o modules3-manual.tex

modules3.pdf: modules3.tex modules3-manual.tex
	xelatex modules3.tex
	xelatex modules3.tex
	xelatex modules3.tex

pdf:	stead3.pdf modules3.pdf stead2-pdfs

stead2-pdfs:
	./makepdfs2

man:
	man ./instead.6 | col -b > instead.txt

wiki:
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:documentation&do=export_xhtml" -O stead2.html
	wget "http://instead.syscall.ru/wiki/doku.php?id=en:gamedev:documentation&do=export_xhtml" -O stead2-en.html

docs:	wiki man pdf

install:
	$(INSTALLD) $(DESTDIR)$(DOCPATH)
	-$(INSTALL) instead.txt *.html *.pdf $(DESTDIR)$(DOCPATH)

	$(INSTALLD) $(DESTDIR)$(MANPATH)
	$(INSTALL) *.6 $(DESTDIR)$(MANPATH)

uninstall:
	$(RM) -rf $(DESTDIR)$(DOCPATH)
	$(RM) $(DESTDIR)$(MANPATH)/instead.*
