-- some stubs for tiny-instead
-- fake game.gui
-- stat, menu
-- fake audio
-- fake input
-- show hints (numbers)
game.hinting = true

-- fake gui
game.gui = {
	fading = 4;
	ways_delim = ' | ';
	inv_delim = ' | ';
	hinv_delim = ' | ';
	hideways = false;
	hideinv = false;
	hidetitle = false;
}

stead.list_search = function(self, n, dis)
	local i
	if stead.tonum(n) then
		i = self:byid(stead.tonum(n), dis);
	end
	if not i then
		i = self:look(n)
		if not i then
			i = self:name(n, dis);
		end
		if not i then
			return nil
		end
	end
	if not dis and isDisabled(stead.ref(self[i])) then
		return nil
	end
	return self[i], i
end
local dict = {}

iface.xref = function(self, str, obj, ...)
	local cmd = ''
	local o = stead.ref(obj)

	if not isObject(o) or isStatus(o) or (not o.id and not isXaction(o)) then
		return str
	end

	if stead.cmd == 'way' then
		cmd = 'go'
	elseif stead.cmd == 'inv' then
		cmd = 'use'
	elseif isSceneUse(o) then
		cmd = 'use'
	elseif isXaction(o) and not o.id then
		cmd = 'act'
	elseif stead.ref(stead.here():srch(obj)) then
		cmd = 'act'
	end

	local a = ''
	local varg = {...}
	for i = 1, stead.table.maxn(varg) do
		a = a..','..varg[i]
	end

	local n = stead.deref(obj)
	local xref = string.format("%s%s", stead.tostr(o.id or n), a)
	if not dict[cmd..xref] then
		stead.table.insert(dict, { xref, cmd } )
		dict[cmd..xref] = #dict
	end
	xref = stead.tostr(dict[cmd..xref])
	return stead.cat(str, "("..xref..")");
end

local tag_all = stead.player_tagall -- save old

stead.player_tagall = function(self)
	dict = {}
	return tag_all(self)
end

local iface_cmd = iface.cmd -- save old

function iface:cmd(inp)
	local cmd, a = stead.getcmd(inp)
	if stead.tonum(cmd) then
		stead.table.insert(a, 1, cmd)
		cmd = 'act'
	end
	if cmd == 'act' or cmd == 'use' or cmd == 'go' then
		if a[1] == '' then -- fix use vs look
			return iface_cmd(self, "look")
		end
		if cmd == 'use' then
			local use = { }
			local c = false
			for i = 1, #a do
				local u = stead.tonum(a[i])
				u = u and dict[u]
				c = c or (u and u[2])
				stead.table.insert(use, u and u[1] or a[i])
			end
			if #a == 1 and c == 'act' then -- fix use 1,2 where 1,2 is object and look
				cmd = 'act'
			end
			inp = cmd .. ','..stead.table.concat(use, ',')
		elseif stead.tonum(a[1]) then
			local d = dict[stead.tonum(a[1])]
			a[1] = d and d[1] or a[1]
			inp = cmd .. ','..stead.table.concat(a, ',')
		end
	end
	return iface_cmd(self, inp)
end

-- menu and stat
stat = function(v)
	v.status_type = true
	v.id = false
	return obj(v)
end

function isStatus(v)
	if stead.type(v) ~= 'table' then
		return false
	end
	if v.status_type then
		return true
	end
	return false
end

function isMenu(v)
	if stead.type(v) ~= 'table' then
		return false
	end
	if v.menu_type then
		return true
	end
	return false
end

stead.menu_save = function(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: menu "..name.." can not be saved!");
		return
	end
	stead.savemembers(h, self, name, need);
end

function menu(v)
	v.menu_type = true
	if v.inv == nil then
		v.inv = function(s)
			local r,v
			r, v = stead.call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				stead.obj_tag(stead.me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.act == nil then
		v.act = function(s)
			local r,v
			r,v = stead.call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				stead.obj_tag(stead.me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.save == nil then
		v.save = stead.menu_save;
	end
	return obj(v);
end

function iface:title(t)
	if type(t) ~= 'string' then return end
	return '['..t..']'
end

function iface:imgl()
	return ''
end

function iface:imgr()
	return ''
end

function iface:nb(t)
	return t == '' and ' ' or t
end

function iface:y()
	return ''
end

function iface:tab(str)
	return stead.tostr(str)..' '
end

-- fake audio and timer
stead.get_music = function()
	return '', 0
end

stead.get_music_loop = function()
	return 0
end

stead.save_music = function(s)
end

stead.restore_music = function(s)
end

stead.finish_music =function()
	if (game._music_loop or 0) == 0 then
		return false
	end
	game._music_loop = -1
	return true
end
stead.set_music = function(s, count)
end

stead.set_music_fading = function(o, i)
end

stead.get_music_fading = function()
end

stead.stop_music = function()
end

stead.is_music = function()
	return false
end

function instead_sound()
	return false
end

stead.is_sound = instead_sound

stead.get_sound = function()
	return 
end

stead.get_sound_chan = function()
	return 0
end

stead.get_sound_loop = function()
	return 0
end

stead.get_sound_fading = function()
	return 0
end

stead.stop_sound = function(chan, fo)
end

stead.add_sound = function(s, chan, loop,fading)
end

stead.set_sound = function(s, chan, loop,fading)
end

-- those are sill in global space
add_sound = stead.add_sound
set_sound = stead.set_sound
stop_sound = stead.stop_sound

get_sound = stead.get_sound
get_sound_loop = stead.get_sound_loop
get_sound_fading = stead.get_sound_fading
get_sound_chan = stead.get_sound_chan

get_music = stead.get_music
get_music_fading = stead.get_music_fading
get_music_loop = stead.get_music_loop

set_music = stead.set_music
set_music_fading = stead.set_music_fading
stop_music = stead.stop_music

save_music = stead.save_music
restore_music = stead.restore_music

is_music = stead.is_music

stead.set_timer = function() end

stead.timer = function()
	return
end
instead_theme_name = function() return 'default' end

stead.module_init(function(s) 
	timer = obj {
		nam = 'timer',
		get = function(s)
			return 0
		end,
		stop = function(s)
		end,
		del = function(s)
		end,
		set = function(s, v)
			return true
		end,
	};
end)

stead.objects.input =  function()
	return obj { -- input object
		system_type = true,
		nam = 'input',
	};
end;
