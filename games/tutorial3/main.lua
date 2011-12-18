-- $Name:Tutorial$
-- $Name(ru):Обучение$
-- $Name(ua):Навчання$

instead_version "1.6.0"
require "xact"


tut_lang = {
	ru = 'Язык',
	en = 'Language',
	ua = 'Мова',
	it = 'Lingua',
	es = 'Idioma',
}

tut_title = {
	ru = 'Обучение',
	en = 'Tutorail',
	ua = 'Навчання',
	it = 'Tutorial',
	es = 'Tutorial',
}

if not LANG or not tut_lang[LANG] then
	LANG = "en"
end

tut_lang = tut_lang[LANG]
tut_title = tut_title[LANG]

main = room {
	nam = tut_title;
	pic = 'instead.png';
	forcedsc = true;
	dsc = txtc (
		txtb(tut_lang)..'^^'..
		img('gb.png')..' '..[[{en|English}^]]..
		img('ru.png')..' '..[[{ru|Русский}^]]..
		img('ua.png')..' '..[[{ua|Українська}^]]..
		img('es.png')..' '..[[{es|Español}^]]..
		img('it.png')..' '..[[{it|Italiano}]]);
	obj = {
		xact("ru", code [[ gamefile('main-ru.lua', true) ]]);
		xact("en", code [[ gamefile('main-en.lua', true) ]]);
		xact("ua", code [[ gamefile('main-ua.lua', true) ]]);
		xact("es", code [[ gamefile('main-es.lua', true) ]]);
		xact("it", code [[ gamefile('main-it.lua', true) ]]);
	}
}