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
		for k,v in stead.pairs(xlat) do
			s = s:gsub(k,v);
		end
	end
	return s;
end

local function kbdxlat(s)
	local kbd
	if s == 'space' then
		return ' '
	end
	if s == 'return' then
		return '\n'
	end

	if s:len() > 1 then
		return
	end

	if input.kbd_alt_xlat and 
		(game.codepage == 'UTF-8' or game.codepage == 'utf-8') then
		kbd = kbdru;
	else
		kbd = kbden
	end

	if kbd and input.kbd_shift then
		kbd = kbd.shifted;
	end

	if not kbd[s] then
		if input.kbd_shift then
			return s:upper();
		end
		return s;
	end
	return kbd[s]
end

game.action = stead.hook(game.action, function (f, s, cmd, ...)
	if cmd == 'kbd_enter' then
		local r,v
		if stead.here().inp_enter then
			r,v = stead.call(stead.here(), 'inp_enter');
		elseif s.inp_enter then
			r,v = stead.call(s, 'inp_enter');
		end
		return r,v -- nothing todo
	end
	return f(s, cmd, ...)
end)

local lookup_inp = function()
	local i,o 
	for i,o in stead.opairs(objs()) do
		o = stead.ref(o)
		if o._edit then
			return o
		end
	end
end

local input_kbd = function(s, down, key)
	if not input._txt then
		return
	end
	if key:find("shift") then
		input.kbd_shift = down
	elseif key:find("alt") then
		if down and input.inp_xlat then
			input.kbd_alt_xlat = not input.kbd_alt_xlat;
		end
		input.kbd_alt = down
	elseif down then
		local o = lookup_inp();
		if not o then
			return
		end
		if input.kbd_alt then
			return
		end
		if key == "return" then
			if o then
				o._edit = false
				o._txt = input._txt
				input._txt = false
				return "kbd_enter"
			end
			return
		end
		if key == "backspace" then
			if input._txt == '' then
				return
			end
			if input._txt:byte(input._txt:len()) >= 128 then
				input._txt = input._txt:sub(1, input._txt:len() - 2);
			else
				input._txt = input._txt:sub(1, input._txt:len() - 1);
			end
			return "wait"
		end
		local c = kbdxlat(key);
		if not c then return end
		if o and stead.type(o.filter) == 'function' then
			c = o:filter(c);
			if not c then return end
		end
		input._txt = input._txt..c;
		return "wait"
	end
end

stead.module_init(function()
	input.cursor = '_'
	input.inp_xlat = true
	input.key = stead.hook(input.key,
	function(f, ...)
		local r = input_kbd(...)
		if r then return r end
		return f(...)
	end)
end)

local function input_esc(s)
	local rep = function(s)
		return txtnb(s)
	end
	if not s then return end
--	return s:gsub("\\","\\\\\\\\"):gsub(">","\\\\>"):gsub("%^","\\%^"):
	local r = s:gsub("[^ ]+", rep):gsub("[ \t]", rep):gsub("{","\\{"):gsub("}","\\}");
	return r
end

function inp(n, info, txt)
	if stead.type(n) ~= 'string' or stead.type(info) ~= 'string' then
		error ("Wrong parameter to inp.", 2);
	end
	local v = { nam = n, _txt = '', info = info }
	if txt then
		v._txt = txt
	end
	v.dsc = function(s)
		if s._edit then
			return s.info..input_esc(input._txt)..input.cursor
		end
		return s.info..input_esc(s._txt)
	end
	v.text = function(s, text)
		local t
		if s._edit then 
			t = input._txt 
		else
			t = s._txt
		end

		if text then
			if s._edit then
				input._txt = text
			else
				s._txt = text
			end
			return
		end
		return t
	end
	v.match = function(s, str)
		local aa = tolow(tostring(str)):gsub("[*]",".*"):gsub("[?]",".?");
		local bb = tolow(tostring(s:text()));
		if bb:find("^"..aa.."$") then
			return true
		end
	end
	v.state = function(s, t)
		local os = s._edit
		if t == nil then
			return os
		end
		s._edit = t
		if os and  t or (not os and not t) then
			return os
		end
		if s._edit then
			input._txt = s._txt;
		else
			s._txt = input._txt
			input._txt = false
		end
		return os
	end;
	v.act = function(s)
		if input._txt and not s._edit then return true end -- somewhere else
		s:state(not s._edit)
		return true
	end
	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format("%s = inp (%q, %q, %q);\n", 
				name, self.nam, self.info, self._txt))
		end
		stead.savemembers(h, self, name, false);
	end
	return obj(v)
end

-- vim:ts=4
