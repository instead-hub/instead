-- $Name:Lernilo$
instead_version "1.6.0"

game.act = 'Vi ne sukcesas.';
game.inv = "Hm.. Ne tio..";
game.use = 'Ne funkcos...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Lernilo',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Bonvenon en reĝimo de lernilo INSTEAD.")..[[^^
	La ludo konsistas el scenoj. Ĉiu luda sceno havas priskribo,
	konsistas el statika kaj dinamika parto. Dinamika parto inkludas objektoj,
	personoj kaj t.p. Ludanto povas interagi kun dinamika parto per muso, 
	per premo sur lumaj ligiloj.^^

	Ĉi tiu sceno nomiĝas "Lernilo" kaj nun vi legas statika parto ŝin priskribon.
	Nura objekto de sceno estas objekto "Plu", kiun vi vidas sub la teksto.
	Konklude, por la daŭrigo de lernilo vi povas premi sur "Plu".]],
	obj = { 
		vobj('continue', '{Plu}'),
	},
};

paper = obj {
	nam = 'papero',
	dsc = 'Unua, ke vi atentas en ĉambro - {folio de papero}.',
	tak = 'Vi prenis papero.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Pura folio de kvadratita papero. Eble, lin elŝiris el kajero.';
		end
		p 'Folio de kvadratita papero, sur kiun skribite vian nomo.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Vi skribas sur folieto sian nomo.';
		end
	end,
};

pencil = obj {
	nam = 'krajono',
	dsc = 'Sur la planko kuŝas {krajono}.',
	tak = 'Vi prenis krajono.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Ordinara ligna krajono.';
	end,
};

