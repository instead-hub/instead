stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if type(r) == 'string' and stead.tick then
		r = r:gsub('\n([^\n])', '<&para;>%1'):gsub('<&para;>', '\n'..txtnb('    '));
		r = r:gsub('^',txtnb('    '))
	end
	return r;
end)
