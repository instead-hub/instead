dofile("menu.lua");

Look = function(s)
	if not s.exam then
		return 'Ничего особенного...';
	end
	return call(s,'exam');
end

Open = function(s)
	if not s.open then
		return 'Это нельзя открыть...';
	end
	if s._opened then
		return 'Уже открыто...';
	end
	local r,v = call(s,'open');
	if v ~= false then
		s._opened = true;
	end
	return r
end

Close = function(s)
	if not s.close then
		return 'Это нельзя закрыть...';
	end
	if not s._opened then
		return 'Уже закрыто...';
	end
	local r,v = call(s,'close');
	if v ~= false then
		s._opened = false;
	end
	return r;
end

Take = function(s)
	if have(s) then
		return 'У вас уже это есть.';
	end
	if not s.take then
		return 'Это нельзя взять...';
	end
	local r,v= call(s,'take');
	if v ~= false then
		take(s);
	end
	return r
end

Drop = function(s)
	if not have(s) then
		return 'У вас нет этого.';
	end
	if not s.drop then
		return 'Это нельзя положить...';
	end
	local r,v = call(here(), 'drop', deref(s)) 
	if not r then
		 r,v = call(s,'drop');
	end
	if v ~= false then
		drop(s);
	end
	return r;
end

Push = function(s)
	if not s.push then
		return 'Это нельзя толкнуть...';
	end
	local r = call(s, 'push');
	return r;
end

Eat = function(s)
	if not s.eat then
		return 'Это нельзя есть...';
	end
	local r,v = call(s,'eat');
	if v ~= false then
		inv():del(s);
	end
	return r;
end

Talk = function(s)
	if not s.talk then
		return 'С этим нельзя говорить...';
	end
	local r,v=call(s, 'talk');
	return r
end

Give = function(s, w)
	local r, v;
	if not have(s) then
		return 'Нельзя это отдать...', false;
	end
	if not s.give then
		return 'Гм... Зачем?...', false;
	end
	r, v = call(s, 'give', w);
	if v ~= false then
		remove(s, me());
	end
	return r,false
end

Use = function(s, w)
	local r,v;
	if givem._state then
		if w == nil then
			return 'Гм... Абсурд...'
		end
		return Give(s, w);
	end
	if not w then
		if not s.useit then
			return 'Гм... Как это может сработать?...'
		end
		r, v = call(s, 'useit', w);
	else
		r, v = call(s, 'useon', w);
	end
	return r, v;
end

function iobj(v)
	v.act = Look;
	v.inv = Look;
	v.Open = Open;
	v.Close = Close;
	v.Push = Push;
	v.Take = Take;
	v.Drop = Drop;
	v.Eat = Eat;
	v.Exam = Look;
	v.Talk = Talk;
	v.use = Use;

	v.dsc = function(s)
		return call(s, 'desc');
	end
	return menu(v);
end

function iroom(v)
	v.dsc = function(s)
		return call(s, 'desc');
	end
	return room(v);
end

-- for aliases
act = {
	exam = function(s)
		return call(s, 'exam');
	end,
	take = function(s)
		return call(s, 'take');
	end,
	drop = function(s)
		return call(s, 'drop');
	end,
	push = function(s)
		return call(s, 'push');
	end,
	eat = function(s)
		return call(s, 'eat');
	end,
	talk = function(s)
		return call(s, 'talk');
	end,
	open = function(s)
		return call(s, 'open');
	end,
	close = function(s)
		return call(s, 'close');
	end,
	useon = function(s, w)
		return call(s, 'useon', w);
	end,
	useit = function(s)
		return call(s, 'useit');
	end,
	give = function(s, w)
		return call(s, 'give', w);
	end
};

takem  = actmenu('> взять', 'Take', true);
dropm  = actmenu('> бросить', 'Drop', false, true);
pushm  = actmenu('> толкнуть', 'Push', true);
eatm   = actmenu('> есть', 'Eat', true, true);
talkm  = actmenu('> говорить', 'Talk', true);
openm  = actmenu('> открыть', 'Open', true, true);
closem = actmenu('> закрыть', 'Close', true, true);
usem   = actmenu('> использовать', 'use', true, true);
givem  = actmenu('> отдать', 'use', true, true, true);

rest   = menu {
	nam = '> отдыхать',
	act = 'Я отдохнул.',
	action_type = true,
};

pocketm = pocket('> с собой');

inv = function(s)
	return pocketm.robj;
end

menu_init();

function actions_init()
	put(pocketm, 'pl');
	put(takem, 'pl');
	put(dropm, 'pl');
	put(pushm, 'pl');
	put(eatm, 'pl');
	put(talkm, 'pl');
	put(openm, 'pl');
	put(closem, 'pl');
	put(usem, 'pl');
	put(givem, 'pl');
	put(rest, 'pl');
--	put(stat { nam = ' ' }, 'pl');
end
