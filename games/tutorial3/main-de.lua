-- $Name:Deutsche Einführung$
instead_version "1.6.0"

game.act = 'Das können Sie nicht.';
game.inv = "Hmm … Falsch …";
game.use = 'Das wird nicht funktionieren.';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Einführung',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Willkommen in der INSTEAD-Einführung.")..[[^^
	Das Spiel besteht aus Szenen. Jede Spielszene hat eine Beschreibung, welche wiederum aus statischen und dynamischen Teilen besteht. Die dynamischen Teile beeinhalten Objekte, Figuren, usw. Ein Spieler kann mit dem dynamischen Teil interagieren, indem er mit der Maus auf die hervorgehobenen Links klickt.^^

	Die aktuelle Szene wird »Einführung« genannt und Sie lesen den statischen Teil ihrer Beschreibung. Das einzige Objekt in dieser Szene ist das »Weiter«-Objekt, welches Sie unter dem Text sehen können. Um mit dem Lernen fortzufahren, können Sie auf »Weiter« klicken.]],
	obj = { 
		vobj('continue', '{Weiter}'),
	},
};

paper = obj {
	nam = 'Papier',
	dsc = 'Das Erste, was Sie im Raum bemerken, ist ein {Blatt Papier}.',
	tak = 'Sie nehmen das Papier.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Ein leeres Blatt kariertes Papier. Vermutlich wurde aus einem Schreibblock ausgerissen.';
		end
		p 'Ein Blatt kariertes Papier, auf dem Ihr Name geschrieben steht.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Sie schreiben Ihren Namen auf das Papier.';
		end
	end,
};

pencil = obj {
	nam = 'Stift',
	dsc = 'Dort liegt ein {Stift} auf dem Boden.';
	tak = 'Sie nehmen den Stift.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Ein gewöhnlicher Stift.';
	end,
};

r1 = room {
	nam = '1. Lektion',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Ausgezeichnet!^Sie haben vermutlich bemerkt, dass der statische Teil der Szenenbeschreibung verschwunden ist und durch die Beschreibungen der Objekte, die Sie genommen haben, ersetzt wurde. Um die vollständige Szenenbeschreibung wieder zu sehen, klicken Sie entweder auf den Szenennamen – »1. Lektion« – oben oder drücken Sie F5. Das Ergebnis wird identisch sein.^^
		{Weiter}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[1. Lektion: Interaktion mit Objekten^^
		Lassen Sie uns mit der Lektion fortfahren. Sie befinden sich in einem Raum. Nehmen Sie beide Objekte, die Sie sehen. Sie können sie einfach mit der Maus anklicken. Falls Sie die Tastatur bevorzugen, können Sie ein Objekt mit den Pfeiltasten auswählen und [Eingabe] drücken, um mit ihm zu interagieren.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = '2. Lektion',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Gut!^^{Weiter}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[2. Lektion: Benutzung des Inventars – 1. Teil^^
		Nun haben Sie Objekte, die Sie untersuchen oder benutzen können. Dafür ist das Inventar da. Um ein beliebiges Objekt zu betrachten, doppelklicken Sie darauf. Alternativ können Sie zum Inventar mit [Tab] wechseln, und ein Objekt auswählen, indem Sie zwei mal die Eingabetaste drücken. Um den Zeiger zurück zur Szenenbeschreibung zu bewegen, drücken Sie erneut [Tab].^^

		Sehen Sie sich das Papier an. Dann tun Sie dasselbe mit dem Stift.]],
};

apple = obj {
	nam = 'Apfel',
	dsc = 'Auf dem Tisch liegt ein {Apfel}.',
	tak = 'Sie nehmen sich einen Apfel vom Tisch.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Sie essen den Apfel.';
		end
		p 'Sieht appetitlich aus.';
	end,
};

desk = obj {
	nam = 'Schreibtisch',
	dsc = 'In dieser Lektion sehen Sie einen hölzernen {Schreibtisch}.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Ein großer Schreibtisch aus Eichenholz. Auf seiner Oberfläche steht mit einem Bleistift geschrieben: »Lorem Ipsum«.';
		end
		p 'Ein großer Schreibtisch aus Eichenholz.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Sie schreiben einige Buchstaben auf den Tisch.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = '3. Lektion',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Weiter}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[3. Lektion: Benutzung des Inventars – 2. Teil^^
		Sie können mit Inventar- oder anderen Objekten der Szene oder des Inventars interagieren. In diesem Raum sehen Sie einen Schreibtisch. Versuchen Sie, den Stift auf ihn anzuwenden.^^

		Um das zu tun, klicken Sie mit der Maus auf den Stift, dann klicken Sie auf den Tisch. Alternativ können Sie dasselbe mit [Tab], [Eingabe] und den Pfeiltasten tun.^^

		Dann sehen Sie sich den Tisch an. Und vergessen Sie nicht, den Apfel zu nehmen, Sie werden ihn in der nächsten Lektion benötigen.]],
	obj = { 'desk' },
};

