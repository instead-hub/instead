-- add this: reuire "dbg"
-- in your project
-- for debug tools
require "input"

function disp_obj()
	local v = obj{
		nam = 'disp',
		act = true,
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
		if isCode(o) then o = stead.string.format("code [[%s]]", stead.functions[o].code); end
		rc = stead.cat(rc, stead.par(' ', 'Key:'..tostring(i),
			'Val:'..tostring(deref(o))));
	end
	seen('disp')._txt = stead.cat('^^', rc)
	return true;
end

dbg_here = function()
	return debug_tool._here
end

list_objects = function()
	local i,o
	local rc = stead.par(' ', 'Room:'..tostring(deref(dbg_here())), 
			'Nam:'..tostring(call(dbg_here(),'nam')));
	for i,o in opairs(objs(dbg_here())) do
		rc = rc..'^';
		o = ref(o)
		rc = stead.cat(rc, stead.par(' ', 'Id:'..tostring(o.id), 
			'Obj:'..tostring(deref(o)), 
			'Nam:'..tostring(call(o, 'nam')), 
			'Disabled:'..tostring(isDisabled(o))));
	end
--	seen('disp')._txt = rc
	return rc
end

list_inv = function()
	local i,o
	local rc=''
	for i,o in opairs(inv()) do
		if rc ~='' then rc = rc..'^' end
		o = ref(o)
		rc = stead.cat(rc, stead.par(' ', 'Id:'..tostring(o.id), 'Obj:'..tostring(deref(o)), 
			'Nam:'..tostring(call(o, 'nam')), 
			'Disabled:'..tostring(isDisabled(o)), 
			'Taken:'..tostring(taken(o))));
	end
	if rc == '' then return end
--	seen('disp')._txt = rc
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
				seen('disp')._txt = stead.cat('^^', f());
				return true
			end
			seen('disp')._txt = "^^Error in exec.";
			return true
		end
		return back();
	end,
	obj = { inp('inp', '{Enter cmd}: ', 'return "Hello World!"'), 
		obj { nam = 'Back', dsc = '^{Back}', act = code [[ back() ]]},
		new [[ disp_obj() ]],
	}
}

dump_object = room {
	nam = "Dump object",
	debug = true,
	forcedsc = true,
	dsc = "Enter object name here to dump.",
	inp_enter = function(s)
		local w = s.obj[1]._txt
		if type(w) == 'string' then
			if not ref(w) then w = objs(dbg_here()):srch(w); end
			return dump_obj(w);
		end
		return back();
	end,
	obj = { inp('inp', '{Enter object}: ', 'main'), 
		obj{nam = 'Here', dsc = '^{Dump here}', act = code[[ return dump_obj(dbg_here())]]},
		obj{nam = 'Player',dsc =  '^{Dump player}', act = code[[ return dump_obj(me())]]},
		obj{nam = 'Lifes', dsc = '^{Dump lifes}', act = code[[ return dump_obj(game.lifes)]]},
		obj{nam = 'Ways', dsc = '^{Dump ways}', act = code[[ return dump_obj(ways(dbg_here()))]]},
		obj{nam = 'Back', dsc = '^{Back}', act = code [[ return back() ]] },
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
				put(phr(n:gsub(":","\\:"), true, [[return goto("]]..k..[[")]]), s);
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
				put(phr(n:gsub(":","\\:"), true, k..':enable(); return take("'..k..'")'), s);
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
				put (phr(k, true, 'drop("'..k..'","'..deref(dbg_here())..'")'), s)
			end
		end
		put (phr('Back', true, 'return back()'), s)
	end
}
function dbg_exit()
	local r
	if stead.api_version < "1.2.0" then
		r = call(dbg_here(), 'dsc');
	end
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
--		new [[ disp_obj ]]
	},
};

debug_tool = menu {
	debug = true,
	forcedsc = true,
	nam = txtb('debug'),
	inv = function(s)
		if here().debug then
			return nil, true --nothing todo
		end
		debug_dlg.__from__ = here();
		s._here = here();
		me().where = 'debug_dlg'; -- force to go
		return goto(self.where);
	end,
};

game.action = stead.hook(game.action, 
function (f, s, cmd, ...)
	if cmd == 'use_debug' then
		return debug_tool:inv()
	end
	return f(s, cmd, unpack(arg))
end)

input.key = stead.hook(input.key,
function(f, s, down, key, ...)
	if not here().debug and down and key == 'f7' then return 'use_debug' end
	return f(s, down, key, unpack(arg))
end)

putf('debug_tool', me());

-- vim:ts=4
