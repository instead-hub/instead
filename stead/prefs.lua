prefs = obj {
	nam = 'preferences',
	system_type = true,
	load = function(s)
		local name = instead_savepath() .. '/prefs';
		local f, err = loadfile(name);
		if not f then return nil end
		f();
	end,
	ini = function(s)
		return s:load()
	end,
	store = function(s)
		s:save()
		stead.clearvar(s);
	end,
	save = function(s) -- save prefs on every save
		local name = instead_savepath() .. '/prefs';
		local h = stead.io.open(name,"wb");
		if not h then return false end
		stead.savemembers(h, s, 'prefs', true);
		h:flush();
		h:close();
	end,
	purge = function(s)
		local name = instead_savepath() .. '/prefs';
		local k,v
		for k,v in pairs(s) do
			if type(v) ~= 'function' and k ~= 'nam' and k ~= 'system_type' then
				s[k] = nil
			end
		end
		return stead.os.remove(name);
	end
};

-- vim:ts=4
