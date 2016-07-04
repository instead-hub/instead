-- $Name:Nederlandstalige Snelstart Training$
instead_version "1.6.0"

game.act = 'Dat kan niet.';
game.inv = "Hmm... Verkeerd...";
game.use = 'Dat werkt niet...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Snelstart Training',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Welkom bij de INSTEAD snelstart training.")..[[^^
	Het spel bestaat uit scènes. Elke scène heeft een beschrijving. Een beschrijving
	bestaat uit vaste en dynamische gedeeltes. Dynamische gedeeltes zijn bijvoorbeeld
	objecten of personages. Een speler kan de dynamische gedeeltes beïnvloeden door
	op de gemarkeerde te klikken.^^

	De huidige scène heet "Snelstart Training" waarvan je op dit moment het statische gedeelte
	van de omschrijving aan het lezen ben. Het enige object in deze scène is het
	"Volgende" object. Die kun je onder deze tekst vinden. Om de rest van de training
	te lezen, moet je op "Volgende" klikken..]],
	obj = { 
		vobj('continue', '{Volgende}'),
	},
};

paper = obj {
	nam = 'papier',
	dsc = 'Het eerste dat in deze ruimte opvalt, is een {blad papier}.',
	tak = 'Je pakt het blad papier.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Een blanco blad gelinieerd papier. Waarschijnlijk uit een notitieboekje gescheurd.';
		end
		p 'Een blad gelinieerd papier waarop jouw naam geschreven staat.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Je schrijft je naam op het papier.';
		end
	end,
};

pencil = obj {
	nam = 'potlood',
	dsc = 'Er ligt een {potlood} op de grond.',
	tak = 'Je pakt het potlood op.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Een heel normaal houten potlood.';
	end,
};

r1 = room {
	nam = 'Les 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Fantastisch!^Je hebt waarschijnlijk gemerkt dat het statische gedeelte van
		de scène beschrijving verdwenen is. Het werd vervangen met de omschrijving van
		de objecten die je gepakt hebt. Als je de volledige omschrijving nogmaals wilt
		lezen, klik je danwel op de naam van de scène bovenin (dus op "Les 1"), of je
		drukt op F5. Beide geeft hetzelfde resultaat.^^
		{Volgende}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Les 1. Werken met objecten^^
		We gaan verder met de training. Je bent in een ruimte. Pak beide objecten die je ziet.
		Je kunt er simpelweg op klikken met de muis. Gebruik je liever het toetsenbord,
		gebruik dan de pijltjes toetsen om het object te selecteren, en druk op "Enter"
		om het op te pakken.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Les 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Goed!^^{Volgende}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Les 2. Het gebruik van de inventaris — Deel I^^
		Je hebt nu objecten die je kunt onderzoeken of gebruiken. Daarvoor is het
		inventarisscherm. Om een object te bekijken, klik je er twee keer op. Of
		je kan met de "Tab" toets naar het inventarisscherm schakelen en vervolgens
		het betreffende object kiezen met de pijltjestoetsen, en dan twee keer op
		"Enter" drukken. Om terug te gaan naar het gedeelte omschrijving kun je
		daarna weer op "Tab" drukken.^^

		Dus, bekijk nu het papier, en bekijk daarna ook het potlood.]],
};

apple = obj {
	nam = 'appel',
	dsc = 'Er ligt een {appel} op de tafel.',
	tak = 'Je pakt de appel van de tafel.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Je eet de appel op.';
		end
		p 'Ziet er smakelijk uit.';
	end,
};

desk = obj {
	nam = 'bureau',
	dsc = 'In deze ruimte zie je een houten {bureau}.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Een groot eiken bureau. Er is met potlood op geschreven: "Lorem Ipsum".';
		end
		p 'Een groot eiken bureau.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Je schrijft enkele letters op het bureau.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Les 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Volgende}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Les 3. Het gebruik van de inventaris — Deel II^^
		Objecten in de inventaris kun je ook gebruiken in combinatie met andere
		objecten. Die andere objecten kunnen zich ook in de inventaris bevinden,
		maar kunnen ook objecten uit de scène beschrijving zijn. In de huidige
		ruimte bevind zich een bureau. Probeer het potlood daarop te gebruiken.^^

		Om dat te doen, klik je eerst op het potlood, vervolgens op het bureau.
		Dat kan natuurlijk ook met het toetsenbord met behulp van de "Tab",
		"Enter" en pijltjestoetsen.^^
  
		Kijk vervolgens naar de tafel. En pak tenslotte ook de appel op, die heb je
		nodig in de volgende les.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Les 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Goed.^^{Volgende}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Les 4. Het gebruik van de inventaris — Deel III^^
		We gaan nu verdere acties met objecten leren.^^

		Om te beginnen schrijf je iets op het papier. Klik daarvoor op het potlood,
		daarna op het papier. Kijk vervolgens op het papier om het resultaat te bekijken.^^

		Eet tenslotte de appel op die je meegenomen hebt uit de vorige ruimte. Klik
		hiervoor twee maal op de appel.^^

		Natuurlijk kan bovenstaande ook weer met het toetsenbord gedaan worden.]],
};

