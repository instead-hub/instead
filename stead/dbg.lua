-- add this: reuire "dbg"
-- in your project
-- for debug tools
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
		put (phr('Exit',true, 'return back()'), s)
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
		put (phr('Exit',true, 'return back()'), s)
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
		put (phr('Exit', true, 'return back()'), s)
	end
}

debug_dlg = dlg {
	debug = true,
	forcedsc = true,
	nam = 'Debug Tool',
	dsc = 'Select tool.',
	obj = {
		[1]=phr('Go to location...', true, [[pon(1); choose_location:gen(); return goto('choose_location')]]),
		[2]=phr('Get object...', true, [[pon(2); choose_object:gen(); return goto('choose_object')]]),
		[3]=phr('Put object...', true, [[pon(3); drop_object:gen(); return goto('drop_object')]]),
		[4]=phr('Exit',true , [[pon(4); return goto(from())]]),
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
