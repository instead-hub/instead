stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if game.codepage ~= 'UTF-8' then
		error("You can not use dash module with non UTF-8 encoding.");
	end
	if type(r) == 'string' then
		r = r:gsub('%-%-', '—');
	end
	return r;
end)
