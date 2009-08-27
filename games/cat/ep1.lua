mywear = obj {
	nam = 'ватник',
	dsc = function(s)
		if here() == stolcorridor then	
			local st='.';
			if not have('gun') then
				st = ', под которым спрятан дробовик.';
			end
			return 'А еще на вешалке висит мой {ватник}'..st;
		else
			return 'На гвоздике, вбитом в сосновую дверь, висит {ватник}.';
		end
	end,
	inv = 'Зима. Но я одет в теплый ватник.',
	tak = function(s)
		if here() == stolcorridor then
			if have('alienwear') then
				return 'Я уже одет... Если я еще схвачу свой ватник, то буду выглядеть подозрительно...', false;
			end
			if me()._walked then
				me()._walked = false;
				inv():add('gun');
				return 'Все-же мой ватник самый лучший!';
			end
			return 'Это слишком заметно... ', false;
		else
			return 'Я снял с гвоздика свой ватник.';
		end
	end, 
	use = function(s, o)
		if o == 'guy' then
			return 'Немного помешкав, вы поменялись ватниками...';
		end
	end
};

money = obj {
	nam = 'деньги',
	inv = 'Большие деньги -- большое зло... Хорошо что у меня немного денег...',
	use = function(s, w)
		if w == 'shopman' then
			if shopman._wantmoney then
				shopman._wantmoney = false;
				return 'Я расплачиваюсь с Владимиром.';
			end
			return 'Я не хочу платить просто так...';
		end
	end
};

mybed = obj {
	nam = 'кровать',
	dsc = 'У окна стоит {кровать}.',
	act = 'Сейчас не время спать.',
};

mytable = obj {
	nam = 'стол',
	dsc = 'В левом углу стоит дубовый {стол} с ящиками.',
	act = function()
		if not have(money) then
			take('money');
			return 'Порывшись в ящиках я достал деньги.';
		end
		return 'Стол... Этот стол я сделал своими руками.';
	end,
};

foto = obj {
	nam = 'фото',
	dsc = 'На столе стоит {фотокарточка} в рамке.',
	tak = 'Я взял фотографию.',
	inv = 'На этой фотографии изображены я и мой Барсик.',
};

gun = obj {
	nam = 'дробовик',
	dsc = 'В правом углу хижины стоит {дробовик}.',
	tak = 'Я взял дробовик и повесил его за спину.',
	inv = function(s)
		local st = '';
		if s._obrez then
			st = ' Кстати, теперь это обрез.';
			if s._hidden then
				st = st..' Он спрятан в моей одежде.';
			end
		end
		if s._loaded then
			return 'Дробовик заряжен...'..st;
		else	
			return 'Разряженный дробовик... Я редко пользовался им в лесу...'..st;
		end
	end,
	use = function(s, w)
		if w == 'guard' then
			return 'Да, они негодяи, но во-первых они люди, а во-вторых все-равно не поможет...', false;
		end
		if w == 'wire' then
			return 'Слишком близко... Тут нужно что-то вроде кусачек...', false;
		end
		if w == 'cam' and not cam._boken then
			cam._broken = true;
			s._loaded = false;
			return 'Я прицелился в камеру и выстрелил из обоих стволов... Глухой выстрел потонул в порывах вьюги...';
		end
		if not s._hidden then
			if w == 'mywear' or w == 'alienwear' then
				if not s._obrez then
					return 'Я попытался спрятать дробовик в одежду, но он слишком длинный.'
				else
					s._hidden = true;
					return 'Теперь я могу спрятать обрез в одежде!';
				end
			end
		end
		if not s._loaded then
			return 'Не заряжен...', false;
		end
		if w == 'mycat' or w == 'shopman' or w == 'guy' then
			return 'Это не моя мысль...', false;
		end
	end
};

fireplace = obj {
	nam = 'камин',	
	dsc = 'У стены стоит {камин}. Огоньки пламени неравномерно освещают хижину.',
	act = 'Мне нравится сидеть у камина долгими зимними вечерами.',
};

