-- $Name:Tutorial Italiano$
instead_version "1.6.0"

game.act = 'Non puoi.';
game.inv = "Hmm... Sbagliato...";
game.use = 'Non funziona...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutorial',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Benvenuto in questa guida introduttiva ad INSTEAD.")..[[^^
	Il gioco consiste di scene. Ogni scena del gioco ha una descrizione, formata da parti statiche e dinamiche. Le parti dinamiche includono oggetti, personaggi, etc... Il giocatore può interagire con la parte dinamica cliccando con il mouse sui link evidenziati.^^

	La scena corrente è chiamata "Tutorial" e state leggendo la parte statica della sua descrizione. L'unico oggetto di questa scena è l'oggetto "Avanti", che potete vedere sotto il testo. Così, per continuare questa guida basta fare click su "Avanti".]],
	obj = { 
		vobj('continua', '{Avanti}'),
	},
};

paper = obj {
	nam = 'foglio',
	dsc = 'La prima cosa che potete notare nella stanza è un {foglio di carta}.',
	tak = 'Prendi il foglio.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Un foglio di carta bianca a quadretti. Probabilmente strappato da un quaderno.';
		end
		p 'Un foglio a quadretti con il tuo nome scritto sopra.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Scrivi il tuo nome sul foglio.';
		end
	end,
};

pencil = obj {
	nam = 'matita',
	dsc = 'C\'è anche una {matita} sul pavimento.',
	tak = 'Raccogli la matita.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Una normale matita di legno.';
	end,
};

r1 = room {
	nam = 'Lezione 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('avanti',
		[[Eccellente!^Avrete probabilmente notato che la parte statica della descrizione della scena è scomparsa, rimpiazzata dalla descrizione degli oggetti che avete preso. Per vedere di nuovo la descrizione completa della scena potete fare click sul nome della scena in alto — "Lezione 1" o premere F5. Il risultato è lo stesso.^^
		{Next}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Lezione 1. Interagire con gli oggetti^^
		Continuiamo la nostra guida. Siete dentro una stanza e potete vedere due oggetti presenti. Per prenderli fate semplicemente click su di essi con il mouse o, se preferite usare la tastiera, potete raccogliere un oggetto usando i tasti freccia e premendo quindi "Invio" per interagire con essi.]],  
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Lezione 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("avanti", "Bene!^^{Avanti}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Lezione 2. Usare l'inventario — Parte I^^
		Ora avete nel vostro inventario due oggetti che potete esaminare od usare. Per esaminare un qualsiasi oggetto fate doppio click su di esso oppure potete posizionare il cursore nell'inventario con il tasto "Tab", scegliete l'oggetto e premete "invio" due volte. Per tornare con il cursore alla descrizione della scena premete di nuovo "Tab"^^  
		Ora, osservate il foglio, e quindi fate lo stesso con la matita.]],
};

apple = obj {
	nam = 'mela',
	dsc = 'C\'è una {mela} sul tavolo.',
	tak = 'Prendi la mela dal tavolo.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Mangi la mela.';
		end
		p 'Sembra appetitosa.';
	end,
};

desk = obj {
	nam = 'scrivania',
	dsc = 'Puoi vedere una {scrivania} di legno.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Una larga scrivania di legno. C\'è un\'iscrizione sul piano: "Lorem Ipsum".';
		end
		p 'Una larga scrivania in legno.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Scrivi alcune lettere sulla scrivania.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Lezione 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continua", "^^{Avanti}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Lezione 3. Usare l'inventario — Parte II^^
		Potete agire con oggetti dell'inventario su altri oggeti della scena o dell'inventario stesso. In questa locazione potete vedere una scrivania. Provate ad usare la matita su di essa.^^
		Per farlo fate click con il mouse sulla matita, e quindi sulla scrivania. Oppure usate la tastiera con i tasti "Tab", "invio" e le frecce.^^
		Ora guardate il tavolo e non scordatevi di prendere la mela, ne avrete bisogno per la prossima dimostrazione.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Lezione 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continua", "Bene.^^{Avanti}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Lezione 4. Usare l'inventario — Parte III^^
		Bene, ora vediamo qualche altra azione con gli oggetti.^^

		Come prima cosa, scrivete qualcosa sul foglio. Fate click sulla matita e quindi sul foglio. Ora potete osservare il foglio e controllare cosa avete scritto.^^
	
		Come secondo passo, mangiate la mela, l'avete presa nella precedente locazione. Per farlo fate doppio click su di essa.^^

		Tutte queste azioni possono essere eseguite anche con la tastiera come già descritto in precedenza.]],
};

