function isXobject(s)
	return type(s) == 'table' and s.xobject_type;
end

__hook_act = function(v, n)
	local k,o
	if type(v.var) == 'table' then
		for k,o in ipairs(v.var) do -- var add
			if o == n then
				stead.table.insert(v.var, 'x'..n);
				break
			end
		end
	end
	return function(s, ...)
		local r, v = call(s, 'x'..n, unpack(arg));
		if type(r) == 'string' then
			r = do_xact(s, r);
		end
		return r,v
	end
end

xobj = stead.inherit(obj, function(v) -- object without name and dsc, but with xact attribute
	v.xdsc = v.dsc
	v.dsc = __hook_act(v, 'dsc');
	v.xact = v.act
	v.act = __hook_act(v, 'act');
	v.xinv = v.inv
	v.inv = __hook_act(v, 'inv');
	v.xtak = v.tak
	v.tak = __hook_act(v, 'tak');
	v.xuse = v.use
	v.use = __hook_act(v, 'use');
	v.xused = v.used
	v.used = __hook_act(v, 'used');
	v.xobject_type = true
	return v
end)

xact = function(n, f) -- just simple action!
	local v = {};
	if type(n) ~= 'string' or (type(f) ~= 'string' and not isCode(f)) then
		error ("Wrong parameter to xact.", 2)
	end
	v.nam = n
	v.act = f;
	v = xobj(v);
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

do_xact = function(self, str)
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
			if o == '' then o = self end
		else
			o = self
			d = s;
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

xroom = stead.inherit(room, function(v)
	v.xdsc = v.dsc
	v.dsc = __hook_act(v, 'dsc');
	return v;
end)
