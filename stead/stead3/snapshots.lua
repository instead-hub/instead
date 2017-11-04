local std = stead
local type = std.type

local SNAPSHOT = false
local INWRITE = false

local snap = std.obj {
	nam = '@snapshots';
	data = {};
	save = function(s, ...)
		if INWRITE then return end
		return std.obj.save(s, ...)
	end;
	write = function(s, name)
		name = name or 'default'
		local fp = { -- fake file object
			data = '';
			write = function(s, str)
				s.data = s.data .. str
			end;
		}
		INWRITE = true std:save(fp) INWRITE = false
		s.data[name] = fp.data
	end;
	make = function(s, name)
		SNAPSHOT = name or 'default'
	end;
	exists = function(s, name)
		name = name or 'default'
		return s.data[name]
	end;
	remove = function(s, name)
		name = name or 'default'
		s.data[name] = nil
	end;
	restore = function(s, name) -- like std:load()
		name = name or 'default'
		if not s:exists(name) then
			return false
		end
		std:reset()
		std.ref 'game':__ini(false)
		local f, err = std.eval(s.data[name])
		if not f then
			std.err(err, 2)
		end
		f();
		std.ref 'game':__ini(true)
		return std.nop()
	end;
}

snapshots = snap

std.mod_cmd(function()
	if SNAPSHOT then
		snap:write(SNAPSHOT)
		SNAPSHOT = nil
	end
end)

-- std.mod_done(function()
-- end)
