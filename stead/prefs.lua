prefs = obj {
	nam = 'preferences',
	system_type = true,
	ini = function(s)
		local name = get_savepath() .. '/prefs';
		local f, err = loadfile(name);
		if not f then return nil end
		f();
	end,
	store = function(s)
		local name = get_savepath() .. '/prefs';
		local h = stead.io.open(name,"w");
		if not h then return false end
		savemembers(h, s, 'prefs', true);
		h:flush();
		h:close();
	end,
	purge = function(s)
		local name = get_savepath() .. '/prefs';
		return stead.os.remove(name);
	end
};

-- vim:ts=4