r5 = room {
	nam = 'Lezione 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Abbiamo già terminato questa parte della guida. Proseguiamo alla prossima, per favore.'
			return false;
		end
	end,
	dsc = [[Lezione 5. Movimenti — Parte I^^
		Ora vedremo i passaggi tra una locazione e l'altra. In questa stanza potete vedere cinque link che portano verso le lezioni numerate. Andate alla 6.^^

		Fate click sul giusto link o usate la tastera - selezionate il link con le frecce e premete "Invio".]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Lezione 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Abbiamo già terminato questa lezione.^ Proseguiamo verso la 6, per favore.', false;
		end
	end,
	dsc = [[Lezione 6. Movimenti - Parte II^^
		Ora passiamo all'ultimo step di questa guida.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6', 'theend'},
};

theend = room {
	nam = 'Gran Finale',
	dsc = [[Con l'aiuto del menu potete scegliere un gioco, salvare o caricare una partita, oltre a svariate altre cose. Per aprire il menu premete "Esc" o fate click sull'icona di menu in basso a destra.^^
		Ora siete pronti a giocare. Buona Fortuna!!!^^
		Scaricate i giochi per INSTEAD qui: ]]..txtu("http://instead-games.sourceforge.net")..[[^^
		Il tutorial si avvale del brano musicale "Instead game-engine" di svenzzon.]],
	obj = { vway('keys', 'Vedi {lista delle scorciatoie da tastiera}.', 'help')},
};

help = room {
	nam = 'Scorciatoie da tastiera',
	dsc = [[
	Esc - Menu.^
	Alt+Q - Esci.^
	Alt+Enter - Schermo intero/finestra.^
	F5 - Ristampa la descrizione completa della scena.^
	Space/Backspace - Scorri la descrizione della scena.^
	Tab/Shift+Tab - Passa da una zona attiva all'altra.^
	PgUp/PgDn - Scorri la zona attiva.^
	Freccia sinistra/destra - Scegli i link nella zona attiva.^
	F2 - Salva la partita.^
	F3 - Carica la partita.^
	F8 - Salvataggio veloce.^
	F9 - Caricamento veloce.^^

	Le scorciatoie sono attive se si sceglie la modalità tastiera nelle impostazioni
	(vedi Menu -> Impostazioni -> Keyboard mode):^^

	Frecce su/giù:^
	 - "Scroll" mode: scorre la zona attiva.^
	 - "Links" mode: sceglie i link nella zona attiva.^
	 - "Smart" mode: sceglie i link e scorre la zona attiva.^^

	Frecce su/giù + Shift o Alt:^
	 - "Scroll" mode: sceglie i link nella zona attiva.^
	 - "Links" mode: scorre la zona attiva.^
	 - "Smart" mode: scorre la zona attiva.
	]],
	obj = { vway('cmdline', 'Vedi {parametri della linea di comando}.', 'help2')},
};

help2 = room {
	nam = 'Parametri della linea di comando',
	dsc = [[
	-nosound - lancia il gioco senza suono;^
	-debug - modalità di debug per sviluppatori di giochi;^
	-theme <theme> - scegli un tema;^
	-game <game> - scegli un gioco;^
	-themespath <path> - percorsi addizionali per i temi;^
	-gamespath <path> - percorsi addizionali per i giochi;^
	-window - modalità finestra;^
	-fullscreen  - modalità a tutto schermo;^
	-noautosave - disabilita l'opzione di autosalvataggio/autocaricamento;^
	-encode <game.lua> [encoded.lua] - comprime il codice sorgente di lua (usate doencfile da lua per caricarlo);^
	-mode [WxH] - usa la risoluzione WxH.
	]],
	obj = { vway('keys', 'Vedi {lista delle scorciatorie da tastiera}.', 'help')},
};
