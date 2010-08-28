game.hinting = true;
game.showlast = true;
game.gui = { fading = 4; }

iface.xref = function(self, str, obj, ...)
	local o = ref(obj);
	local cmd=''

	if not isObject(o) or isStatus(o) or not o.id then
		return str;
	end

	if ref(ways():srch(obj)) then
		cmd = 'go ';
	elseif isMenu(o) then
		cmd = 'act ';
	elseif isSceneUse(o) then
		cmd = 'use ';	
	end
	local a = ''
	local i
	for i = 1, stead.table.maxn(arg) do
		a = a..','..arg[i]
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

iface.nb = function(self, str)
	if str == nil then return nil; end;
	if str == '' then return ''; end
	return "<w:"..str..">";
end;

iface.under = function(self, str)
	if str == nil then return nil; end; 
	return stead.cat('<u>',str,'</u>');
end;

iface.em = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<i>',str,'</i>');
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

iface.bold = function(self, str)
	if str == nil then return nil; end;
	return stead.cat('<b>',str,'</b>');
end;

iface.inv = function(self, str)
	if str then
		str = stead.string.gsub(str, '\\'..stead.delim, '<&delim;>');
		str = stead.string.gsub(str, stead.delim, '^');
		str = stead.string.gsub(str, '<&delim;>', stead.delim);
	end
	return str
end;

iface.ways = function(self, str)
	if str then
		str = stead.string.gsub(str, '\\'..stead.delim,  '<&delim;>');
		str = stead.string.gsub(str, stead.delim, ' | ');
		str = stead.string.gsub(str, '<&delim;>', stead.delim);
		return '<c>'..str..'</c>';
	end
	return str
end;

function get_title()
	local s = call(here(), 'nam');
	if type(s) == 'string' then
		s = stead.string.gsub(s, '\\'..stead.delim, stead.delim);
	end
	return s
end

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
	if arg == nil then
		return false
	end
	for i=1,stead.table.maxn(arg) do
		if type(arg[i]) == 'string' then
			local s = stead.string.gsub(arg[i],'\t', ' '):gsub('[\n]+', ' '):gsub('%^','\n');
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
	r,v = call_bool(h, 'fading');
	if r then
		return true, v
	end
	g,v = call_bool(game, 'fading', h);
	return g and r ~= false, v
end

function get_fading()
	local r, v
	r, v = isFading()
	if v == nil then v = game.gui.fading end
	return r,v
end
-- vim:ts=4
