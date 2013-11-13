namespace = function(v)
	v.ini = function(s, load)
		local kn = s.key_name
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
			if tonumber(k) then
				k = kn.."["..tonumber(k).."]"
			else
				k = kn.."["..stead.string.format("%q", k).."]"
			end
			if isObject(s) then
				if not load then
					call_key(k, s)
					stead.check_object(k, s)
				end
				call_ini(k, s, load)
			elseif isCode(k, s) then
				if not load then
					call_codeblock(k, s)
				end
			end
		end, 
		function(s)
			return isObject(s) or isCode(s)
		end)
	end
	v.save = function(self, name, h, need)
		if need then
			print ("Warning: namespace "..name.." can not be saved!");
			return
		end
		stead.savemembers(h, self, name, need);
	end
	v.nam = 'namespace';
	v.system = true
	return obj(v)
end
