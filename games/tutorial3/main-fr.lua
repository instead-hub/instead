-- $Name:Tutoriel Français$
instead_version "1.6.0"

game.act = 'Vous ne pouvez pas.';
game.inv = "Hmm… Mauvais…";
game.use = 'Ne fonctionne pas…';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutoriel',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Bienvenue dans le tutoriel de INSTEAD.")..[[^^
	Le jeu se compose de scènes. Chaque scène de jeu a une description, constituée de parties statiques et dynamiques. La partie dynamique comprend des objets, des personnages, etc. Un joueur peut interagir avec la partie dynamique en cliquant avec la souris sur les liens en surbrillance.^^

	La scène actuelle est appelée « Tutoriel » et vous êtes en train de lire la partie statique de sa description. Le seul objet dans cette scène est l'objet « Suivant », que vous pouvez voir ci-dessous. Ainsi, pour continuer à apprendre, cliquez sur « Suivant ».]],
	obj = { 
		vobj('continue', '{Suivant}'),
	},
};

paper = obj {
	nam = 'papier',
	dsc = 'La première chose que vous remarquez dans la pièce est une {feuille de papier}.',
	tak = 'Vous prenez le papier.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Une feuille de papier quadrillé. Probablement arrachée d’un livre.';
		end
		p 'Une feuille de papier quadrillé avec votre nom.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Vous écrivez votre nom sur le papier.';
		end
	end,
};

pencil = obj {
	nam = 'crayon',
	dsc = 'Il y a un {crayon} sur le sol.',
	tak = 'Vous prenez le crayon.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Un crayon ordinaire, en bois.';
	end,
};

r1 = room {
	nam = 'Leçon nº 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Excellent !^Vous avez probablement remarqué que la partie statique de la description de la scène a disparu, remplacée par les descriptions des objets que vous avez pris. Pour voir à nouveau la description complète de la scène, cliquez sur le nom de la scène au sommet – « Leçon nº 1 » – ou appuyez sur F5. Le résultat est le même.^^
		{Suivant}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Leçon nº 1. Interagir avec les objets^^
		Continuons la leçon. Vous êtes à l'intérieur d'une pièce. Prenez les deux objets que vous y voyez. Vous pouvez simplement cliquer dessus avec la souris, ou, si vous préférez le clavier, vous pouvez choisir un objet avec les touches fléchées et appuyer sur « Entrée » pour interagir avec lui.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Leçon nº 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Bien !^^{Suivant}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Leçon nº 2. Utiliser l'inventaire — Partie I^^
		Vous avez maintenant des objets que vous pouvez examiner ou utiliser. L'inventaire est fait pour cela. Pour regarder un objet, double-cliquez dessus. Ou alors, vous pouvez faire passer le curseur dans l'inventaire avec « Tab », y choisir un objet et appuyer sur « Entrée » deux fois. Pour ramener le curseur à la description de la scène, appuyez sur « Tab » à nouveau.^^

		Regardez maintenant le papier. Faites ensuite la même chose avec le crayon.]],
};

apple = obj {
	nam = 'pomme',
	dsc = 'Il y a une {pomme} sur le bureau.',
	tak = 'Vous prenez la pomme sur le bureau.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Vous mangez la pomme.';
		end
		p 'Elle est appétissante.';
	end,
};

desk = obj {
	nam = 'bureau',
	dsc = 'Dans cette leçon, vous voyez un {bureau} en bois.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Un grand bureau en chêne. Il y a dessus une inscription au crayon : « Lorem Ipsum ».';
		end
		p 'Un grand bureau en chêne.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Vous griffonnez plusieurs lettres sur le bureau.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Leçon nº 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Suivant}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Leçon nº 3. Utiliser l'inventaire — Partie II^^
		Vous pouvez faire agir les objets de l'inventaire sur d'autres objets de la scène ou de l'inventaire. Dans cette salle, vous verrez un bureau. Essayez d'utiliser un crayon sur lui.^^ 

		Pour ce faire, cliquez avec la souris sur le crayon, puis cliquez sur le bureau. Ou alors, vous pouvez faire la même chose au clavier avec « Tab », « Entrée » et les flèches.^^
  
		Il suffit ensuite de regarder le bureau pour voir le résultat. Et n'oubliez pas de prendre la pomme, vous en aurez besoin dans la prochaine leçon.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Leçon nº 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Bien.^^{Suivant}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Leçon nº 4. Utiliser l’inventaire — Partie III^^
		Bon, maintenant, nous allons étudier plusieurs autres actions sur les objets.^^
	
		Tout d’abord, écrivez quelque chose sur le papier. Cliquez sur le crayon, puis sur le papier. Vous pourrez ensuite regarder le document pour voir le résultat.^^
	
		Deuxièmement, mangez la pomme que vous avez prise dans la salle précédente. Double-cliquez sur elle.^^

		Toutes ces actions peuvent être faites avec le clavier comme décrit précédemment.]],
};

