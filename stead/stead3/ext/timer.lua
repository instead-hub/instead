-- raw iface to timer
-- luacheck: read globals instead_timer
local std = stead
local type = std.type

local instead = std.ref '@instead'

instead.timer = instead_timer

local timer = std.obj {
	nam = '@timer';
	ini = function(s)
		if s.__timer then
			s:set(s.__timer)
		end
	end;
	get = function(s)
		return s.__timer or 0;
	end;
	stop = function(s)
		return s:set(-1)
	end;
	set = function(s, v)
		if type(v) ~= 'number' then
			std.err("Wrong argument to timer:set(): "..std.tostr(v), 2)
		end
		s.__timer = v
		instead.timer(s.__timer)
		return true
	end;
	callback = function(_)
		return '@timer'
	end
}

std.timer = function() -- sdl part call this one
	if std.type(timer.callback) == 'function' then
		return timer:callback();
	end
	return
end

std.mod_done(function(_)
	timer:stop()
end)

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@timer' then
		return
	end
	local r, v = std.call(stead.here(), 'timer');
	if not r and not v then
		r, v = stead.call(std.game, 'timer');
	end
	if not r and not v then -- nothing todo
		return nil, false
	end
	return r, v
end)
