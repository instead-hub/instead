-- luacheck: globals dprint
-- luacheck: read globals walk disable enable drop take remove

local std = stead
local type = std.type
local table = std.table
local input = std.ref '@input'
local string = std.string
local pairs = std.pairs
local ipairs = std.ipairs
local txt = std.ref '@iface'
local instead = std.ref '@instead'
local iface = txt

local function use_text_event(key)
	if key == "return" or key == "space" then
		return false
	end
	return instead.text_input and instead.text_input()
end

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

local kbdalt = false

local function txt_esc(s)
	local rep = function(self)
		return txt:nb(self)
	end
	if not s then return end
	local r = s:gsub("[<>]+", rep):gsub("[ \t]", rep);
	return r
end

local function dispof(v)
	local d = std.titleof(v) or std.dispof(v)
	local tag = (type(v.tag) == 'string' and v.tag)
	if type(d) ~= 'string' then
		d = tag or 'n/a'
	else
		d = d..((tag and '/'..tag) or '')
	end
	return d
end

local function show_obj(s, o, pfx, verbose)
	local wh = o:where()
	if wh then
		wh = '@'..std.tostr(std.nameof(wh))..'['..(dispof(wh))..']'
	else
		wh = ''
	end
	s:printf("%s%s%snam: %s%s | disp:%s\n",
		pfx or '',
		o:disabled() and '%' or '',
		o:closed() and '-' or '',
		std.tostr(std.nameof(o)),
		wh,
		dispof(o))
	if verbose then
		for k, v in pairs(o) do
			s:printf("*[%s] = %s\n", std.tostr(k), std.dump(v) or 'n/a')
		end
		return
	end
	for _, v in ipairs(o.obj) do
		pfx = (pfx or '' .. '    ')
		show_obj(s, v, pfx)
	end
end

local function show_room(s, o)
	s:printf("nam: %s | title: %s | disp: %s\n",
		std.tostr(std.nameof(o)), std.tostr(std.titleof(o) or 'n/a'), dispof(o))
	s:printf("    way: ")
	for k, v in ipairs(o.way) do
		if k ~= 1 then
			s:printf(" | ")
		end
		s:printf("%s[%s]", std.tostr(std.nameof(v)), dispof(v))
	end
	s:printf("\n")
end

local take = take

local function show_decl(s, t)
	local d = std.ref '@declare':declarations()
	for k, v in pairs(d) do
		if not t or v["type"] == t then
			local dump = std.dump(_G[k])
			if dump == '' then dump = std.tostr(_G[k]) end
			s:printf("%s %s = %s\n", v["type"], k, dump)
		end
	end
end

