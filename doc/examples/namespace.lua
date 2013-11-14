namespace = function(v)
	v.old_ini = v.ini
	v.ini = function(s, load)
		local kn = s.key_name
		local NS = s
		local obs = {}
		local function call_key(k, o)
			o.key_name = k;
		end
		local function call_codekey(k, o)
			stead.functions[o].key_name = k;
		end
		local function call_ini(k, o, ...)
			v = stead.par('', v, stead.call(o, 'ini', ...));
		end
		for_each(s, kn, function(k, s)
			if s.key_name then
				return
			end
			if tonumber(k) then
				k = kn.."["..tonumber(k).."]"
			else
				if s.namespace_type then
					s.namespace_nam = kn.."."..k
				end
				k = kn.."["..stead.string.format("%q", k).."]"
			end
			if isObject(s) then
				if not load then
					stead.table.insert(obs, s)
					call_key(k, s)
					stead.check_object(k, s)
				end
				call_ini(k, s, load)
			elseif isCode(k, s) then
				if not load then
					stead.table.insert(obs, s)
					call_codeblock(k, s)
				end
			end
		end, 
		function(s)
			return isObject(s) or isCode(s)
		end)
		for k,v in ipairs(obs) do
			if not v.NS then
				v.NS = NS
			end
		end
		if type(s.old_ini) == 'function' then
			s:old_ini()
		end
	end

	v.save = function(self, name, h, need)
		if need then
			print ("Warning: namespace "..name.." can not be saved!");
			return
		end
		stead.savemembers(h, self, name, need);
	end

	local add_mode = true

	if type(v.nam) ~= 'string' and type(v[1]) ~= 'string' then
		v.nam = function(s)
			if s.namespace_nam then
				return s.namespace_nam
			end
			return 'namespace'
		end
		add_mode = false
	elseif type(v.nam) ~= 'string' then
		v.nam = v[1]
	end

	v.namespace_type = true

	local t = v

	if add_mode then
		local r, a, b

		t = stead.eval("return "..v.nam)
		if t then r, t = pcall(t) end
		if type(t) ~= 'table' then
			local f = stead.eval(v.nam..' = { }')
			if not f then
				error ("Wrong namespace name: "..v.nam, 2)
			end
			pcall(f)
		end

		t = stead.eval("return "..v.nam)
		if t then r, t = pcall(t) end
		if type(t) ~= 'table' then
			error ("Wrong namespace name: "..v.nam, 2)
		end

		for a, b in pairs(v) do
			t[a] = b
		end
	end
	t = obj(t)
	if not add_mode then
		return t
	end
end
