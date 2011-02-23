-- add this: reuire "dbg"
-- in your project
-- for debug tools
require "input"

function _xref_escape(n)
	local delim = ':'
	if stead.api_version >= "1.2.2" then
		delim = stead.delim;
	end
	if xact then
		n = n:gsub("\\?[\\"..delim.."]", { [delim] = "\\"..delim } )
	end
	return n
end

function ordered_n(t)
	local ordered = {};
	local i,v, max;
	max = 0;
	for i,v in pairs(t) do
		local o = { k = i; v = v };
		stead.table.insert(ordered, o);
		max = max + 1;
	end
	stead.table.sort(ordered, function(a, b)
		if isObject(a.v) and not isObject(b.v) then
			return true
		end
		if not isObject(a.v) and isObject(b.v) then
			return false
		end
		if isObject(a.v) and isObject(b.v) then
			local n = call(a.v, 'nam');
			local m = call(b.v, 'nam');
			if type(n) ~= 'string' and type(m) ~= 'string' then
				return false
			end
			if type(n) ~= 'string' then
				return true
			end
			if type(m) ~= 'string' then
				return false
			end
			return n < m;
		end
		return false
	end)
	ordered.i = 1;
	ordered.max = max;
	return ordered;
end

function snext(t, k)
	local v
	if not k then
		k = ordered_n(t);
	end
	if k.i > k.max then
		return nil
	end
	v = k[k.i]
	k.i = k.i + 1
	return k, v.v, v.k;
end

function spairs(s, var)
	return snext, s, nil;
end

function disp_obj()
	local v = obj {
		nam = 'disp',
		act = true,
		dsc = function(s)
			local r = s._txt
			s._txt = nil;
			return r
		end;
		save = function(self, name, h, need)
			if need then
				h:write(stead.string.format("%s  = disp_obj();\n", name));
			end
			savemembers(h, self, name, false);
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
	local i,o,n
	local rc=''
	for i,o in pairs(w) do
		local t = stead.tostring(o);
		if t == i then
			t = tostring(o);
		end
		if t then
			if rc ~='' then rc = rc..'^' end
			local n = '';
			if type(o) ~= 'function' and isObject(ref(o)) then
				n = call(ref(o), 'nam');
				if type(n) ~= 'string' then n = '' else n = ' : '..n; end
			end
			rc = stead.cat(rc, stead.par(' ', tostring(i)..' : '..t..n));
		end
	end
	seen('disp')._txt = stead.cat('^^', rc)
	return true;
end

dump_globals = function()
	local i,o
	local rc=''
	if type(variables) ~= 'table' then
		return
	end
	for i,o in ipairs(variables) do
		local v = _G[o];
		local t = stead.tostring(v);
		if t then
			if rc ~='' then rc = rc..'^' end
			rc = stead.cat(rc, stead.par(' ', tostring(o)..' : '..t));
		end
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
	system_type = true, 
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
		obj { nam = 'Back', dsc = '^{Back}', act = code [[ back() ]] },
		disp_obj(),
	}
}

dump_object = room {
	nam = "Dump object",
	debug = true,
	system_type = true, 
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
		obj{nam = 'Lifes', dsc = '^{Dump lifes}', act = code[[ return dump_obj(debug_tool.lifes)]]},
		obj{nam = 'Ways', dsc = '^{Dump ways}', act = code[[ return dump_obj(ways(dbg_here()))]]},
		obj{nam = 'Globals', dsc = '^{Dump globals}', act = code [[return dump_globals()]] },
		obj{nam = 'Back', dsc = '^{Back}', act = code [[ return back() ]] },
		disp_obj() }
}

choose_location = dlg {
	debug = true,
	system_type = true, 
	forcedsc = true,
	nam = 'Go to',
	dsc = 'Select location.',
	gen = function(s)
		local k,v,kk
		objs(s):zap();
		for k,v,kk in spairs(_G) do
			if isRoom(v) and not v.debug then
				local n = tostring(call(v, 'nam'));
				local o = kk;
				if type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put(phr(n, true, [[timer:set(debug_tool._timer); game.lifes:cat(debug_tool.lifes); return goto(]]..o..[[)]]), s);
				end
			end
		end
		put (phr('Back',true, 'return back()'), s)
	end
}

choose_object = dlg {
	debug = true,
	system_type = true, 
	forcedsc = true,
	nam = 'Get object',
	dsc = 'Select object to get.',
	gen = function(s)
		local k,v,kk
		objs(s):zap();
		for k,v,kk in spairs(_G) do
			if isObject(v) and not isPhrase(v) and not isRoom(v) and not isPlayer(v) and not v.debug and not have(v) and not isStatus(v) then
				local n = tostring(call(v, 'nam'));
				local o = kk;
				if type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put(phr(n, true, o..':enable(); return take('..o..')'), s);
				end
			end
		end
		put (phr('Back',true, 'return back()'), s)
	end
}

drop_object = dlg {
	debug = true,
	forcedsc = true,
	system_type = true, 
	nam = 'Drop object',
	dsc = 'Select object to drop.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in ipairs(inv()) do
			v = ref(v);
			if not v.debug then
				local n = tostring(call(v, 'nam'));
				local o = deref(v);
				if type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put (phr(n, true, o..':enable(); drop('..o..','..deref(dbg_here())..')'), s)
				end
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
	game.lifes:cat(debug_tool.lifes);
	timer:set(debug_tool._timer);
	return par ('^^', back(), r);
end

debug_dlg = dlg {
	debug = true,
	system_type = true, 
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
	},
};

debug_tool = menu {
	debug = true,
	system_type = true,
	forcedsc = true,
	nam = txtb('debug'),
	lifes = list {},
	inv = function(s)
		if here().debug then
			return nil, true --nothing todo
		end
		debug_dlg.__from__ = here();
		s._timer = timer:get();
		timer:stop();
		s.lifes:zap();
		s.lifes:cat(game.lifes);
		game.lifes:zap();
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
	return f(s, cmd, ...)
end)

stead.module_init(function()
	input.key = stead.hook(input.key,
	function(f, s, down, key, ...)
		if not here().debug and down and key == 'f7' then return 'use_debug' end
		return f(s, down, key, ...)
	end)
	putf('debug_tool', me());
end)


-- vim:ts=4
