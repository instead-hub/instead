dlg = stead.inherit(dlg, function(v)
	v.entered = stead.hook(v.entered, function(f, ...)
		local r,v = f(unpack(arg))
		me():disable_all();
		return r,v
	end)
	v.left = stead.hook(v.left, function(f, ...)
		me():enable_all();
		return f(unpack(arg))
	end)
	return v
end)
