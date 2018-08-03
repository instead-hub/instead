include ../Rules.make
include ../config.make

all:

clean:
	rm -f *.aux *.log *.out *.toc stead3-manual-ru.tex modules3-manual-ru.tex stead3-manual.tex
	rm -f *.pdf instead.txt

stead3-manual-ru.tex: stead3-ru.md
	multimarkdown stead3-ru.md -t latex -o stead3-manual-ru.tex

stead3-ru.pdf: stead3-ru.tex stead3-manual-ru.tex
	xelatex stead3-ru.tex
	xelatex stead3-ru.tex
	xelatex stead3-ru.tex

stead3-manual.tex: stead3.md
	multimarkdown stead3.md -t latex -o stead3-manual.tex

stead3.pdf: stead3.tex stead3-manual.tex
	xelatex stead3.tex
	xelatex stead3.tex
	xelatex stead3.tex

modules3-manual-ru.tex: modules3-ru.md
	multimarkdown modules3-ru.md -t latex -o modules3-manual-ru.tex

modules3-ru.pdf: modules3-ru.tex modules3-manual-ru.tex
	xelatex modules3-ru.tex
	xelatex modules3-ru.tex
	xelatex modules3-ru.tex

pdf:	stead3-ru.pdf modules3-ru.pdf stead3.pdf
	rm -f *.aux *.log *.out *.toc stead3-manual-ru.tex modules3-manual-ru.tex stead3-manual.tex

man:
	man ./instead.6 | col -b > instead.txt

docs:	man pdf

install:
	$(INSTALLD) $(DESTDIR)$(DOCPATH)
	-$(INSTALL) instead.txt *.pdf $(DESTDIR)$(DOCPATH)

	$(INSTALLD) $(DESTDIR)$(MANPATH)
	$(INSTALL) *.6 $(DESTDIR)$(MANPATH)

uninstall:
	$(RM) -rf $(DESTDIR)$(DOCPATH)
	$(RM) $(DESTDIR)$(MANPATH)/instead.*
