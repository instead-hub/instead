-- $Name:English Tutorial$
instead_version "1.6.0"

game.act = 'You can\'t.';
game.inv = "Hmm... Wrong...";
game.use = 'Won\'t work...';

game.pic = 'instead.png';

set_music('instead.ogg');

main = room {
	nam = 'Tutorial',
	act = function()
		walk('r1');
	end,
	dsc = txtc("Welcome to INSTEAD tutorial mode.")..[[^^
	The game consists of scenes. Each game scene has a description, consisting of static and dynamic parts. Dynamic part includes objects, characters, etc.  A player can interact with the dynamic part by clicking mouse on the highlighted links.^^

	The current scene is called "Tutorial" and you are reading the static part of its description. The only object in this scene is the "Next" object, which you can see below the text. Thus, to continue learning you can click "Next".]],
	obj = { 
		vobj('continue', '{Next}'),
	},
};

paper = obj {
	nam = 'paper',
	dsc = 'The first thing you notice in the room is a {sheet of paper}.',
	tak = 'You take the paper.',
	var { seen = false, haswriting = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		if not s.haswriting then
			return 'A blank sheet of squared paper. Probably torn out of the copy-book.';
		end
		p 'A sheet of squared paper with your name on it.';
	end,
	used = function(s, w)
		if w == pencil and here() == r4 then
			s.haswriting = true;
			p 'You write your name on the paper.';
		end
	end,
};

pencil = obj {
	nam = 'pencil',
	dsc = 'There is a {pencil} on the floor.',
	tak = 'You pick the pencil.',
	var { seen = false },
	inv = function(s)
		if here() == r2 then
			s.seen = true;
		end
		p 'An ordinary wooden pencil.';
	end,
};

r1 = room {
	nam = 'Lesson 1',
	enter = function()
		lifeon('r1');
	end,
	life = function(s)
		if not have('paper') or not have('pencil') then
			return
		end
		put(vway('continue',
		[[Excellent!^You've probably noticed that the static part of the scene description disappeared, replaced by the descriptions of the objects you've taken. To see the full scene description again either click on the scene name at the top — "Lesson 1" or press F5. The result is the same.^^
		{Next}]], 'r2'));
		lifeoff('r1');
	end,
	dsc = [[Lesson 1. Interacting with objects^^
		Let's continue the lesson. You are inside a room. Take both objects that you see. You can just click on them with mouse. If you prefer keyboard, you can pick an object with arrow keys and press "Enter" to interact with it.]],
	obj = { 'paper', 'pencil'},
};

r2 = room {
	nam = 'Lesson 2',
	enter = function()
		lifeon('r2');
	end,
	life = function(s)
		if not paper.seen or not pencil.seen then
			return
		end
		put(vway("continue", "Good!^^{Next}", 'r3'));
		lifeoff('r2');
	end,
	dsc = [[Lesson 2. Using the inventory — Part I^^
		Now you've got objects that you can examine or use. That's what the inventory is for. To look at any object double click on it. Or you can switch the cursor to the inventory with "Tab", choose an object and press "enter" twice. To return the cursor to the scene description, press "Tab" again.^^

		So, look at the paper. Then do the same with the pencil.]],
};

apple = obj {
	nam = 'apple',
	dsc = 'There\'s an {apple} on the table.',
	tak = 'You take an apple from the table.',
	var { knife = false },
	inv = function(s)
		if here() == r4 then
			remove(s, me());
			return 'You eat the apple.';
		end
		p 'Looks appetizing.';
	end,
};

desk = obj {
	nam = 'desk',
	dsc = 'In this lesson you see a wooden {desk}.',
	var { seen = false, haswriting = false },
	act = function(s)
		if s.haswriting then
			s.seen = true;
			return 'Large oaken desk. There\'s a pencil inscription on its top: "Lorem Ipsum".';
		end
		p 'Large oaken desk.';
	end,
	used = function(s, w)
		if w == pencil and not s.haswriting then
			s.haswriting = true;
			return 'You write several letters on the desk.';
		end
	end,
	obj = { 'apple' },
};

r3 = room {
	nam = 'Lesson 3',
	enter = function()
		lifeon('r3');
	end,
	life = function(s)
		if not desk.seen or not have 'apple' then
			return
		end
		put(vway("continue", "^^{Next}", 'r4'));
		lifeoff('r3');
	end,
	dsc = [[Lesson 3. Using the inventory — Part II^^
		You can act with inventory objects on other objects of the scene or inventory. In this room you'll see a desk. Try using pencil on it.^^ 

		To do that click mouse on the pencil, then click on the desk. Or you can do the same with "Tab", "enter" and arrows.^^
  
		Then look at the table. And don't forget to take the apple, you'll need it in the next lesson.]],
	obj = { 'desk' },
};

r4 = room {
	nam = 'Lesson 4',  
	enter = function()
		apple.knife = false;
		lifeon('r4');
	end,
	life = function(s)
		if not paper.haswriting or have('apple') then
			return
		end
		put(vway("continue", "Good.^^{Next}", 'r5'));
		lifeoff('r4');
	end,
	dsc = [[Lesson 4. Using the inventory — Part III^^
		Good, now let's learn several more actions with objects.^^
	
		First, write something on the paper. Click pencil, then paper. Then you can look at the paper to see your writing.^^
	
		Second, eat an apple, you've taken in the previous room. Double click on it.^^

		All those actions can be done with keyboard as described previously.]],
};

r5 = room {
	nam = 'Lesson 5',
	exit = function(s, t)
		if t ~= r6 then
			p 'We\'ve already finished this lesson.^ Go to lesson 6, please.'
			return false;
		end
	end,
	dsc = [[Lesson 5. Moving — Part I^^
		Now we'll study the passages between rooms. In this room you see five links with lesson numbers. Go to lesson 6.^^

		Click on the appropriate link or use keyboard — pick the link with arrows and press "Enter".]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6'},
};

r6 = room {
	nam = 'Lesson 6',
	exit = function(s, t)
		if t ~= theend then
			return 'We\'ve already finished this lesson..^ Go to the last lesson, please.', false;
		end
	end,
	dsc = [[Lesson 6. Moving - Part II^^
		Now go to the last lesson.]],
	way = { 'r1', 'r2', 'r3', 'r4', 'r6', 'theend'},
};

theend = room {
	nam = 'Last lesson',
	dsc = [[With the help of the menu you can choose a game, save or load the state, or do other things. To open the menu press "Esc" or click the menu icon at the bottom right.^^
		Now you are ready to play. Good luck!!!^^
		Download games for INSTEAD here: ]]..txtu("http://instead-games.sourceforge.net")..[[^^
		The tutorial uses the track "Instead game-engine" from svenzzon.]],
	obj = { vway('keys', 'See {list of keyboard shortcuts}.', 'help')},
};

help = room {
	nam = 'Keyboard shortcuts',
	dsc = [[
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
	obj = { vway('cmdline', 'See {command line parameters}.', 'help2')},
};

help2 = room {
	nam = 'Command line parameters',
	dsc = [[
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
	obj = { vway('keys', 'See {list of keyboard shortcuts}.', 'help')},
};