r5 = room {
	nam = 'Leçon nº 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Nous avons déjà terminé cette leçon…^ En avant pour la leçon nº 6, s’il vous plaît.'
			return false;
		end
	end,
	dsc = [[Leçon nº 5. En mouvement — Partie I^^
		Maintenant, nous allons étudier les déplacements entre les pièces. Dans cette salle, vous voyez cinq liens pointant vers les autres leçons. Allez à la leçon nº 6.^^

		Cliquez sur le lien approprié ou utilisez le clavier — sélectionnez le lien a l’aide des flèches et appuyez ensuite sur « Entrée ».]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Leçon nº 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Nous avons déjà terminé cette leçon…^ En avant pour la dernière leçon, s’il vous plaît.', false;
		end
	end,
	dsc = [[Leçon nº 6. En mouvement — Partie II^^
		Rendez-vous à la dernière leçon.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r5', 'theend'},
};

theend = room {
	nam = 'Dernière leçon',
	dsc = [[À l’aide du menu, vous pouvez choisir un jeu, sauvegarder ou charger une partie, ou faire d'autres choses encore. Pour ouvrir le menu, appuyez sur « Esc » ou cliquez sur l'icône de menu en bas à droite (avec le thème par défaut).^^
		Maintenant, vous êtes prêt à jouer. Bonne chance !!!^^
		Téléchargez des jeux pour INSTEAD ici :^]]
		..txtu("http://instead-games.sourceforge.net")..
		[[^^Le tutoriel utilise « INSTEAD game-engine » de svenzzon comme thème musical.]],
	obj = { vway('keys', 'Voir la {liste des raccourcis clavier}.', 'help')},
};

help = room {
	nam = 'Raccourcis clavier',
	dsc = [[
	Esc – Menu^
	Alt+Q – Quitter^
	Alt+Entrée – Plein écran/Fenêtre^
	F5 – Rafraîchir la scène^
	Espace/Effacement arrière – Faire défiler la description de la scène^
	Tab/Maj+Tab – Basculer entre les zones actives^
	PgUp/PgDn – Faire défiler les zones actives^
	Flèches gauche/droite – Sélection des liens dans la zone active^
	F2 – Sauvegarder la partie^
	F3 – Charger une partie^
	F8 – Sauvegarde rapide^
	F9 – Chargement rapide^^

	Les raccourcis dépendent du mode de clavier choisi^
	(cf. Menu → Réglages → [Autres] → Mode clavier) :^^

	Flèches haut/bas :^
	 - mode « Scroll » : défilement zone active.^
	 - mode « Links » : sélection des liens dans la zone active.^
	 - mode « Smart » : à la fois sélection des liens et défilement dans la zone active.^^

	Flèches haut/bas + Shift ou Alt :^
	 - mode « Scroll » : sélection des liens dans la zone active.^
	 - mode « Links » : défilement dans la zone active.^
	 - mode « Smart » : défilement dans la zone active.
	]],
	obj = { vway('cmdline', 'Voir les {paramètres de la ligne de commande}.', 'help2')},
};

help2 = room {
	nam = 'Paramètres de la ligne de commande',
	dsc = [[
	-nosound – lancement sans son^
	-debug – mode debugage pour les développeurs^
	-theme <theme> – choisir un thème^
	-game <game> – choisir un jeu^
	-themespath <path> – chemin supplémentaire pour les thèmes^
	-gamespath <path> – chemin supplémentaire pour les jeux^
	-window – mode fenêtré^
	-fullscreen – mode plein écran^
	-noautosave – désactiver les options de sauvegarde auto./chargement auto.^
	-encode <game.lua> [encoded.lua] – encoder code source lua (utilisation doencfile de lua pour charger)^
	-mode [WxH] – utiliser la résolution WxH (LxH).
	]],
	obj = { vway('keys', 'Voir la {liste des raccourcis clavier}.', 'help')},
};
