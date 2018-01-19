local std = stead
local input = std.ref '@input'
local instead = std.ref '@instead'

-- luacheck: no self
function input:event(...)
-- luacheck: self
	local a
	for _, v in std.ipairs {...} do
		a = (a and (a..', ') or ' ') .. std.dump(v)
	end
	return '@user_event'.. a or ''
end

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@user_event' then
		return
	end
	local r, v =  std.call(instead, 'onevent', cmd[2])
	if not r and not v then -- nothing todo
		return nil, false
	end
	return r, v
end)
