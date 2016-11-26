local declarations = {}

function declare(n)
	if type(n) == 'string' then
		declarations[n] = true
		return
	end
	if type(n) == 'table' then
		local k, v
		for k, v in stead.pairs(n) do
			declarations[k] = true
			stead.rawset(_G, k, v)
		end
	end
	error ("Wrong parameter to declare", 2)
end

if RELEASE then
	return
end

setmetatable(_G, {
	__index = function(_, n)
		if declarations[n] then
			return
		end
		local f = stead.getinfo(2, "S").source
		if f:find("/stead/", 1, true) then
			print ("Uninitialized global variable: "..n.." in: "..f)
		else
			error ("Uninitialized global variable: "..n.." in: "..f, 2)
		end
	end;
	__newindex = function(t, k, v)
		if not declarations[k] and type(v) ~= 'function' and not isObject(v) then
			local f = stead.getinfo(2, "S").source
			if f:find("/stead/", 1, true) then
				print ("Set uninitialized variable: "..k.." in: "..f)
			else
				error ("Set uninitialized variable: "..k.." in: "..f, 2)
			end
		end
		stead.rawset(t, k, v)
	end
})
