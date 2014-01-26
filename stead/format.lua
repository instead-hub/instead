format = {
	nam = 'format';
	object_type = true;
	system_type = true;
	para = false;
	para_space = '    ';
	quotes = false;
	dash = false;
	filter = nil;
	nopara = '_';
	save = function(self, name, h, need)
		local k,v
		for k,v in stead.pairs(self) do
			if k == 'para' or k == 'para_space' or k == 'quotes' or
			    k == 'dash' or k == 'nopara' then
				local s = stead.tostring(v)
				h:write(stead.string.format("format[%q] = %s;\n", k, s))
			end
		end
	end;
}

stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local utf8
	local r = f(...)
	if game.codepage == 'UTF-8' or game.codepage == 'utf-8' then
		utf8 = true
	end
	if stead.type(r) == 'string' and stead.state then
		if stead.type(format.filter) == 'function' and stead.state then
			r = format.filter(r);
		end
		if stead.call_bool(format, 'dash') and utf8 then
			r = r:gsub('([^-])%-%-([^-])', '%1—%2');
			r = r:gsub('^%-%-([^-])', '—%1');
		end
		if stead.call_bool(format, 'quotes') and utf8 then
			r = r:gsub('_"','«'):gsub('"_',"»");
			r = r:gsub('"([^"]*)"','«%1»');
			r = r:gsub(',,','„'):gsub("''",'”');
		end
		if stead.call_bool(format, 'para') then
			r = r:gsub('\n([^\n])', '<&para;>%1'):gsub('<&para;>[ \t]*'..format.nopara,'\n'):gsub('<&para;>[ \t]*', '\n'..txtnb(format.para_space));
			r = r:gsub('^[ \t]*', '<&para;>'):gsub('<&para;>[ \t]*'..format.nopara,''):gsub('<&para;>[ \t]*', txtnb(format.para_space));
		end
	end
	return r;
end)

-- vim:ts=4