mycat = obj {
	nam = 'Барсик',
	_lflast = 0,
	lf = {
		[1] = 'Барсик шевелится у меня за пазухой.',
		[2] = 'Барсик выглядывает из за пазухи.',
		[3] = 'Барсик мурлычит у меня за пазухой.',
		[4] = 'Барсик дрожит у меня за пазухой.',
		[5] = 'Я чувствую тепло Барсика у себя за пазухой.',
		[6] = 'Барсик высовывает голову из за пазухи и осматривает местность.',
	},
	life = function(s)
		local r = rnd(6);
		if r > 2 then
			return;
		end 
		r = rnd(6);
		while (s._lflast == r) do
			r = rnd(6);
		end
		s._lflast = r;
		return s.lf[r];
	end,
	desc = { [1] = 'Возле камина уютно свернувшись в клубок спит мой кот {Барсик}.',
		 [2] = '{Барсик} изучает местность вокруг хижины.',
		 [3] = '{Барсик} сидит на соседнем сидении.',
		 [4] = '{Барсик} что-то изучает у мусорных баков...',
		 [5] = '{Барсик} трется у моих ног.',
	},
	inv = 'Барсик у меня за пазухой... Бедный мой котик... Я спасу тебя!!! И весь мир...',
	dsc = function(s)
		local state
		if here() == home then
			state = 1;
		elseif here() == forest then
			state = 2;
		elseif here() == inmycar then
			state = 3;
		elseif here() == village then
			state = 4;
		elseif here() == escape3 then
			state = 5;
		end
		return s.desc[state];
	end,
	act = function(s)
		if here() == escape3 then
			take('mycat');
			lifeon('mycat');
			return 'Я забираю Барсика к себе за пазуху.';
		end
		return 'Я почесал Барсика за ушами...';
	end,
};

inmycar = room {
	nam = 'в машине',
	dsc = 'Я в своей машине... Моя рабочая лошадка...',
	pic = 'gfx/incar.png',
	way = {'forest', 'village'},
	enter = function(s, f)
		local s = 'Я открываю дверь машины.';
		if have('mybox') then
			return 'Я не могу залезть в кабину вместе с этим ящиком...', false;
		end
		if seen('mycat') then
			s = s..' Барсик запрыгивает в кабину.'
			move('mycat','inmycar');
		elseif not me()._know_where then
			return 'Нет... Сначала я должен найти Барсика!', false
		end
		if f == 'guarddlg' then
			return 'Хмм... Нужно что-то придумать...';
		end
		return cat(s, ' Ну что же, пора ехать...');
	end,
	exit = function(s, t)
		local s=''
		if seen('mycat') then
			s = ' Барсик выпрыгивает из машины первым.';
			move('mycat',t);
		end
		if ref(t) ~= from() then
			from().obj:del('mycar');
			move('mycar', t);
			return [[
Машина неохотно заводится... После длинного пути я, наконец, выключаю мотор и открываю дверь...]]..s;
		end
		return 'Нет... Кажется я что-то забыл...'..s;
	end
};

mycar = obj {
	nam = 'моя машина',
	desc = {
	[1] = 'Перед хижиной стоит мой старенький {пикап} Toyota.',
	[2] = 'На стоянке машин стоит мой старенький {пикап}.',
	[3] = 'Возле КПП стоит мой {пикап}.',
	[4] = 'За углом стены стоит мой {пикап}.',
	},
	dsc = function(s)
		local state
		if here() == forest then
			state = 1;
		elseif here() == village then
			state = 2;
		elseif here() == inst then
			state = 3;
		elseif here() == backwall then
			state = 4;
		end
		return s.desc[state];
	end,
	act = function(s)
		return goto('inmycar');
	end
};

iso = obj {
	nam = 'изолента',
	inv = 'Моток изоленты. Синего цвета...',
	use = function(s, o)
		if o == 'trap' and not trap._iso then
			trap._iso = true;
			return 'Я изолировал капкан изолентой.';
		end
		if o == 'wire' then
			return 'Зачем мне это? Я все-равно не пролезу по колючей проволоке. К тому же я не могу ее изолировать -- меня долбанет током!';
		end
	end
};

