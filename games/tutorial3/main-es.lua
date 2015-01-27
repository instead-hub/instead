-- $Name:Tutorial Español$
instead_version "1.6.0"

game.act = 'No puedes hacer eso.';
game.inv = "Hmm... Nop...";
game.use = 'No pasa nada...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutorial',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Bienvenido al tutorial de INSTEAD.")..[[^^
	En INSTEAD un juego se divide en "escenas". Cada escena tiene una descripción, conformada por secciones estáticas y dinámicas. Las secciones estáticas pueden incluir objetos, personajes, etc. Como jugador puedes interactuar con las partes dinámicas al hacer click en los links resaltados.^^
	La escena actual se llama "Tutorial" y tu estás leyendo la sección estática de su descripción. El único objeto en esta escena es el objeto "Siguiente", que puedes ver más abajo. Esto es: para seguir aprendiendo haz click en "Siguiente".]],
	obj = { 
		vobj('continue', '{Siguiente}'),
	},
};

paper = obj {
	nam = 'papel',
	dsc = 'Lo primero que ves en la habitación es un {pedazo de papel}.',
	tak = 'Coges el papel.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Una hoja blanca y cuadrada. Probablemente arrancada de un libro.';
		end
		p 'Una hoja cuadrada de papel con tu nombre escrito en ella.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Escribes tu nombre en el papel.';
		end
	end,
};

pencil = obj {
	nam = 'lápiz',
	dsc = 'Hay un {lápiz} en el piso.',
	tak = 'Coges el lápiz.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Un lápiz común de madera.';
	end,
};

r1 = room {
	nam = 'Lección 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Genial!^Te abrás dado cuenta que la parte estática de la descripción ya no está; reemplazada por las descripciones de los objetos que cogiste. Para ver nuevamente la descricpión completa de esta escena puedes clickear en el nombre arriba (Lección 1) o pulsar F5. Es lo mismo.^^
		{Siguiente}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Lección 1. Interactuar con objetos^^
			Continuemos con el tutorial. Estás en una habitación. Coge los dos objetos que ves aquí. Para ello, puedes clickear sobre ellos con el ratón o -si lo prefieres- usar las flechas de tu teclado para posicionarte y el "Enter" para interactuar.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Lección 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Bien!^^{Siguiente}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Lección 2. Usando el inventario — Parte I^^
		Ahora tienes en tu poder objetos que puedes examinar o usar. Eso es tu inventario. Para examinar un objeto haz doble click en él. O puedes manejar el cursor con la tecla "Tab", elegir un objeto y presionar Enter dos veces. Para volver el cursor a la descripción de la escena, presiona nuevamente "Tab".^^
		
		Ok, examina el papel. Y haz lo propio con el lápiz.]],
};

apple = obj {
	nam = 'manzana',
	dsc = 'Hay una {manzana} sobre la mesa.',
	tak = 'Coges la manzana de la mesa.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Te comes la manzana.';
		end
		p 'Luce apetecible.';
	end,
};

desk = obj {
	nam = 'escritorio',
	dsc = 'Aquí ves un {escritorio} de madera.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Un gran escritorio de roble. Hay algo escrito en él: "Lorem Ipsum".';
		end
		p 'Un gran escritorio de roble.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			p 'Garabateas unas palabras sobre el escritorio.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Lección 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Siguiente}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Lección 3. Usando el inventario — Parte II^^
		Puedes combinar una acción con un objeto de tu inventario y un objeto de la escena o del mismo inventario. En esta habitación verás un escritorio. Prueba usar el lápiz con él.^^
		
		Para ello, haz click sobre el lápiz, y entonces click sobre el escritorio. O puedes conseguir lo mismo con  "Tab", "Enter" y las flechas de tu teclado.^^
		
		A continuación, examina el escritorio. Y no te olvides de coger la manzana. La necesitarás para la próxima lección.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Lección 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have 'apple' then
			return
		end
		put(vway("continue", "Bien!^^{Siguiente}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Lección 4. Usando el inventario — Parte III^^
		Bien, ahora veamos un par de acciones más sobre objetos.^^
		
		Primero, escribir algo en el papel. Haz click en el lápiz y luego en el papel. Y entonces examina el papel para ver lo que haz escrito.^^
		
		Segundo, cómete la manzana que cogiste en la escena anterior. Haz doble click en ella.^^
		
		Todas estas acciones también pueden ejecutarse con tu teclado, como ya se ha descrito.]],
};

r5 = room {
	nam = 'Lección 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Ya hemos terminado esta lección.^ Ve a la lección 6, por favor.'
			return false;
		end
	end,
	dsc = [[Lección 5. Moviéndose — Parte I^^
		Ahora analizaremos el movimiento entre localidades. En esta habitación puedes ver 5 links con los nombres de las lecciones. Ve a la lección 6.^^
		
		Haz click en el link correspondiente o usa el teclado (ubica el cursor con las flechas y presiona "Enter").]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Lección 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Ya hemos terminado esta lección.^ Ve a la última lección, por favor.', false;
		end
	end,
	dsc = [[Lección 6. Moviéndose - Parte II^^
		Ahora ve a la última lección.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6', 'theend'},
};

theend = room {
	nam = 'Ultima lección',
	dsc = [[Mediante el menú flotante, puedes elegir un juego, salvar o cargar un juego, y un par de cosas más. Para abrirlo presiona "Esc" o haz click en el icono de menú abajo a la derecha.^^
		Ahora ya estás listo para jugar. Buena suerte!!!^^
		Descarga nuevos juegos para INSTEAD aquí: ]]..txtu("http://instead-games.sourceforge.net")..[[^^
		Este tutorial usa el tema "Instead game-engine" de svenzzon.]],
	obj = { vway('keys', 'Ver {lista de atajos de teclado}.', 'help')},
};

help = room {
	nam = 'Atajos de Teclado',
	dsc = [[
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
	obj = { vway('cmdline', 'Ver {parámetros línea de comandos}.', 'help2')},
};

help2 = room {
	nam = 'Parámetros línea de comandos',
	dsc = [[
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
	obj = { vway('keys', 'Ver {lista de atajos de teclado}.', 'help')},
};
