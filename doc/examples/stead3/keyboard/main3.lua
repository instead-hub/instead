require "keyboard"

keyboard.alt_xlat = true

room {
	title = '?';
	nam = 'main';
	name = false;
	onkbd = function(s, w)
		if w == 'instead' then
			walk 'theend'
			return
		end
		s.name = w
	end;
	dsc = function(s)
		if s.name then
			p ("Привет, ", s.name)
		else
			p [[Как вас {@keyboard "Имя"|зовут}?]];
		end
	end
}

room {
	nam = 'theend';
	title = 'Конец';
	dsc = [[WOW!]];
}
