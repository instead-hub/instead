-- $Name:Tutorial Français$
instead_version "1.6.0"

game.act = 'Vous ne pouvez pas.';
game.inv = "Hmm... Mauvais...";
game.use = 'Ne fonctionne pas...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutorial',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Bienvenue dans le tutorial de INSTEAD.")..[[^^
	Le jeu se compose de scènes. Chaque scène de jeu a une description, constitué de parties statiques et dynamiques. La partie dynamique comprend des objets, des personnages, etc Un joueur peut interagir avec la partie dynamique en cliquant sur la souris sur les liens en surbrillance.^^

	La scène actuelle est appelée "Tutoriel" et vous êtes en train de lire la partie statique de sa description. Le seul objet dans cette scène est l'objet "Suivant", que vous pouvez voir ci-dessous. Ainsi, pour continuer à apprendre, cliquer sur "Suivant".]],
	obj = { 
		vobj('continue', '{Suivant}'),
	},
};

paper = obj {
	nam = 'papier',
	dsc = 'La première chose que vous remarquerez dans la pièce est une {feuille de papier}.',
	tak = 'Vous prenez le papier.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Une feuille de papier quadrillé. Probablement arraché de la copie-livre.';
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
		p 'Un crayon de bois ordinaire.';
	end,
};

r1 = room {
	nam = 'leçon 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Excellent!^Vous avez probablement remarqué que la partie statique de la description de la scène a disparu, remplacé par les descriptions des objets que vous avez prises. Pour voir la description complète de la scène à nouveau cliquer sur le nom de la scène au sommet - "Leçon 1" ou appuyez sur F5. Le résultat est le même.^^
		{Next}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Leçon 1. Interagir avec des objets^^
		Continuons la leçon. Vous êtes à l'intérieur d'une pièce. Prenez les deux objets que vous voyez. Vous pouvez simplement cliquer dessus avec la souris. Si vous préférez le clavier, vous pouvez choisir un objet avec les touches fléchées et appuyez sur "Entrée" pour interagir avec lui.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Leçon 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Bien!^^{Suivant}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Leçon 2. Utiliser l'inventaire — Part I^^
		Maintenant, vous avez des objets que vous pouvez examiner ou utiliser. L'inventaire est fait pour cela. Pour regarder un objet double-cliquez dessus. Ou vous pouvez passer le curseur sur l'inventaire avec "Tab", choisir un objet et appuyez sur "Entrée" deux fois. Pour ramener le curseur à la description de la scène, appuyez sur "Tab" à nouveau.^^

		Alors, regardez le papier. Ensuite, faites la même chose avec le crayon.]],
};

apple = obj {
	nam = 'pomme',
	dsc = 'Il y a une {pomme} sur la table.',
	tak = 'Vous prenez la pomme sur la table.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Vous mangez la pomme.';
		end
		p 'Elle est appétisante.';
	end,
};

