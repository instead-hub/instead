if stead.api_version >= "1.6.3" then
	game.gui.hideinv = function(s)
		if stead.call_bool(game, 'hideinv') then
			return true
		end
		if stead.call_bool(here(), 'hideinv') then
			return true
		end
	end
else
	room = stead.inherit(room, function(v)
		v.entered = stead.hook(v.entered, function(f, s, ...)
			if stead.call_bool(s, 'hideinv') then
				me():disable_all();
			end
			return f(s, ...)
		end)
		v.left = stead.hook(v.left, function(f, s, ...)
			if stead.call_bool(s, 'hideinv') then
				me():enable_all();
			end
			return f(s, ...)
		end)
		return v
	end)
end
