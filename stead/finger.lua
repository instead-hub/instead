require "theme"

finger = {
	nam = 'finger';
	object_type = true;
	system_type = true;
	fingers_list = { };
	add = function(s, press, fid, x, y, px, py)
		if press then
			local v = s:lookup(fid)
			if v then
				v.x, v.y, v.px, v.py = x, y, px, py
			else
				stead.table.insert(s.fingers_list, 
					{ id = fid, x = x, y = y, px = px, py = py })
			end
			return
		end
		local v, k
		v, k = s:lookup(fid)
		if k then
			stead.table.remove(s.fingers_list, k)
			return
		end 
	end;
	list = function(s)
		local k,v
		local new = {}
		for k,v in stead.ipairs(s.fingers_list) do
			local x, y, pressure
			x, y, pressure = stead.finger_pos(v.id)
			if x then
				v.x, v.y, v.pressure = x, y, pressure
				stead.table.insert(new, v)
			end
		end
		s.fingers_list = new
		return new
	end;
	get = function(s, fid)
		local v,k
		local x, y, pressure
		x, y, pressure = stead.finger_pos(fid)
		if not x then
			v, k = s:lookup(fid)
			if k then
				stead.table.remove(s.fingers_list, k)
			end
			return
		end
		return x, y, pressure
	end;
	lookup = function(s, fid)
		local k,v
		for k,v in stead.ipairs(s.fingers_list) do
			if v.id == fid then
				return v, k
			end
		end
	end;
}

stead.module_init(function()
	input.finger = function(s, press, fid, x, y, px, py, ...)
		finger:add(press, fid, x, y, px, py, ...)
		local cmd = 'finger '
		cmd = cmd..stead.tostr(press)..','..stead.tostr(fid)..',' .. x .. ','.. y;
		if px then
			cmd = cmd .. ',' .. px .. ',' .. py
		end
		return cmd
	end
end)

game.action = stead.hook(game.action, 
function(f, s, cmd, press, fid, x, y, px, py, ...)
	if cmd == 'finger' then
		local r,v
		local x2 = px
		local y2 = py

		if stead.here().finger then
			s = stead.here()
		end

		if not s.finger then
			return
		end

		if stead.tonum(px) then
			x2 = stead.tonum(px)
		end

		if stead.tonum(py) then
			y2 = stead.tonum(py)
		end

		if press == 'true' then
			press = true
		else
			press = false
		end
		r,v = stead.call(s, 'finger', press, fid, stead.tonum(x), stead.tonum(y), x2, y2, ...);
		return r,v
	end
	return f(s, cmd, press, fid, x, y, px, py, ...)
end)
-- vim:ts=4
