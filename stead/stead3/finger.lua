local std = stead
local input = std.ref '@input'
local type = std.type
local table = table

finger = std.obj {
	nam = '@finger';
	{
		fingers_list = { };
	};
	filter = function(s, press, fid, x, y, px, py) -- by default, all finger events
		return true
	end
}

function finger:event(press, fid, x, y, px, py)
	local s = self
	if press then
		local v = s:lookup(fid)
		if v then
			v.x, v.y, v.px, v.py = x, y, px, py
		else
			table.insert(s.fingers_list,
				{ id = fid, x = x, y = y, px = px, py = py })
		end
		return
	end
	local v, k = s:lookup(fid)
	if k then
		table.remove(s.fingers_list, k)
	end
end

function finger:list()
	local s = self
	local new = {}
	for k, v in std.ipairs(s.fingers_list) do
		local x, y, pressure = instead.finger_pos(v.id)
		if x then
			v.x, v.y, v.pressure = x, y, pressure
			table.insert(new, v)
		end
	end
	s.fingers_list = new
	return new
end

function finger:get(fid)
	local s = self
	local v, k
	local x, y, pressure = instead.finger_pos(fid)
	if not x then
		v, k = s:lookup(fid)
		if k then
			table.remove(s.fingers_list, k)
		end
		return
	end
	return x, y, pressure
end

function finger:lookup(fid)
	local s = self
	for k, v in std.ipairs(s.fingers_list) do
		if v.id == fid then
			return v, k
		end
	end
end

function input:finger(press, fid, x, y, px, py, ...)
	local a
	finger:event(press, fid, x, y, px, py, ...)
	if not finger:filter(press, fid, x, y, px, py, ...) then
		return
	end
	for k, v in std.ipairs {press, fid, x, y, px, py, ...} do
		a = (a and (a..', ') or ' ') .. std.dump(v)
	end
	return '@finger'.. (a or '')
end

std.mod_cmd(function(cmd)
	if cmd[1] ~= '@finger' then
		return
	end
	local r, v
	r, v = std.call(std.here(), 'onfinger', cmd[2], cmd[3], cmd[4], cmd[5], cmd[6], cmd[7])
	if not r and not v then
		r, v = std.call(std.game, 'onfinger', cmd[2], cmd[3], cmd[4], cmd[5], cmd[6], cmd[7])
	end
	if not r and not v then -- nothing todo
		return nil, false
	end
	return r, v
end)