local	commands = {
	{ nam = 'quit',
		act = function(s)
			s.on = false
			s:disable();
			return std.nop()
		end;
	};
	{ nam = 'find',
		{ nam = 'obj',
			act = function(s, par)
				if not par then
					return
				end
				std.for_each_obj(function(v)
					if v == s then
						return
					end
					local disp = dispof(v)
					local nam = std.tostr(std.nameof(v))
					if disp:find(par, 1, true) or nam:find(par, 1, true) then
						s:printf("nam: %s disp: %s\n", nam, disp)
					end
				end)
			end
		};
		{ nam = 'dsc',
			act = function(s, par)
				if not par then
					return
				end
				std.for_each_obj(function(v)
					if v == s then
						return
					end
					local dsc = std.par(' ', std.call(v, 'dsc'), std.call(v, 'decor')) or ''
					local st, e = dsc:find(par, 1, true)
					if st then
						local nam = std.tostr(std.nameof(v))
						local disp = dispof(v)
						st = st - 32
						if st < 0 then st = 1 end
						s:printf("nam: %s disp: %s dsc: %s\n", nam, disp, dsc:sub(st, e + 32))
					end
				end)
			end
		};
	};
	{ nam = 'show',
		{ nam = 'obj',
			act = function(s, par)
				if par == '*' then
					local objs = {}
					std.for_each_obj(function(v)
						if not v:type 'room' then
							table.insert(objs, v)
						end
					end)
					table.sort(objs, function(a, b)
						return std.tostr(std.nameof(a)) < std.tostr(std.nameof(b))
					end)
					for i = 1, #objs do
						show_obj(s, objs[i])
					end
					return
				end
				if not par then
					for i = 1, #std.here().obj do
						show_obj(s, std.here().obj[i])
					end
					return
				end
				s:printf("[object]\n")
				local st, r = std.pcall(function()
					show_obj(s, std.object(std.tonum(par) or par), '    ', true) end)
				if not st then
					s:printf("%s\n", r)
					return
				end
				return
			end;
		},
		{ nam = 'inv',
			act = function(s)
				s:printf("[inventory]\n")
				for _, v in ipairs(std.me():inventory()) do
					show_obj(s, v, '    ')
				end
			end;
		};
		{ nam = 'room',
			act = function(s, par)
				if par == '*' then
					local rooms = {}
					std.for_each_obj(function(v)
						if v:type 'room' then
							table.insert(rooms, v)
						end
					end)
					table.sort(rooms, function(a, b)
						return std.tostr(std.nameof(a)) < std.tostr(std.nameof(b))
					end)
					for i = 1, #rooms do
						show_room(s, rooms[i])
					end
					return
				end
				local r, v = std.pcall(function() return par and std.object(par) or std.here() end)
				if not r then
					s:printf("%s\n", v)
					return
				end
				if not std.is_obj(v, 'room') then
					s:printf("It is not the room.\n")
					return
				end
				s:printf("[room]\n    ")
				show_room(s, v)
				s:printf("[objects]\n")
				for _, o in ipairs(std.here().obj) do
					show_obj(s, o, '    ')
				end
			end;
		};
		{
			nam = 'declare',
			act = function(s, par)
				if par == '*' then
					show_decl(s)
				else
					show_decl(s, 'declare')
				end
			end
		},
		{
			nam = 'global',
			act = function(s, _)
				show_decl(s, 'global')
			end
		},
		{
			nam = 'const',
			act = function(s, _)
				show_decl(s, 'const')
			end
		}
	};
	{ nam = 'take',
	  act = function(s, par)
		if not par then
			return
		end
		local st, r, v = s:eval(take, std.tonum(par) or par)
		if not st then
			s:printf("%s\n", r)
			return
		end
		return r, v
	  end;
	},
	{ nam = 'drop',
	  act = function(s, par)
		if not par then
			return
		end
		local st, r, v = s:eval(drop, std.tonum(par) or par)
		if not st then
			s:printf("%s\n", r)
			return
		end
		return r, v
	  end;
	},
	{ nam = 'remove',
	  act = function(s, par)
		if not par then
			return
		end
		local st, r, v = s:eval(remove, std.tonum(par) or par)
		if not st then
			s:printf("%s\n", r)
			return
		end
		return r, v
	  end;
	},
	{ nam = 'enable',
	  act = function(s, par)
		if not par then
			return
		end
		local st, r, v = s:eval(enable, std.tonum(par) or par)
		if not st then
			s:printf("%s\n", r)
			return
		end
		return r, v
	  end;
	},
	{ nam = 'disable',
	  act = function(s, par)
		if not par then
			return
		end
		local st, r, v = s:eval(disable, std.tonum(par) or par)
		if not st then
			s:printf("%s\n", r)
			return
		end
		return r, v
	  end;
	},

	{ nam = 'dump',
		act = function(s, par)
			if not par then
				return
			end
			local f, err = std.eval('return ('..par..')')
			if not f then
				s:printf("%s\n", err)
				return
			end
			err, f = std.pcall(f)
			if not err then
				s:printf("%s\n", f)
				return
			end
			if std.is_obj(f) then
				show_obj(s, f, '', true)
			else
				s:printf("%s\n", std.dump(f))
			end
		end
	};
	{ nam = 'eval',
		act = function(s, par)
			if not par then
				return
			end
			local f, err = std.eval(par)
			if not f then
				s:printf("%s\n", err)
				return
			end
			f, err = std.pcall(f)
			if not f then
				s:printf("%s\n", err)
				return
			end
		end
	};
	{ nam = 'walk',
		act = function(s, par)
			if not par then
				return
			end
			local st, r, v = s:eval(walk, par, false)
			if not st then
				s:printf("%s\n", r)
				return
			end
			return r, v
		end;
	};
	{ nam = 'clear',
		act = function(s)
			s:cls()
		end;
	};
	{ nam = 'help',
		act = function(s)
			s:printf([[Use <tab> key to complete words.
Use <up>/<down> for history.
Use ctrl-l to clear screen.
Use ctrl-d or f6 to enter/exit debugger.
Some useful commands:
    show obj * - show all objects
    show room * - show all rooms
    find obj <string> - search obj
    find dsc <string> - search obj (in dsc and decor)
    show obj <name> - show object (in verbose mode)
    show room <name> - show room
    show room - show here
    walk <name> - walk anywhere
]]);
		end;
	};
	lookup = function(s, inp)
		local cmd = std.split(inp)
		local cur = s
		local found
		local last_found
		for k, v in ipairs(cmd) do
			found = nil
			for _, c in ipairs(cur) do
				if v == c.nam then
					cur = c
					found = k
					last_found = k
					break
				end
			end
			if not found then
				break
			end
		end
		local par
		if last_found then
			for i = last_found + 1, #cmd do
				par = (par and (par .. ' ') or '').. cmd[i]
			end
		else
			par = inp
		end
		return cur, par
	end;
	completion = function(s, inp)
		local cmd, par = s:lookup(inp)
		if not cmd then
			return
		end
		if #cmd == 0 then
			if par then
				local vars = {}
				std.for_each_obj(function(v, var)
					if std.tostr(v.nam):find(par, 1, true) == 1 and std.tostr(v.nam) ~= par then
						table.insert(var, std.tostr(v.nam))
					end
				end, vars)
				return vars
			end
			return
		end
		local var = {}
		for i = 1, #cmd do
			if cmd[i].nam:find(par or '', 1, true) == 1 then
				table.insert(var, cmd[i].nam)
			end
		end
		return var
	end;
	save = function() end;
};

