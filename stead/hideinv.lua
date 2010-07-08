room = stead.inherit(room, function(v)
	v.entered = stead.hook(v.entered, function(f, s, ...)
		if call_bool(s, 'hideinv') then
			me():disable_all();
		end
		return f(s, unpack(arg))
	end)
	v.left = stead.hook(v.left, function(f, s, ...)
		if call_bool(s, 'hideinv') then
			me():enable_all();
		end
		return f(s, unpack(arg))
	end)
	return v
end)
