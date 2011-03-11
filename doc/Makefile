include ../Rules.make
include ../config.make

pdf:
	# try xetex here, and hope it works ;)
	pdflatex manual.tex
	makeindex manual.idx
	pdflatex manual.tex
	$(RM) -f manual.aux manual.log manual.toc manual.out manual.idx manual.ind manual.ilg
#to-do: interpret $PATH

wiki:
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:documentation&do=export_xhtml" -O writing_games.html
	wget "http://instead.syscall.ru/wiki/doku.php?id=en:gamedev:documentation&do=export_xhtml" -O writing_games-en.html
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:documentation&do=export_raw" -O writing_games.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=en:gamedev:documentation&do=export_raw" -O writing_games-en.txt

	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:click&do=export_raw" -O modules/click.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:format&do=export_raw" -O modules/format.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:hideinv&do=export_raw" -O modules/hideinv.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:prefs&do=export_raw" -O modules/prefs.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:timer&do=export_raw" -O modules/timer.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:xact&do=export_raw" -O modules/xact.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:dash&do=export_raw" -O modules/dash.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:dbg&do=export_raw" -O modules/dbg.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:hotkeys&do=export_raw" -O modules/hotkeys.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:para&do=export_raw" -O modules/para.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:quotes&do=export_raw" -O modules/quotes.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:theme&do=export_raw" -O modules/theme.txt
	wget "http://instead.syscall.ru/wiki/doku.php?id=ru:gamedev:modules:kbd&do=export_raw" -O modules/kbd.txt


clean:
all:
install:
	$(INSTALLD) $(DOCPATH)
	$(INSTALL) *.html *.txt *.pdf $(DOCPATH)
	$(INSTALLD) $(DOCPATH)/modules
	$(INSTALL) modules/*.txt $(DOCPATH)/modules
	$(INSTALLD) $(MANPATH)
	$(INSTALL) *.6 $(MANPATH)

uninstall:
	$(RM) -rf $(DOCPATH)
	$(RM) $(MANPATH)/instead.*
