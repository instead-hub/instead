stead.wroom_enter = function(self, ...)
	local w = self.where
	if stead.type(w) ~= 'table' then
		w = stead.call(self, 'where')
	end
	local r, v = stead.walk(w);
	if v ~= false then
		self._toggle = true
	end
	return r, v
end

stead.wroom_save = function(self, name, h, need)
	if need then
		local a = stead.tostring(self.oldname);
		local b = stead.tostring(self.newname);
		local c = stead.tostring(self.where);

		if a == nil or b == nil or c == nil then
			error ("Can not save wroom "..name.."\nFunctions can not be saved, use code [[ ]]");
		end
		local t = stead.string.format("%s = wroom(%s, %s, %s);\n",
			name, a, b, c);
		h:write(t);
	end
	stead.savemembers(h, self, name, false);
end

function wroom(a, b, c)
	local v = room { vroom_type = true, nam = a, where = c, enter = stead.wroom_enter, save = stead.wroom_save };
	v.newname = b;
	v.oldname = a;
	v._toggle = false
	if c == nil then -- only two parameters
		v.newname = nil
		v.where = b
	end
	v.nam = function(s)
		if s._toggle and s.newname then
			return stead.call(s, 'newname')
		else
			return stead.call(s, 'oldname');
		end
	end
	return v
end
