obj = inherit(obj, function(v)
	if v.use then
		v.use = hook(v.use, function(f, s, on, ...)
			return f(s, ref(on), unpack(arg))
		end)
	end
	if v.used then
		v.used = hook(v.used, function(f, s, by, ...)
			return f(s, ref(by), unpack(arg))
		end)
	end
	return v
end)

room = inherit(room, function(v)
	if v.enter then
		v.enter = hook(v.enter, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.entered then
		v.entered = hook(v.entered, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.exit then
		v.exit = hook(v.exit, function(f, s, to, ...)
			return f(s, ref(to), unpack(arg))
		end)
	end
	if v.left then
		v.left = hook(v.left, function(f, s, to, ...)
			return f(s, ref(to), unpack(arg))
		end)
	end
	return v
end)
