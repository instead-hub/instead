xact = function(n, f) -- just simple action!
	local v = {};

	if f == nil and type(n) == 'table' then
		f = n[2];
		n = n[1];
	end

	if type(n) ~= 'string' then
		error ("Wrong parameter to xact.", 2)
	end
	v.xaction_type = true
	v.nam = n
	v.act = f;
	v = obj(v);
	v.save = function(self, name, h, need)
		if need then
			local f = self.act;
			f = stead.tostring(f);
			if f == nil then
				error("Can not save xact: "..name);
			end
			h:write(stead.string.format("%s = xact(%q, %s);\n", name, self.nam, f))
		end
		savemembers(h, self, name, false);
	end
	return v
end

__do_xact = function(str, self)
	local aarg = {}
	local function parg(v)
		stead.table.insert(aarg, v);
		return ''
	end
	local xrefrep = function(str)
		local s = stead.string.gsub(str,'[{}]','');
		local o,d,a, oo;
		local delim = ':'

		if stead.api_version >= "1.2.2" then
			delim = stead.delim;
		end
		s = s:gsub("\\"..delim, "<&delim;>");
		local i = s:find(delim, 1, true);
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
				if isObject(self) then
					oo = self
				else
					error("Empty link: "..s, 3);
				end
			else
				oo = objs():srch(o)
				if not oo then
					oo = ref(o, true)
				end
			end
		elseif isObject(self) then
			oo = self
			d = s;
		else
			error("Wrong link: "..s, 3);
		end
		d = d:gsub("<&delim;>", delim);
		return xref(d, ref(oo, true), unpack(aarg));
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

obj = stead.inherit(obj, function(v)
	v.xref = function(s, str)
		return __do_xact(str, s);
	end
	return v
end)

function xdsc(n)
	local v = {}
	v.nam = true
	if n == nil then
		v.disp = 'xdsc'
	elseif type(n) == 'string' then
		v.disp = n;
	else
		error("Wrong parameter to xdsc.", 2);
	end
	v.dsc = function(s)
		return call(here(), s.disp);
	end
	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format("%s = xdsc(%q);\n", name, self.disp))
		end
		savemembers(h, self, name, false);
	end
	return obj(v)
end

xroom = stead.inherit(room, function(v)
	v.look = stead.hook(v.look, function(f, s,...)
		local xdsc = call(s, 'xdsc');
		return par(' ', xdsc, f(s, unpack(arg)));
	end)
	return v
end)
-- vim:ts=4
