instead_version "1.7.0"
require "keyboard"
require "xact"

input.verbose = true
main = room {
	nam = '?';
	dsc = function(s)
		if read.text ~= '' then
			p "Привет, "
			p (read.text,"!")
		else
			p [[Как вас {xwalk(read)|зовут}?]];
		end
	end
}

read = keyboard {
	nam = 'Имя:';
	msg = "Поле ввода:";
}
