instead_version "1.6.3"
require "proxymenu"
require "hideinv"

game.forcedsc = true

minv = obj_menu('С СОБОЙ', 'exam', false, true);
mlook = obj_menu('ОСМОТРЕТЬ', 'exam', true);
mtake = obj_menu('ВЗЯТЬ', 'take', true);
mdrop = obj_menu('БРОСИТЬ', 'drop', false, true);
meat = obj_menu('ЕСТЬ', 'eat', true, true);
mpush = obj_menu('ТОЛКАТЬ', 'push', true);
muse = use_menu('ИСПОЛЬЗОВАТЬ', 'useon', 'used', 'useit', true, true);
mgive = use_menu('ОТДАТЬ', 'give', 'accept', false, true, true, true);
mwalk = obj_menu('ИДТИ', 'walk', false, false, true);

game.useit = 'Не помогло.'
game.use = 'Не сработает.'
game.give = 'Отдать? Ни за что!'
game.eat = 'Не буду это есть.'
game.drop = 'Еще пригодится.'
game.exam = 'Ничего необычного.'
game.take = 'Стоит ли это брать?'
game.push = 'Ничего не произошло.'

game.after_take = function(s, w)
	take(w)
end

game.after_drop = function(s, w)
	drop(w)
end

put(minv, me())
put(mlook, me())
put(mtake, me())
put(mdrop, me())
put(meat, me())
put(mpush, me())
put(muse, me())
put(mgive, me())
-- put(mwalk, me())

status = stat {
	_Turns = 0,
	life = function(s)
		s._Turns = s._Turns + 1;
	end;
	nam = function(s)
		return 'Статус игрока: '..s._Turns..'^';
	end
};
lifeon 'status'

put(status, me());

knife = obj {
	nam = 'ножик',
	dsc = 'На полу валяется ножик.',
	exam = 'Бесполезный перочинный ножик.',
}

main = room {
	nam = 'intro',
	hideinv = "true",
	dsc = 'Введение',
	exit = function(s)		
		inv():add('knife');
	end,
	obj = { vway('next','{Дальше}.', 'r1') }
}

cube = obj {
	nam = 'куб',
	dsc = 'В центре комнаты находится куб.',
	take = 'Вы взяли куб',
	exam = 'Мультифункциональный куб -- написано на кубе.',
	drop = 'Вы положили куб.',
	useit = 'Как можно использовать куб?',
	talk = 'Вы поговорили с кубом.',
	eat = function(s)
		return 'Вы не можете разгрызть куб.', false;
	end,
	open = 'Вы открыли куб.',
	close = 'Вы закрыли куб.',
	push = 'Вы толкаете куб.',
	give = function(s, w)
		return 'Вы пытаетесь отдать куб объекту: '..deref(w)..'.', false
	end,
	useon = function(s, w)
		return 'Вы пытаетесь юзать куб на объект: '..deref(w)..'. Получилось!'
	end,
	used = 'Куб поюзан.',
};

sphere = obj {
	nam = 'сфера',
	dsc = 'В центре комнаты находится сфера.',
	take = 'Вы взяли сферу',
	exam = 'Мультифункциональная сфера -- написано на сфере.',
	drop = 'Вы положили сферу.',
	useit = 'Как можно использовать сферу?',
	talk = 'Вы поговорили с сферой.',
	eat = function(s)
		return 'Вы не можете разгрызть сферу.', false;
	end,
	open = 'Вы открыли сферу.',
	close = 'Вы закрыли сферу.',
	push = 'Вы толкаете сферу.',
	give = function(s, w)
		return 'Вы пытаетесь отдать сферу объекту: '..nameof(w)..'.', false
	end,
	useon = function(s, w)
		return 'Вы пытаетесь юзать сферу на объект: '..nameof(w)..'. Получилось!'
	end,
	used = 'Сфера поюзана.',
};

r1 = room {
	nam = 'комната',
	dsc = 'Вы в комнате',
	obj = { cube, sphere },
}
