local std = stead

local preferences = std.obj {
	nam = '@prefs',
	load = function(s)
		local name = std.savepath() .. '/prefs';
		local f, err = std.loadfile(name);
		if not f then
			return false, err
		end
		local g = std.game
		std.game = nil -- to make vars rw
		f();
		std.game = g
		return true
	end,
	store = function(s)
		return s:save()
	end,
	save = function(s) -- save prefs on every save
		local name = std.savepath() .. '/prefs';
		local name_tmp = name..'.tmp'
		local fp, err = std.io.open(name_tmp, "wb");
		if not fp then
			return false
		end
		std.obj.save(s, fp, 'std "@prefs"')
		fp:flush();
		fp:close();
		std.os.remove(name)
		return std.os.rename(name_tmp, name);
	end,
	purge = function(s)
		local name = stead.savepath() .. '/prefs';
		return std.os.remove(name);
	end
};

local loaded

std.mod_start(function()
	loaded = prefs:load()
end)

std.mod_done(function()
	if loaded then
		prefs:store()
		loaded = false
	end
end)

prefs = preferences