r5 = room {
	nam = 'Les 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Die les is al afgerond.^ Ga naar les 6, aub.'
			return false;
		end
	end,
	dsc = [[Les 5. Verplaatsen — Deel I^^
		We bestuderen nu hoe je je verplaatst naar een andere ruimte. In deze ruimte
		zie je vijf links met naar verschillende lessen. Ga naar les 6.^^

		Klik op de betreffende les met de muis, of gebruik het toetsenbord. (Gebruik
		de pijltjestoetsen om de juiste les te selecteren, en druk dan op "Enter".]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Les 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Die les is al afgerond.^ Ga naar de laatste les aub.', false;
		end
	end,
	dsc = [[Les 6. Verplaatsen — Deel II^^
		Verplaats jezelf nu naar de laatste les.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r5', 'theend'},
};

theend = room {
	nam = 'Laatste les',
	dsc = [[In het menu kun je onder andere een ander spel kiezen. Je kunt ook de
		huidige stand opslaan of weer laden. Om het menu te openen, toets "Esc"
		of klik met de muis op het hamburger icoon rechts onderin.^^
		Je bent nu klaar om te spelen. Veel plezier!!!^^
		Download spellen voor INSTEAD hier: ^]]..txtc(txtu("http://instead-games.sourceforge.net"))..[[^^
		De achtergrondmuziek in deze training is van svenzzon en heet "Instead game-engine".]],
	obj = { vway('keys', 'Zie ook de {lijst met sneltoetsen}.', 'help')},
};

help = room {
	nam = 'Sneltoetsen',
	dsc = [[
	Esc - Menu.^
	Alt+Q - Afsluiten.^
	Alt+Enter - Volledig scherm/venster.^
	F5 - Ververs de scène.^
	Spatie/Backspace - Scroll door de scène beschrijving.^
	Tab/Shift+Tab - Schakel tussen de beschrijving en inventaris zone.^
	PgUp/PgDn - Scroll de tekst van de actieve zone.^
	Pijltjes rechts/links - Kies objecten in de actieve zone.^
	F2 - Het spel opslaan.^
	F3 - Een opgeslagen spel laden.^
	F8 - Snel opslaan.^
	F9 - Snel laden.^^

	Sneltoetsen die afhankelijk zijn van de pijltjestoetsen mode
	(zie Menu -> Instellingen -> Overig -> Pijltjestoetsen):^^

	Functie pijltje omhoog/omlaag:^
	 - in "Scroll" mode: scroll de tekst van de actieve zone.^
	 - in "Object" mode: kies objecten in het zichtbare gedeelte van de actieve zone.^
	 - in "Intelligente" mode: zowel scrollen als objecten kiezen in de actieve zone.^^

	Functie pijltje omhoog/omlaag met Shift of Alt:^
	 - in "Scroll" mode: kies objecten in het zichtbare gedeelte van de actieve zone.^
	 - in "Object" mode: scroll de tekst van de actieve zone.^
	 - in "Intelligente" mode: scroll de tekst van de actieve zone.
	]],
	obj = { vway('cmdline', 'Zie {commandoregel parameters}.', 'help2')},
};

help2 = room {
	nam = 'Commandoregel parameters',
	dsc = [[
	-nosound - start zonder geluid;^
	-debug - debug mode voor ontwikkelaars van spellen;^
	-theme <theme> - kies een skin/thema;^
	-game <game> - kies een spel;^
	-themespath <path> - extra folder met skins/thema's;^
	-gamespath <path> - extra folder met spellen;^
	-window - gebruik een venster;^
	-fullscreen  - gebruik het volledige scherm;^
	-noautosave - schakel het automatisch opslaan en laden uit;^
	-encode <game.lua> [encoded.lua] - encodeer lua source code (gebruik "doencfile" in lua om het te laden);^
	-mode [WxH] - gebruik de schermresolutie WxH.
	]],
	obj = { vway('keys', 'Zie ook de {lijst met sneltoetsen}.', 'help')},
};

-- vim: set fileencoding=utf-8 nobomb noet foldmethod=syntax nofoldenable foldnestmax=1:
