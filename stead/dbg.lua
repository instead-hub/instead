-- add this: reuire "dbg"
-- in your project
-- for debug tools
require "input"

local function _xref_escape(n)
	local delim = ':'
	if stead.api_atleast(1, 2, 2) then
		delim = stead.delim;
	end
	if xact then
		n = n:gsub("\\?[\\"..delim.."]", { [delim] = "\\"..delim } )
	end
	return n
end

local function ordered_n(t)
	local ordered = {};
	local i,v, max;
	max = 0;
	for i,v in stead.pairs(t) do
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
			local n = stead.call(a.v, 'nam');
			local m = stead.call(b.v, 'nam');
			if stead.type(n) ~= 'string' and stead.type(m) ~= 'string' then
				return false
			end
			if stead.type(n) ~= 'string' then
				return true
			end
			if stead.type(m) ~= 'string' then
				return false
			end
			if n == m then
				local o1, o2
				o1 = stead.deref(a.v)
				o2 = stead.deref(b.v)
				if stead.type(o1) == 'string' and stead.type(o2) == 'string' then
					return o1 < o2
				end
			end
			return n < m;
		end
		return false
	end)
	ordered.i = 1;
	ordered.max = max;
	return ordered;
end

local function snext(t, k)
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

local function spairs(s, var)
	return snext, s, nil;
end

function dbg_disp_obj()
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
				h:write(stead.string.format("%s  = dbg_disp_obj();\n", name));
			end
			stead.savemembers(h, self, name, false);
		end
	}
	return v;
end

dbg_dump_obj = function(w)
	w = stead.ref(w)

	if stead.type(w) ~= 'table' then
		seen('disp')._txt = '^^No such object.';
		return true
	end
	local i,o,n
	local rc = ''
	for i,o in stead.pairs(w) do
		local t = stead.tostring(o);
		if t == i then
			t = stead.tostr(o);
		end
		if t then
			if rc ~='' then rc = rc..'^' end
			local n = '';
			if isObject(o) then
				n = stead.call(o, 'nam');
				if stead.type(n) ~= 'string' then n = '' else n = ' : '..n; end
			end
			rc = stead.cat(rc, stead.par(' ', stead.tostr(i)..' : '..t..n));
		end
	end
	seen('disp')._txt = stead.cat('^^', rc)
	return true;
end

dbg_dump_globals = function()
	local i,o
	local rc=''
	if stead.type(variables) ~= 'table' then
		return
	end
	for i,o in stead.ipairs(variables) do
		local v = _G[o];
		local t = stead.tostring(v);
		if t then
			if rc ~='' then rc = rc..'^' end
			rc = stead.cat(rc, stead.par(' ', stead.tostr(o)..' : '..t));
		end
	end
	seen('disp')._txt = stead.cat('^^', rc)
	return true;
end

dbg_here = function()
	return debug_tool._here
end

dbg_list_objects = function()
	local i,o
	local dis = function(o)
		if isDisabled(o) then
			return ", disabled"
		end
		return ''
	end
	local rc = stead.par(' ', 'Room:'..stead.tostr(stead.deref(dbg_here())), 
			'Nam:'..stead.tostr(stead.call(dbg_here(),'nam')));
	for i,o in stead.opairs(objs(dbg_here())) do
		rc = rc..'^';
		o = stead.ref(o)
		rc = stead.cat(rc, stead.par(' ', 'Id: '..stead.tostr(o.id)..', '..
			stead.tostr(stead.deref(o))..': '..stead.tostr(stead.call(o, 'nam'))..dis(o)));
	end
--	seen('disp')._txt = rc
	return rc
end

dbg_list_inv = function()
	local i,o
	local rc=''
	local dis = function(o)
		if isDisabled(o) then
			return ", disabled"
		end
		return ''
	end

	local tak = function(o)
		if taken(o) then
			return ", taken"
		end
		return ''
	end

	for i,o in stead.opairs(inv()) do
		if rc ~='' then rc = rc..'^' end
		o = stead.ref(o)
		rc = stead.cat(rc, stead.par(' ', 'Id: '..stead.tostr(o.id)..', '..
			stead.tostr(stead.deref(o))..': '..stead.tostr(stead.call(o, 'nam'))..dis(o)..tak(o)));
	end
	if rc == '' then return end
--	seen('disp')._txt = rc
	return rc
end

dbg_execute_cmd = room {
	nam = "Execute Lua code",
	debug = true,
	pic = true,
	system_type = true, 
	forcedsc = true,
	dsc = "Enter Lua code here to exec.",
	inp_enter = function(s)
		if stead.type(s.obj[1]._txt) == 'string' then
			local f = stead.eval(s.obj[1]._txt);
			if f then
				seen('disp')._txt = stead.cat('^^', f());
				return true
			end
			seen('disp')._txt = "^^Error in exec.";
			return true
		end
		return stead.back();
	end,
	obj = { inp('inp', '{Enter cmd}: ', 'return "Hello World!"'), 
		obj { nam = 'Back', dsc = '^{Back}', act = code [[ stead.back() ]] },
		dbg_disp_obj(),
	},
	exit = function(s)
		s.obj[1]:state(false)
	end;
}

dbg_dump_object = room {
	nam = "Dump object",
	debug = true,
	pic = true,
	system_type = true, 
	forcedsc = true,
	dsc = "Enter object name here to dump.",
	inp_enter = function(s)
		local w = s.obj[1]._txt
		if stead.type(w) == 'string' then
			if not stead.ref(w) then w = objs(dbg_here()):srch(w); end
			return dbg_dump_obj(w);
		end
		return stead.back();
	end,
	obj = { inp('inp', '{Enter object}: ', 'main'), 
		obj{nam = 'Here', dsc = '^{Dump here}', act = code[[ return dbg_dump_obj(dbg_here())]]},
		obj{nam = 'Player',dsc =  '^{Dump player}', act = code[[ return dbg_dump_obj(stead.me())]]},
		obj{nam = 'Lifes', dsc = '^{Dump lifes}', act = code[[ return dbg_dump_obj(debug_tool.lifes)]]},
		obj{nam = 'Ways', dsc = '^{Dump ways}', act = code[[ return dbg_dump_obj(ways(dbg_here()))]]},
		obj{nam = 'Globals', dsc = '^{Dump globals}', act = code [[return dbg_dump_globals()]] },
		obj{nam = 'Back', dsc = '^{Back}', act = code [[ return stead.back() ]] },
		dbg_disp_obj() },
	exit = function(s)
		s.obj[1]:state(false)
	end;
}

dbg_choose_location = dlg {
	debug = true,
	pic = true,
	system_type = true, 
	forcedsc = true,
	nam = 'Go to',
	dsc = 'Select location.',
	gen = function(s)
		local k,v,kk
		objs(s):zap();
		for k,v,kk in spairs(_G) do
			if isRoom(v) and not v.debug then
				local n = stead.tostr(stead.call(v, 'nam'));
				local o = kk;
				if stead.type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put(phr(n, true, [[timer:set(debug_tool._timer); game.lifes:cat(debug_tool.lifes); return stead.walk(]]..o..[[)]]), s);
				end
			end
		end
		put (phr('Back',true, 'return stead.back()'), s)
	end
}

dbg_choose_object = dlg {
	debug = true,
	pic = true,
	system_type = true, 
	forcedsc = true,
	nam = 'Get object',
	dsc = 'Select object to get.',
	gen = function(s)
		local k,v,kk
		objs(s):zap();
		for k,v,kk in spairs(_G) do
			if isObject(v) and not isPhrase(v) and not isRoom(v) and not isPlayer(v) and not v.debug and not have(v) and not isStatus(v) then
				local n = stead.tostr(stead.call(v, 'nam'));
				local o = kk;
				if stead.type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put(phr(n, true, o..':enable(); return take('..o..')'), s);
				end
			end
		end
		put (phr('Back',true, 'return stead.back()'), s)
	end
}

dbg_drop_object = dlg {
	debug = true,
	pic = true,
	forcedsc = true,
	system_type = true, 
	nam = 'Drop object',
	dsc = 'Select object to drop.',
	gen = function(s)
		local k,v
		objs(s):zap();
		for k,v in stead.ipairs(inv()) do
			v = stead.ref(v);
			if not v.debug then
				local n = stead.tostr(stead.call(v, 'nam'));
				local o = stead.deref(v);
				if stead.type(o) == 'string' then
					n = n..' : '..o;
					n = _xref_escape(n);
					put (phr(n, true, o..':enable(); drop('..o..','..stead.deref(dbg_here())..')'), s)
				end
			end
		end
		put (phr('Back', true, 'return stead.back()'), s)
	end
}

function dbg_exit()
	local r
	if not stead.api_atleast(1, 2, 0) then
		r = stead.call(dbg_here(), 'dsc');
	end
	game.lifes:cat(debug_tool.lifes);
	timer:set(debug_tool._timer);
	return stead.par ('^^', stead.back(), r);
end

debug_dlg = dlg {
	debug = true,
	pic = true,
	system_type = true, 
	forcedsc = true,
	nam = 'Debug Tool',
	dsc = 'Select tool.',
	obj = {
		phr('Go to location...', true, [[pon(); dbg_choose_location:gen(); return stead.walk('dbg_choose_location')]]),
		phr('Get object...', true, [[pon(); dbg_choose_object:gen(); return stead.walk('dbg_choose_object')]]),
		phr('Put object...', true, [[pon(); dbg_drop_object:gen(); return stead.walk('dbg_drop_object')]]),
		phr('Current scene...', true, [[pon(); return dbg_list_objects();]]),
		phr('Inventory...', true, [[pon(); return dbg_list_inv();]]),
		phr('Dump object...', true, [[pon(); return stead.walk(dbg_dump_object);]]),
		phr('Exec Lua string...', true, [[pon(); return stead.walk('dbg_execute_cmd')]]),
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
		if stead.here().debug then
			return nil, true --nothing todo
		end
		debug_dlg.__from__ = stead.here();
		s._timer = timer:get();
		timer:stop();
		s.lifes:zap();
		s.lifes:cat(game.lifes);
		game.lifes:zap();
		s._here = stead.here();
		stead.me().where = 'debug_dlg'; -- force to go
		return stead.walk(self.where);
	end,
};

game.action = stead.hook(game.action, 
function (f, s, cmd, ...)
	if cmd == 'use_debug' then
		return debug_tool:inv()
	elseif cmd == 'exit_debug' then
		stead.me().where = 'debug_dlg';
		dbg_execute_cmd.obj[1]:state(false)
		dbg_dump_object.obj[1]:state(false)
		return dbg_exit()
	end
	return f(s, cmd, ...)
end)

stead.module_init(function()
	input.key = stead.hook(input.key,
	function(f, s, down, key, ...)
		if down and key == 'f7' then 
			if stead.here().debug then
				return 'exit_debug'
			else
				return 'use_debug'
			end
		end
		
		return f(s, down, key, ...)
	end)
	putf('debug_tool', stead.me());
end)


-- vim:ts=4
