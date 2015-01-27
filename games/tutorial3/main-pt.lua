-- $Name:Tutorial Português$
instead_version "1.6.0"

game.act = 'Isso não é permitido.';
game.inv = "Hmm... Não...";
game.use = 'Não funciona...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutorial',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Bem-vindo ao tutorial do INSTEAD.")..[[^^
	Cada jogo consiste de cenas. Cada cena do jogo tem uma descrição, com partes estáticas e dinâmicas. Partes dinâmicas incluem objetos, personagens, etc.  Um jogador pode interagir com a parte dinâmica clicando com o mouse nos links realçados.^^

	A cena corrente é chamada de "Tutorial" e você está lendo a parte estática da sua descrição. O único objeto nesta cena é o objeto "Próxima", que você pode ver abaixo do texto. Então, para continuar aprendendo você pode clicar "Próxima".]],
	obj = { 
		vobj('continue', '{Próxima}'),
	},
};

paper = obj {
	nam = 'papel',
	dsc = 'A primeira coisa que você vê no local é uma {folha de papel}.',
	tak = 'Você pega o papel.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'Uma folha branca e quadrada de papel. Provavelmente rasgada de um livro.';
		end
		p 'Uma folha quadrada de papel com seu nome nela.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'Você escreve o seu nome no papel.';
		end
	end,
};

pencil = obj {
	nam = 'lápis',
	dsc = 'Há um {lápis} no chão.',
	tak = 'Você pega o lápis.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'Um simples lápis de madeira.';
	end,
};

r1 = room {
	nam = 'Lição 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Excelente!^Você provavelmente notou que a parte estática da descrição da cena desapareceu, substituída pelas descrições dos objetos que você pegou. Para ver a descrição completa da cena novamente clique no seu nome no alto da da página — "Lição 1" ou tecle F5. O resultado é o mesmo.^^
		{Próxima}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Lição 1. Interagindo com objetos^^
		Continuando com a lição. Você está num local. Pegue os objetos que você vê. Basta clicar neles com o mouse. Se preferir o teclado, você pode apontar para um objeto com as setas direcionais e pressionar "Enter" para interagir com ele.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Lição 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Muito Bem!^^{Próxima}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Lição 2. Usando o inventário — Parte I^^
		Agora você tem objetos para examinar ou usar. O inventário é para isso. Para examinar algum objeto dê duplo clique nele. Ou você pode mover o cursor para o inventário com a tecla "Tab", escolha um objeto e tecle "enter" duas vezes. Para levar o cursor de volta para a descrição da cena, tecle "Tab" novamente.^^

		Então, examine o lápis. Faça o mesmo com o papel.]],
};

apple = obj {
	nam = 'maçã',
	dsc = 'Há uma {maçã} na mesa.',
	tak = 'Você pega a maçã da mesa.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'Você comeu a maçã.';
		end
		p 'Hum, apetitosa.';
	end,
};

desk = obj {
	nam = 'escrivaninha',
	dsc = 'Você vê aqui uma {escrivaninha} de madeira.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Uma grande escrivaninha de carvalho. Há uma inscrição a lápis no seu topo: "Lorem Ipsum".';
		end
		p 'Uma grande escrivaninha de carvalho.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'Você escreve várias letras na escrivaninha.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Lição 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Próxima}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Lição 3. Usando o inventário — Parte II^^
		Você pode usar objetos do inventário para afetar outros objetos da cena ou do inventário. Neste local você vê uma escrivaninha. Tente usar o lápis nela.^^ 

		Para isso clique com o mouse no lápis, então clique na escrivaninha. Ou você pode fazer o mesmo com "Tab", "enter" e as setas do teclado.^^
  
		Então examine a mesa. E não esqueça de pegar a maçã, você precisará dela na próxima lição.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Lição 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Muito bem.^^{Próxima}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Lição 4. Usando o inventário — Parte III^^
		Bom, agora vamos aprender diversas outras ações com objetos.^^
	
		Primeiro, escreva algo no papel. Clique no lápis, depois no papel. Então você pode examinar o papel para ver os seus escritos.^^
	
		Segundo, coma a maçã que você pegou no local anterior. Dê duplo clique nela.^^

		Todas estas ações podem ser realizadas com o teclado, conforme descrito antes.]],
};

r5 = room {
	nam = 'Lição 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'Já terminamos esta lição.^ Favor seguir para a lição 6.'
			return false;
		end
	end,
	dsc = [[Lição 5. Movendo-se — Parte I^^
		Agora vamos estudar as passagens entre os locais. Neste local você vê cinco links com números de lições. Vá para a lição 6.^^

		Clique no link apropriado ou use o teclado — selecione o link com as setas e tecle "Enter".]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Lição 6',
	exit = function(s, t)
		if t ~= theend then
			return 'Já terminamos esta lição..^ Favor seguir para a última lição.', false;
		end
	end,
	dsc = [[Lição 6. Movendo-se - Parte II^^
		Agora vá para a última lição.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6', 'theend'},
};

theend = room {
	nam = 'Última lição',
	dsc = [[Com a ajuda do menu você pode escolher um jogo, salvar ou carregar o seu estado, ou fazer outras coisas. Para abrir o menu tecle "Esc" ou clique no ícone do menu à direita no fundo da página.^^
		Agora você está pronto para jogar. Boa sorte!!!^^
		Baixe jogos aqui: ]]..txtu("http://instead-games.sourceforge.net/")..[[^^
		O tutorial usa o tema "Instead game-engine" de svenzzon.]],
	obj = { vway('keys', 'Ver {lista de atalhos de teclado}.', 'help')},
};

help = room {
	nam = 'Atalhos de teclado',
	dsc = [[
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
	obj = { vway('cmdline', 'Ver {parâmetros de linha de comando}.', 'help2')},
};

help2 = room {
	nam = 'parâmetros de linha de comando',
	dsc = [[
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
	obj = { vway('keys', 'Ver {lista de atalhos de teclado}.', 'help')},
};
