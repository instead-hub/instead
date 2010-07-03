format = {
	para = false;
	para_space = '    ';
	quotes = false;
	dash = false;
	filter = nil
}

stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local utf8
	local r = f(unpack(arg))
	if game.codepage == 'UTF-8' or game.codepage == 'utf-8' then
		utf8 = true
	end
	if type(r) == 'string' then
		if type(format.filter) == 'function' and stead.state then
			r = format.filter(r);
		end
		if call_bool(format, 'dash') and utf8 then
			r = r:gsub('([^-])%-%-([^-])', '%1—%2');
			r = r:gsub('^%-%-([^-])', '—%1');
		end
		if call_bool(format, 'quotes') and utf8 then
			r = r:gsub('_"','«'):gsub('"_',"»");
			r = r:gsub('"([^"]*)"','«%1»');
		end
		if call_bool(format, 'para') and stead.state then
			r = r:gsub('\n([^\n])', '<&para;>%1'):gsub('<&para;>[ \t]*', '\n'..txtnb(format.para_space));
			r = r:gsub('^[ \t]*',txtnb(format.para_space))
		end
	end
	return r;
end)
