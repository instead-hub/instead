local attr = "hideinv"
if stead.api_atleast(1, 6, 3) then
	game.gui.hideinv = function(s)
		if stead.call_bool(stead.here(), 'hideinv') then
			return true
		end
	end
	attr = "noinv"
end
room = stead.inherit(room, function(v)
	v.entered = stead.hook(v.entered, function(f, s, ...)
		if stead.call_bool(s, attr) then
			me():disable_all();
		end
		return f(s, ...)
	end)
	v.left = stead.hook(v.left, function(f, s, ...)
		if stead.call_bool(s, attr) then
			me():enable_all();
		end
		return f(s, ...)
	end)
	return v
end)
