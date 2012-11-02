require "hideinv"
require "kbd"

local keyb = {
	{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", },
	{ "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", },
	{ "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", [[\]], },
	{ "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", },
}

local kbden = {
	shifted = {
	["1"] = "!",
	["2"] = "@",
	["3"] = "#",
	["4"] = "$",
	["5"] = "%",
	["6"] = "^",
	["7"] = "&",
	["8"] = "*",
	["9"] = "(",
	["0"] = ")",
	["-"] = "_",
	["="] = "+",
	["["] = "{",
	["]"] = "}",
	["\\"] = "|",
	[";"] = ":",
	["'"] = "\"",
	[","] = "<",
	["."] = ">",
	["/"] = "?",
	}
}

local kbdru = {
	["q"] = "й",
	["w"] = "ц",
	["e"] = "у",
	["r"] = "к",
	["t"] = "е",
	["y"] = "н",
	["u"] = "г",
	["i"] = "ш",
	["o"] = "щ",
	["p"] = "з",
	["["] = "х",
	["]"] = "ъ",
	["a"] = "ф",
	["s"] = "ы",
	["d"] = "в",
	["f"] = "а",
	["g"] = "п",
	["h"] = "р",
	["j"] = "о",
	["k"] = "л",
	["l"] = "д",
	[";"] = "ж",
	["'"] = "э",
	["z"] = "я",
	["x"] = "ч",
	["c"] = "с",
	["v"] = "м",
	["b"] = "и",
	["n"] = "т",
	["m"] = "ь",
	[","] = "б",
	["."] = "ю",
	["`"] = "ё",
	
	shifted = {
	["q"] = "Й",
	["w"] = "Ц",
	["e"] = "У",
	["r"] = "К",
	["t"] = "Е",
	["y"] = "Н",
	["u"] = "Г",
	["i"] = "Ш",
	["o"] = "Щ",
	["p"] = "З",
	["["] = "Х",
	["]"] = "Ъ",
	["a"] = "Ф",
	["s"] = "Ы",
	["d"] = "В",
	["f"] = "А",
	["g"] = "П",
	["h"] = "Р",
	["j"] = "О",
	["k"] = "Л",
	["l"] = "Д",
	[";"] = "Ж",
	["'"] = "Э",
	["z"] = "Я",
	["x"] = "Ч",
	["c"] = "С",
	["v"] = "М",
	["b"] = "И",
	["n"] = "Т",
	["m"] = "Ь",
	[","] = "Б",
	["."] = "Ю",
	["`"] = "Ё",
	["1"] = "!",
	["2"] = "@",
	["3"] = "#",
	["4"] = ";",
	["5"] = "%",
	["6"] = ":",
	["7"] = "?",
	["8"] = "*",
	["9"] = "(",
	["0"] = ")",
	["-"] = "_",
	["="] = "+",
	}
}
local kbdlower = {
	['А'] = 'а',
	['Б'] = 'б',
	['В'] = 'в',
	['Г'] = 'г',
	['Д'] = 'д',
	['Е'] = 'е',
	['Ё'] = 'ё',
	['Ж'] = 'ж',
	['З'] = 'з',
	['И'] = 'и',
	['Й'] = 'й',
	['К'] = 'к',
	['Л'] = 'л',
	['М'] = 'м',
	['Н'] = 'н',
	['О'] = 'о',
	['П'] = 'п',
	['Р'] = 'р',
	['С'] = 'с',
	['Т'] = 'т',
	['У'] = 'у',
	['Ф'] = 'ф',
	['Х'] = 'х',
	['Ц'] = 'ц',
	['Ч'] = 'ч',
	['Ш'] = 'ш',
	['Щ'] = 'щ',
	['Ъ'] = 'ъ',
	['Э'] = 'э',
	['Ь'] = 'ь',
	['Ы'] = 'ы',
	['Ю'] = 'ю',
	['Я'] = 'я',
}

local function tolow(s)
	if not s then
		return
	end
	s = s:lower();
	local xlat = kbdlower
	if xlat then
		local k,v
		for k,v in pairs(xlat) do
			s = s:gsub(k,v);
		end
	end
	return s;
end

local function input_esc(s)
	local rep = function(s)
		return txtnb(s)
	end
	if not s then return end
	local r = s:gsub("[^ ]+", rep):gsub("[ \t]", rep):gsub("{","\\{"):gsub("}","\\}");
	return r
end

local function kbdxlat(s, k)
	local kbd

	if k:len() > 1 then
		return
	end

	if s.alt_xlat and 
		(game.codepage == 'UTF-8' or game.codepage == 'utf-8') then
		kbd = kbdru;
	else
		kbd = kbden
	end

	if kbd and s.shift then
		kbd = kbd.shifted;
	end

	if not kbd[k] then
		if s.shift then
			return k:upper();
		end
		return k;
	end
	return kbd[k]
end

keyboard = function(v)
	stead.add_var(v, { text = '', alt = false, shift = false, alt_xlat = false});
	v.hideinv = true
	if not v.cursor then
		v.cursor = '_'
	end
	if not v.msg then
		v.msg = 'Ввод: ';
	end
	v.entered = function(s)
		s.text = '';
		s.alt = false
		s.shift = false
		hook_keys('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z');
		hook_keys('1', '2', '3', '4', '5', '6', '7', '8', '9', '0');
		hook_keys("-", "=", "[", "]", "\\", ";", "'", ",", ".", "/");

		hook_keys('space')
		hook_keys('backspace');
		hook_keys('left alt', 'right alt', 'alt')
		hook_keys('left shift', 'right shift', 'shift')
		hook_keys('return');
	end
	v.left = function(s)
		unhook_keys('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z');
		unhook_keys('1', '2', '3', '4', '5', '6', '7', '8', '9', '0');
		unhook_keys("-", "=", "[", "]", "\\", ";", "'", ",", ".", "/");
		unhook_keys('space')
		unhook_keys('backspace');
		unhook_keys('left alt', 'right alt', 'alt')
		unhook_keys('left shift', 'right shift', 'shift')
		unhook_keys('return');
	end
	v.kbd = function(s, press, key)
		if key:find("alt") then
			s.alt = press
			if s.alt then
				s.alt_xlat = not s.alt_xlat
			end
			return true
		end

		if key:find("shift") then
			s.shift = press
			return true
		end

		if not press then
			return
		end
		if s.alt then
			return
		end
		local o = stead.here():srch('key');
		return stead.call(o, 'act', key);
	end
	
	v.xdsc = function(s)
		p (s.msg)
		p (input_esc(s.text..s.cursor))
		pn()
		local k,v
		for k, v in ipairs(keyb) do
			local kk, vv
			local row = ''
			for kk, vv in ipairs(v) do
				local a = kbdxlat(s, vv)
				if vv == ',' then
					vv = 'comma'
				end
				row = row.."{key("..vv..")|"..input_esc(a).."}"..txtnb "  ";
			end
			pn(txtc(row))
		end
		pn (txtc[[{key(alt)|«Alt»}    {key(shift)|«Shift»}    {key(cancel)|«Отмена»}    {key(backspace)|«Забой»}    {key(return)|«Ввод»}]]);
	end;
	v.obj = {
		obj {
			nam = 'key';
			act = function(s, w)
				if w == 'comma' then
					w = ','
				end
				if w:find("alt") then
					stead.here().alt_xlat = not stead.here().alt_xlat
					return true
				end

				if w:find("shift") then
					stead.here().shift = not stead.here().shift
					return true
				end

				if w == 'space' then
					w = ' '
				end
				if w == 'backspace' then
					if not stead.here().text or stead.here().text == '' then
						return
					end
					if stead.here().text:byte(stead.here().text:len()) >= 128 then
						stead.here().text = stead.here().text:sub(1, stead.here().text:len() - 2);
					else
						stead.here().text = stead.here().text:sub(1, stead.here().text:len() - 1);
					end
				elseif w == 'cancel' then
					stead.here().text = '';
					stead.back();
				elseif w == 'return' then
					return stead.back();
				else
					w = kbdxlat(stead.here(), w)
					stead.here().text = stead.here().text..w;
				end
				return true
			end
		}
	}
	return xroom(v)
end
