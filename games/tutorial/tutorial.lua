require "snd"
require "fmt"

obj {
	nam = '$';
	act = function(s, w)
		return w
	end;
}

fmt.dash = true

function L(t)
	return t[LANG] or t.en
end

game.act = L {
	ru = 'Не получается.',
	de = 'Das können Sie nicht.',
	en = "You can't.",
	es = 'No puedes hacer eso.',
	fr = 'Vous ne pouvez pas.',
	it = 'Non puoi.',
	nl = 'Dat kan niet.',
	pt = 'Isso não é permitido.',
	uk = 'Не виходить.',
}

game.inv = L {
	ru = 'Гм.. Не то..',
	de = 'Hmm … Falsch …',
	en = "Hmm... Wrong...",
	es = "Hmm... Nop...",
	fr = "Hmm… Mauvais…",
	it = "Hmm... Sbagliato...",
	nl = "Hmm... Verkeerd...",
	pt = "Hmm... Não...",
	uk = "Гм.. Не те..",
}

game.use = L {
	ru = 'Не сработает...',
	de = 'Das wird nicht funktionieren.',
	en = "Won't work...",
	es = 'No pasa nada...',
	fr = 'Ne fonctionne pas…',
	it = 'Non funziona...',
	nl = 'Dat werkt niet...',
	pt = 'Não funciona...',
	uk = 'Не спрацює...',
}

game.pic = 'instead.png'

snd.music 'instead.ogg'
xact.walk = walk

room {
	nam = 'main';
	disp = L{ ru = 'Обучение',
		en = 'Tutorial',
		de = 'Einführung',
		es = 'Tutorial',
		fr = 'Tutoriel',
		it = 'Tutorial',
		nl = 'Snelstart Training',
		pt = 'Tutorial',
		uk = 'Навчання',
	};
	dsc = L{
		ru = [[{$fmt c|Добро пожаловать в режим обучения INSTEAD.}^^
	Игра состоит из сцен. Каждая сцена игры имеет описание,
	состоящее из статической и динамической части. Динамическая часть включает объекты,
	персонажей и т.д. С динамической частью игрок может взаимодействовать с помощью мыши, 
	нажимая на подсвеченные ссылки.^^

	Данная сцена называется "Обучение" и сейчас Вы читаете статическую часть её описания.
	Единственным объектом сцены является объект "Дальше", который Вы видите внизу текста.
	Итак, для продолжения обучения Вы можете нажать на "Дальше".^^

	{@ walk r1|Дальше}]],

		en = [[
	{$fmt c|Welcome to INSTEAD tutorial mode.}^^
	The game consists of scenes. Each game scene has a description, consisting of static
	and dynamic parts. Dynamic part includes objects, characters, etc. A player can interact
	with the dynamic part by clicking mouse on the highlighted links.^^

	The current scene is called "Tutorial" and you are reading the static part of its
	description. The only object in this scene is the "Next" object, which you can see below the text.
	Thus, to continue learning you can click "Next".^^

	{@ walk r1|Next}]],

		de = [[{$fmt c|Willkommen in der INSTEAD-Einführung.}^^
	Das Spiel besteht aus Szenen. Jede Spielszene hat eine Beschreibung, welche wiederum
	aus statischen und dynamischen Teilen besteht. Die dynamischen Teile beeinhalten Objekte,
	Figuren, usw. Ein Spieler kann mit dem dynamischen Teil interagieren, indem er mit
	der Maus auf die hervorgehobenen Links klickt.^^

	Die aktuelle Szene wird »Einführung« genannt und Sie lesen den statischen Teil
	ihrer Beschreibung. Das einzige Objekt in dieser Szene ist das »Weiter«-Objekt,
	welches Sie unter dem Text sehen können. Um mit dem Lernen fortzufahren,
	können Sie auf »Weiter« klicken.^^

	{@ walk r1|Weiter}]],

		es =  [[{$fmt c|Bienvenido al tutorial de INSTEAD.}^^
	En INSTEAD un juego se divide en "escenas". Cada escena tiene una descripción,
	conformada por secciones estáticas y dinámicas. Las secciones estáticas pueden
	incluir objetos, personajes, etc. Como jugador puedes interactuar con las partes
	dinámicas al hacer click en los links resaltados.^^

	La escena actual se llama "Tutorial" y tu estás leyendo la sección estática de su
	descripción. El único objeto en esta escena es el objeto "Siguiente", que puedes
	ver más abajo. Esto es: para seguir aprendiendo haz click en "Siguiente".^^

	{@ walk r1|Siguiente}]],

		fr = [[{$fmt c|Bienvenue dans le tutoriel de INSTEAD.}^^
	Le jeu se compose de scènes. Chaque scène de jeu a une description, constituée de parties statiques et dynamiques.
	La partie dynamique comprend des objets, des personnages, etc.
	Un joueur peut interagir avec la partie dynamique en cliquant avec la
	souris sur les liens en surbrillance.^^

	La scène actuelle est appelée « Tutoriel » et vous êtes en train de lire la
	partie statique de sa description. Le seul objet dans cette scène est
	l'objet « Suivant », que vous pouvez voir ci-dessous. Ainsi, pour
	continuer à apprendre, cliquez sur « Suivant ».^^

	{@ walk r1|Suivant}]],

		it = [[{$fmt c|Benvenuto in questa guida introduttiva ad INSTEAD.}^^
	Il gioco consiste di scene. Ogni scena del gioco ha una descrizione, formata da parti statiche e dinamiche.
	Le parti dinamiche includono oggetti, personaggi, etc... Il giocatore può interagire con la
	parte dinamica cliccando con il mouse sui link evidenziati.^^

	La scena corrente è chiamata "Tutorial" e state leggendo la parte statica della sua descrizione.
	L'unico oggetto di questa scena è l'oggetto "Avanti", che potete vedere sotto il testo.
	Così, per continuare questa guida basta fare click su "Avanti".^^

	{@ walk r1|Avanti}]],

		nl = [[{$fmt c|Welkom bij de INSTEAD snelstart training.}^^
	Het spel bestaat uit scènes. Elke scène heeft een beschrijving. Een beschrijving
	bestaat uit vaste en dynamische gedeeltes. Dynamische gedeeltes zijn bijvoorbeeld
	objecten of personages. Een speler kan de dynamische gedeeltes beïnvloeden door
	op de gemarkeerde te klikken.^^

	De huidige scène heet "Snelstart Training" waarvan je op dit moment het statische gedeelte
	van de omschrijving aan het lezen ben. Het enige object in deze scène is het
	"Volgende" object. Die kun je onder deze tekst vinden. Om de rest van de training
	te lezen, moet je op "Volgende" klikken..^^

	{@ walk r1|Volgende}]],

		pt = [[{$fmt c|Bem-vindo ao tutorial do INSTEAD.}^^
	Cada jogo consiste de cenas. Cada cena do jogo tem uma descrição, com partes estáticas
	e dinâmicas. Partes dinâmicas incluem objetos, personagens, etc.
	Um jogador pode interagir com a parte dinâmica clicando com o mouse nos links realçados.^^

	A cena corrente é chamada de "Tutorial" e você está lendo a parte estática da sua
	descrição. O único objeto nesta cena é o objeto "Próxima",
	que você pode ver abaixo do texto. Então, para continuar aprendendo
	você pode clicar "Próxima".^^

	{@ walk r1|Próxima}]],

		uk = [[{$fmt c|Вітаємо в режимі навчання INSTEAD.}^^
	Гра складається зі сцен. Кожна сцена гри має опис,
	що складається зі статичної та динамічної частини. Динамічна частина включає об'єкти,
	персонажі й т.і. З динамічною частиною гравець може взаємодіяти за допомогою миші, 
	нажимаючи на підсвічені посилання.^^

	Дана сцена має назву "Навчання" і зараз Ви читаєте статичну частину її опису.
	Єдиним об'єктом сцени є об'єкт "Далі", котрий Ви бачите внизу тексту.
	Отже, для продовження навчання Ви можете натиснути на "Далі".^^

	{@ walk r1|Далі}]],
	}
}

