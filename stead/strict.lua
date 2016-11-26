setmetatable(_G, {
	__index = function(_, n)
		local f = stead.getinfo(2, "S").source
		if f:byte(1) == 0x40 and not f:find("/stead/", 1, true) then
			print ("Uninitialized global variable: "..n.." in: "..f:sub(2))
		end
	end;
	__newindex = function(t, k, v)
		local f = stead.getinfo(2, "S").source
		if f:byte(1) == 0x40 and not f:find("/stead/", 1, true) and not isObject(v) then
			print ("Set uninitialized variable: "..k.." in: "..f:sub(2))
		end
		rawset(t, k, v)
	end
})
