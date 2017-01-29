local G = {
}

local function variables(g)
	if g == _G then
		return G.variables
	end
	if type(g) == 'table' then
		return g.variables
	end
end

local function variables_save(g)
	if g == _G then
		return G.variables_save
	end
	if type(g) == 'table' then
		return g.variables_save
	end
end

function isForSave(k, v, s) -- k - key, v - value, s -- parent table
	local i,o
	if stead.type(variables_save(s)) == 'table' and
		variables_save(s)[k] then
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
	return stead.string.find(k, '_') == 1
end

local function __vars_add(s, v, set)
	if not variables(s) then s.variables = {} end
	for k, o in stead.pairs(v) do
		if stead.tonum(k) then
			stead.table.insert(variables(s), o);
		elseif variables(s)[k] then
			error ("Variable overwrites variables object: "..stead.tostr(k))
		elseif k ~= 'variable_type' then
			if set and not isObject(o) then 
				if stead.rawget(s, k) then
					if s == _G then
						print ("Global variable '"..stead.tostr(k).."' conflicts with "..stead.type(stead.rawget(s, k)));
					else
						error ("Variable conflict: "..stead.tostr(k));
					end
				end
				stead.table.insert(variables(s), k);
				stead.rawset(s, k, o)
			else
				variables(s)[k] = o
			end
		end
	end
end

local function __vars_fill(v)
	if stead.type(v) ~= 'table' then
		return
	end
	for k, o in stead.ipairs(v) do
		if stead.type(o) == 'table' and o.variable_type then
			__vars_add(v, o);
			stead.rawset(v, k, false)
		end
	end
	if stead.type(variables(v)) == 'table' then
		local vars = {}
		if v == _G then
			G.variables_save = {}
		else
			v.variables_save = {}
		end
		for k, o in stead.pairs(variables(v)) do
			if stead.tonum(k) and stead.type(o) == 'string' then
				stead.table.insert(vars, o)
			else
				if stead.rawget(v, k) then
					error ("Variable overwrites object property: "..stead.tostr(k));
				end
				stead.rawset(v, k, o)
				stead.table.insert(vars, k);
			end
		end
		for k, o in stead.ipairs(vars) do
			variables_save(v)[o] = true
		end
		if v == _G then
			G.variables = vars;
		else
			v.variables = vars;
		end
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
local function mod_init()
	if stead.type(variables(_G)) == 'table' then
		for k, v in stead.ipairs(variables(_G)) do
			stead.rawset(_G, v, nil)
		end
	end
	G.variables = {}
	G.variables_save = {}
	if not stead.api_atleast(2, 5, 0) then -- fake data
		stead.rawset(_G, 'variables_save', {}) -- to save broken saves
		stead.rawset(_G, 'variables', {})
	end
end
stead.module_init(mod_init)
stead.module_done(mod_init)

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
-- require 'strict'
-- vim:ts=4