local embed =	{
	on = false;
	key_shift = false;
	cursor = 1;
	input = '';
	output = [[INSTEAD dbg 0.1
Written by Peter Kosyh in 2017
Type "help" to see help
]];
	hint = '';
	history = {};
	history_pos = 1;
	key_alt = false;
	key_ctrl = false;
	kbd_alt_xlat = false;
	__last_disp = false;
	__nostrict = false;
	last_timer = false;
};

local theme = {}

local function theme_var(a, b)
	local ov = instead.theme_var(a)
	if b then
		theme[a] = ov
		return instead.theme_var(a, b)
	end
	return ov
end

local function theme_reset()
	for k, v in pairs(theme) do
		instead.theme_var(k, v)
	end
	theme = {}
end

local funcs = {}

local function instead_func(a)
	local ov = instead[a]
	funcs[a] = ov
	std.rawset(instead, a, function() end)
end

local function instead_reset()
	for k, v in pairs(funcs) do
		std.rawset(instead, k, v)
	end
	funcs = {}
end

local std_dbg = {}

local function std_debug(a)
	if not a then
		for k, v in pairs(std_dbg) do
			std['debug_'..k] = v
		end
		return
	end
	std_dbg[a] = std['debug_'..a]
	std['debug_'..a] = false
end

local render_callback = false

