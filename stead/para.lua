stead.fmt = hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if type(r) == 'string' then
		r = r:gsub('~', txtnb('    '));
	end
	return r;
end)
