isForSave = hook(isForSave, 
function (f, k, v, s, ...) -- k - key, v - value, s -- parent table
	local i,v
	if type(s.var) == 'table' then
		for i,v in ipairs(s.var) do
			if v == k then
				return true
			end
		end
	end
	return f(k, v, s, unpack(arg))
end)

function __vars_fill(v)
	local k,o
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
	end
}

obj = inherit(obj, 
function(v)
	__vars_fill(v)
	return v
end)
