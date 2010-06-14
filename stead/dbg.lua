-- add this: reuire "dbg"
-- in your project
-- for debug tools
require "input"

list_objects = function()
	local i,o
	local rc = par(' ', 'Room:', tostring(deref(from())), 'Nam:', 
		call(from(),'nam'));
	rc = cat(rc,'^');
	for i,o in opairs(objs(from())) do
		o = ref(o)
		rc = cat(rc, par(' ', 'Id:', tostring(o.id), 'Obj:', tostring(deref(o)), 
			'Nam:', call(o, 'nam'), 'Disabled:', 
			tostring(isDisabled(o))),
			'^');
	end
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
				return f();
			end
			return "Error in exec.";
		end
		return back();
	end,
	act = function(s, w)
		return back();
	end,
	obj = { inp('{Enter cmd}:', 'return "Hello World!"'), 
	    vobj(1, 'Back', '^{Back}')}
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
		phr('Exec Lua string...', true, [[pon(); drop_object:gen(); return goto('execute_cmd')]]),
		phr('Exit',true , [[pon(); return goto(from())]]),
	},
};

debug_tool = menu {
	debug = true,
	forcedsc = true,
	nam = txtb('debug'),
	inv = function(s)
		debug_dlg.__from__ = deref(here());
		me().where = 'debug_dlg'; -- force to go
		local r = call(debug_dlg, 'enter');
		return r;
	end
};
putf('debug_tool', me());
