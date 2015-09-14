require "theme"

click = {
	nam = 'click';
	object_type = true;
	system_type = true;
	bg = false;
	press = false;
	button = false;
	save = function(self, name, h, need)
		local s = stead.tostring(self.bg)
		h:write(stead.string.format("click[%q] = %s;\n", 'bg', s))
		s = stead.tostring(self.press)
		h:write(stead.string.format("click[%q] = %s;\n", 'press', s))
		s = stead.tostring(self.button)
		h:write(stead.string.format("click[%q] = %s;\n", 'button', s))
	end;
}

stead.module_init(function()
	input.click = stead.hook(input.click, 
	function(f, s, press, mb, x, y, px, py, ...)
		local cmd = 'click '
		local act = false
		if ( press or click.press ) and ( mb == 1 or click.button ) then
			cmd = cmd..stead.tostr(press)..','..stead.tostr(mb);

			if click.bg or theme.get 'scr.gfx.mode' == 'direct' then
				act = true
				cmd = cmd .. ',' .. x .. ','.. y
			end

			if px then
				act = true
				cmd = cmd .. ',' .. px .. ',' .. py
			end

			if act then
				return cmd
			end
		end
		return f(s, press, mb, x, y, px, py, ...)
	end)
end)

game.action = stead.hook(game.action, 
function(f, s, cmd, press, mb, x, y, px, py, ...)
	if cmd == 'click' then
		local r,v
		local x2 = px
		local y2 = py

		if stead.tonum(mb) then
			mb = stead.tonum(mb)
		end

		if stead.tonum(px) then
			x2 = stead.tonum(px)
		end

		if stead.tonum(py) then
			y2 = stead.tonum(py)
		end

		if stead.here().click then
			s = stead.here()
		end

		if press == 'true' then
			press = true
		else
			press = false
		end

		if s.click then
			if click.press then
				if click.button then
					r,v = stead.call(s, 'click', press, mb, stead.tonum(x), stead.tonum(y), x2, y2, ...);
				else
					r,v = stead.call(s, 'click', press, stead.tonum(x), stead.tonum(y), x2, y2, ...);
				end
			else
				if click.button then
					r,v = stead.call(s, 'click', mb, stead.tonum(x), stead.tonum(y), x2, y2, ...);
				else
					r,v = stead.call(s, 'click', stead.tonum(x), stead.tonum(y), x2, y2, ...);
				end
			end
		end
		if r == nil and v == nil and not stead.api_atleast(1, 3, 5) then
			return nil, true
		end
		return r,v
	end
	return f(s, cmd, press, mb, x, y, px, py, ...)
end)
-- vim:ts=4
