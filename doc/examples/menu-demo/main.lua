dofile("actions.lua");
game.use = 'Не сработает...';

status = stat {
	Turns = 0,
	nam = function(s)
		s.Turns = s.Turns + 1;
		return 'Статус игрока: '..s.Turns..'^';
	end
};

knife = iobj {
	nam = 'ножик',
	desc = 'На полу валяется {ножик}.',
	exam = 'Бесполезный перочинный ножик.',
}

main = room {
	nam = 'intro',
	dsc = 'Введение',
	exit = function(s)
		me().obj:add('status');
		actions_init();
		inv():add('knife');
	end,
	obj = { vway('next','{Дальше}.', 'r1') }
}

cube = iobj {
	nam = 'куб',
	desc = 'В центре комнаты находится {куб}.',
	take = 'Вы взяли куб',
	exam = 'Мультифункциональный куб -- написано на кубе.',
	inv = act.exam,
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

sphere = iobj {
	nam = 'сфера',
	desc = 'В центре комнаты находится {сфера}.',
	take = 'Вы взяли сферу',
	exam = 'Мультифункциональная сфера -- написано на сфере.',
	inv = act.exam,
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
		return 'Вы пытаетесь отдать сферу объекту: '..deref(w)..'.', false
	end,
	useon = function(s, w)
		return 'Вы пытаетесь юзать сферу на объект: '..deref(w)..'. Получилось!'
	end,
	used = 'Сфера поюзана.',
};

r1 = room {
	nam = 'комната',
	dsc = 'Вы в комнате',
	obj = { cube, sphere },
}
