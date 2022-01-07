-- $Name:Tutorial$
-- $Name(ru):Обучение$
-- $Name(uk):Навчання$
-- $Name(de):Einführung$
-- $Name(es):Tutorial$
-- $Name(fr):Tutoriel$
-- $Name(it):Tutorial$
-- $Name(pt):Tutorial$
-- $Name(nl):Snelstart Training$
-- $Name(eo):Lernilo$

instead_version "1.6.0"
require "xact"

tut_lang = {
	ru = 'Язык',
	en = 'Language',
	ua = 'Мова',
	it = 'Lingua',
	es = 'Idioma',
	pt = 'Linguagem',
	fr = 'Langue',
	de = 'Sprache',
	nl = 'Taal',
	eo = "Lingvo",
}

tut_title = {
	ru = 'Обучение',
	en = 'Tutorial',
	ua = 'Навчання',
	it = 'Tutorial',
	es = 'Tutorial',
	pt = 'Tutorial',
	fr = 'Tutoriel',
	de = 'Einführung',
	nl = 'Snelstart Training',
	eo = 'Lernilo',
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
		img('uk.png')..' '..[[{uk|Українська}^]]..
		img('es.png')..' '..[[{es|Español}^]]..
		img('br.png')..' '..[[{pt|Português}^]]..
		img('it.png')..' '..[[{it|Italiano}^]]..
		img('fr.png')..' '..[[{fr|Français}^]]..
		img('de.png')..' '..[[{de|Deutsch}^]]..
		img('nl.png')..' '..[[{nl|Nederlands}^]]..
		img('eo.png')..' '..[[{eo|Esperanto}^]]..
		'');
	obj = {
		xact("ru", code [[ gamefile('main-ru.lua', true) ]]);
		xact("en", code [[ gamefile('main-en.lua', true) ]]);
		xact("uk", code [[ gamefile('main-uk.lua', true) ]]);
		xact("es", code [[ gamefile('main-es.lua', true) ]]);
		xact("it", code [[ gamefile('main-it.lua', true) ]]);
		xact("pt", code [[ gamefile('main-pt.lua', true) ]]);
		xact("fr", code [[ gamefile('main-fr.lua', true) ]]);
		xact("de", code [[ gamefile('main-de.lua', true) ]]);
		xact("nl", code [[ gamefile('main-nl.lua', true) ]]);
		xact("eo", code [[ gamefile('main-eo.lua', true) ]]);
	}
}
