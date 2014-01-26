function isForSave(k, v, s) -- k - key, v - value, s -- parent table
	local i,o
	if stead.type(s.variables_save) == 'table' and 
		s.variables_save[k] then
			return true
	end
	if stead.type(k) == 'function' then
		return false
	end
	if stead.type(v) == 'function' or stead.type(v) == 'userdata' then
		return false
	end
	if stead.type(k) ~= 'string' then
		return false
	end
	return stead.string.find(k, '_') ==  1
end

local function __vars_add(s, v, set)
	local k, o
	if stead.type(s.variables) ~= 'table' then s.variables = {} end
	for k,o in stead.pairs(v) do
		if stead.tonum(k) then
			stead.table.insert(s.variables, o);
		elseif s.variables[k] then
			error ("Variable overwrites variables object: "..stead.tostr(k))
		elseif k ~= 'variable_type' then
			if set and not isObject(o) then 
				if s[k] then
					if s == _G then
						print ("Global variable '"..stead.tostr(k).."' conflicts with "..stead.type(s[k]));
					else
						error ("Variable conflict: "..stead.tostr(k));
					end
				end
				stead.table.insert(s.variables, k);
				s[k] = o
			else
				s.variables[k] = o
			end
		end
	end
end

local function __vars_fill(v)
	local k,o
	if stead.type(v) ~= 'table' then
		return
	end
	for k,o in stead.ipairs(v) do
		if stead.type(o) == 'table' and o.variable_type then
			__vars_add(v, o);
			v[k] = false
		end
	end
	if stead.type(v.variables) == 'table' then
		local k,o
		local vars = {}
		v.variables_save = {}
		for k,o in stead.pairs(v.variables) do
			if stead.tonum(k) and stead.type(o) == 'string' then
				stead.table.insert(vars, o)
			else
				if v[k] then
					error ("Variable overwrites object property: "..stead.tostr(k));
				end
				v[k] = o
				stead.table.insert(vars, k);
			end
		end
		for k,o in stead.ipairs(vars) do
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

stead.add_var = function(s, v) 
	if not v then 
		v = s 
		s = _G 
	end
	if stead.type(v) ~= 'table' then
		error("Wrong parameter to stead.add_var.");
	end
	if not v.variable_type then
		v = var(v)
	end
	__vars_add(s, v, true)
	__vars_fill(s)
end

stead.module_init(function()
	local k,v
	if stead.type(variables) == 'nil' then
		variables = {}
		return
	end 
	if stead.type(variables) ~= 'table' then
		return
	end
	for k,v in stead.ipairs(variables) do
		_G[v] = nil
	end
	variables = {}
end)

function var(v)
	v.variable_type = true
	return v
end

function global(v)
	if stead.type(v) ~= 'table' then
		error("Wrong parameter to global.", 2);
	end
	__vars_add(_G, v, true);
end
-- vim:ts=4
