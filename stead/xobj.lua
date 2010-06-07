function xobj(v) -- object without name and dsc, but with xact attribute
	if v.nam == nil then
		v.nam = ''
	end
	if v.dsc == nil then
		v.dsc = ''
	end
	v.xact = v.act
	v.act = function(s)
		local r,v = call(s, 'xact');
		if type(r) == 'string' then r = do_xact(r) end
		return r,v
	end
	return obj(v)
end

xact = function(f) -- just simple action!
	local v = {};
	v = xobj(v);
	v.xact = f;
	return v
end

do_xact = function(str)
	local xrefrep = function(str)
		local s = stead.string.gsub(str,'[{}]','');
		local o = stead.string.gsub(s,'^(.*):.*$','%1');
		local d = stead.string.gsub(s,'^.*:(.*)$','%1');
		return xref(d, ref(o));
	end
	if type(str) ~= 'string' then return end
	local s = stead.string.gsub(str,'{[^}]+}', xrefrep);
	return s;
end

xdsc = function(v)
	v.nam = ''
	v.xdsc = v.dsc
	v.dsc = function(s)
		local str = call(s, 'xdsc');
		if type(str) == 'string' then
			return do_xact(str);
		end
		return str
	end
	return obj(v)
end

xdsc_obj = obj {
	nam = '',
	dsc = function(s)
		local str = call(here(), 'xdsc');
		if type(str) == 'string' then
			return do_xact(str);
		end
		return str
	end,
}

function xroom(v)
	v = room(v)
	v.obj:add('xdsc_obj', 1); -- first object is always meta-descriptor
	return v;
end
