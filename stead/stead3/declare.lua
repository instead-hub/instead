local declarations = {}
local variables = {}
local std = stead

local type = std.type
local rawget = std.rawget
local rawset = std.rawset
local pairs = std.pairs
local table = std.table
local next = std.next

local function __declare_one(k, v, t)
	local link

	if type(k) ~= 'string' then -- k:find("^[a-zA-Z_][a-zA-Z0-9_]*$") then
		std.err ("Wrong declaration name: "..k, 3)
	end
	if declarations[k] then
		std.err ("Duplicate declaration: "..k, 3)
	end

	link = rawget(_G, k)

	declarations[k] = { value = v, type = t, link = link }

	if link and v ~= link then
		std.err("Overwite global declaration: "..std.tostr(k), 3)
	end

	if t == 'global' then
		if type(v) == 'function' and not std.functions[v] then
			std.err("Use declare to declare function: "..k, 3)
		end
		rawset(_G, k, v)
		variables[k] = true
	end
	if type(v) == 'function' and not std.functions[v] then
		std.functions[v] = k
	end
	if std.__in_init and std.is_obj(v, 'list') then
		table.insert(v.__in_init, v)
	end
end

local function __declare(n, t)
	if stead.game then
		std.err ("Use "..t.." only in global context", 2)
	end
	if type(n) == 'string' then
		return function(v)
			if v == nil then
				v = false
--				std.err("Wrong declaration: "..std.tostr(t), 2)
			end
			__declare_one(n, v, t)
		end
	end
	if type(n) ~= 'table' then
		std.err ("Wrong parameter to "..n, 2)
	end
	for k, v in std.pairs(n) do
		__declare_one(k, v, t)
	end
	return n
end

function std.const(n)
	return __declare(n, 'const')
end

function std.global(n)
	return __declare(n, 'global')

end

function std.declare(n)
	return __declare(n, 'declare')
end

local function depends(t, tables, deps)
	if type(t) ~= 'table' then return end
	if tables[t] then
		deps[t] = tables[t]
	end
	for k, v in pairs(t) do
		if type(v) == 'table' and not std.getmt(v) then
			depends(v, tables, deps)
		end
	end
end

local function makedeps(nam, depends, deps)
	local ndeps = {}
	local rc = false

	local t = rawget(_G, nam)
	if type(t) ~= 'table' then
		return
	end
	if type(depends[nam]) ~= 'table' then
		return
	end
	local d = depends[nam]
	for k, v in pairs(d) do
		local dd = depends[v]
		if dd and k ~= t then
			ndeps[k] = v
			rc = rc or makedeps(v, depends, deps)
		end
	end
	if not next(ndeps) then
		depends[nam] = nil
		table.insert(deps, t)
		rc = true
	else
		depends[nam] = ndeps
	end
	return rc
end

local function mod_save(fp)
	-- save global variables
	std.tables = {}
	local tables = {}
	local deps = {}
	for k, v in pairs(declarations) do -- name all table variables
		local o = rawget(_G, k) or v.value
		if type(o) == 'table' then
			if not tables[o] then
				tables[o] = k
			end
		end
	end

	for k, v in pairs(variables) do
		local d = {}
		local o = rawget(_G, k)
		depends(o, tables, d)
		if k == tables[o] then -- self depend
			d[o] = nil
		end
		if next(d) then
			deps[k] = d
		end
	end

	std.tables = tables -- save all depends

	for k, v in pairs(variables) do -- write w/o deps
		local o = rawget(_G, k)
		if not deps[k] then
			std.save_var(o, fp, k)
		end
	end
	for k, v in pairs(variables) do
		local d = {}
		while makedeps(k, deps, d) do
			for i=1, #d do
				std.save_var(d[i], fp, k)
			end
			d = {}
		end
	end
end

local function mod_init()
	std.setmt(_G, {
	__index = function(_, n)
		local d = declarations[n]
		if d then --
			if std.game and (d.type ~= 'const') then
				rawset(_, n, d.value)
			end
			return d.value
		end
		local f = std.getinfo(2, "S").source
		std.err ("Uninitialized global variable: "..n.." in "..f, 2)
	end;
	__newindex = function(t, k, v)
		if not std.game and std.is_obj(v) then -- autodeclare objects
			__declare_one(k, v, 'declare')
		end
		local d = declarations[k]
		if d then
			if v == d.value then
				return --nothing todo
			end
			if not std.game then
				d.value = v
				return
			end
			if d.type == 'const' then
				std.err ("Modify read-only constant: "..k, 2)
			else
				d.value = v
				rawset(t, k, v)
			end
			return
		end
		if std.game or type(v) ~= 'function' then
			local f = std.getinfo(2, "S").source
			if f ~= '=[C]' then
				std.err ("Set uninitialized variable: "..k.." in "..f, 2)
			end
		end
		rawset(t, k, v)
	end
	})
end

std.obj {
	nam = '@declare';
	ini = function(s, ...)
-- init all list objs
		for k, v in pairs(declarations) do
			if std.is_obj(v.value, 'list') then
				v.value:__ini(...)
			end
		end
	end;
	declarations = function()
		return declarations;
	end;
	variables = function()
		return variables
	end;
}

local function mod_done()
	std.setmt(_G, {})
	local decl = {}
	for k, v in pairs(declarations) do
		local o = std.rawget(_G, k) or v.value
		if std.is_system(o) then -- save system declarations
			decl[k] = v
		elseif not v.link then
			rawset(_G, k, nil)
		end
	end
	std.tables = {}
	std.functions = {}
	declarations = decl
	variables = {}
end

std.mod_init(mod_init)
std.mod_done(mod_done)
std.mod_save(mod_save)

const = std.const
global = std.global
declare = std.declare
