prefs = obj {
	nam = 'preferences',
	system_type = true,
	load = function(s)
		local name = get_savepath() .. '/prefs';
		local f, err = loadfile(name);
		if not f then return nil end
		f();
	end,
	ini = function(s)
		return s:load()
	end,
	store = function(s)
		local name = get_savepath() .. '/prefs';
		local h = stead.io.open(name,"w");
		if not h then return false end
		savemembers(h, s, 'prefs', true);
		h:flush();
		h:close();
	end,
	save = function(s)
	end,
	purge = function(s)
		local name = get_savepath() .. '/prefs';
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
