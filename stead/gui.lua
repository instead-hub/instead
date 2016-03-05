game.hinting = true;
game.showlast = true;

game.gui = { 
	fading = 4; 
	ways_delim = ' | ';
	inv_delim = '\n';
	hinv_delim = ' | ';
	hideways = false;
	hideinv = false;
	hidetitle = false;
}

iface.xref = function(self, str, obj, ...)
	local o = stead.ref(obj);
	local cmd=''

	if not isObject(o) or isStatus(o) or (not o.id and not isXaction(o)) then
		return str;
	end

	if stead.ref(ways():srch(obj)) then
		cmd = 'go ';
	elseif isMenu(o) then
		cmd = 'act ';
	elseif isSceneUse(o) then
		cmd = 'use ';
	elseif isXaction(o) and not o.id then
		cmd = 'act ';
	end
	local a = ''
	local i
	local varg = {...}
	for i = 1, stead.table.maxn(varg) do
		a = a..','..varg[i]
	end
	if isXaction(o) and not o.id then
		return stead.cat('<a:'..cmd..stead.deref(obj)..a..'>',str,'</a>');
	end
	return stead.cat('<a:'..cmd..'0'..stead.tostr(o.id)..a..'>',str,'</a>');
end;
iface.anchor = function(self)
	return '<a:#>'
end
iface.enum = function(self, n, str)
	if str == nil or n == nil then return nil; end;
	return n..self:nb(' - ')..str;
end

iface.title = function(self, str)
	return nil
end;

iface.img = function(self, str)
	if str == nil then return nil; end;
	return "<g:"..str..">";
end;

iface.imgl = function(self, str)
	if str == nil then return nil; end;
	return "<g:"..str.."\\|left>";
end;

iface.imgr = function(self, str)
	if str == nil then return nil; end;
	return "<g:"..str.."\\|right>";
end;

iface.nb = function(self, str)
	if stead.type(str) ~= 'string' then return nil end
	return "<w:"..str:gsub("\\", "\\\\\\\\"):gsub(">","\\>"):gsub("%^","\\^")..">";
end;

iface.under = function(self, str)
	if str == nil then return nil; end; 
	return stead.cat('<u>',str,'</u>');
end;

iface.em = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<i>',str,'</i>');
end;

iface.st = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<s>',str,'</s>');
end;

iface.right = function(self, str)
	if str == nil then return nil; end; 
	return stead.cat('<r>',str,'</r>');
end;

iface.left = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<l>',str,'</l>');
end;

iface.center = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<c>',str,'</c>');
end;

iface.just = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<j>',str,'</j>');
end;

iface.tab = function(self, str, al)
	if stead.tonum(str) then
		str = stead.tostr(str)
	end
	if stead.type(str) ~= 'string' then
		return nil;
	end
	if al == 'right' then
		str = str .. ",right"
	elseif al == 'center' then
		str = str .. ",center"
	end
	return '<x:'..str..'>'
end;

iface.y = function(self, str, al)
	if stead.tonum(str) then
		str = stead.tostr(str)
	end
	if stead.type(str) ~= 'string' then
		return nil;
	end
	if al == 'middle' then
		str = str .. ",middle"
	elseif al == 'top' then
		str = str .. ",top"
	end
	return '<y:'..str..'>'
end;

iface.bold = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<b>',str,'</b>');
end;

iface.top = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<t>',str,'</t>');
end;

iface.bottom = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<d>',str,'</d>');
end;

iface.middle = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<m>',str,'</m>');
end;

iface.inv = function(self, str)
	return str
end;

iface.ways = function(self, str)
	return str
end;