trap = obj {
	nam = 'капкан',
	dsc = 'В траве лежит стальной {капкан}.',
	tak = 'Проклятые браконьеры! Я беру капкан себе.',
	inv = function(s)
		if s._salo then
			return 'Большая мышеловка! К тому же изолированная изолентой.';
		end
		if s._iso then
			return 'Стальной. Очень острый. К тому же изолированный изолентой.';
		else
			return 'Стальной. Очень острый.';
		end
	end,
	use = function(s, o)
		if o == 'wire' and not wire._broken then 
			if not s._iso then
				return 'Капкан железный... Тряхонет током и будь здоров...';
			end
			wire._broken = true;
			onwall.way:add('eside');
			return 'Я подношу взведенный капкан к проволоке... Как я и думал -- капкан перебил проволоку!';
		end
	end
};

deepforest = room {
	i = 0,
	nam = 'чаща',
	pic = 'gfx/deepforest.png',
	dsc = function(s)
		local st = 'Я в чаще... ';
		if s._i == 1 then
			return st..'Сосны и ели... Больше ничего...';
		elseif s._i == 2 then
			return st..'Красивая березы -- только бы не заблудиться...';
		elseif s._i == 3 then
			return st..'Непроходимая чаща... Ничего не пойму -- я что -- заблудился?...';
		elseif s._i == 4 then
			return st..'Красивое озеро... Да... Может пора возвращаться?';
		elseif s._i == 5 then
			s._trap = true;
			return st..'Какие-то кусты... Кусты.. Кусты...';
		else
			return st..'Пенек... Какой красивый пенек...';
		end
	end, 
	enter = function(s,f)
		if f == 'forest' then
			s._trap = false;
		end
		s._lasti = s._i;
		while (s._i == s._lasti) do
			s._i = rnd(6);
		end
		s.obj:del('trap');
		s.way:del('forest');
		if s._i == 5 and not inv():srch('trap') then
			s.obj:add('trap');
		end
		if s._i == 3 and s._trap then
			s.way:add('forest');
		end
		if f == 'forest' and inv():srch('trap') then
			return [[Спасибо, я уже погулял по лесу...]], false;
		end
		if f == 'deepforest' then
			return 'Хмм... Посмотрим...';
		end
		return [[В дикую чащу, пешком?
Хм... Почему бы и нет -- это же моя работа... Браконьеров погоняю...]], true;
--Я пол часа бродил по лесу, когда наткнулся на капкан...
--Проклятые ьраконьеры! Я взял капкан с собой.]], false;
	end,
	way = {'deepforest'},
};

road = room {
	nam = 'дорога',
	enter = function()
		return 'Пешком? Нееет...', false;
	end
};

forest = room {
	nam = 'перед хижиной',
	pic = 'gfx/forest.png',
	dsc = [[
На улице перед хижиной все занесено снегом. Дикий лес окружает хижину со всех сторон. Дорога, ведущая в поселок занесена снегом.]],
	way = { 'home', 'deepforest', 'road' },
	obj = { 'mycar' },
};

home = room {
	nam = 'хижина',
	pic = function(s)
		if not seen('mycat') then
			return "gfx/house-empty.png"
		end
		return "gfx/house.png";
	end,
	dsc = [[
В этой хижине я провел 10 лет. 10 лет назад я своими руками построил ее. Довольно тесно, но уютно.]],
	obj = { 'fireplace', 'mytable', 'foto', 'mycat', 'gun', 
	vobj(1,'окно', 'В хижине есть единственное {окно}.'), 
	'mybed', 'mywear' },
	way = { 'forest' },
	act = function(s,o)
		if o == 1 then
			return 'За окном белым-бело...';
		end
	end,
	exit = function()
		if not have('mywear') then
			return 'На улице холодно... Я не пойду туда без моего ватника.', false
		end
		if seen(mycat) then
			move('mycat','forest');
			return [[
Когда я выходил из хижины, Барсик внезапно проснулся и бросился мне под ноги. 
Я погладил его за ушами -- Значит едем вместе?
]]
		end
	end
};
---------------- here village begins
truck = obj {
	nam = 'черная машина',
	dsc = 'Черная {машина} с тонированными стеклами стоит возле магазина.',
	act = 'Гм... Это фургон... Кузов бронирован, это видно по нагрузке на колеса...',
};

