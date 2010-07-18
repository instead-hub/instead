include ../Rules.make
include ../config.make

pdf:
	pdflatex manual.tex
	makeindex manual.idx
	pdflatex manual.tex
	$(RM) -f manual.aux manual.log manual.toc manual.out manual.idx manual.ind manual.ilg

wiki:
	wget "http://instead.pinebrush.com/wiki/doku.php?id=ru:documentation&do=export_xhtml" -O writing_games.html
	wget "http://instead.pinebrush.com/wiki/doku.php?id=en:documentation&do=export_xhtml" -O writing_games-en.html
	wget "http://instead.pinebrush.com/wiki/doku.php?id=ru:documentation&do=export_raw" -O writing_games.txt
	wget "http://instead.pinebrush.com/wiki/doku.php?id=en:documentation&do=export_raw" -O writing_games-en.txt


clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.html *.txt *.pdf $(DOCPATH)
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*
