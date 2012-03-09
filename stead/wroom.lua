function wroom_enter(self, ...)
	local r, v = stead.walk(call(self, 'where'));
	if v ~= false then
		self._toggle = true
	end
	return r, v
end

function wroom_save(self, name, h, need)
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
	local v = room { vroom_type = true, nam = a, where = c, enter = wroom_enter, save = wroom_save };
	v.newname = b;
	v.oldname = a;
	v._toggle = false
	if c == nil then -- only two parameters
		v.newname = nil
		v.where = b
	end
	v.nam = function(s)
		if s._toggle and s.newname then
			return call(s, 'newname')
		else
			return call(s, 'oldname');
		end
	end
	return v
end
