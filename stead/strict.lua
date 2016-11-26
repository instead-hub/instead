function declare(n)
	if type(n) == 'string' then
		stead.rawset(_G, n, false)
		return
	end
	if type(n) == 'table' then
		local k, v
		for k, v in stead.pairs(n) do
			stead.rawset(_G, k, v)
		end
	end
	error ("Wrong parameter to declare", 2)
end

setmetatable(_G, {
	__index = function(_, n)
		local f = stead.getinfo(2, "S").source
		if f:byte(1) == 0x40 and not f:find("/stead/", 1, true) then
			error ("Uninitialized global variable: "..n.." in: "..f:sub(2), 2)
		end
	end;
	__newindex = function(t, k, v)
		local f = stead.getinfo(2, "S").source
		if f:byte(1) == 0x40 and not f:find("/stead/", 1, true) and type(v) ~= 'function' and not isObject(v) then
			error ("Set uninitialized variable: "..k.." in: "..f:sub(2), 2)
		end
		stead.rawset(t, k, v)
	end
})
