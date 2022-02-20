-- $Name:Tutorial$
-- $Name(ru):Обучение$
-- $Name(uk):Навчання$
-- $Name(de):Einführung$
-- $Name(es):Tutorial$
-- $Name(fr):Tutoriel$
-- $Name(it):Tutorial$
-- $Name(pt):Tutorial$
-- $Name(nl):Snelstart Training$
require 'fmt'

local tut_lang = {
	ru = 'Язык',
	en = 'Language',
	ua = 'Мова',
	it = 'Lingua',
	es = 'Idioma',
	pt = 'Linguagem',
	fr = 'Langue',
	de = 'Sprache',
	nl = 'Taal',
}

local tut_title = {
	ru = 'Обучение',
	en = 'Tutorial',
	ua = 'Навчання',
	it = 'Tutorial',
	es = 'Tutorial',
	pt = 'Tutorial',
	fr = 'Tutoriel',
	de = 'Einführung',
	nl = 'Snelstart Training',
}

if not LANG or not tut_lang[LANG] then
	LANG = "en"
end

tut_lang = tut_lang[LANG]
tut_title = tut_title[LANG]

xact.load = function(w)
	gamefile('main-'..w..'.lua', true)
end

room {
	nam = 'main';
	title = tut_title;
	pic = 'instead.png';
	decor = fmt.c (
		fmt.b(tut_lang)..'^^'..
		[[{$fmt img|gb.png} {@ load en|English}^
		{$fmt img|ru.png} {@ load ru|Русский}^
		{$fmt img|uk.png} {@ load uk|Українська}^
		{$fmt img|es.png} {@ load es|Español}^
		{$fmt img|br.png} {@ load pt|Português}^
		{$fmt img|it.png} {@ load it|Italiano}^
		{$fmt img|fr.png} {@ load fr|Français}^
		{$fmt img|de.png} {@ load de|Deutsch}^
		{$fmt img|nl.png} {@ load nl|Nederlands}]]
	);
}
