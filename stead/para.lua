para_mod = {
	space = '    ',
}

stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if type(r) == 'string' and stead.state then
		r = r:gsub('\n([^\n])', '<&para;>%1'):gsub('<&para;>[ \t]*', '\n'..txtnb(para_mod.space));
		r = r:gsub('^[ \t]*',txtnb(para_mod.space))
	end
	return r;
end)
