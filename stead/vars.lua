function isForSave(k, v, s) -- k - key, v - value, s -- parent table
	local i,o
	if type(s.var) == 'table' then
		for i,o in ipairs(s.var) do
			if o == k then
				return true
			end
		end
	end
	if type(k) == 'function' then
		return false
	end
	if type(v) == 'function' or type(v) == 'userdata' then
		return false
	end
	return stead.string.find(k, '_') ==  1
end

function __vars_fill(v)
	local k,o
	if type(v) ~= 'table' then
		return
	end
	if type(v.var) == 'table' then
		local k,o
		local vars = {}
		for k,o in pairs(v.var) do
			if tonumber(k) and type(o) == 'string' then
				stead.table.insert(vars, o)
			else
				if v[k] then
					error ("Variable overwrites object property: "..tostring(k));
				end
				v[k] = o
				stead.table.insert(vars, k);
			end
		end
		v.var = vars;
	end
end

vars_object = obj {
	nam = 'vars',
	ini = function(s)
		__vars_fill(_G)
		__vars_fill(pl)
		__vars_fill(game)
	end
}

obj = stead.hook(obj, 
function(f, v, ...)
	__vars_fill(v)
	return f(v, unpack(arg))
end)
