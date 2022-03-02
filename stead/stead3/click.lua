local std = stead
local input = std.ref '@input'

-- luacheck: globals click

click = std.obj {
	nam = '@click';
-- luacheck: no unused args
	filter = function(s, press, btn, x, y, px, py)
-- luacheck: unused args
		return press and px
	end
}

function input:wheel(x, y)
	local a
	for _, v in std.ipairs {'scroll', x, y} do
		a = (a and (a..', ') or ' ') .. std.dump(v)
	end
	return '@click'.. (a or '')
end

function input:click(press, btn, x, y, px, py)
	local a
	if not click:filter(press, btn, x, y, px, py) then
		return
	end
	for _, v in std.ipairs {'click', press, btn, x, y, px, py} do
		a = (a and (a..', ') or ' ') .. std.dump(v)
	end
	return '@click'.. (a or '')
end

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@click' then
		return
	end
	if cmd[2] ~= 'click' and cmd[2] ~= 'scroll' then
		return
	end
	local r, v
	r, v = std.call(std.here(), 'on'..cmd[2], cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8])
	if not r and not v then
		r, v = std.call(std.game, 'on'..cmd[2], cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8])
	end
	if not r and not v then -- nothing todo
		return nil, false
	end
	return r, v
end)