local dbg = std.obj {
	pri = 16384;
	nam = '@dbg';
	embed;
	{ commands = commands },
	enable = function(s)
		instead_func('get_picture')
		instead_func('get_fading')
		instead_func('get_title')
		instead_func('get_ways')
		render_callback = (std.ref '@sprite').render_callback(false)
		local timer = stead.ref '@timer'
		s.last_timer = timer:get()
		timer:stop()
		std_debug('input')
		std_debug('output')
		std_debug('xref')
		s.__last_disp = std.game:lastdisp()
		s.__nostrict = std.nostrict or false

		local w, h = std.tonum(theme_var 'scr.w'), std.tonum(theme_var 'scr.h')
		theme_var('scr.gfx.mode', 'embedded')
		theme_var('scr.gfx.bg', '')
		theme_var('scr.col.bg', 'white')
		theme_var('win.col.fg', 'black')
		theme_var('inv.mode', 'disabled')
		local padw = w > 320 and 16 or 0
		local padh = h > 320 and 16 or 0
		theme_var('win.x', padw)
		theme_var('win.y', padh)
		theme_var('win.w', w - padw)
		theme_var('win.h', h - padh)
		theme_var('menu.button.x', w)
		theme_var('menu.button.y', h)
		theme_var('win.fnt.size', 16)
		theme_var('win.scroll.mode', 3)
		std.nostrict = true
		iface:raw_mode(true)
	end;
	disable = function(s)
		theme_reset()
		std_debug()
		std.nostrict = s.__nostrict
		instead_reset()
		std.ref('@sprite').render_callback(render_callback)
		iface:raw_mode(false)
		local timer = stead.ref '@timer'
		timer:set(s.last_timer)
		std.game:lastdisp(s.__last_disp)
	end;
	inp_split = function(s)
		local pre = s.input:sub(1, s.cursor - 1);
		local post = s.input:sub(s.cursor);
		return pre, post
	end;
	eval = function(s, fn, ...)
		local st, r, _ = std.pcall(fn, ...)
		if not st then
--			s:printf("%s\n", r)
			return false, r
		else
			s.on = false
			s:disable()
			if std.me():moved() then
				return true, nil, true
			else
				return true, std.nop()
			end
		end
	end;
	cls = function(s)
		s.output = '';
		s.hint = '';
	end;
	completion = function(s, edit)
		local hint = s.commands:completion(s.input)
		if not hint or #hint == 0 then
			s.hint = ''
			return
		end
		if #hint == 1 and edit ~= false then
			local _, par = s.commands:lookup(s.input)
			if par then
				local len = par:len()
				s.input = s.input:sub(1, s.input:len() - len)
			end
			s.input = s.input .. hint[1]..' '
			s.cursor = #s.input + 1
			s:completion(edit)
			return
		end
		s.hint = ''
		for _, v in ipairs(hint) do
			s.hint = s.hint .. v .. ' '
		end
	end;
	printf = function(s, fmt, ...)
		s.output = s.output .. std.string.format(fmt, ...)
	end;
	exec = function(s)
		local c, par = s.commands:lookup(s.input)
		if not c or not c.act then
			return s:completion()
		end
		if #s.history == 0 or s.history[#s.history] ~= s.input then
			table.insert(s.history, s.input)
		end
		s.history_pos = 0
		s:printf('$ '..s.input..'\n')
		s.input = ''
		s.hint = ''
		s.cursor = 1
		return c.act(s, par)
	end;
	dsc = function(s) -- display debugger
		std.pr (txt_esc(s.output))
		if s.kbd_alt_xlat then
			std.pr (txt:bold '&')
		else
			std.pr (txt:bold ' ')
		end
		local pre, post = s:inp_split()
		std.pr (txt:bold '$ '.. txt:bold(txt_esc(pre))..txt:bold '|'..txt:bold(txt_esc(post)) ..'\n')
		if s.hint == '' then s.hint = '?' end
		std.pr (s.hint ..'\n')
		std.pr (txt:anchor())
	end;
	text = function(s, text)
		if not use_text_event() or not s.on then
			return
		end
		if text == " " then -- key will handle this
			return
		end
		return '@dbg text '..string.format("%q", text)
	end;
	key = function(s, press, key)
		if key:find 'shift' then
			s.key_shift = press
			return
		elseif key:find 'ctrl' then
			s.key_ctrl = press
			return
		elseif key:find 'alt' then
			s.key_alt = press
			if s.on then
				if not press and kbdalt then
					s.kbd_alt_xlat = not s.kbd_alt_xlat
				end
				return 'look'
			end
			return
		end
		if not press then
			return
		end
		if s.key_ctrl or s.key_alt then
			if s.on and key == 'q' then
				return '@dbg toggle'
			end
			if key == 'q' or key == 'r' then
				return
			end
		end
		if s.key_shift then
			if key == 'up' or key == 'down' then
				return
			end
		end
		if (key == 'f7'and not s.key_ctrl and not s.key_alt) or (s.key_ctrl and key == 'd') then
			s.key_ctrl = false -- sticky bug
			return '@dbg toggle'
		end
		if s.on then
			return '@dbg key '..string.format("%q", key)
		end
	end;
}

local function utf_bb(b, pos)
	return utf8_prev(b, pos)
end

local function utf_ff(b, pos)
	return utf8_next(b, pos)
end

local function key_xlat(s)
	local kbd

	if s == 'return' then return '\n' end
	if s == 'space' then return ' ' end
	if s:len() > 1 then
		return
	end

	if dbg.kbd_alt_xlat and (std.game.codepage == 'UTF-8' or std.game.codepage == 'utf-8') and kbdalt then
		kbd = kbdalt
	else
		kbd = kbden
	end

	if kbd and dbg.key_shift then
		kbd = kbd.shifted;
	end

	if not kbd[s] then
		if dbg.key_shift then
			return s:upper();
		end
		return s;
	end
	return kbd[s]
