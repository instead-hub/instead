game.hinting = true;
game.showlast = true;

iface.xref = function(self, str, obj)
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

	return cat('<a:'..cmd..'0'..tostring(o.id)..'>',str,'</a>');
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
	return "<w:"..str..">";
end;

iface.under = function(self, str)
	if str == nil then return nil; end; 
	return cat('<u>',str,'</u>');
end;

iface.em = function(self, str)
	if str == nil then return nil; end;
	return cat('<i>',str,'</i>');
end;

iface.right = function(self, str)
	if str == nil then return nil; end; 
	return cat('<r>',str,'</r>');
end;

iface.left = function(self, str)
	if str == nil then return nil; end;
	return cat('<l>',str,'</l>');
end;

iface.center = function(self, str)
	if str == nil then return nil; end;
	return cat('<c>',str,'</c>');
end;

iface.bold = function(self, str)
	if str == nil then return nil; end;
	return cat('<b>',str,'</b>');
end;

iface.inv = function(self, str)
	if str then
		str = stead.string.gsub(str,'([^\\]),','%1^');
		str = stead.string.gsub(str,'\\,',',');
	end
	return str
end;

iface.ways = function(self, str)
	if str then
		str = stead.string.gsub(str,'([^\\]),','%1 | ');
		str = stead.string.gsub(str,'\\,',',');
		return '<c>'..str..'</c>';
	end
	return str
end;

function get_title()
	local s = call(here(), 'nam');
	return stead.string.gsub(s, '\\,',',');
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
		h:write(name.." = menu {nam = '"..tostring(self.nam).."'}\n");
	end
	savemembers(h, self, name, need);
end

MENU_TAG_ID = 10000

function menu(v)
	v.menu_type = true
	if v.inv == nil then
		v.inv = function(s)
			local r
			r = call(s, 'menu');
			if r == nil then
				obj_tag(me(), MENU_TAG_ID); -- retag menu field
			end
			return r, true
		end
	end
	if v.act == nil then
		v.act = function(s)
			local r
			r = call(s, 'menu');
			if r == nil then
				obj_tag(me(), MENU_TAG_ID); -- retag menu field
			end
			return r, true
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