obj {
	nam = 'paper';
	disp = L {
		ru = 'бумага',
		en = 'paper',
		de = 'Papier',
		es = 'papel',
		fr = 'papier',
		it = 'foglio',
		nl = 'papier',
		pt = 'papel',
		uk = 'папір',
	};
	dsc = L {
		ru = 'Первое, что Вы замечаете в комнате -- {листок бумаги}.',
		en = 'The first thing you notice in the room is a {sheet of paper}.',
		de = 'Das Erste, was Sie im Raum bemerken, ist ein {Blatt Papier}.',
		es = 'Lo primero que ves en la habitación es un {pedazo de papel}.',
		fr = 'La première chose que vous remarquez dans la pièce est une {feuille de papier}.',
		it = 'La prima cosa che potete notare nella stanza è un {foglio di carta}.',
		nl = 'Het eerste dat in deze ruimte opvalt, is een {blad papier}.',
		pt = 'A primeira coisa que você vê no local é uma {folha de papel}.',
		uk = 'Перше, що Ви помічаєте в кімнаті -- {аркуш паперу}.',
	};
	tak = L {
		ru = 'Вы взяли бумагу.',
		en = 'You take the paper.',
		de = 'Sie nehmen das Papier.',
		es = 'Coges el papel.',
		fr = 'Vous prenez le papier.',
		it = 'Prendi il foglio.',
		nl = 'Je pakt het blad papier.',
		pt = 'Você pega o papel.',
		uk = 'Ви взяли папір.',
	};
	seen = false;
	haswriting = false;
	inv = function(s)
		if here()^'r2' then
			s.seen = true
		end
		if not s.haswriting then
			p (L {
				ru = 'Чистый лист клетчатой бумаги. Похоже, его вырвали из тетради.',
				en = 'A blank sheet of squared paper. Probably torn out of the copy-book.',
				de = 'Ein leeres Blatt kariertes Papier. Vermutlich wurde aus einem Schreibblock ausgerissen.',
				es = 'Una hoja blanca y cuadrada. Probablemente arrancada de un libro.',
				fr = 'Une feuille de papier quadrillé. Probablement arrachée d’un livre.',
				it = 'Un foglio di carta bianca a quadretti. Probabilmente strappato da un quaderno.',
				nl = 'Een blanco blad gelinieerd papier. Waarschijnlijk uit een notitieboekje gescheurd.',
				pt = 'Uma folha branca e quadrada de papel. Provavelmente rasgada de um livro.',
				uk = 'Чистий аркуш картатого паперу. Схоже, його вирвали з зошита.',
			})
			return
		end
		p (L {
			ru = 'Лист клетчатой бумаги, на котором написано Ваше имя.',
			en = 'A sheet of squared paper with your name on it.',
			de = 'Ein Blatt kariertes Papier, auf dem Ihr Name geschrieben steht.',
			es = 'Una hoja cuadrada de papel con tu nombre escrito en ella.',
			fr = 'Une feuille de papier quadrillé avec votre nom.',
			it = 'Un foglio a quadretti con il tuo nome scritto sopra.',
			nl = 'Een blad gelinieerd papier waarop jouw naam geschreven staat.',
			pt = 'Uma folha quadrada de papel com seu nome nela.',
			uk = 'Аркуш картатого паперу, на якому написано Ваше ім`я.',
		})
	end;
	used = function(s, w)
		if w^'pencil' and here()^'r4' then
			s.haswriting = true
			p (L {
				ru = 'Вы пишете на листке своё имя.',
				en = 'You write your name on the paper.',
				de = 'Sie schreiben Ihren Namen auf das Papier.',
				es = 'Escribes tu nombre en el papel.',
				fr = 'Vous écrivez votre nom sur le papier.',
				it = 'Scrivi il tuo nome sul foglio.',
				nl = 'Je schrijft je naam op het papier.',
				pt = 'Você escreve o seu nome no papel.',
				uk = 'Ви пишите на аркуші своє ім`я.',
			})
		end
	end
}

obj {
	nam = 'pencil';
	disp = L {
		ru = 'карандаш',
		en = 'pencil',
		de = 'Stift',
		es = 'lápiz',
		fr = 'crayon',
		it = 'matita',
		nl = 'potlood',
		pt = 'lápis',
		uk = 'олівець',
	};
	dsc = L {
		ru = 'На полу лежит {карандаш}.',
		en = 'There is a {pencil} on the floor.',
		de = 'Dort liegt ein {Stift} auf dem Boden.',
		es = 'Hay un {lápiz} en el piso.',
		fr = 'Il y a un {crayon} sur le sol.',
		it = "C'è anche una {matita} sul pavimento.",
		nl = 'Er ligt een {potlood} op de grond.',
		pt = 'Há um {lápis} no chão.',
		uk = 'На підлозі лежить {олівець}.',
	};
	tak = L {
		ru = 'Вы подобрали карандаш.',
		en = 'You pick the pencil.',
		de = 'Sie nehmen den Stift.',
		es = 'Coges el lápiz.',
		fr = 'Vous prenez le crayon.',
		it = 'Raccogli la matita.',
		nl = 'Je pakt het potlood op.',
		pt = 'Você pega o lápis.',
		uk = 'Ви підібрали олівець.',
	};
	seen = false;
	inv = function(s)
		if here() ^ 'r2' then
			s.seen = true
		end
		p (L {
			ru = 'Обычный деревянный карандаш.',
			en = 'An ordinary wooden pencil.',
			de = 'Ein gewöhnlicher Stift.',
			es = 'Un lápiz común de madera.',
			fr = 'Un crayon ordinaire, en bois.',
			it = 'Una normale matita di legno.',
			nl = 'Een heel normaal houten potlood.',
			pt = 'Um simples lápis de madeira.',
			uk = 'Звичайний дерев`яний олівець.',
		})
	end
}