r4 = room {
	nam = '4. Lektion',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Gut.^^{Weiter}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[4. Lektion: Benutzung des Inventars – 3. Teil^^
		Gut, nun lassen Sie uns einige weitere Aktionen mit Objekten lernen.^^

		Erstens: Schreiben Sie etwas auf das Papier. Klicken Sie den Stift, dann das Papier an. Dann können Sie sich das Papier ansehen, um sich Ihr Schreiben anzusehen.

		Zweitens: Essen Sie den Apfel, den Sie vom vorherigen Raum genommen haben. Doppelklicken Sie ihn.^^

		All diese Aktionen können mit der Tastatur durchgeführt werden, wie zuvor beschrieben.]],
};

r5 = room {
	nam = '5. Lektion',
	exit = function(s, t)
		if t ~= r6 then
			p 'Wir haben diese Lektion bereits fertiggestellt.^ Bitte gehen Sie zur 6. Lektion.'
			return false;
		end
	end,
	dsc = [[5. Lektion: Fortbewegung – 1. Teil^^
		Nun werden wir die Fortbewegung zwischen Räumen erlernen. In diesem Raum sehen Sie fünf Links mit Lektionsnummern. Gehen Sie in die 6. Lektion.^^

		Klicken Sie auf den passenden Link oder benutzen Sie die Tastatur – wählen Sie den Link mit den Pfeiltasten aus und drücken Sie [Eingabe].]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = '6. Lektion',
	exit = function(s, t)
		if t ~= theend then
			return 'Wir haben diese Lektion bereits fertiggestellt.^ Bitte gehen Sie zur letzen Lektion.', false;
		end
	end,
	dsc = [[6. Lektion: Fortbewegung – 2. Teil^^
		Nun gehen Sie zur letzten Lektion.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r5', 'theend'},
};

theend = room {
	nam = 'Letzte Lektion',
	dsc = [[Mithilfe des Menüs können Sie ein Spiel starten, den Spielstand speichern oder laden, oder andere Dinge tun. Um das Menü zu öffnen, drücken Sie [Esc] oder klicken sie das Menüsymbol unten rechts an.^^
		Nun sind Sie bereit zum Spielen. Viel Glück!^^
		Laden Sie sich Spiele für INSTEAD von dort herunter:^^]]..txtu("http://instead-games.sourceforge.net")..[[^^
		Die Einführung benutzt die Musik »Instead game-engine« von svenzzon.]],
	obj = { vway('keys', 'Siehe {Liste der Tastenkürzel}.', 'help')},
};

help = room {
	nam = 'Tastenkürzel',
	dsc = [[
	[Esc] – Menü.^
	[Alt]+[Q] – Beenden.^
	[Alt]+[Enter] – Vollbild/Fenster.^
	[F5] – Szene neu laden.^
	[Leertaste]/[Rücktaste] – Szenenbeschreibung scrollen.^
	[Tab]/[Umschalt]+[Tab] – Zwischen aktiven Zonen wechseln.^
	[Bild auf]/[Bild ab] – In der aktiven Zone scrollen.^
	[Links]/[Rechts] – Links in der aktiven Zone auswählen.^
	[F2] – Spiel speichern.^
	[F3] – Spiel laden.^
	[F8] – Schnellspeichern.^
	[F9] – Schnellladen.^^

	Tastenkürzel, die vom ausgewählten Tastaturmodus abhängen
	(siehe Menü -> Einstellungen -> Tastaturmodus):^^

	[Hoch]/[Runter]:^
	 – Scrollmodus: In der aktiven Zone scrollen.^
	 – Linksmodus: Links in der aktiven Zone auswählen.^
	 – Intelligenter Modus: Sowohl Links auswählen als auch scrollen.^^

	[Hoch]/[Runter] + [Umschalt] oder [Alt]:^
	 – Schrollmodus: Links in der aktiven Zone auswählen.^
	 – Linksmodus: In der aktiven Zone scrollen.^
	 – Intelligenter Modus: In der aktiven Zone scrollen.
	]],
	obj = { vway('cmdline', 'Siehe {Kommandozeilenparameter}.', 'help2')},
};

help2 = room {
	nam = 'Kommandozeilenparameter',
	dsc = [[
	-nosound – Ohne Ton starten;^
	-debug – Debugmodus für Spielentwickler;^
	-theme <Thema> – Ein Thema auswählen;^
	-game <Spiel> – Ein Spiel auswählen;^
	-themespath <Pfad> – Zusätzlicher Pfad für Themen;^
	-gamespath <Pfad> – zusätzlicher Pfad für Spiele;^
	-window – Fenstermodus;^
	-fullscreen – Vollbildmodus;^
	-noautosave – Automatisches Speichern/Laden deaktivieren;^
	-encode <Spiel.lua> [kodiert.lua] – Lua-Quellcode kodieren (benutzen Sie doencfile aus Lua, um es zu laden);^
	-mode [BxH] – Auflösung von B×H benutzen.
	]],
	obj = { vway('keys', 'Siehe {Liste der Tastenkürzel}.', 'help')},
};