desk = obj {
	nam = 'bureau',
	dsc = 'Dans cette leçon, vous voyez un {bureau} en bois.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Un grand bureau en chêne. Il y a une inscription au crayon: "Lorem Ipsum".';
		end
		p 'Un grand bureau en chêne.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Vous écrivez plusieurs lettres sur le bureau.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Leçon 3',
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
	dsc = [[Leçon 3. Utiliser l'inventaire — Part II^^
		Vous pouvez agir avec les objets de l'inventaire sur d'autres objets de la scène ou de l'inventaire. Dans cette salle, vous verrez un bureau. Essayez d'utiliser un crayon sur elle.^^ 

		Pour ce faire, cliquez la souris sur le crayon, puis cliquez sur le bureau. Ou vous pouvez faire la même chose avec "Tab", "enter" et flèches.^^
  
		Il suffit de regarder la table. Et n'oubliez pas de prendre la pomme, vous en aurez besoin dans la prochaine leçon.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Leçon 4',  
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
	dsc = [[Leçon 4. Utiliser l'inventaire — Part III^^
		Bon, maintenant, nous allons apprendre plusieurs autres actions avec les objet.^^
	
		Tout d'abord, écrire quelque chose sur le papier. Cliquez crayon, puis papier. Ensuite, vous pouvez regarder le document pour voir votre écriture.^^
	
		Deuxièmement, manger la pomme, que vous avez pris dans la salle précédente. Double-cliquez sur elle.^^

		Toutes ces actions peuvent être faites avec le clavier comme décrit précédemment.]],
};

r5 = room {
	nam = 'Leçon 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Nous avons déjà terminé cette leçon.^ En avant pour la leçon 6, s\'il vous plaît.'
			return false;
		end
	end,
	dsc = [[Leçon 5. en mouvement — Part I^^
		Maintenant, nous allons étudier les passages entre les pièces. Dans cette salle, vous voyez cinq liens avec nombre de leçons. Aller à la leçon 6.^^

		Cliquez sur le lien approprié ou utiliser le clavier - chercher le lien avec les flèches et appuyez sur "Entrée".]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Leçon 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Nous avons déjà terminé cette leçon..^ En avant pour la dernière leçon, s\'il vous plaît.', false;
		end
	end,
	dsc = [[Leçon 6. en mouvement - Part II^^
		Now go to the last lesson.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6', 'theend'},
};

theend = room {
	nam = 'dernière leçon',
	dsc = [[Avec l'aide du menu, vous pouvez choisir un jeu, sauvegarder ou charger un jeu, ou faire d'autres choses. Pour ouvrir le menu, appuyez sur "Esc" ou cliquez sur l'icône de menu en bas à droite.^^
		Maintenant, vous êtes pret à jouer. Bonne chance!!!^^
		Télécharger des jeux pour INSTEAD ici: ]]..txtu("http://instead-games.sourceforge.net")..[[^^
		Le didacticiel utilise "Instead game-engine" de svenzzon en thème musical.]],
	obj = { vway('keys', 'See {liste des raccourcis clavier}.', 'help')},
};

help = room {
	nam = 'Raccourcis clavier',
	dsc = [[
	Esc - Menu.^
	Alt+Q - Quitter.^
	Alt+Enter - Plein écran/Fenêtre.^
	F5 - Rafraîchir la scène.^
	Space/Backspace - Faites défiler la description de la scène.^
	Tab/Shift+Tab - Basculer entre les zones actives.^
	PgUp/PgDn - Faites défiler les zones actives.^
	Arrows left/right - Choisissez les liens dans la zone active.^
	F2 - Sauvegarder.^
	F3 - Charger un jeu.^
	F8 - Sauvegarde rapide.^
	F9 - Chargement rapide.^^

	Raccourcis, qui dépendent du mode de clavier choisi
	(regardez Menu -> Configuration -> mode clavier):^^

	Flèches haut/bas:^
	 - "Scroll" mode: défilement zone active.^
	 - "Links" mode: choisir des liens dans la zone active.^
	 - "Smart" mode: à la fois de choisir les liens et faire défiler la zone active.^^

	Flèches haut/bas + Shift ou Alt:^
	 - "Scroll" mode: choisir des liens dans la zone active.^
	 - "Links" mode: défilement zone active.^
	 - "Smart" mode: défilement zone active.
	]],
	obj = { vway('cmdline', 'Regardez {paramètres de ligne de commande}.', 'help2')},
};

help2 = room {
	nam = 'paramètres de ligne de commande',
	dsc = [[
	-nosound - Lancement sans son;^
	-alsa - force l'utilisation d'ALSA (Linux uniquement);^
	-debug - mode debugage pour les développeurs;^
	-theme <theme> - choisir un thème;^
	-game <game> - choisir un jeu;^
	-themespath <path> - chemin supplémentaire pour les thèmes;^
	-gamespath <path> - chemin supplémentaire pour les jeux;^
	-window - mode fenêtré;^
	-fullscreen  - mode plein écran;^
	-noautosave - désactiver les options sauvegarde auto./Chargement auto. ;^
	-encode <game.lua> [encoded.lua] - encoder code source lua (utilisation doencfile de lua pour charger);^
	-mode [WxH] - utiliser la résolution WxH.
	]],
	obj = { vway('keys', 'Regardez {liste des raccourcis clavier}.', 'help')},
};