guydlg = dlg {
	pic = 'gfx/guy.png',
	nam = 'разговор с бездомным',
	dsc = 'Я подошел к нему... Он оглянулся и посмотрел на меня беглым взглядом - невысокий человек в потертой кепке и драном ватнике.',
	obj = {
		[1] = phr('Привет! Холодно наверное?', 'Да... Немного...'),
		[2] = phr('Как случилось что ты оказался на улице?', 
[[Когда то я хотел стать кандидатом наук... Писал диссертацию на тему строения материи.. Но... Мой мозг
переутомился... Я пытался успокоиться и вот... Теперь я здесь...]]),
		[3] = phr('Как тебя зовут?', 'Эдуард...'),
		[4] = _phr('Когда я уходил тут возле тебя был Кот... Где он?', 'Гм...', 'pon(5)'),
		[5] = _phr('Да... Кот. Обычный кот, бродящий по снегу возле мусорных баков.', 'Так это был твой кот? Эммм...', 'pon(6)');
		[6] = _phr('Да... Это мой Барсик! Говори же!', 
'... Ммм... Кажется его взял этот человек... Ммм... -- холодок пробежал у меня по спине...', 'pon(7)'),
		[7] = _phr('Куда, куда он поехал?', 'Извини, братишка, я не видел...', 'shopdlg:pon(4); pon(8);'),
		[8] = phr('Ладно... Не важно...', '...', 'pon(8); back()'),
	},
	exit = function()
		pon(1);
		return 'Он отвернулся от меня и снова стал шарить по бакам...';
	end
};

guy = obj {
	nam = 'бездомный',
	dsc = 'В мусорных баках копается {бездомный}.',
	act = function()
		return goto('guydlg');
	end,
	used = function(s, w)
		if w == 'money' then
			return [[
Я подошел и попытался дать немного денег... -- Мне не нужны чужие деньги... -- ответил он.]];
		else
			return 'Зачем это ему?';
		end
	end,
};

nomoney = function()
	pon(1,2,3,4,5);
	shopdlg:pon(2);
	return cat('Тут я вспоминаю, что у меня нет денег... Совсем...^',back());
end

ifmoney ='if not have("money") then return nomoney(); end; shopman._wantmoney = true; ';

dshells = obj {
	nam = 'гильзы',
	dsc = function(s)
		if here()._dshells > 4 then
			return 'Под ногами валяется '..here()._dshells..' {гильз} от моего дробовика...';
		else 
			return 'Под ногами валяются '..here()._dshells..' {гильзы} от моего дробовика...';
		end
	end,
	act = 'Это мои гильзы... Мне они больше не нужны...';
};

function dropshells()
	if here() == deepforest then
		return;
	end
	if not here()._dshells then
		here()._dshells = 2;
	else
		here()._dshells = here()._dshells + 2;
	end
	here().obj:add('dshells');
end

shells = obj {
	nam = 'патроны',
	inv = 'Патроны для моего дробовика. Я очень редко их использую в лесу, в основном -- против браконьеров.',
	use = function(s, on)
		if on == 'gun' then
			if gun._loaded then
				return 'Уже заряжен...';
			end
			if gun._loaded == false then
				gun._loaded = true;
				dropshells();
				return 'Открыв дробовик я выбрасываю две гильзы и перезаряжаю дробовик.';
			end
			gun._loaded = true;
			return 'Я беру два патрона и отправляю их в оба ствола дробовика...';
		end
	end
};

news = obj {
	nam = 'газета',
	inv = [[
Свежая газета... <<недавно построенный в тайге институт квантовой механики категорически опровергает 
причастность к аномальным явлениям>>.. Гм...]],
	used = function(s, w)
		if w == 'poroh' then
			if have('trut') then
				return 'У меня уже есть трут.';
			end
			inv():add('trut');
			inv():del('poroh');
			return 'Я высыпаю порох на клочок бумаги, которую я оторвал от газеты...';
		end
	end,
};