r1 = room {
	nam = 'Leciono 1',
	enter = code [[ lifeon('r1') ]],
	life = function(s)
		if not have 'paper' or not have 'pencil' then
			return
		end
		put(vway('continue',
		[[Bone!^Vi probable rimarkis, ke statika parto de sceno
		neniĝis, montritis priskribo de aĵoj, kiujn vi prenis.
		Por ree ekrigardi plena priskribo de sceno, eble premi sur ŝin
		titilo, montriĝas en supra parto de fenestro - "Leciono 1". Plie
		eble ekpreni klavo F5 sur klavaro. ^^
		{Plu}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Leciono 1. Interago kun objektoj^^
		Ni daŭrigas leciono. Nun vi lokiĝas en ĉambro. Prenu ambaŭ aĵojn,
		kiujn vi vidas. Ni memorigas, ke por tio vi povas uzi muso.
		Se vi oportune uzi klavaro - eble elekti bezone aĵo per sagklavoj kaj 
		premi "Enigo" por interago kun lin.
	]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Leciono 2',
	enter = code [[ lifeon('r2') ]],
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Bone!^^{Plu}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Leciono 2. Uzado inventaro - Parto 1^^
		Nun ĉe vi aperis aĵoj, kiujn eble uzi aŭ esplori.
		Por tio destinas inventaro. Vi povas ekrigardi ajna aĵo
		de inventaro, klakos per la muso sur aĵo. Samo eble fari per klavaro: premu "Tab", por komutos sur inventaro,
		elektu bezona aĵo kaj du fojo premu "Enigo". Por reveni reen al priskribo de sceno, premu "Tab" ankoraŭfoje.^^

		Do, ekrigardu sur papero. Poste, ripetu tion operacion kun krajono.]],
};

apple = obj {
	nam = 'pomo',
	dsc = 'Sur tablo kuŝas {pomo}.',
	tak = 'Vi prenis pomo de tablo.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Vi manĝas pomo.';
		end
		p 'Aspektas apetite.';
	end,
};

desk = obj {
	nam = 'tablo',
	dsc = 'En tion lecion vi vidas ligna {skribotablo}.',
	var { haswriting = false, seen = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Granda kverka skribotablo. Sur tablo vidiĝas malgranda krajonsurskribo: "Lorem Ipsum".';
		end
		p 'Granda kverka skribotablo.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			p 'Vi skribas sur tablo iom literoj.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Leciono 3',
	enter = code [[ lifeon('r3') ]],
	life = function(s)
		if not desk.seen or not have('apple') then
			return
		end
		put(vway("continue", "^^{Plu}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Leciono 3. Uzado inventaro - Parto 2^^
		Vi povas agi per aĵoj de inventaro sur aliaj aĵoj de sceno aŭ inventaro. 
		En tiu ĉambro vi vidas tablo. Ekprovu efiki per krajono sur tablo.^^ 

		Por tio premu per muso sur krajono, depost sur tablo. Samo eble fari kun 
		klavaro: uzigas klavoj "Tab", "Enigo" kaj "sagoj".^^
  
		Deposte ekrigardu sur tablo. Kaj ne forgesu preni pomo, ĝi taŭgos al ni en sekva leciono.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Leciono 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Bone.^^{Plu}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Leciono 4. Uzado inventaro - Parto 3^^
		Bone, nun ni ellernos pli iom agoj kun aĵoj.^^
	
		Unue, ekskribu io sur papero. Por tio premu per muso sur krajono, 
		deposte sur papero. Deposte tion vi povas rigardi sur papero, por ekvidi vian skribo.^^
	
		Due, manĝu pomo, kiun vi prenis sur la tablo en ĝisnuna ĉambro. 
		Por tio kliku sur pomo en inventaro.^^

		Ĉiujn priskribita agoj eble plennumo per klavaro ankaŭ, kiel rigardite en ĝisnuna leciono.]],
};

r5 = room {
	nam = 'Leciono 5',
	exit = function(s, t)
		if t ~= r6 then
			return 'Tiu leciono ni jam pasis.^Bonvolu, pasu en leciono 6.', false;
		end
	end,
	dsc = [[Leciono 5. Translokado - Parto 1^^
		Nun ni ellernos transiroj inter ĉambroj. En tiu ĉambro vi vidas kvin kromaj ligiloj kun numbroj de lecionoj. Pasu en leciono 6.^^

		Por tio premu per muso sur respektiva ligilo. Ankaŭ vi povas uzi
		klavaro - elektu bezona ligilo kun sagklavoj kaj premu "Enigo" por la transiro.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Leciono 6',
	exit = function(s, t)
		if t ~= theend then
			p 'Jena leciono ni jam pasis.^ Bonvolu, pasu en la lasta leciono.'
			return false;
		end
	end,
	dsc = [[Leciono 6. Translokado - Parto 2^^
		Nun pasu en lasta leciono]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r5', 'theend'},
};

theend = room {
	nam = 'Lasta leciono',
	dsc = [[Vi povas elekti ludo, konservi kaj ŝargi ŝin stato kaj plennumi alia agoj per menuo.
		Por aperti menuo premu klavo "Esc", premu per muso sur karaktro de menuo (dekstre demalsupre).^^
		Nun vi prete al ludo. Sukceson!!!^^
		Ludoj por INSTEAD eble elŝuti ĉi tie: ]]..txtu("https://instead-hub.github.io")..[[^^
		En la lernilo uzas trako "Instead game-engine" de svenzzon]],
	obj = { vway('keys', 'Ekrigardi {listo de klavoj}.', 'help')},
};

help = room {
	nam = 'Listo de klavoj',
	dsc = [[
	Esc - Aperto de menuo;^
	Alt+Q - Eliro;^
	Alt+Enter - Tutekrana/fenestra reĝimo;^
	F5 - Refreŝigi sceno;^
	Spaceto/Backspace - Rulumo de la scenpriskribo;^
	Tab/Shift+Tab - Ŝanĝado inter aktivaj zonoj;^
	PgUp/PgDn - Rulumo de aktiva zono;^
	Sagetoj maldekstren/dekstren - Elekto de ligiloj en aktiva zono;^
	F2 - Konservado de ludo;^
	F3 - Ŝargado de ludo;^
	F8 - Rapida konservado de ludo;^
	F9 - Rapida ŝargado de ludo.^^

	Klavoj, konduto tiujn depende de elekta klavreĝimo
	(rigardu Menuo -> Agordoj -> Reĝimo de klavaro): ^^

	Sagoj supre/malsupre:^
	 - En reĝimo "Rulumo": rulumo de aktiva zono;^
	 - En reĝimo "Ligiloj": elekto de ligiloj en aktiva zono;^
	 - En reĝimo "Saĝa": samtempe elekto de ligiloj kaj rulumo de aktiva zono.^^

	Sagoj supre/malsupre + Shift aŭ Alt:^
	 - En reĝimo "Rulumo": elekto de ligiloj en aktiva zono;^
	 - En reĝimo "Ligiloj": rulumo de aktiva zono;^
	 - En reĝimo "Saĝa": rulumo de aktiva zono.
	]],
	obj = { vway('cmdline', 'Ekrigardi {parametroj de la komandlinio}.', 'help2')},
};

help2 = room {
	nam = 'Parametroj de la komandlinio',
	dsc = [[
	-nosound - lanĉo sen sono;^
	-debug - sencimiga reĝimo por programisto de la ludoj;^
	-theme <etoso> - elekto de la etoso;^
	-game <ludo> - elekto de la ludo;^
	-themespath <vojo> - kroma vojo al etosoj;^
	-gamespath <vojo> - kroma vojo al ludoj;^
	-windows - fenestra reĝimo;^
	-fullscreen - tutekrana reĝimo;^
	-noautosave - malŝalti aŭtokonservado/aŭtoŝargado;^
	-encode <game.lua> [encoded.lua] - Ĉifri lua-fontaĵo (uzu doencfile por ŝargi tia dosiero);^
	-mode [LARĜOxALTO] - instali distingivo.
	]],
	obj = { vway('keys', 'Ekrigardi {listo de klavoj}.', 'help')},
};