instead.get_inv = function(horiz)
	local r = stead.call_value(game.gui, 'hideinv');
	if r then
		return
	end
	local str = iface:cmd("inv");
	if str then
		str = stead.string.gsub(str, '\n$','');
		if not horiz then
			str = stead.string.gsub(str, '\\?['.. stead.delim ..']', 
				{ [stead.delim] = game.gui.inv_delim, ['\\'..stead.delim] = stead.delim });
		else
			str = stead.string.gsub(str, '\\?['.. stead.delim ..']', 
				{ [stead.delim] = game.gui.hinv_delim, ['\\'..stead.delim] = stead.delim });
		end
	end
	return str
end

instead.get_ways = function()
	local r = stead.call_value(game.gui, 'hideways');
	if r then
		return
	end
	local str = iface:cmd("way");
	if str and str ~= '' then
		str = stead.string.gsub(str, '\n$','');
		str = stead.string.gsub(str, '\\?['..stead.delim ..']', 
			{ [stead.delim] = game.gui.ways_delim, [ '\\'..stead.delim ] = stead.delim });
		return iface:center(str);
	end
	return str
end

instead.get_title = function()
	local r = stead.call_value(game.gui, 'hidetitle');
	if r then
		return
	end

	local s

	if stead.api_atleast(1, 2, 0) then
		s = stead.dispof(stead.here());
	else
		s = stead.call(stead.here(), 'nam');
	end
	if stead.type(s) == 'string' and s ~= '' then
		stead.state = false
		s = "<c><b>"..stead.fmt(s).."</b></c>";
		s = stead.string.gsub(s, '\\'..stead.delim, stead.delim);
	end
	return s
end

instead.finish_music = function()
	if instead.get_music_loop() == 0 then
		return false
	end
	local n = stead.get_music()
	stead.set_music(n, -1);
	return true
end

instead.isEnableSave = isEnableSave;
instead.isEnableAutosave = isEnableAutosave;

-- here is gui staff only
function stat(v)
	v.status_type = true
	return obj(v);
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

stead.menu_save = function(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: menu "..name.." can not be saved!");
		return
	end
	stead.savemembers(h, self, name, need);
end

MENU_TAG_ID = 1000000

function menu(v)
	v.menu_type = true
	if v.inv == nil then
		v.inv = function(s)
			local r,v
			r,v = stead.call(s, 'menu');
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

function isMenu(v)
	if stead.type(v) ~= 'table' then
		return false
	end
	if v.menu_type then
		return true
	end
	return false
end

stead.fmt = function(...)
	local i, res
	local a={...}

	for i=1,stead.table.maxn(a) do
		if stead.type(a[i]) == 'string' then
			local s = stead.string.gsub(a[i],'\t', stead.space_delim):gsub('[\n]+', stead.space_delim);
			s = stead.string.gsub(s, '\\?[\\^]', { ['^'] = '\n', ['\\^'] = '^',
				['\\\\'] = '\\' });
			res = stead.par('', res, s);
		end
	end
	return res
end

game.fading = function(s)
	local rc = false
	local p = stead.call(stead.here(), 'pic');
	if stead.cmd == 'load' then
		game.lastpic = p;
		return true
	end
	if not stead.state then --only for states!!!
		return false
	end
	if game._time == 1 then -- first cmd
		game.lastpic = p;
		return true
	end
	if PLAYER_MOVED or game.lastpic ~= p then
		rc = true
	end
	game.lastpic = p
	return rc
end

game.gui.is_fading = function() --to check fading from sdl gui
	local r,g,v
	local h = stead.here()
	if not isRoom(h) then
		return false
	end
	r,v = stead.call_value(h, 'fading');
	if r then
		if stead.tonum(r) and v == nil then
			return true, stead.tonum(r)
		end
		return r, v
	end
	g,v = stead.call_value(game, 'fading');
	if stead.tonum(g) and v == nil then
		return true, stead.tonum(g)
	end
	return g, v
end

instead.get_fading = function()
	local r, v
	r, v = stead.call_value(game.gui, 'is_fading')
	if v == nil and r then v = game.gui.fading end
	return r,v
end
-- vim:ts=4
