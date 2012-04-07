-- $Name: Диалог$
instead_version "1.6.3"
require "para"
require "dash"
require "quotes"
require "hotkeys"

main = dlg {
	nam = '...';
	entered = function(s)
		p [[У тебя одна минута на то, чтобы объяснить мне, как добраться до хранилища.^
		— Что?^
		Бац! В глазах сверкнуло и боль такая сильная, что кажется, будто она — единственное, что я сейчас чувствую. Даже сильнее страха.^
		— Одна минута, — говорит он и прижимает холодный кружок дула к моему лбу.]];
	end;
	phr = {
		{ 'Что?',
			[[Бац! Оказывается, может быть ещё больнее, чем в первый раз.^ 
			— Ещё раз скажешь "что", выстрелю.]], 
			[[ pon 'что?' ]] };
		{ false, tag = 'что?', 
			'Что?', 
			[[Он стискивает зубы и стреля...]], 
			[[ walk 'theend' ]] };		
		{ 'Но я здесь не работаю...', 
			[[ — Но я здесь не работаю, я вообще не знаю, где тут что!^
			— Какого чёрта тогда ты делаешь в кабинке оператора? ]], 
			[[ pjump 'кто ты?' ]] };
		{ tag = 'про подвал', 
			'Хранилище в подвале.', 
			[[— Хранилище находится в подвале...^
			— Уже лучше. А теперь, как туда добраться?]], 
			[[ pjump 'хранилище' ]] },
		{ },

		{ tag = 'кто ты?', },
		{ 	'Я техник.', 
			[[— Я просто техник, мне позвонили и попросили починить компьютер.
			Они иногда вызывают меня сюда, но я не знаю, где тут сейф, честное слово!^
			— Занятно. Тогда почему на твоём бейдже написано "Старший кассир"?]],
			[[ pjump 'бейдж' ]] },
		{ 'Я от вас прячусь.', 
			[[ — Я тут от вас прячусь...^
			— А, ты один из посетителей?^ 
			— Да.^
			— Я вот не видел, как ты сюда пробегал, ты сюда зашёл ещё до того, как я вошёл в здание?^
			— Да!^
			— А как же ты через турникет прошёл?]], 
			[[ pon('что?'); pjump 'лжец' ]],
			},
		{ 'Тоже решил денег взять.', 
			[[— Да я вот решил тоже денег взять. Под шумок, так сказать. Хе-хе.^
			— А, вон оно что. Но на тебе форма сотрудника банка. 
			То есть, ты не только лжец, но ещё и вор? Двадцать секунд. Хе-хе.]],
			[[ poff('что?'); pjump 'лжец' ]],
		},
		{ },

		{ tag = 'бейдж', 
			'Я его одолжил, чтобы через турникет пройти.',
			[[— Я его одолжил, чтобы пройти через турникет, я часто так делаю, а то они никак мне собственный не сделают...^
			— У кого одолжил?^
			Пальцем в толстого очкарика на полу, тут же затрясшего головой.^
			— Окей, спасибо, техник, — и спускает куро...]], 
			[[ walk 'theend' ]] },
		{ },

		{ tag = 'лжец' },
		{ alias = 'что?' },
		{ alias = 'про подвал' },
		{ },

		{ tag = 'хранилище', empty = function()
			pon "нет пути"
		end, },
		{ "По лестнице...", 
			[[— Вон за той дверью в подвал ведёт лестница, по ней можно спуститься до главного коридора, из него в архив, а уже через архив к двери хранилища.^
			— Ещё что-нибудь, о чём мне как грабителю полезно было бы узнать?]],
			[[ psub 'лестница' ]]
		},

		{ tag = 'на лифте', "На лифте...", 
			[[— В кабинете управляющего есть лифт, спускающийся прямо в хранилище. Только вряд ли вы до него доберётесь.^
			— Почему это?]],
			[[ psub 'лифт' ]]
		},
		{ false, tag = "нет пути", 
			"Нет",
			[[— К сожалению, больше путей нет.^
			— Вот же задачка. Монетку бросить что ли... Это всё? Может ещё что-нибудь знаешь?]],
			[[ pjump "про деньги" ]]
		},
		{ },

		{ tag = 'лестница', 
			empty = function(s)
				p "^— А другой путь есть?"
				pret();
			end,
		},
		{ "О лестинце...",
			[[— На лестнице сторожит охранник с оружием.^
			— Каким оружием?]],
			[[ psub 'оружие' ]]
		},
		{ "О коридоре...", 
			[[— Поперёк коридора идут лазерные лучи.^
			— Красные или зелёные?]],
			[[ psub 'коридор' ]]
		},
		{ "Об архиве...", 
			[[— В архиве люди пропадают...^
			— Что?!^
			— Люди, говорю, в архиве пропадают.^
			— Да это я понял! Почему пропадают?^
			— Не знаю, но, поговаривают, что нужно быть поосторожнее со шкафами F.^
			— Почему?^
			— Пропали сотрудники Фриманн, Фрекель, Фаркопс и Фонг. У вас какая фамилия?^
			— Фицжеральд.^
			— О...
			]],
			[[ if not here():empty 'лестница' then p '^— Дальше.' end]]
		},
		{ "О двери в хранилищие...", 
			[[— Толщина двери в хранилище полтора метра.^
			— Ого...
			— И замок «Sargent & Greenleaf».^
			— Ой...^
			— Пол внутри под напряжением десять тысяч вольт.^
			— Ох...^
			— А снаружи камеры.
			]],
			[[ if not here():empty 'лестница' then p '^— Понятно. Дальше.' end]]
		},
		{},

		{ tag = 'коридор', 
			"Красные.",
			[[— Красные, кажется. А что есть какая-то разница?]],
			[[ if not here():empty 'лестница' then p '^— Не твоего ума дело, дальше давай.'; pret()  end]]
		},
		{
			"Зелёные.",
			[[— Зелёные, кажется, а что?^
			— Проклятье, у меня дейтеранопия.^
			— Зелёный цвет не различаете?^
			— Да.^
			— Сочувствую.]],
			[[ if not here():empty 'лестница' then p '^— Спасибо. Дальше.'; pret()  end]]
		},
		{},

		{ tag = 'оружие',
			"Ружьё какое-то...",
			[[— Не знаю, я не разбираюсь.]],
			[[ if not here():empty 'лестница' then p '^— Дальше.'; end pret()]]
		},
		{
			"Benelli M4...",
			[[— Benelli M4 Super 90, шестизарядный, с телескопическим прикладом, пистолетной рукояткой, планкой Пикатинни...]],
			[[ if not here():empty 'лестница' then p '^— Всё, заткнись, давай дальше.'; end pret()]]
		},
		{},

		{ tag = "лифт", empty = function()
			p "— А другой путь есть?"
			pret()
		end},
		{  "Управляющий.", 
			[[— В кабинете скорее всего сидит сам управляющий.^
			— И чего?^
			— Он чемпион города по гарлемскому боксу.^
			— Впервые слышу про такой бокс.^
			— Основная особенность его в том, что во время боя разрешено использовать кастеты, биты и автоматическое оружие.]],
			[[ if not here():empty() then p "— Хмм, ну ладно, допустим, я с ним разберусь, что ещё?" end ]],
		},
		{  "Собаки.", 
			[[— Собаки.^
			— Собаки?^
			— Да.^
			— Большие?^
			— Очень.^
			— Много?^
			— Четыре.]],
			[[ if not here():empty() then p "— Ох... Ну допустим, с собаками я как-нибудь управлюсь. Что-то ещё?." end]],
		},
		{ "Системы идентификации.",
			[[— В лифте стоят системы идентификации, которые пропускают только управляющего.^
			— Какие системы?]],
			[[ psub 'идентификация' ]],
		},
		{},

		{ tag = "идентификация", empty = function()
			pon "по системам все";
		end},

		{ false, tag = 'по системам все',
			[[По системам идентификации всё.]],
			code = [[ if not here():empty('лифт') then p "— Ещё что-нибудь по лифту?" end pret() ]]
		},

		{ "Отпечатки.",
			[[— Сканер отпечатка правой ладони.^
			— Это, теоретически, можно обойти. Дальше.]],
		},

		{ "Сетчатка глаза.",
			[[— Для этого у меня есть инструменты и необходимые навыки, — он зловеще ухмыляется. — Дальше.]],
		},

		{ "Вес.",
			[[— Датчик веса.^
			— Хм. Сколько весит местный управляющий?^
			— 124 килограмма. А вы сколько весите?^
			— 61. Мда. Тут даже клонирующая машина бы не помогла. Ладно, дальше.]],
		},
		{},

		{ tag = "про деньги", 
			"Про деньги.",
			[[— Знаю про деньги в хранилище.^
			— Так, и что с ними?^
			— Их там нет.^
			— Как нет?!^
			— Ну мы переезжаем в другой район города и все деньги и ценности уже перевезли сегодня ночью.^
			— Так почему ты сразу не сказал?!^
			— Ну про это вы как раз не спрашивали.^
			— Проклятье!^^

			И с этими словами он выбегает из отделения банка, где его ловит 
			экипаж инкассаторской машины, приехавшей, чтобы как раз перевезти 
			содержимое хранилища в другой район города. Хе-хе.]],
			[[ walk 'theend' ]],
		},
		{}
	}
}
theend = room {
	nam = 'Конец';
}