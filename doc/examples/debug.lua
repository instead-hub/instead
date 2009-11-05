-- add this: dofile("debug.lua")
-- after last line in your main.lua
-- for debug tools

choose_location = dlg {
	debug = true,
	nam = 'Go to',
	dsc = 'Select location.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isRoom(v) and not v.debug then
				local n = call(v, 'nam');
				put(phr(n, '', [[return goto("]]..k..[[")]]), s);
			end
		end
		put (phr('Exit','','back()'), s)
	end
}

choose_object = dlg {
	debug = true,
	nam = 'Get object',
	dsc = 'Select object to get.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isObject(v) and not isRoom(v) and not isPlayer(v) and not v.debug and not have(v) and not isStatus(v) then
				local n = call(v, 'nam');
				put(phr(n, '', k..':enable(); return take("'..k..'")'), s);
			end
		end
		put (phr('Exit','','back()'), s)
	end
}

drop_object = dlg {
	debug = true,
	nam = 'Drop object',
	dsc = 'Select object to drop.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in pairs(_G) do
			if isObject(v) and not isRoom(v) and not isPlayer(v) and not v.debug and have(v) then
				local n = call(v, 'nam');
				put (phr(k,'','drop("'..k..'","'..deref(from())..'")'), s)
			end
		end
		put (phr('Exit','','back()'), s)
	end
}

debug_dlg = dlg {
	debug = true, 
	nam = 'Debug Tool',
	dsc = 'Select tool.',
	obj = {
		[1]=phr('Go to location...', nil ,[[pon(1); choose_location:gen(); return goto('choose_location')]]),
		[2]=phr('Get object...', nil , [[pon(2); choose_object:gen(); return goto('choose_object')]]),
		[3]=phr('Put object...', nil , [[pon(3); drop_object:gen(); return goto('drop_object')]]),
		[4]=phr('Exit', nil , [[pon(4); back()]]),
	},
};

debug_tool = obj {
	debug = true,
	nam = txtb('debug'),
	inv = function(s)
		debug_dlg.__from__ = deref(here());
		me().where = 'debug_dlg'; -- force to go
		local r = call(debug_dlg, 'enter');
		return r;
	end
};

putf('debug_tool', me());
