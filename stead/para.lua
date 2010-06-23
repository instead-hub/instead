para_mod = obj {
	nam = 'para',
	ini = function(s)
		s.time = stead.time()
	end,
	space = '    ',
}

stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if type(r) == 'string' and stead.time() > para_mod.time then
		para_mod.time = stead.time()
		r = r:gsub('\n([^\n])', '<&para;>%1'):gsub('<&para;>[ \t]*', '\n'..txtnb(para_mod.space));
		r = r:gsub('^',txtnb(para_mod.space))
	end
	return r;
end)