hamb = obj {
	nam = 'гамбургер',
	inv = function()
		inv():del('hamb');
		return 'Я перекусил. Вредная пища...';
	end
};

zerno = obj {
	nam = 'крупа',
	inv = 'Просто гречка. Гречневая крупа...',
};

shop2 = dlg {
	nam = 'купить',
	pic = 'gfx/shopbuy.png',
	obj = { 
	[1] = phr('Патронов... Мне нужны патроны...', 'Хорошо... Цена как обычно', ifmoney..'inv():add("shells")'),
	[2] = phr('Зерна..', 'Хорошо... ', ifmoney..'inv():add("zerno")'),
	[3] = phr('И еще гамбургер...', 'Ок..', ifmoney..'inv():add("hamb")'),
	[4] = phr('Свежую прессу...', 'Конечно...', ifmoney..'inv():add("news")'),
	[5] = phr('Моток изоленты...', 'Да. Держи.', ifmoney..'inv():add("iso")'),
	[6] = phr('Ничего не надо...', 'Как пожелаешь.', 'pon(6); back()'),
	[7] = _phr('Еще мне нужна лестница и кусачки...', 'Извини, этого у меня нет -- качает головой Владимир'), 
	},
	exit = function(s)
		if have('news') then
			s.obj[4]:disable();
		end
	end
};

shopdlg = dlg {
	nam = 'разговор с продавцом',
	pic = 'gfx/shopman.png',
	dsc = 'Маленькие глазки буравят меня маслянистым взглядом.',
	obj = {
	[1] = phr('Здравствуй, Владимир! Ну как оно?', 'Здравствуй, '..me().nam..'... Да потихоньку... - Владимир хитро улыбается.', 'pon(2)'),
	[2] = _phr('Хочу сделать покупки.', 'Хорошо... Давай посмотрим, что тебе нужно?', 'pon(2); return goto("shop2")'),
	[3] = phr('Ну пока!...', 'Ага... Удачи!', 'pon(3); return back();'),
	[4] = _phr('Здесь только что был человек -- кто он?', 'Гм? -- тонкие брови Володи приподнимаются..','pon(5)'),
	[5] = _phr('Он почему-то взял моего кота... Наверное подумал, что он бездомный... Кто этот человек в сером пальто?',
[[
Вообще-то он какая-то шишка... - поскреб Владимир свой небритый подбородок. -- В этом новом институте, что построили в 
нашей глуши год назад... -- пенсне Владимира задергалось в такт его речи -- он часто заходит в наш магазин, 
не любит толпы -- эти физики -- ну ты понимаешь... Странный народ -- Владимир пожал плечами...]],'pon(6)'),
	[6] = _phr('А где этот институт находится?', 
'Да на 127-ом.. Только это, знаешь чего -- Владимир понизил голос -- об этом институте всякое говорят...', 'me()._know_where = true; inmycar.way:add("inst");pon(7)'),
	[7] = _phr('Я только заберу своего кота назад...', 'Ну смотри, как знаешь.. Я бы на твоем месте... - качает головой Владимир. - Да, кажется его фамилия Белин. Я видел его кредитку... Хотя ты знаешь -- я их не принимаю -- Владимир зашмакал губами, пенсне хитро зашевелилось.'),
	},
};

shopman = obj {
	nam = 'продавец',
	dsc = 'За прилавком стоит {продавец}. Довольно полное его лицо с небритой щетиной дополняет монокль.',
	act = function()
		return goto('shopdlg');
	end
};

