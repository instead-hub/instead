require "theme"

click = {
	nam = 'click';
	object_type = true;
	system_type = true;
	bg = false;
	save = function(self, name, h, need)
		local s = stead.tostring(self.bg)
		h:write(stead.string.format("click[%q] = %s;\n", 'bg', s))
	end;
}

stead.module_init(function()
	input.click = stead.hook(input.click, 
	function(f, s, press, mb, x, y, px, py, ...)
		local cmd = 'click '
		if press then
			if click.bg or theme.get 'scr.gfx.mode' == 'direct' then
				cmd = cmd .. x .. ','.. y
				if px then
					cmd = cmd .. ','
				end
			end
			if px then
				cmd = cmd .. px .. ',' .. py
			end
			if cmd ~= 'click ' then
				return cmd
			end
		end
		return f(s, press, mb, x, y, px, py, ...)
	end)
end)

game.action = stead.hook(game.action, 
function(f, s, cmd, x, y, px, py, ...)
	if cmd == 'click' then
		local r,v
		local x2 = px
		local y2 = py

		if tonumber(px) then
			x2 = tonumber(px)
		end

		if tonumber(py) then
			y2 = tonumber(py)
		end

		if here().click then
			r,v = call(here(), 'click', tonumber(x), tonumber(y), x2, y2, ...);
		elseif s.click then
			r,v = call(s, 'click', tonumber(x), tonumber(y), x2, y2, ...);
		end
		return r,v
	end
	return f(s, cmd, x, y, px, py, ...)
end)
-- vim:ts=4