room {
	nam = 'r1';
	disp = L {
		ru = 'Урок 1',
		en = 'Lesson 1',
		de = '1. Lektion',
		es = 'Lección 1',
		fr = 'Leçon nº 1',
		it = 'Lezione 1',
		nl = 'Les 1',
		pt = 'Lição 1',
		uk = 'Урок 1',
	};
	enter = function(s)
		lifeon(s)
	end;
	life = function(s)
		if not have 'paper' or not have 'pencil' then
			return
		end
		lifeoff(s)
		enable '#cont'
	end;
	dsc = L {
		ru = [[Урок 1. Взаимодействие с объектами^^
		Продолжим урок. Сейчас Вы находитесь в комнате. Возьмите оба предмета,
		которые Вы видите. Напомним, что для этого Вы можете просто
		использовать мышь. Если Вам удобнее пользоваться клавиатурой -- можно
		выбрать нужный предмет с помощью клавиш со стрелками и нажать "Ввод" для
		взаимодействия с ним.]],

		en = [[Lesson 1. Interacting with objects^^
		Let's continue the lesson. You are inside a room. Take both objects that you see.
		You can just click on them with mouse. If you prefer keyboard, you can pick
		an object with arrow keys and press "Enter" to interact with it.]],

		de = [[1. Lektion: Interaktion mit Objekten^^
		Lassen Sie uns mit der Lektion fortfahren. Sie befinden sich in einem Raum.
		Nehmen Sie beide Objekte, die Sie sehen. Sie können sie einfach mit der Maus anklicken.
		Falls Sie die Tastatur bevorzugen, können Sie ein Objekt mit den Pfeiltasten auswählen
		und [Eingabe] drücken, um mit ihm zu interagieren.]],

		es = [[Lección 1. Interactuar con objetos^^
		Continuemos con el tutorial. Estás en una habitación. Coge los dos objetos que ves aquí.
		Para ello, puedes clickear sobre ellos con el ratón o -si lo prefieres- usar las flechas
		de tu teclado para posicionarte y el "Enter" para interactuar.]],

		fr = [[Leçon nº 1. Interagir avec les objets^^
		Continuons la leçon. Vous êtes à l'intérieur d'une pièce.
		Prenez les deux objets que vous y voyez. Vous pouvez simplement cliquer dessus avec la souris,
		ou, si vous préférez le clavier, vous pouvez choisir un objet avec les touches
		fléchées et appuyer sur « Entrée » pour interagir avec lui.]],

		it = [[Lezione 1. Interagire con gli oggetti^^
		Continuiamo la nostra guida. Siete dentro una stanza e potete vedere due oggetti presenti.
		Per prenderli fate semplicemente click su di essi con il mouse o, se preferite
		usare la tastiera, potete raccogliere un oggetto usando i tasti freccia
		e premendo quindi "Invio" per interagire con essi.]],

		nl = [[Les 1. Werken met objecten^^
		We gaan verder met de training. Je bent in een ruimte. Pak beide objecten die je ziet.
		Je kunt er simpelweg op klikken met de muis. Gebruik je liever het toetsenbord,
		gebruik dan de pijltjes toetsen om het object te selecteren, en druk op "Enter"
		om het op te pakken.]],

		pt = [[Lição 1. Interagindo com objetos^^
		Continuando com a lição. Você está num local. Pegue os objetos que você vê.
		Basta clicar neles com o mouse. Se preferir o teclado, você pode apontar para
		um objeto com as setas direcionais e pressionar "Enter" para interagir com ele.]],

		uk = [[Урок 1. Взаємодія з об`єктами^^
		Продовжимо урок. Зараз Ви знаходитесь в кімнаті. Візьміть обидва предмети, 
		котрі Ви бачите. Нагадаємо, що для цього Ви можете просто
		використовувати мишу. Якщо Вам зручніше використовувати клавіатуру -- можна
		обрати потрібний предмет за допомогою клавіш зі стрілками и натиснути "Введення" для
		взаємодії з ними.]],
	};
}:with { 'paper', 'pencil', 
	obj {
		nam = '#cont';
		dsc = L {
			ru = [[Отлично!^Вы скорее всего заметили, что статическая часть описания сцены
		исчезла, уступив место описанию предметов, которые Вы взяли.
		Чтобы снова посмотреть полное описание сцены, можно нажать на её
		название, отображаемое в верхней части окна -- "Урок 1". Кроме того,
		можно нажать клавишу F5 на клавиатуре, результат будет таким же.^^

		{Дальше}]],

			en = [[Excellent!^You've probably noticed that the static part of the scene description disappeared,
		replaced by the descriptions of the objects you've taken. To see the full scene description
		again either click on the scene name at the top — "Lesson 1" or press F5. The result is the same.^^

		{Next}]],

			de = [[Ausgezeichnet!^
		Sie haben vermutlich bemerkt, dass der statische Teil der Szenenbeschreibung verschwunden
		ist und durch die Beschreibungen der Objekte, die Sie genommen haben, ersetzt wurde.
		Um die vollständige Szenenbeschreibung wieder zu sehen, klicken Sie entweder auf den Szenennamen – »1. Lektion« – oben oder drücken Sie F5.
		Das Ergebnis wird identisch sein.^^

		{Weiter}]],

			es = [[Genial!^
		Te abrás dado cuenta que la parte estática de la descripción ya no está; reemplazada por las descripciones de los objetos que cogiste.
		Para ver nuevamente la descricpión completa de esta escena puedes clickear en el nombre arriba (Lección 1) o pulsar F5. Es lo mismo.^^

		{Siguiente}]],

			fr = [[Excellent !^Vous avez probablement remarqué que la partie statique de la description de la scène a disparu,
		remplacée par les descriptions des objets que vous avez pris. Pour voir à nouveau la description complète
		de la scène, cliquez sur le nom de la scène au sommet – « Leçon nº 1 » – ou appuyez sur F5. Le résultat est le même.^^

		{Suivant}]],

			it = [[Eccellente!^Avrete probabilmente notato che la parte statica della descrizione della scena è scomparsa,
		rimpiazzata dalla descrizione degli oggetti che avete preso. Per vedere di nuovo la descrizione completa
		della scena potete fare click sul nome della scena in alto — "Lezione 1" o premere F5.
		Il risultato è lo stesso.^^

		{Next}]],

			nl = [[Fantastisch!^Je hebt waarschijnlijk gemerkt dat het statische gedeelte van
		de scène beschrijving verdwenen is. Het werd vervangen met de omschrijving van
		de objecten die je gepakt hebt. Als je de volledige omschrijving nogmaals wilt
		lezen, klik je danwel op de naam van de scène bovenin (dus op "Les 1"), of je
		drukt op F5. Beide geeft hetzelfde resultaat.^^

		{Volgende}]],

			pt = [[Excelente!^Você provavelmente notou que a parte estática da descrição
		da cena desapareceu, substituída pelas descrições dos objetos que você pegou.
		Para ver a descrição completa da cena novamente clique no seu nome no alto da
		da página — "Lição 1" ou tecle F5. O resultado é o mesmo.^^

		{Próxima}]],

			uk = [[Відмінно!^Ви, швидше за все помітили, що статична частина опису сцени
		зникла, поступившись місцем опису предметів, котрі Ви взяли.
		Щоб знову подивитись повний опис сцени, можна нажати на її
		назву, відображену у верхній частині вікна -- "Урок 1". Окрім того,
		можна нажати клавішу F5 на клавіатурі, результат буде такий само.^^

		{Далі}]],
		};
		act = function()
			walk 'r2'
		end;
	}:disable();
}

room {
	nam = 'r2';
	disp = L {
		ru = 'Урок 2',
		en = 'Lesson 2',
		de = '2. Lektion',
		es = 'Lección 2',
		fr = 'Leçon nº 2',
		it = 'Lezione 2',
		nl = 'Les 2',
		pt = 'Lição 2',
		uk = 'Урок 2',
	};
	enter = function(s)
		lifeon(s)
	end;
	life = function(s)
		if not _'paper'.seen or not _'pencil'.seen then
			return
		end
		lifeoff(s)
		enable '#cont'
	end,
	dsc = L {
		ru = [[Урок 2. Использование инвентаря - Часть I^^
		Теперь у Вас появились предметы, которые можно использовать или изучать.
		Для этого предназначен инвентарь. Вы можете посмотреть на любой предмет 
		инвентаря дважды щелкнув мышью по этому предмету. То же самое можно 
		сделать с помощью клавиатуры: нажмите "Tab", чтобы переключиться на панель 
		инвентаря, выберите нужный предмет и дважды
		нажмите "Ввод". Чтобы вернуться назад к описанию сцены, нажмите "Tab" еще раз.^^

		Итак, посмотрите на бумагу. Затем, повторите эту операцию с карандашом.]],

		en = [[Lesson 2. Using the inventory — Part I^^
		Now you've got objects that you can examine or use. That's what the inventory is for.
		To look at any object double click on it. Or you can switch the cursor to the inventory with "Tab",
		choose an object and press "enter" twice. To return the cursor to the scene description,
		press "Tab" again.^^

		So, look at the paper. Then do the same with the pencil.]],

		de = [[2. Lektion: Benutzung des Inventars – 1. Teil^^
		Nun haben Sie Objekte, die Sie untersuchen oder benutzen können. Dafür ist das Inventar da.
		Um ein beliebiges Objekt zu betrachten, doppelklicken Sie darauf. Alternativ können Sie zum Inventar mit [Tab] wechseln,
		und ein Objekt auswählen, indem Sie zwei mal die Eingabetaste drücken.
		Um den Zeiger zurück zur Szenenbeschreibung zu bewegen, drücken Sie erneut [Tab].^^

		Sehen Sie sich das Papier an. Dann tun Sie dasselbe mit dem Stift.]],

		es = [[Lección 2. Usando el inventario — Parte I^^
		Ahora tienes en tu poder objetos que puedes examinar o usar. Eso es tu inventario.
		Para examinar un objeto haz doble click en él. O puedes manejar el cursor con la tecla "Tab",
		elegir un objeto y presionar Enter dos veces.
		Para volver el cursor a la descripción de la escena, presiona nuevamente "Tab".^^

		Ok, examina el papel. Y haz lo propio con el lápiz.]],

		fr = [[Leçon nº 2. Utiliser l'inventaire — Partie I^^
		Vous avez maintenant des objets que vous pouvez examiner ou utiliser.
		L'inventaire est fait pour cela. Pour regarder un objet, double-cliquez dessus.
		Ou alors, vous pouvez faire passer le curseur dans l'inventaire avec « Tab »,
		y choisir un objet et appuyer sur « Entrée » deux fois. Pour ramener le curseur
		à la description de la scène, appuyez sur « Tab » à nouveau.^^

		Regardez maintenant le papier. Faites ensuite la même chose avec le crayon.]],

		it = [[Lezione 2. Usare l'inventario — Parte I^^
		Ora avete nel vostro inventario due oggetti che potete esaminare od usare.
		Per esaminare un qualsiasi oggetto fate doppio click su di esso oppure potete
		posizionare il cursore nell'inventario con il tasto "Tab", scegliete l'oggetto e
		premete "invio" due volte. Per tornare con il cursore alla descrizione della
		scena premete di nuovo "Tab"^^

		Ora, osservate il foglio, e quindi fate lo stesso con la matita.]],

		nl = [[Les 2. Het gebruik van de inventaris — Deel I^^
		Je hebt nu objecten die je kunt onderzoeken of gebruiken. Daarvoor is het
		inventarisscherm. Om een object te bekijken, klik je er twee keer op. Of
		je kan met de "Tab" toets naar het inventarisscherm schakelen en vervolgens
		het betreffende object kiezen met de pijltjestoetsen, en dan twee keer op
		"Enter" drukken. Om terug te gaan naar het gedeelte omschrijving kun je
		daarna weer op "Tab" drukken.^^

		Dus, bekijk nu het papier, en bekijk daarna ook het potlood.]],

		pt = [[Lição 2. Usando o inventário — Parte I^^
		Agora você tem objetos para examinar ou usar. O inventário é para isso.
		Para examinar algum objeto dê duplo clique nele. Ou você pode mover o cursor
		para o inventário com a tecla "Tab", escolha um objeto e tecle "enter" duas vezes.
		Para levar o cursor de volta para a descrição da cena, tecle "Tab" novamente.^^

		Então, examine o lápis. Faça o mesmo com o papel.]],

		uk = [[Урок 2. Використання інвентарю - Частина I^^
		Тепер у Вас з'явилися предмети, котрі можно використовувати чи вивчати.
		Для цього призначений інвентар. Ви можете подивитися на будь-який предмет 
		інвентарю двічи клацнувши мишею по цьому предмету. Те ж саме можна
		зробити за допомогою клавіатури: нажміть "Tab", щоб перемкнутися на панель 
		інвентарю, оберіть потрібний предмет і двічі
		нажміть "Введення". Щоб повернутися назад до опису сцени, нажмить "Tab" ще раз.^^

		Отже, подивіться на папір. Потім, повторіть цю операцію з олівцем.]],
	};
}:with {
	obj {
		nam = '#cont';
		dsc = L {
			ru = "Хорошо!^^{Дальше}",
			en = "Good!^^{Next}",
			de = "Gut!^^{Weiter}",
			es = "Bien!^^{Siguiente}",
			fr = "Bien !^^{Suivant}",
			it = "Bene!^^{Avanti}",
			nl = "Goed!^^{Volgende}",
			pt = "Muito Bem!^^{Próxima}",
			uk = "Гарно!^^{Далі}",
		};
		act = function()
			walk 'r3'
		end;
	}:disable();
}

obj {
	nam = 'apple';
	disp = L {
		ru = 'яблоко',
		en = 'apple',
		de = 'Apfel',
		es = 'manzana',
		fr = 'pomme',
		it = 'mela',
		nl = 'appel',
		pt = 'maçã',
		uk = 'яблуко',
	};
	dsc = L {
		ru = 'На столе лежит {яблоко}.',
		en = "There's an {apple} on the table.",
		de = 'Auf dem Tisch liegt ein {Apfel}.',
		es = 'Hay una {manzana} sobre la mesa.',
		fr = 'Il y a une {pomme} sur le bureau.',
		it = "C'è una {mela} sul tavolo.",
		nl = 'Er ligt een {appel} op de tafel.',
		pt = 'Há uma {maçã} na mesa.',
		uk = 'На столі лежить {яблуко}.',
	};
	tak = L {
		ru = 'Вы взяли яблоко со стола.',
		en = "You take an apple from the table.",
		de = 'Sie nehmen sich einen Apfel vom Tisch.',
		es = 'Coges la manzana de la mesa.',
		fr = 'Vous prenez la pomme sur le bureau.',
		it = 'Prendi la mela dal tavolo.',
		nl = 'Je pakt de appel van de tafel.',
		pt = 'Você pega a maçã da mesa.',
		uk = 'Ви взяли яблуко зі столу.',
	};
	knife = false;
	inv = function(s)
		if here() ^ 'r4' then
			remove(s)
			p (L {
				ru = "Вы съедаете яблоко.",
				en = "You eat the apple.",
				de = 'Sie essen den Apfel.',
				es = 'Te comes la manzana.',
				fr = 'Vous mangez la pomme.',
				it = 'Mangi la mela.',
				nl = 'Je eet de appel op.',
				pt = 'Você comeu a maçã.',
				uk = 'Вы з`їдаєте яблуко.',
			})
			return
		end
		p (L {
			ru = "Выглядит аппетитно.",
			en = "Looks appetizing.",
			de = 'Sieht appetitlich aus.',
			es = 'Luce apetecible.',
			fr = 'Elle est appétissante.',
			it = 'Sembra appetitosa.',
			nl = 'Ziet er smakelijk uit.',
			pt = 'Hum, apetitosa.',
			uk = 'Виглядає апетитно.',
		})
	end
}

obj {
	nam = 'desk';
	disp = L {
		ru = 'стол',
		en = 'desk',
		de = 'Schreibtisch',
		es = 'escritorio',
		fr = 'bureau',
		it = 'scrivania',
		nl = 'bureau',
		pt = 'escrivaninha',
		uk = 'стіл',
	},
	dsc = L {
		ru = 'На этом уроке вы видите деревянный {письменный стол}.',
		en = 'In this lesson you see a wooden {desk}.',
		de = 'In dieser Lektion sehen Sie einen hölzernen {Schreibtisch}.',
		es = 'Aquí ves un {escritorio} de madera.',
		fr = 'Dans cette leçon, vous voyez un {bureau} en bois.',
		it = 'Puoi vedere una {scrivania} di legno.',
		nl = 'In deze ruimte zie je een houten {bureau}.',
		pt = 'Você vê aqui uma {escrivaninha} de madeira.',
		uk = 'В цьому уроці ви бачите дерев`яний {письмовий стіл}.',
	},
	haswriting = false;
	seen = false;
	act = function(s)
		if s.haswriting then
			s.seen = true;
			p (L {
				ru = 'Большой дубовый письменный стол. На столешнице видна мелкая надпись карандашом: "Lorem Ipsum".',
				en = [[Large oaken desk. There's a pencil inscription on its top: "Lorem Ipsum".]],
				de = 'Ein großer Schreibtisch aus Eichenholz. Auf seiner Oberfläche steht mit einem Bleistift geschrieben: »Lorem Ipsum«.',
				es = 'Un gran escritorio de roble. Hay algo escrito en él: "Lorem Ipsum".',
				fr = 'Un grand bureau en chêne. Il y a dessus une inscription au crayon : « Lorem Ipsum ».',
				it = [[Una larga scrivania di legno. C'è un'iscrizione sul piano: "Lorem Ipsum".]],
				nl = 'Een groot eiken bureau. Er is met potlood op geschreven: "Lorem Ipsum".',
				pt = 'Uma grande escrivaninha de carvalho. Há uma inscrição a lápis no seu topo: "Lorem Ipsum".',
				uk = 'Великий дубовий письмовий стіл. На стільниці видно дрібний напис олівцем: "Lorem Ipsum".',
			})
			return
		end
		p (L {
			ru = "Большой дубовый письменный стол.",
			en = "Large oaken desk.",
			de = 'Ein großer Schreibtisch aus Eichenholz.',
			es = 'Un gran escritorio de roble.',
			fr = 'Un grand bureau en chêne.',
			it = 'Una larga scrivania in legno.',
			nl = 'Een groot eiken bureau.',
			pt = 'Uma grande escrivaninha de carvalho.',
			uk = 'Великий дубовий письмовий стіл.',
		})
	end;
	used = function(s, w)
		if w ^ 'pencil' and not s.haswriting then
			s.haswriting = true
			p (L {
				ru = "Вы пишете на столешнице несколько букв.",
				en = "You write several letters on the desk.",
				de = 'Sie schreiben einige Buchstaben auf den Tisch.',
				es = 'Garabateas unas palabras sobre el escritorio.',
				fr = 'Vous griffonnez plusieurs lettres sur le bureau.',
				it = 'Scrivi alcune lettere sulla scrivania.',
				nl = 'Je schrijft enkele letters op het bureau.',
				pt = 'Você escreve várias letras na escrivaninha.',
				uk = 'Ви пишете на столешні декілька літер.',
			})
		end
	end;
}:with { 'apple' }

room {
	nam = 'r3';
	disp = L {
		ru = 'Урок 3',
		en = 'Lesson 3',
		de = '3. Lektion',
		es = 'Lección 3',
		fr = 'Leçon nº 3',
		it = 'Lezione 3',
		nl = 'Les 3',
		pt = 'Lição 3',
		uk = 'Урок 3',
	};
	enter = function(s)
		lifeon(s)
	end;
	life = function(s)
		if not _'desk'.seen or not have 'apple' then
			return
		end
		lifeoff(s)
		enable '#cont'
	end;
	dsc = L {
		ru = [[Урок 3. Использование инвентаря - Часть II^^
		Вы можете действовать предметами инвентаря на другие предметы сцены или инвентаря. 
		В этой комнате вы увидите стол. Попробуйте воздействовать карандашом на стол.^^ 

		Для этого нажмите мышью на карандаш, а затем на стол. То же самое можно сделать
		с помощью клавиатуры: используя клавиши "Tab", "Ввод" и "стрелки".^^
  
		Затем посмотрите на стол. И не забудьте взять яблоко, оно нам пригодится в следующем уроке.]],

		en = [[Lesson 3. Using the inventory — Part II^^
		You can act with inventory objects on other objects of the scene or inventory. In this room you'll see a desk. Try using pencil on it.^^

		To do that click mouse on the pencil, then click on the desk. Or you can do the same with "Tab", "enter" and arrows.^^
  
		Then look at the table. And don't forget to take the apple, you'll need it in the next lesson.]],

		de = [[3. Lektion: Benutzung des Inventars – 2. Teil^^
		Sie können mit Inventar- oder anderen Objekten der Szene oder des Inventars interagieren.
		In diesem Raum sehen Sie einen Schreibtisch. Versuchen Sie, den Stift auf ihn anzuwenden.^^

		Um das zu tun, klicken Sie mit der Maus auf den Stift, dann klicken Sie auf den Tisch.
		Alternativ können Sie dasselbe mit [Tab], [Eingabe] und den Pfeiltasten tun.^^

		Dann sehen Sie sich den Tisch an. Und vergessen Sie nicht, den Apfel zu nehmen,
		Sie werden ihn in der nächsten Lektion benötigen.]],

		es = [[Lección 3. Usando el inventario — Parte II^^
		Puedes combinar una acción con un objeto de tu inventario y un objeto de la escena o del mismo inventario.
		En esta habitación verás un escritorio. Prueba usar el lápiz con él.^^

		Para ello, haz click sobre el lápiz, y entonces click sobre el escritorio.
		O puedes conseguir lo mismo con  "Tab", "Enter" y las flechas de tu teclado.^^

		A continuación, examina el escritorio. Y no te olvides de coger la manzana.
		La necesitarás para la próxima lección.]],

		fr = [[Leçon nº 3. Utiliser l'inventaire — Partie II^^
		Vous pouvez faire agir les objets de l'inventaire sur d'autres objets de la scène ou de l'inventaire. 
		Dans cette salle, vous verrez un bureau. Essayez d'utiliser un crayon sur lui.^^

		Pour ce faire, cliquez avec la souris sur le crayon, puis cliquez sur le bureau.
		Ou alors, vous pouvez faire la même chose au clavier avec « Tab », « Entrée » et les flèches.^^
  
		Il suffit ensuite de regarder le bureau pour voir le résultat. Et n'oubliez pas de prendre la pomme,
		vous en aurez besoin dans la prochaine leçon.]],

		it = [[Lezione 3. Usare l'inventario — Parte II^^
		Potete agire con oggetti dell'inventario su altri oggeti della scena o dell'inventario stesso.
		In questa locazione potete vedere una scrivania. Provate ad usare la matita su di essa.^^

		Per farlo fate click con il mouse sulla matita, e quindi sulla scrivania.
		Oppure usate la tastiera con i tasti "Tab", "invio" e le frecce.^^

		Ora guardate il tavolo e non scordatevi di prendere la mela,
		ne avrete bisogno per la prossima dimostrazione.]],

		nl = [[Les 3. Het gebruik van de inventaris — Deel II^^
		Objecten in de inventaris kun je ook gebruiken in combinatie met andere
		objecten. Die andere objecten kunnen zich ook in de inventaris bevinden,
		maar kunnen ook objecten uit de scène beschrijving zijn. In de huidige
		ruimte bevind zich een bureau. Probeer het potlood daarop te gebruiken.^^

		Om dat te doen, klik je eerst op het potlood, vervolgens op het bureau.
		Dat kan natuurlijk ook met het toetsenbord met behulp van de "Tab",
		"Enter" en pijltjestoetsen.^^

		Kijk vervolgens naar de tafel. En pak tenslotte ook de appel op, die heb je
		nodig in de volgende les.]],

		pt = [[Lição 3. Usando o inventário — Parte II^^
		Você pode usar objetos do inventário para afetar outros objetos da cena ou do
		inventário. Neste local você vê uma escrivaninha. Tente usar o lápis nela.^^

		Para isso clique com o mouse no lápis, então clique na escrivaninha.
		Ou você pode fazer o mesmo com "Tab", "enter" e as setas do teclado.^^
  
		Então examine a mesa. E não esqueça de pegar a maçã, você precisará dela
		na próxima lição.]],

		uk = [[Урок 3. Використання інвентаря - Частина II^^
		Ви можете задіяти предмети інвентарю на інші предмети сцени чи інвентарю. 
		В цій комнаті ви бачите стіл. Спробуйте задіяти олівець на стіл.^^ 

		Для цього клацніть мишей на олівець, а потім на стіл. Те ж саме можна зробити
		за допомогою клавіатури: використовуючи клавіши "Tab", "Введення" й "стрілки".^^
  
		Потім подивіться на стіл. І не забудьте взяти яблуко, воно нам знадобиться в наступному уроці.]],
	};
}:with {
	'desk';
	obj {
		nam = '#cont';
		dsc = L {
			ru = "{$|^^}{Дальше}",
			en = "{$|^^}{Next}",
			de = "{$|^^}{Weiter}",
			es = "{$|^^}{Siguiente}",
			fr = "{$|^^}{Suivant}",
			it = "{$|^^}{Avanti}",
			nl = "{$|^^}{Volgende}",
			pt = "{$|^^}{Próxima}",
			uk = "{$|^^}{Далі}",
		};
		act = function()
			walk 'r4'
		end;
	}:disable();
}

room {
	nam = 'r4';
	disp = L {
		ru = 'Урок 4',
		en = 'Lesson 4',
		de = '4. Lektion',
		es = 'Lección 4',
		fr = 'Leçon nº 4',
		it = 'Lezione 4',
		nl = 'Les 4',
		pt = 'Lição 4',
		uk = 'Урок 4',
	};
	enter = function(s)
		_'apple'.knife = false
		lifeon(s)
	end;
	life = function(s)
		if not _'paper'.haswriting or have 'apple' then
			return
		end
		lifeoff(s)
		enable '#cont'
	end;
	dsc = L {
		ru = [[Урок 4. Использование инвентаря - Часть III^^
		Хорошо, теперь изучим еще несколько действий с предметами.^^

		Во-первых, напишите что-нибудь на бумаге. Для этого нажмите мышью
		на карандаш, а затем на бумагу. После этого вы можете посмотреть
		на бумагу, чтобы увидеть вашу запись.^^

		Во-вторых, съешьте яблоко, которое Вы взяли со стола в предыдущей комнате.
		Для этого дважды щелкните по яблоку в инвентаре.^^

		Все описанные действия можно выполнить с помощью клавиатуры так же,
		как показано в предыдущем уроке.]],

		en = [[Lesson 4. Using the inventory — Part III^^
		Good, now let's learn several more actions with objects.^^

		First, write something on the paper. Click pencil, then paper. Then you can look at the paper to see your writing.^^

		Second, eat an apple, you've taken in the previous room. Double click on it.^^

		All those actions can be done with keyboard as described previously.]],

		de = [[4. Lektion: Benutzung des Inventars – 3. Teil^^
		Gut, nun lassen Sie uns einige weitere Aktionen mit Objekten lernen.^^

		Erstens: Schreiben Sie etwas auf das Papier. Klicken Sie den Stift, dann das Papier an.
		Dann können Sie sich das Papier ansehen, um sich Ihr Schreiben anzusehen.

		Zweitens: Essen Sie den Apfel, den Sie vom vorherigen Raum genommen haben. Doppelklicken Sie ihn.^^

		All diese Aktionen können mit der Tastatur durchgeführt werden, wie zuvor beschrieben.]],

		es = [[Lección 4. Usando el inventario — Parte III^^
		Bien, ahora veamos un par de acciones más sobre objetos.^^

		Primero, escribir algo en el papel. Haz click en el lápiz y luego en el papel.
		Y entonces examina el papel para ver lo que haz escrito.^^

		Segundo, cómete la manzana que cogiste en la escena anterior. Haz doble click en ella.^^

		Todas estas acciones también pueden ejecutarse con tu teclado, como ya se ha descrito.]],

		fr = [[Leçon nº 4. Utiliser l’inventaire — Partie III^^
		Bon, maintenant, nous allons étudier plusieurs autres actions sur les objets.^^

		Tout d’abord, écrivez quelque chose sur le papier. Cliquez sur le crayon, puis sur le papier.
		Vous pourrez ensuite regarder le document pour voir le résultat.^^

		Deuxièmement, mangez la pomme que vous avez prise dans la salle précédente.
		Double-cliquez sur elle.^^

		Toutes ces actions peuvent être faites avec le clavier comme décrit précédemment.]],

		it = [[Lezione 4. Usare l'inventario — Parte III^^
		Bene, ora vediamo qualche altra azione con gli oggetti.^^

		Come prima cosa, scrivete qualcosa sul foglio. Fate click sulla matita e quindi sul foglio.
		Ora potete osservare il foglio e controllare cosa avete scritto.^^

		Come secondo passo, mangiate la mela, l'avete presa nella precedente locazione.
		Per farlo fate doppio click su di essa.^^

		Tutte queste azioni possono essere eseguite anche con la tastiera come già descritto in precedenza.]],

		nl = [[Les 4. Het gebruik van de inventaris — Deel III^^
		We gaan nu verdere acties met objecten leren.^^

		Om te beginnen schrijf je iets op het papier. Klik daarvoor op het potlood,
		daarna op het papier. Kijk vervolgens op het papier om het resultaat te bekijken.^^

		Eet tenslotte de appel op die je meegenomen hebt uit de vorige ruimte. Klik
		hiervoor twee maal op de appel.^^

		Natuurlijk kan bovenstaande ook weer met het toetsenbord gedaan worden.]],

		pt = [[Lição 4. Usando o inventário — Parte III^^
		Bom, agora vamos aprender diversas outras ações com objetos.^^

		Primeiro, escreva algo no papel. Clique no lápis, depois no papel.
		Então você pode examinar o papel para ver os seus escritos.^^

		Segundo, coma a maçã que você pegou no local anterior.
		Dê duplo clique nela.^^

		Todas estas ações podem ser realizadas com o teclado, conforme descrito antes.]],

		uk = [[Урок 4. Використання інвентарю - Частина III^^
		Гаразд, тепер вивчимо ще декілька дій з предметами.^^

		По-перше, напішить що-небудь на папері. Для цього нажміть мишею 
		на олівець, а потім на папір. Після цього ви можете подивитись
		на папір, щоб побачити ваш запис.^^

		По-друге, з'їште яблуко, котре Ви взяли зі столу в попередній кімнаті. 
		Для цього двічи клацніть по яблуку в інвентарі.^^

		Всі описані дії можна виконати за допомогою клавіатури так само, 
		як показано в попередньому уроці.]],
	};
}:with {
	obj {
		nam = '#cont';
		dsc = L {
			ru = "Хорошо.^^{Дальше}",
			en = "Good.^^{Next}",
			de = "Gut.^^{Weiter}",
			es = "Bien!^^{Siguiente}",
			fr = "Bien.^^{Suivant}",
			it = "Bene.^^{Avanti}",
			nl = "Goed.^^{Volgende}",
			pt = "Muito bem.^^{Próxima}",
			uk = "Гаразд.^^{Далі}",
		};
		act = function()
			walk 'r5'
		end;
	}:disable();
}

room {
	nam = 'r5';
	disp = L {
		ru = 'Урок 5',
		en = 'Lesson 5',
		de = '5. Lektion',
		es = 'Lección 5',
		fr = 'Leçon nº 5',
		it = 'Lezione 5',
		nl = 'Les 5',
		pt = 'Lição 5',
		uk = 'Урок 5',
	};
	onexit = function(s, t)
		if t ^ 'r6' then
			return
		end
		p (L {
			ru = "Этот урок мы уже прошли.^ Пожалуйста, перейдите на урок 6.",
			en = "We've already finished this lesson.^ Go to lesson 6, please.",
			de = 'Wir haben diese Lektion bereits fertiggestellt.^ Bitte gehen Sie zur 6. Lektion.',
			es = 'Ya hemos terminado esta lección.^ Ve a la lección 6, por favor.',
			fr = 'Nous avons déjà terminé cette leçon…^ En avant pour la leçon nº 6, s’il vous plaît.',
			it = 'Abbiamo già terminato questa parte della guida. Proseguiamo alla prossima, per favore.',
			nl = 'Die les is al afgerond.^ Ga naar les 6, aub.',
			pt = 'Já terminamos esta lição.^ Favor seguir para a lição 6.',
			uk = 'Цей урок ми вже пройшли.^ Будь ласка, перейдіть на урок 6.',
		})
		return false
	end;
	dsc = L {
		ru = [[Урок 5. Перемещение - Часть I^^
		Теперь изучим переходы между комнатами. В этой комнате Вы видите пять 
		дополнительных ссылок с номерами уроков. Перейдите на урок 6. ^^

		Для этого нажмите мышью на соответствующую ссылку. Вы также можете воспользоваться
		клавиатурой -- выберите нужную ссылку с помощью клавиш со стрелками 
		и нажмите "Ввод" для перехода.]],

		en = [[Lesson 5. Moving — Part I^^
		Now we'll study the passages between rooms. In this room you see five links with lesson numbers. Go to lesson 6.^^

		Click on the appropriate link or use keyboard — pick the link with arrows and press "Enter".]],

		de = [[5. Lektion: Fortbewegung – 1. Teil^^
		Nun werden wir die Fortbewegung zwischen Räumen erlernen.
		In diesem Raum sehen Sie fünf Links mit Lektionsnummern. Gehen Sie in die 6. Lektion.^^

		Klicken Sie auf den passenden Link oder benutzen Sie die Tastatur – wählen Sie den Link mit den Pfeiltasten aus und drücken Sie [Eingabe].]],

		es = [[Lección 5. Moviéndose — Parte I^^
		Ahora analizaremos el movimiento entre localidades.
		En esta habitación puedes ver 5 links con los nombres de las lecciones.
		Ve a la lección 6.^^

		Haz click en el link correspondiente o usa el teclado (ubica el cursor con las flechas y presiona "Enter").]],

		fr = [[Leçon nº 5. En mouvement — Partie I^^
		Maintenant, nous allons étudier les déplacements entre les pièces. Dans cette salle,
		vous voyez cinq liens pointant vers les autres leçons. Allez à la leçon nº 6.^^

		Cliquez sur le lien approprié ou utilisez le clavier — sélectionnez le lien a l’aide des flèches et appuyez ensuite sur « Entrée ».]],

		it = [[Lezione 5. Movimenti — Parte I^^
		Ora vedremo i passaggi tra una locazione e l'altra. In questa stanza potete
		vedere cinque link che portano verso le lezioni numerate. Andate alla 6.^^

		Fate click sul giusto link o usate la tastera - selezionate il link con le frecce e premete "Invio".]],

		nl = [[Les 5. Verplaatsen — Deel I^^
		We bestuderen nu hoe je je verplaatst naar een andere ruimte. In deze ruimte
		zie je vijf links met naar verschillende lessen. Ga naar les 6.^^

		Klik op de betreffende les met de muis, of gebruik het toetsenbord. (Gebruik
		de pijltjestoetsen om de juiste les te selecteren, en druk dan op "Enter".]],

		pt = [[Lição 5. Movendo-se — Parte I^^
		Agora vamos estudar as passagens entre os locais.
		Neste local você vê cinco links com números de lições. Vá para a lição 6.^^

		Clique no link apropriado ou use o teclado — selecione o link com as setas e tecle "Enter".]],

		uk = [[Урок 5. Переміщення - Частина I^^
		Тепер вивчимо переходи між кімнатами. В цій кімнаті Ви бачите п'ять 
		допоміжних посилань з номерами уроків. Перейдіть на урок 6. ^^

		Для цього натисніть мишею на відповідне посилання. Ви також можете використати
		клавіатуру -- виберіть потрібне посилання за допомогою клавіш зі стрілками 
		та нажміть "Введення" для переходу.]],
	};
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
}

room {
	nam = 'r6';
	disp = L {
		ru = 'Урок 6',
		en = 'Lesson 6',
		de = '6. Lektion',
		es = 'Lección 6',
		fr = 'Leçon nº 6',
		it = 'Lezione 6',
		nl = 'Les 6',
		pt = 'Lição 6',
		uk = 'Урок 6',
	};
	onexit = function(s, t)
		if t ^ 'theend' then
			return
		end
		p (L {
			ru = "Этот урок мы уже прошли.^ Пожалуйста, перейдите на последний урок.",
			en = "We've already finished this lesson..^ Go to the last lesson, please.",
			de = 'Wir haben diese Lektion bereits fertiggestellt.^ Bitte gehen Sie zur letzen Lektion.',
			es = 'Ya hemos terminado esta lección.^ Ve a la última lección, por favor.',
			fr = 'Nous avons déjà terminé cette leçon…^ En avant pour la dernière leçon, s’il vous plaît.',
			it = 'Abbiamo già terminato questa lezione.^ Proseguiamo verso la 6, per favore.',
			nl = 'Die les is al afgerond.^ Ga naar de laatste les aub.',
			pt = 'Já terminamos esta lição..^ Favor seguir para a última lição.',
			uk = 'Цей урок ми вже пройшли.^ Будь ласка, перейдіть на останній урок.',
		})
		return false
	end;
	dsc = L {
		ru = [[Урок 6. Перемещение - Часть II^^
		Теперь перейдите на последний урок.]],

		en = [[Lesson 6. Moving - Part II^^
		Now go to the last lesson.]],

		de = [[6. Lektion: Fortbewegung – 2. Teil^^
		Nun gehen Sie zur letzten Lektion.]],

		es =  [[Lección 6. Moviéndose - Parte II^^
		Ahora ve a la última lección.]],

		fr = [[Leçon nº 6. En mouvement — Partie II^^
		Rendez-vous à la dernière leçon.]],

		it = [[Lezione 6. Movimenti - Parte II^^
		Ora passiamo all'ultimo step di questa guida.]],

		nl = [[Les 6. Verplaatsen — Deel II^^
		Verplaats jezelf nu naar de laatste les.]],

		pt = [[Lição 6. Movendo-se - Parte II^^
		Agora vá para a última lição.]],

		uk = [[Урок 6. Переміщення - Частина II^^
		Тепер перейдіть на останній урок.]],
	};
	way = { 'r1', 'r2', 'r3', 'r4', 'r5', 'theend'}
}

room {
	nam = 'theend';
	disp = L {
		ru = 'Последний урок',
		en = 'Last lesson',
		de = 'Letzte Lektion',
		es = 'Ultima lección',
		fr = 'Dernière leçon',
		it = 'Gran Finale',
		nl = 'Laatste les',
		pt = 'Última lição',
		uk = 'Останній урок',
	};
	dsc = L {
		ru = [[Вы можете выбирать игру, сохранять и загружать её состояние и выполнять 
		другие действия с помощью меню. Для вызова меню нажмите клавишу "Esc" или 
		нажмите мышью на символ меню (справа снизу).^^
		Теперь Вы готовы к игре. Удачи!!!^^
		Игры для INSTEAD можно скачать здесь: {$fmt u|https://instead-hub.github.io}^^
		В обучении использован трек "Instead game-engine" от svenzzon.]],

		en = [[With the help of the menu you can choose a game, save or load the state,
		or do other things. To open the menu press "Esc" or click the menu icon at the bottom right.^^
		Now you are ready to play. Good luck!!!^^
		Download games for INSTEAD here: {$fmt u|http://instead-games.sourceforge.net}^^
		The tutorial uses the track "Instead game-engine" from svenzzon.]],

		de = [[Mithilfe des Menüs können Sie ein Spiel starten, den Spielstand speichern oder laden,
		oder andere Dinge tun. Um das Menü zu öffnen, drücken Sie [Esc] oder klicken sie das Menüsymbol unten rechts an.^^
		Nun sind Sie bereit zum Spielen. Viel Glück!^^
		Laden Sie sich Spiele für INSTEAD von dort herunter: {$fmt u|http://instead-games.sourceforge.net}^^
		Die Einführung benutzt die Musik »Instead game-engine« von svenzzon.]],

		es = [[Mediante el menú flotante, puedes elegir un juego, salvar o cargar un juego, y un par de cosas más.
		Para abrirlo presiona "Esc" o haz click en el icono de menú abajo a la derecha.^^
		Ahora ya estás listo para jugar. Buena suerte!!!^^
		Descarga nuevos juegos para INSTEAD aquí: {$fmt u|http://instead-games.sourceforge.net}^^
		Este tutorial usa el tema "Instead game-engine" de svenzzon.]],

		fr = [[À l’aide du menu, vous pouvez choisir un jeu, sauvegarder ou charger une partie,
		ou faire d'autres choses encore. Pour ouvrir le menu, appuyez sur « Esc » ou cliquez
		sur l'icône de menu en bas à droite (avec le thème par défaut).^^
		Maintenant, vous êtes prêt à jouer. Bonne chance !!!^^
		Téléchargez des jeux pour INSTEAD ici : {$fmt u|http://instead-games.sourceforge.net}^^
		Le tutoriel utilise « INSTEAD game-engine » de svenzzon comme thème musical.]],

		it = [[Con l'aiuto del menu potete scegliere un gioco, salvare o caricare una partita,
		oltre a svariate altre cose. Per aprire il menu premete "Esc" o fate click
		sull'icona di menu in basso a destra.^^
		Ora siete pronti a giocare. Buona Fortuna!!!^^
		Scaricate i giochi per INSTEAD qui: {$fmt u|http://instead-games.sourceforge.net}^^
		Il tutorial si avvale del brano musicale "Instead game-engine" di svenzzon.]],

		nl = [[In het menu kun je onder andere een ander spel kiezen. Je kunt ook de
		huidige stand opslaan of weer laden. Om het menu te openen, toets "Esc"
		of klik met de muis op het hamburger icoon rechts onderin.^^
		Je bent nu klaar om te spelen. Veel plezier!!!^^
		Download spellen voor INSTEAD hier: {$fmt u|http://instead-games.sourceforge.net}^^
		De achtergrondmuziek in deze training is van svenzzon en heet "Instead game-engine".]],

		pt = [[Com a ajuda do menu você pode escolher um jogo, salvar ou carregar o seu estado,
		ou fazer outras coisas. Para abrir o menu tecle "Esc" ou clique no ícone do
		menu à direita no fundo da página.^^
		Agora você está pronto para jogar. Boa sorte!!!^^
		Baixe jogos aqui: {$fmt u|http://instead-games.sourceforge.net}^^
		O tutorial usa o tema "Instead game-engine" de svenzzon.]],

		uk = [[Ви можете вибирати гру, зберігати та завантажувати її стан і виконувати
		інші дії за допомогою меню. Для виклику меню натисніть клавішу "Esc" або 
		натисніть мишею на символ меню (справа знизу).^^
		Тепер Ви готові до гри. Хай щастить!!!^^
		Ігри для INSTEAD можно завантажити тут: {$fmt u|https://instead-hub.github.io}^^
		Для навчання використан трек "Instead game-engine" вiд svenzzon.]],
	};
	decor = L {
		ru = 'Посмотреть {@ walk help|список клавиш}.',
		en = 'See {@ walk help|list of keyboard shortcuts}.',
		de = 'Siehe {@ walk help|Liste der Tastenkürzel}.',
		es = 'Ver {@ walk help|lista de atajos de teclado}.',
		fr = 'Voir la {@ walk help|liste des raccourcis clavier}.',
		it = 'Vedi {@ walk help|lista delle scorciatoie da tastiera}.',
		nl = 'Zie ook de {@ walk help|lijst met sneltoetsen}.',
		pt = 'Ver {@ walk help|lista de atalhos de teclado}.',
		uk = 'Подивитись {@ walk help|список клавіш}.',
	}
}

room {
	nam = 'help';
	disp = L {
		ru = 'Список клавиш',
		en = 'Keyboard shortcuts',
		de = 'Tastenkürzel',
		es = 'Atajos de Teclado',
		fr = 'Raccourcis clavier',
		it = 'Scorciatoie da tastiera',
		nl = 'Sneltoetsen',
		pt = 'Atalhos de teclado',
		uk = 'Список клавіш',
	};
	dsc = L {
		ru = [[
	Esc - Вызов меню;^
	Alt+Q - Выход;^
	Alt+Enter - Полноэкранный/оконный режим;^
	F5 - Обновление сцены;^
	Пробел/Backspace - Прокрутка описания сцены;^
	Tab/Shift+Tab - Переключение между активными зонами;^
	PgUp/PgDn - Прокрутка активной зоны;^
	Стрелки влево/вправо - Выбор ссылок в активной зоне;^
	F2 - Сохранение игры;^
	F3 - Загрузка игры;^
	F8 - Быстрое сохранение игры;^
	F9 - Быстрая загрузка игры.^^

	Клавиши, поведение которых зависит от выбранного режима клавиатуры
	(см. Меню -> Настройки -> Режим клавиатуры):^^

	Стрелки вверх/вниз:^
	 - В режиме "Прокрутка": прокрутка активной зоны;^
	 - В режиме "Ссылки": выбор ссылок в активной зоне;^
	 - В режиме "Умный": одновременно выбор ссылок и прокрутка активной зоны.^^

	Стрелки вверх/вниз + Shift или Alt:^
	 - В режиме "Прокрутка": выбор ссылок в активной зоне;^
	 - В режиме "Ссылки": прокрутка активной зоны;^
	 - В режиме "Умный": прокрутка активной зоны.
	]],

		en = [[
	Esc - Menu.^
	Alt+Q - Exit.^
	Alt+Enter - Full screen/window.^
	F5 - Refresh the scene.^
	Space/Backspace - Scroll the scene description.^
	Tab/Shift+Tab - Switch between active zones.^
	PgUp/PgDn - Scroll the active zone.^
	Arrows left/right - Choose links in the active zone.^
	F2 - Save game.^
	F3 - Load game.^
	F8 - Quick save.^
	F9 - Quick load.^^

	Shortcuts, that depend on the chosen keyboard mode
	(see Menu -> Settings -> Keyboard mode):^^

	Arrows up/down:^
	 - "Scroll" mode: scroll active zone.^
	 - "Links" mode: choose links in the active zone.^
	 - "Smart" mode: both choose links and scroll active zone.^^

	Arrows up/down + Shift or Alt:^
	 - "Scroll" mode: choose links in the active zone.^
	 - "Links" mode: scroll active zone.^
	 - "Smart" mode: scroll active zone.
	]],

		de = [[
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

		es = [[
	Esc - Menú.^
	Alt+Q - Salir.^
	Alt+Enter - Pantalla Completa/Ventana.^
	F5 - Actualizar escena.^
	Espacio/Backspace - Desplazar la descripción de la escena.^
	Tab/Shift+Tab - Cambiar entres zonas activas.^
	PgUp/PgDn - Desplazar el contenido de la zona activa.^
	Teclas izqu/derecha - Elegir links en la zona activa.^
	F2 - Salvar juego.^
	F3 - Cargar juego.^
	F8 - Salvado Express.^
	F9 - Carga Express.^^

	Atajos que dependen del modo de teclado elegido (ver Menú->Opciones->Modo Teclado):^^

	Flechas arriba/abajo:^
	 - Modo "Scroll": hace scroll de zona activa.^
	 - Modo "Links": elige los links en la zona activa.^
	 - Modo "Smart": combina las dos anteriores.^^

	Flechas arriba/abajo + Shift o Alt:^
	 - Modo "Scroll": elige los links en la zona activa.^
	 - Modo "Links": hace scroll de zona activa.^
	 - Modo "Smart": hace scroll de zona activa.
	]],

		fr = [[
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

		it = [[
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

		nl = [[
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

		pt = [[
	Esc - Menu.^
	Alt+Q - Sair.^
	Alt+Enter - Tela cheia/janela.^
	F5 - Atualizar cena.^
	Space/Backspace - Rolar descrição da cena.^
	Tab/Shift+Tab - Alternar áreas ativas.^
	PgUp/PgDn - Rolar área ativa.^
	Arrows left/right - Selecionar links na área ativa.^
	F2 - Salvar jogo.^
	F3 - Carregar jogo.^
	F8 - Salvar rápido.^
	F9 - Carregar rápido.^^

	Atalhos que dependem do modo de teclado escolhido
	(Ver Menu -> Opções -> Modo de teclado):^^

	Setas acima/abaixo:^
	 - Modo "Scroll": rolar área ativa.^
	 - Modo "Links": selecionar links na área ativa.^
	 - Modo "Smart": selecionar links e rolar área ativa.^^

	Setas acima/abaixo + Shift ou Alt:^
	 - Modo "Scroll": selecionar links na área ativa.^
	 - Modo "Links": rolar área ativa.^
	 - Modo "Smart": rolar área ativa.
	]],

		uk = [[
	Esc - Виклик меню;^
	Alt+Q - Вихід;^
	Alt+Enter - Повноекранний/віконний режим;^
	F5 - Оновлення сцени;^
	Пробел/Backspace - Прокрутка опису сцени;^
	Tab/Shift+Tab - Переключення між активними зонами;^
	PgUp/PgDn - Прокрутка активної зони;^
	Стрілки вліво/вправо - Вибір посиланнь в активній зоні;^
	F2 - Збереження гри;^
	F3 - Завантаження гри;^
	F8 - Швидке збереження гри;^
	F9 - Швидке завантаження гри.^^

	Клавіши, поведінка яких залежить від обраного режиму клавіатури
	(див. Меню -> Налаштування -> Режим клавіатури):^^

	Стрілки вгору/вниз:^
	 - В режимі "Прокрутка": прокрутка активної зони;^
	 - В режимі "Посилання": вибір посиланнь в активній зоні;^
	 - В режимі "Розумний": одночасний вибір посиланнь та прокрутка активної зони.^^

	Стрілки вгору/вниз + Shift або Alt:^
	 - В режимі "Прокрутка": вибір посиланнь в активній зоні;^
	 - В режимі "Посилання": прокрутка активної зони;^
	 - В режимі "Розумний": прокрутка активної зони.
	]],
	};
	decor = L {
		ru = 'Посмотреть {@ walk help2|параметры командной строки}.',
		en = 'See {@ walk help2|command line parameters}.',
		de = 'Siehe {@ walk help2|Kommandozeilenparameter}.',
		es = 'Ver {@ walk help2|parámetros línea de comandos}.',
		fr = 'Voir les {@ walk help2|paramètres de la ligne de commande}.',
		it = 'Vedi {@ walk help2|parametri della linea di comando}.',
		nl = 'Zie {@ walk help2|commandoregel parameters}.',
		pt = 'Ver {@ walk help2|parâmetros de linha de comando}.', 
		uk = 'Подивитись {@ walk help2|параметри командного рядка}.',
	};
}

room {
	nam = 'help2';
	disp = L {
		ru = 'Параметры командной строки',
		en = 'Command line parameters',
		de = 'Kommandozeilenparameter',
		es = 'Parámetros línea de comandos',
		fr = 'Paramètres de la ligne de commande',
		it = 'Parametri della linea di comando',
		nl = 'Commandoregel parameters',
		pt = 'parâmetros de linha de comando',
		uk = 'Параметри командного рядка',
	};
	dsc = L {
		ru = [[
	-nosound - запуск без звука;^
	-debug - отладочный режим для разработчика игр;^
	-theme <тема> - выбор темы;^
	-game <игра> - выбор игры;^
	-themespath <путь> - дополнительный путь к темам;^
	-gamespath <путь> - дополнительный путь к играм;^
	-windows - оконный режим;^
	-fullscreen - полноэкранный режим;^
	-noautosave - отключить автосохранение/автозагрузку;^
	-encode <game.lua> [encoded.lua] - закодировать исходный текст lua (используйте doencfile для загрузки такого файла);^
	-mode [ШИРИНАxВЫСОТА] - установить разрешение.
	]],

		en = [[
	-nosound - launch without sound;^
	-debug - debug mode for game developers;^
	-theme <theme> - pick a theme;^
	-game <game> - pick a game;^
	-themespath <path> - additional path to themes;^
	-gamespath <path> - additional path to games;^
	-window - windowed mode;^
	-fullscreen  - fullscreen mode;^
	-noautosave - disable autosave/autoload option;^
	-encode <game.lua> [encoded.lua] - encode lua source code (use doencfile from lua to load it);^
	-mode [WxH] - use WxH resolution.
	]],

		de = [[
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

		es = [[
	-nosound - lanza sin sonido;^
	-debug - modo debug para desarrolladores;^
	-theme <theme> - elige un tema;^
	-game <game> - elige un juego;^
	-themespath <path> - ruta adicional para los temas;^
	-gamespath <path> -  ruta adicional para los juegos;^
	-window - modo ventana;^
	-fullscreen  - pantalla completa;^
	-noautosave - deshabilita opción autosalvar/autocargar;^
	-encode <game.lua> [encoded.lua] - encode del código fuente lua (usar doencfile desde lua para cargarlo);^
	-mode [WxH] - usa resolusion WxH.
	]],

		fr = [[
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

		it = [[
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

		nl = [[
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

		pt = [[
	-nosound - executar sem som;^
	-debug - modo de depuração para desenvolvedores de jogos;^
	-theme <tema> - escolha um tema;^
	-game <jogo> - escolha um jogo;^
	-themespath <endereço> - endereço adicional para temas;^
	-gamespath <endereço> - endereço adicional para jogos;^
	-window - modo de janela;^
	-fullscreen  - modo de tela cheia;^
	-noautosave - desabilitar opções de salvar/carregar automaticamente;^
	-encode <jogo.lua> [codificado.lua] - codificar código-fonte Lua (usar doencfile de Lua para carregá-lo);^
	-mode [WxH] - usar resolução WxH.
	]],

		uk = [[
	-nosound - запуск без звуку;^
	-debug - режим налагоджування для розробників ігр;^
	-theme <тема> - вибір теми;^
	-game <гра> - вибір гри;^
	-themespath <шлях> - додатковий шлях до тем;^
	-gamespath <шлях> - додатковий шлях до ігр;^
	-windows - віконний режим;^
	-fullscreen - повноекранний режим;^
	-noautosave - відключити автозбереження/автозагрузку;^
	-encode <game.lua> [encoded.lua] - закодувати вихідний текст lua (використовуйте doencfile для загрузки такого файла);^
	-mode [ШИРИНАxВИСОТА] - встановити роздільну здатність екрану.
	]]
	};
	decor = L {
		ru = 'Посмотреть {@ walk help|список клавиш}.',
		en = 'See {@ walk help|list of keyboard shortcuts}.',
		de = 'Siehe {@ walk help|Liste der Tastenkürzel}.',
		es = 'Ver {@ walk help|lista de atajos de teclado}.',
		fr = 'Voir la {@ walk help|liste des raccourcis clavier}.',
		it = 'Vedi {@ walk help|lista delle scorciatorie da tastiera}.',
		nl = 'Zie ook de {@ walk help|lijst met sneltoetsen}.',
		pt = 'Ver {@ walk help|lista de atalhos de teclado}.',
		uk = 'Подивитись {@ walk help|список клавіш}.',
	};
}