shop = room {
	nam = 'магазин',
	pic = 'gfx/inshop.png',
	enter = function(s, f)
		if village.obj:look('truck') then
			village.obj:del('truck');
			village.obj:del('mycat');
			return [[
Когда я заходил в магазин, я чуть не с толкнулся с неприятным типом в сером пальто и
шляпе с длинными полями... Он извинился каким-то шипящим голосом и сделал вид, что приподнимает шляпу... Из под
ее полей блеснули белые зубы... Дойдя до прилавка я услышал звук запускающегося двигателя.]];
		end
	end, 
	act = function(s,w)
		if w == 1 then
			return 'Теперь на стоянке стоит только моя машина.';
		end
	end,
	dsc = [[
Это довольно странный магазин... Здесь вы найдете и скобяные изделия, и продукты и
даже патроны... Не удивительно, ведь это единственный магазин на 100км...]],
	way = { 'village' },
	obj = {'shopman',vobj(1, 'окно', 'Сквозь {окно} видно стоянку машин.') },
	exit = function(s, t)
		if t ~= 'village' then
			return;
		end
		if shopman._wantmoney then
			return 'Я собираюсь выйти, когда меня останавливает деликатное покашливание Владимира... Конечно, я забыл заплатить...', false;
		end
		if not have('news') then
			shop2.obj[4]:disable();
			inv():add('news');
			return 'Я собираюсь уходить, когда меня останавливает голос Владимира -- Возьми свежую прессу, для тебя -- бесплатно. Я возвращаюсь, беру газету и выхожу из магазина.';
		end
	end
};

carbox = obj {
	_num = 0,
	nam = function(s)
		if s._num > 1 then
			return 'ящики в машине';
		else
			return 'ящик в машине';
		end
	end,
	act = function(s)
		if inv():srch('mybox') then
			return 'У меня уже есть ящик в руках...';
		end
		s._num = s._num - 1;
		if s._num == 0 then
			mycar.obj:del('carbox');
		end
		take('mybox');
		return 'Я взял ящик из машины.';
	end,
	dsc = function(s)
		if s._num == 0 then
			return;
		elseif s._num == 1 then
			return 'В кузове моей машины лежит один {ящик}.';
		elseif s._num < 5 then
			return 'В кузове моей машины лежат '..tostring(s._num)..' {ящика}.';
		else	
			return 'В кузове моей машины лежит '..tostring(s._num)..' {ящиков}.';
		end
	end,
};

mybox = obj {
	nam = 'ящик',
	inv = 'Я держу в руках ящик.... Добротно сделанная вещь! Пригодится в хозяйстве.',
	use = function(s, o)
		if o == 'boxes' then
			inv():del('mybox');
			return 'Я положил ящик обратно...';
		end
		if o == 'mycar' then
			inv():del('mybox');
			mycar.obj:add('carbox');
			carbox._num = carbox._num + 1;
			return 'Я положил ящик в кузов своей машины...';
		end
		if o == 'ewall' or o == 'wboxes' then
			if not cam._broken then
				return 'Мне мешает камера...';
			end
			inv():del('mybox');
			ewall.obj:add('wboxes');
			wboxes._num = wboxes._num + 1;
			if wboxes._num > 1 then
				return 'Я поставил следующий ящик на предыдущий...';
			end
			return 'Я поставил ящик у стены...';
		end
	end
};

boxes = obj {
	nam = 'ящики',
	desc = {
		[1] = 'Около стоянки валяются пустые деревянные {ящики} из-под тушенки.',
	},
	dsc = function(s)
		local state = 1;
		return s.desc[state];
	end,
	act = function(s, t)
		if carbox._num >= 5 then
			return 'А может хватит уже брать ящики?...';
		end
		if inv():srch('mybox') then
			return 'У меня уже есть один ящик...';
		end
		take('mybox');
		return 'Я взял ящик в руки.';
	end,
};

