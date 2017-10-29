require "keyboard"
xact.walk = walk

room {
	title = '?';
	nam = 'main';
	dsc = function(s)
		if _'read'.text ~= '' then
			p "Привет, "
			p (_'read'.text,"!")
		else
			p [[Как вас {@ walk read|зовут}?]];
		end
	end
}

keyboard {
	nam = 'read';
	title = 'Имя:';
	alt_xlat = true;
	msg = "Поле ввода:";
}
