require "theme"

click = {
	nam = 'click';
	object_type = true;
	system_type = true;
	bg = false;
	press = false;
	save = function(self, name, h, need)
		local s = stead.tostring(self.bg)
		h:write(stead.string.format("click[%q] = %s;\n", 'bg', s))
		s = stead.tostring(self.press)
		h:write(stead.string.format("click[%q] = %s;\n", 'press', s))
	end;
}

stead.module_init(function()
	input.click = stead.hook(input.click, 
	function(f, s, press, mb, x, y, px, py, ...)
		local cmd = 'click '
		if press or click.press then

			cmd = cmd..tostring(press)..',';

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
function(f, s, cmd, press, x, y, px, py, ...)
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
			s = here()
		end
		if press == 'true' then
			press = true
		else
			press = false
		end
		if s.click then
			if click.press then
				r,v = call(s, 'click', press, tonumber(x), tonumber(y), x2, y2, ...);
			else
				r,v = call(s, 'click', tonumber(x), tonumber(y), x2, y2, ...);
			end
		end
		return r,v
	end
	return f(s, cmd, x, y, px, py, ...)
end)
-- vim:ts=4