village = room {
	nam = 'стоянка перед магазином',
	dsc = 'Привычное место перед магазином. Стоянка машин. Все в снегу...',
	pic = 'gfx/shop.png',
	act = function(s, w)
		if w == 1 then
			return 'Баки как баки... Белый снег прикрывает мусор...';
		end	
	end,
	exit = function(s, t)
		if t == 'shop' and seen('mycat') then
			return 'Я позвал барсика, но он был сильно увлечен мусорными баками... Ладно -- я не на долго...';
		end
	end,
	enter = function(s, f)
		if ewall:srch('wboxes') and wboxes._num == 1 then
			ewall.obj:del('wboxes');
			ewall._stolen = true;
			wboxes._num = 0;
		end
		if f == 'shop' and not s._ogh then
			s._ogh = true;
			set_music("mus/revel.s3m");
			guydlg:pon(4);
			guydlg:poff(8);
			return 'Окинув стоянку беглым взглядом я позвал -- Барсик! Барсик! -- Куда запропастился мой кот?';
		end
	end,
	way = { 'road', 'shop' },
	obj = { 'truck', vobj(1,'баки', 'Ржавые мусорные {баки} покрыты снегом.'), 'guy','boxes' },
};
----------- trying to go over wall
function guardreact()
	pon(7);
	if inst:srch('mycar') then
		inst.obj:del('mycar');
		inmycar.way:add('backwall');
		inst.way:add('backwall');
		return cat([[Четверо людей с автоматами провожают меня до моей машины.
Мне пришлось завести двигатель и отъехать от института. Я проехал с дюжину километров, прежде чем в зеркале заднего вида 
исчез военный джип, с моими провожающими... ]], goto('inmycar'));
	end
	return cat([[Четверо вооруженных людей вышвыривают меня из КПП.^^]], goto('inst'));
end

guarddlg = dlg {
	nam = 'охранник',
	pic = 'gfx/guard.png',
	dsc = [[Передо мной угловатое лицо охранника. Его глаза глядят насмешливо, но уголки рта загнуты
вниз, что не располагает к беседе...]],
	obj = {
	[1] = phr('Моего кота по ошибке забрал сотрудник вашего института -- мне нужно войти.','-- Пропуск...', 'poff(2); pon(3);'),
	[2] = phr('Я забыл свой пропуск -- можно мне зайти?','-- Нет...', 'poff(1); pon(3);'),
	[3] = _phr('Вы знаете Белина? У него мой кот -- мне нужно его забрать...', '-- Нет пропуска?', 'pon(4)'),
	[4] = _phr('Я просто пришел забрать своего кота! Дайте телефон Белина.', 
[[Глаза охранника меняют свой цвет. Уголки губ поднимаются наверх -- вот что, господин хороший, -- я так понял, 
пропуска у вас нет, идите-ка отсюда, пока можете...]], 'pon(5, 6)'),
	[5] = _phr('Ну все, щас я дам по твоей роже...', 'Рука охранника тянется к автомату. ', 'poff(6); return guardreact();'), 
	[6] = _phr('Ладно, я пошел...', '-- Не спеши - охранник уже не скрывает свою ухмылку - ты мне не нравишься...','poff(5); return guardreact()'),
	[7] = _phr('Щас я вас всех перестреляю из своего дробовика...', 'На этот раз охранник даже не отвечает. Его налитые кровью глаза красноречивей всяких слов.','return guardreact()'),
	},
};
guard = obj {
	nam = 'охрана',
	dsc = [[
В будке сидит {охрана}. Кажется, она вооружена автоматами калашникова.
]],
	act = function(s)
		return goto('guarddlg');
	end,
};
kpp = room {
	nam = 'КПП',
	pic = 'gfx/kpp.png',
	dsc = [[КПП -- контрольно пропускной пункт не оставляет никаких сомнений в том, что в институте не жалуют посторонних. Шлагбаум. Решетчатая будка. И тишина.
]],
	obj = { 'guard' },
	way = { 'inst' }
};
inst = room {
	nam = 'институт',
	pic = 'gfx/inst.png',
	dsc = [[
Институт возвышается посреди пустынного снежного поля. Его зловещие контуры напоминают скорее тюрьму, чем научное
учреждение. Позади территории института находятся железнодорожные пути. ]],
	act = function(s, w)
		if w == 1 then  
			return 'Высота стены около 5 метров. Но этого мало -- сверху проходит колючая проволока -- думаю, она под напряжением...';
		end
		if w == 2 then
			return 'Нет, Владимир был прав... Это какой-то военный штаб...';
		end
		if w == 3 then	
			return 'Да -- это, похоже, тот самый фургон, в котором человек в сером пальто увез моего Барсика.';
		end
	end,
	used = function(s, w, b)
		if b == 'mybox' and w == 1 then
			return 'Я думаю, меня сразу заметит охрана.';
		end
		if w == 2 and b == 'gun' and gun._loaded then
			return 'Меня посадят... Или просто побьют... Охранники совсем недалеко.';
		end
		if w == 3 and b == 'gun' and gun._loaded then
			return 'Мне нужен мой кот, а не разрушения...';
		end
	end,
	obj = {vobj(1, 'стена', 'Здание института окружено массивной бетонной {стеной}. В центре находится КПП.'),
		vobj(2, 'камеры', 'На вышках установлены {камеры} слежения.'),
		vobj(3, 'фургон', 'За шлагбаумом виднеется черный {фургон}')},
	way = { 'road', 'kpp' },
	exit = function(s, t)
		if have('mybox') and t ~= 'inmycar' then
			return 'Я не буду ходить с ящиком в руках...', false;
		end
	end,
};

