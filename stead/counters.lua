if stead.api_version < "1.6.3" then
	error ("Counters module can not run with api version < 1.6.3", 3)
end

local function inc_nr(v, n)
	local name = '__'..n..'_nr'
	if not v[name] then
		v[name] = 0
	end
	v[name] = v[name] + 1
end

local function read_nr(v, n, set)
	local name = '__'..n..'_nr'
	v = stead.ref(v)
	if type(v) ~= 'table' then
		return v
	end
	if not v[name] then
		if type(set) == 'number' then v[name] = set end
		return 0
	end
	n = v[name]
	if type(set) == 'number' then v[name] = set end
	return n
end

game.onact = stead.hook(game.onact, function(f, v, w, ...)
	inc_nr(v, 'act');
	inc_nr(w, 'act');
	return f(v, w, ...)
end)

game.onuse = stead.hook(game.onuse, 
function(f, v, w, ...)
	inc_nr(v, 'use');
	inc_nr(w, 'use');
	return f(v, w, ...)
end)

game.oninv = stead.hook(game.oninv, 
function(f, v, w, ...)
	inc_nr(v, 'inv');
	inc_nr(w, 'inv');
	return f(v, w, ...)
end)

game.onwalk = stead.hook(game.onwalk, 
function(f, v, w, ...)
	inc_nr(v, 'walk');
	inc_nr(w, 'walk');
	return f(v, w, ...)
end)

function act_count(s, v)
	if s then return read_nr(s, 'act', v) end
	return read_nr(game, 'act', v)
end

function inv_count(s, v)
	if s then return read_nr(s, 'inv', v) end
	return read_nr(game, 'inv', v)
end

function use_count(s, v)
	if s then return read_nr(s, 'use', v) end
	return read_nr(game, 'use', v)
end

function walk_count(s, v)
	if s then return read_nr(s, 'walk', v) end
	return read_nr(game, 'walk', v)
end
