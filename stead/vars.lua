function isForSave(k, v, s) -- k - key, v - value, s -- parent table
	local i,o
	if type(s.variables_save) == 'table' and 
		s.variables_save[k] then
			return true
	end
	if type(k) == 'function' then
		return false
	end
	if type(v) == 'function' or type(v) == 'userdata' then
		return false
	end
	return stead.string.find(k, '_') ==  1
end

function __vars_add(s, v, set)
	local k, o
	for k,o in pairs(v) do
		if tonumber(k) then
			stead.table.insert(s.variables, o);
		elseif s.variables[k] then
			error ("Variable overwrites variables object: "..tostring(k))
		elseif k ~= 'variable_type' then
			if set and not isObject(o) then 
				if s[k] then
					error ("Global variable conflict: "..tostring(k));
				end
				stead.table.insert(s.variables, k);
				s[k] = o
			else
				s.variables[k] = o
			end
		end
	end
end

function __vars_fill(v)
	local k,o
	if type(v) ~= 'table' then
		return
	end
	for k,o in ipairs(v) do
		if type(o) == 'table' and o.variable_type then
			if type(v.variables) ~= 'table' then v.variables = {} end
			__vars_add(v, o);
			v[k] = nil
		end
	end
	if type(v.variables) == 'table' then
		local k,o
		local vars = {}
		v.variables_save = {}
		for k,o in pairs(v.variables) do
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
		for k,o in ipairs(vars) do
			v.variables_save[o] = true
		end
		v.variables = vars;
	end
end

vars_object = obj {
	nam = 'vars',
	system_type = true,
	ini = function(s)
		__vars_fill(_G)
		__vars_fill(pl)
		__vars_fill(game)
	end
}

obj = stead.hook(obj, 
function(f, v, ...)
	__vars_fill(v)
	return f(v, ...)
end)

stead.module_init(function()
	local k,v
	if type(variables) == 'nil' then
		variables = {}
		return
	end 
	if type(variables) ~= 'table' then
		return
	end
	for k,v in ipairs(variables) do
		_G[v] = nil
	end
	variables = {}
end)

function var(v)
	v.variable_type = true
	return v
end

function global(v)
	if type(v) ~= 'table' then
		error("Wrong parameter to global.", 2);
	end
	__vars_add(_G, v, true);
end
-- vim:ts=4