cam = obj {
	nam = 'камера слежения',
	dsc = function(s)
		if not s._broken then
			return 'Неподалеку от меня -- одна из {камер} слежения. Я прижимаюсь к стене, чтобы меня не заметили.';
		end
		return 'Неподалеку валяются осколки {камеры} слежения. Их уже запорошило снегом.';
	end,
	act = function(s)
		if not s._broken then
			return 'Проклятая камера...';
		end
		return 'Ха... Получил, проклятый механизм? Интересно, когда придет охрана...';
	end,
};

wire = obj {
	nam = 'колючая проволока',
	dsc = function(s)
		if s._broken then
			return 'Перед моими глазами обрывки колючей {проволоки}.';
		end
		return 'Перед моими глазами колючая {проволока}.';
	end,
	act = function(s)
		if s._broken then
			return 'Теперь она безопасна! Можно пробраться внутрь...';
		end
		return 'А вдруг она под напряжением?';
	end,
};

onwall = room {
	pic = 'gfx/onwall.png',
	nam = 'на стене',
	dsc = 'Я стою на ящиках, моя голова находится на уровне вершины стены. Холодно.',
	enter = function(s)
		if have('mybox') then
			return 'Я не могу взобраться на стену с ящиком в руках.', false;
		end
		if wboxes._num < 5 then
			return 'Я пытаюсь взобраться на стену... Но все-еще слишком высоко...',false;
		end
		return 'Я взбираюсь на стену по ящикам.';
	end,
	obj = { 'wire' },
	way = { 'backwall' }
};

wboxes = obj {
	_num = 0,
	nam = function(s)
		if (s._num > 1) then
			return 'ящики у стены';
		end
		return 'ящик у стены';
	end,
	act = function(s)
		return goto('onwall');
	end,
	dsc = function(s)
		if s._num == 0 then
			return;
		elseif s._num == 1 then
			return 'У стены лежит один {ящик}.';
		elseif s._num < 5 then
			return 'У стены стоит '..tostring(s._num)..' {ящика}, поставленные один на другой.';
		else	
			return 'У стены стоят '..tostring(s._num)..' {ящиков}, поставленные один на другой.';
		end
	end,
};

ewall = obj {
	nam = 'стена',
	dsc = '{Стена} здесь возвышается на 4 метра. Снежная метель с воем бросает ледяные снежинки к ее подножию.',
	act = function(s)
		if not s._ladder then
			s._ladder = true;
			shop2:pon(7);
		end
		return 'Слишком высокая... Нужна лестница.';
	end
};

backwall = room {
	pic = 'gfx/instback.png',
	enter = function(s, f)
		local st = '';
		if ewall._stolen then
			ewall._stolen = false;
			st = 'Ого!!! Кто-то украл мой ящик!!!';
		end
		if f == 'inmycar'  then
			return 'Отлично... Кажется удалось добраться незамеченным...'..' '..st;
		end
		return 'Плутая по снежному полю, я добрался до задней стены.'..' '..st;
	end,
	nam = 'восточная стена института',
	dsc = 'Я нахожусь у задней стороны института.',
	obj = { 'ewall', 'cam' },
	way = { 'inst', },
	exit = function(s, t)
		if have('mybox') and t ~= 'inmycar' then
			return 'Я не буду ходить с ящиком в руках...', false;
		end
	end,
};