end

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@dbg' then
		if dbg.on then
			if cmd[1] == 'look' then
				std.abort()
				return std.call(dbg, 'dsc')
			end
			return nil, false
		end
		return
	end
	if cmd[2] == 'toggle' then
		dbg.on = not dbg.on
		if dbg.on then
			dbg:enable()
			std.abort()
			return std.call(dbg, 'dsc')
		else
			dbg:disable()
			return std.nop()
		end
	elseif cmd[2] == 'key' then
		local key = cmd[3]
		if key:find 'return' and not dbg.key_ctrl and not dbg.key_alt then
			local r, v = dbg:exec()
			if r ~= nil or v ~= nil then
				return r, v
			end
		elseif key:find '^backspace' then
			if dbg.input == '' then
				std.abort()
				return std.call(dbg, 'dsc'), true
			end
			local pre, post = dbg:inp_split()
			if not pre or pre == '' then
				std.abort()
				return std.call(dbg, 'dsc'), true
			end
			local i = utf_bb(pre)
			dbg.input = dbg.input:sub(1, pre:len() - i) .. post
			dbg.cursor = dbg.cursor - i
		elseif key:find '^tab' then
			dbg:completion()
		elseif key:find 'home' or (key == 'a' and dbg.key_ctrl) then
			dbg.cursor = 1
		elseif key:find 'end'  or (key == 'e' and dbg.key_ctrl) then
			dbg.cursor = #dbg.input + 1
		elseif (key == 'k' and dbg.key_ctrl) then
			dbg.cursor = 1
			dbg.input = ''
		elseif (key == 'l' and dbg.key_ctrl) then
			dbg:cls()
		elseif key:find '^right' then
			if dbg.cursor <= dbg.input:len() then
				local i = utf_ff(dbg.input, dbg.cursor)
				dbg.cursor = dbg.cursor + i
			end
			if dbg.cursor > dbg.input:len() then dbg.cursor = dbg.input:len() + 1 end
		elseif key:find '^left' then
			if dbg.cursor > 1 then
				local i = utf_bb(dbg.input, dbg.cursor - 1)
				dbg.cursor = dbg.cursor - i
			end
			if dbg.cursor < 1 then dbg.cursor = 1 end
		elseif key:find '^up' then
			local s = dbg
			if #s.history == 0 then
				return
			end
			if s.history_pos == 0 then
				s.history_pos = #s.history + 1
			end
			s.history_pos = s.history_pos - 1
			if s.history_pos == 0 then
				s.history_pos = 1
			end
			s.input = s.history[s.history_pos]
			s.cursor = #s.input + 1
		elseif key:find '^down' then
			local s = dbg
			if #s.history == 0 or s.history_pos == #s.history then
				return
			end
			s.history_pos = s.history_pos + 1
			if s.history_pos > #s.history then
				s.history_pos = #s.history
			end
			s.input = s.history[s.history_pos]
			s.cursor = #s.input + 1
		elseif not use_text_event(key) and key_xlat(key) then
			local k = key_xlat(key)
			local pre, post = dbg:inp_split()
			dbg.cursor = dbg.cursor + k:len()
			dbg.input = pre .. k .. post
		else
			return nil, false
		end
		dbg:completion(false)
		std.abort()
		return std.call(dbg, 'dsc'), true
	elseif cmd[2] == 'text' then
		local text = cmd[3]
		local pre, post = dbg:inp_split()
		dbg.cursor = dbg.cursor + text:len()
		dbg.input = pre .. text .. post
		dbg:completion(false)
		std.abort()
		return std.call(dbg, 'dsc'), true
	end
end, -100)

function std.dprint(...)
	local a = { ... }
	dbg:printf("dbg> ");
	for i = 1, #a do
		if i ~= 1 then
			dbg:printf("%s", ' ')
			std.io.stderr:write(' ')
		end
		dbg:printf("%s", std.tostr(a[i]))
		std.io.stderr:write(std.tostr(a[i]))
	end
	dbg:printf("\n")
	std.io.stderr:write('\n')
	std.io.stderr:flush()
end

dprint = std.dprint
local oldlang
local okey, otext

local hooked = false
std.mod_start(function(_)
	local st, r
	if not use_text_event() and oldlang ~= LANG then
		st, r = std.pcall(function() return require ('dbg-'..LANG) end)
		if st and r then
			std.dprint("dbg: Using '"..LANG.."' keyboard layout.")
			kbden, kbdalt = r.main, r.alt
		end
	end
	oldlang = LANG
	iface:raw_mode(false)
	if not hooked then
		okey = input.key;
		otext = input.text;
		hooked = true
	end
	std.rawset(input, 'key', function(_, ...)
		return dbg:key(...) or
			(not dbg.on and okey and okey(input, ...))
	end)
	std.rawset(input, 'text', function(_, ...)
		return dbg:text(...) or
			(not dbg.on and otext and otext(input, ...))
	end)
end, -100)

std.mod_done(function()
	hooked = false
	iface:raw_mode(false)
	std.rawset(input, 'key', okey)
	std.rawset(input, 'text', otext)
end, -100)

-- std.rawset(_G, 'dbg',  std.ref '@dbg')
