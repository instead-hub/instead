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

obj = inherit(obj, 
function(v)
	local k,o
	local vars = {}
	if type(v.var) == 'table' then
		for k,o in pairs(v.var) do
			if tonumber(k) and type(o) == 'string' then
				stead.table.insert(vars, o)
			else
				v[k] = o
				stead.table.insert(vars, k);
			end
		end
		v.var = vars;
	end
	return v
end)
