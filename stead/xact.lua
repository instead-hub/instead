xact = function(n, f) -- just simple action!
	local v = {};
	if type(n) ~= 'string' or (type(f) ~= 'string' and not isCode(f)) then
		error ("Wrong parameter to xact.", 2)
	end
	v.nam = n
	v.act = f;
	v = obj(v);
	v.save = function(self, name, h, need)
		if need then
			local f = self.xact;
			if isCode(f) then
				f = stead.string.format("code %q", stead.functions[f].code);
			else
				f = stead.string.format("%q", f);
			end
			h:write(stead.string.format("%s = xact(%q, %s);\n", name, self.nam, f))
		end
		savemembers(h, self, name, false);
	end
	return v
end

__do_xact = function(str)
	local aarg = {}
	local function parg(v)
		stead.table.insert(aarg, v);
		return ''
	end
	local xrefrep = function(str)
		local s = stead.string.gsub(str,'[{}]','');
		local o,d, a;
		local i = s:find(":", 1, true);
		aarg = {}
		if i then
			o = s:sub(1, i - 1);
			d = s:sub(i + 1);
			i = o:find("(", 1, true);
			if i then
				a = o:sub(i);
				o = o:sub(1, i - 1);
				a:gsub('[^,()]+', parg);
			end
			if o == '' then 
				error("Empty link: "..s, 3);
			end
		else
			error("Wrong link: "..s, 3);
		end
		local oo = objs():srch(o)
		if not oo then
			oo = ref(o)
		end
		return xref(d, ref(oo), unpack(aarg));
	end
	if type(str) ~= 'string' then return end
	local s = stead.string.gsub(str,'{[^}]+}', xrefrep);
	return s;
end

stead.fmt = stead.hook(stead.fmt, function(f, ...)
	local r = f(unpack(arg))
	if type(r) == 'string' and stead.state then
		r = __do_xact(r);
	end
	return r;
end)
