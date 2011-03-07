game.hinting = true;
game.showlast = true;

game.gui = { 
	fading = 4; 
	ways_delim = ' | ';
	inv_delim = '\n';
	hinv_delim = ' | ';
}

iface.xref = function(self, str, obj, ...)
	local o = ref(obj);
	local cmd=''

	if not isObject(o) or isStatus(o) or (not o.id and not isXaction(o)) then
		return str;
	end

	if ref(ways():srch(obj)) then
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
		return stead.cat('<a:'..cmd..deref(obj)..a..'>',str,'</a>');
	end
	return stead.cat('<a:'..cmd..'0'..tostring(o.id)..a..'>',str,'</a>');
end;

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
	if type(str) ~= 'string' then return nil end
	return "<w:"..str:gsub(">","\\>"):gsub("%^","\\%^")..">";
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
	if tonumber(str) then
		str = tostring(str)
	end
	if type(str) ~= 'string' then
		return nil;
	end
	if al == 'right' then
		str = str .. ",right"
	elseif al == 'center' then
		str = str .. ",center"
	end
	return '<x:'..str..'>'
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

function get_inv(horiz)
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
instead.get_inv = get_inv;

function get_ways()
	local str = iface:cmd("way");
	if str and str ~= '' then
		str = stead.string.gsub(str, '\n$','');
		str = stead.string.gsub(str, '\\?['..stead.delim ..']', 
			{ [stead.delim] = game.gui.ways_delim, [ '\\'..stead.delim ] = stead.delim });
		return iface:center(str);
	end
	return str
end
instead.get_ways = get_ways;

function get_title()
	local s
	if stead.api_version >= "1.2.0" then
		s = call(here(), 'disp');
	end
	if type(s) ~= 'string' then
		s = call(here(), 'nam');
	end
	if type(s) == 'string' and s ~= '' then
		stead.state = false
		s = "<c><b>"..stead.fmt(s).."</b></c>";
		s = stead.string.gsub(s, '\\'..stead.delim, stead.delim);
	end
	return s
end

instead.get_title = get_title;
instead.get_picture = get_picture;
instead.get_music = get_music;
instead.get_sound = get_sound;
instead.set_sound = set_sound;
instead.get_autosave = get_autosave;
instead.get_music_loop = get_music_loop;
instead.dec_music_loop = dec_music_loop;

instead.isEnableSave = isEnableSave;
instead.isEnableAutosave = isEnableAutosave;
instead.autosave = autosave;


-- here is gui staff only
function stat(v)
	v.status_type = true
	return obj(v);
end

function isStatus(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.status_type then
		return true
	end
	return false
end

function menu_save(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: menu "..name.." can not be saved!");
		return
	end
	savemembers(h, self, name, need);
end

MENU_TAG_ID = 10000

function menu(v)
	v.menu_type = true
	if v.inv == nil then
		v.inv = function(s)
			local r,v
			r,v = call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				obj_tag(me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.act == nil then
		v.act = function(s)
			local r,v
			r,v = call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				obj_tag(me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.save == nil then
		v.save = menu_save;
	end
	return obj(v);
end

function isMenu(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.menu_type then
		return true
	end
	return false
end

fmt = function(...)
	local i, res
	local a={...}

	for i=1,stead.table.maxn(a) do
		if type(a[i]) == 'string' then
			local s = stead.string.gsub(a[i],'\t', ' '):gsub('[\n]+', ' ');
			s = stead.string.gsub(s, '\\?[\\^]', { ['^'] = '\n', ['\\^'] = '^',
				['\\\\'] = '\\' });
			res = stead.par('', res, s);
		end
	end
	return res
end

stead.fmt = fmt

game.fading = function(s)
	local rc = false
	local p = call(here(), 'pic');
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

function isFading() --to check fading from sdl gui
	local r,g,v
	local h = here()
	if not isRoom(h) then
		return false
	end
	r,v = call_value(h, 'fading');
	if r then
		if tonumber(r) and v == nil then
			return true, tonumber(r)
		end
		return true, v
	end
	g,v = call_value(game, 'fading', h);
	if tonumber(g) and v == nil then
		v = tonumber(g)	
		g = true
	end
	return g and r ~= false, v
end

instead.get_fading = function()
	local r, v
	r, v = isFading()
	if v == nil then v = game.gui.fading end
	return r,v
end
-- vim:ts=4
