local std = stead
local input = std.ref '@input'
local type = std.type

keys = std.obj {
	nam = '@keys';
	{
		__alt = false;
		__ctrl = false;
		__shift = false;
		__state = {};
	};
	state = function(s, key)
		return s.__state[key] or false
	end;
	event = function(s, press, key)
		if key:find 'alt' then
			key = 'alt'
		elseif key:find 'ctrl' then
			key = 'ctrl'
		elseif key:find 'shift' then
			key = 'shift'
		end
		s.__state[key] = press
	end;
	filter = function(s, press, key)
		return false
	end
}

function input:key(press, key)
	local a

	keys:event(press, key)

	if not keys:filter(press, key) then
		return
	end

	for _, v in std.ipairs {press, key} do
		a = (a and (a..', ') or ' ') .. std.dump(v)
	end
	return '@key'.. (a or '')
end

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@key' then
		return
	end
	local r, v
	r, v = std.call(std.here(), 'onkey', cmd[2], cmd[3])
	if not r and not v then
		r, v = std.call(std.game, 'onkey', cmd[2], cmd[3])
	end
	if not r and not v then -- nothing todo
		return nil, false
	end
	return r, v
end)
