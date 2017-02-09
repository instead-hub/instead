local std = stead
local type = std.type

local SNAPSHOT = false

local snap = std.obj {
	nam = '@snaphots';
	data = {};
	write = function(s, name)
		name = name or 'default'
		local fp = { -- fake file object
			data = '';
			write = function(s, str)
				s.data = s.data .. str
			end;
		}
		std:save(fp)
		s.data[name] = fp.data
	end;
	make = function(s)
		SNAPSHOT = true
	end;
	exist = function(s, name)
		name = name or 'default'
		return s.data[name]
	end;
	remove = function(s, name)
		name = name or 'default'
		s.data[name] = nil
	end;
	restore = function(s, name) -- like std:load()
		name = name or 'default'
		if not s:exist(name) then
			return false
		end
		std:reset()
		std.ref 'game':ini(true)

		local f, err = std.eval(s.data[name])
		if not f then
			std.err(err, 2)
		end
		f();
		std.ref 'game':ini()
		return std.nop()
	end;
}

snapshots = snap

std.mod_cmd(function()
	if SNAPSHOT then
		snap:write(snap.SNAPSHOT)
		SNAPSHOT = nil
	end
end)

std.mod_done(function()
	snap.data = {}
end)
