-- add this: reuire "dbg"
-- in your project
-- for debug tools
require "input"

function disp_obj()
	local v = obj{
		nam = 'disp',
		dsc = function(s)
			local r = s._txt
			s._txt = nil;
			return r
		end
	}
	return v;
end

dump_obj = function(w)
	w = ref(w)
	if type(w) ~= 'table' then
		seen('disp')._txt = '^^No such object.';
		return true
	end
	local i,o
	local rc=''
	for i,o in pairs(w) do
		if rc ~='' then rc = rc..'^' end
		rc = cat(rc, par(' ', 'Key:'..tostring(i),
			'Val:'..tostring(deref(o))));
	end
	seen('disp')._txt = cat('^^', rc)
	return true;
end

list_objects = function()
	local i,o
	local rc = par(' ', 'Room:'..tostring(deref(from())), 
			'Nam:'..tostring(call(from(),'nam')));
	for i,o in opairs(objs(from())) do
		rc = rc..'^';
		o = ref(o)
		rc = cat(rc, par(' ', 'Id:'..tostring(o.id), 
			'Obj:'..tostring(deref(o)), 
			'Nam:'..tostring(call(o, 'nam')), 
			'Disabled:'..tostring(isDisabled(o))));
	end
	seen('disp')._txt = rc
	return rc
end

list_inv = function()
	local i,o
	local rc=''
	for i,o in opairs(inv()) do
		if rc ~='' then rc = rc..'^' end
		o = ref(o)
		rc = cat(rc, par(' ', 'Id:'..tostring(o.id), 'Obj:'..tostring(deref(o)), 
			'Nam:'..tostring(call(o, 'nam')), 
			'Disabled:'..tostring(isDisabled(o)), 
			'Taken:'..tostring(taken(o))));
	end
	if rc == '' then return end
	seen('disp')._txt = rc
	return rc
end

execute_cmd = room {
	nam = "Execute Lua code",
	debug = true,
	forcedsc = true,
	dsc = "Enter Lua code here to exec.",
	inp_enter = function(s)
		if type(s.obj[1]._txt) == 'string' then
			local f = loadstring(s.obj[1]._txt);
			if f then
				seen('disp')._txt = cat('^^', f());
				return true
			end
			seen('disp')._txt = "^^Error in exec.";
			return true
		end
		return back();
	end,
	act = function(s, w)
		return back();
	end,
	obj = { inp('{Enter cmd}: ', 'return "Hello World!"'), 
		vobj(1, 'Back', '^{Back}'),
		new [[ disp_obj() ]],
	}
}

dump_object = room {
	nam = "Dump object",
	debug = true,
	forcedsc = true,
	dsc = "Enter object name here to dump.",
	inp_enter = function(s)
		if type(s.obj[1]._txt) == 'string' then
			return dump_obj(s.obj[1]._txt);
		end
		return back();
	end,
	act = function(s, w)
		if w == 1 then
			return back();
		elseif w == 2 then
			return dump_obj(from(from()));
		elseif w == 3 then
			return dump_obj(me());
		elseif w == 4 then
			return dump_obj(game.lifes);
		elseif w == 5 then
			return dump_obj(ways(from(from())));
		end

	end,
	obj = { inp('{Enter object}: ', 'main'), 
		vobj(2, 'Here', '^{Dump here}'),
		vobj(3, 'Player', '^{Dump player}'),
		vobj(4, 'Lifes', '^{Dump lifes}'),
		vobj(5, 'Ways', '^{Dump ways}'),
		vobj(1, 'Back', '^{Back}'),
		new[[ disp_obj() ]]}
}

choose_location = dlg {
	debug = true,
	forcedsc = true,
	nam = 'Go to',
	dsc = 'Select location.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isRoom(v) and not v.debug then
				local n = call(v, 'nam');
				put(phr(n, true, [[return goto("]]..k..[[")]]), s);
			end
		end
		put (phr('Back',true, 'return back()'), s)
	end
}

choose_object = dlg {
	debug = true,
	forcedsc = true,
	nam = 'Get object',
	dsc = 'Select object to get.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isObject(v) and not isRoom(v) and not isPlayer(v) and not v.debug and not have(v) and not isStatus(v) then
				local n = call(v, 'nam');
				put(phr(n, true, k..':enable(); return take("'..k..'")'), s);
			end
		end
		put (phr('Back',true, 'return back()'), s)
	end
}

drop_object = dlg {
	debug = true,
	forcedsc = true,
	nam = 'Drop object',
	dsc = 'Select object to drop.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isObject(v) and not isRoom(v) and not isPlayer(v) and not v.debug and have(v) then
				local n = call(v, 'nam');
				put (phr(k, true, 'drop("'..k..'","'..deref(from())..'")'), s)
			end
		end
		put (phr('Back', true, 'return back()'), s)
	end
}
function dbg_exit()
	local r = call(from(), 'dsc');
	return par ('^^', back(), r);
end
debug_dlg = dlg {
	debug = true,
	forcedsc = true,
	nam = 'Debug Tool',
	dsc = 'Select tool.',
	obj = {
		phr('Go to location...', true, [[pon(); choose_location:gen(); return goto('choose_location')]]),
		phr('Get object...', true, [[pon(); choose_object:gen(); return goto('choose_object')]]),
		phr('Put object...', true, [[pon(); drop_object:gen(); return goto('drop_object')]]),
		phr('Current scene...', true, [[pon(); return list_objects();]]),
		phr('Inventory...', true, [[pon(); return list_inv();]]),
		phr('Dump object...', true, [[pon(); return goto(dump_object);]]),
		phr('Exec Lua string...', true, [[pon(); return goto('execute_cmd')]]),
		phr('Exit',true , [[pon(); return dbg_exit()]]),
		new [[ disp_obj ]]
	},
};

debug_tool = menu {
	debug = true,
	forcedsc = true,
	nam = txtb('debug'),
	inv = function(s)
		debug_dlg.__from__ = deref(here());
		me().where = 'debug_dlg'; -- force to go
		local r = par('^^', call(debug_dlg, 'enter'), call(debug_dlg, 'dsc'));
		return r;
	end,
};

game.action = hook(game.action, 
function (f, s, cmd, ...)
	if cmd == 'use_debug' then
		return debug_tool:inv()
	end
	return f(s, cmd, unpack(arg))
end)

input.key = hook(input.key,
function(f, s, down, key, ...)
	if not here().debug and down and key == 'f7' then return 'use_debug' end
	return f(s, down, key, unpack(arg))
end)

putf('debug_tool', me());
