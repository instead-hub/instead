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
			if click.bg then
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
function(f, s, cmd, x, y, ...)
	if cmd == 'click' then
		local r,v
		if here().click then
			r,v = call(here(), 'click', x, y, ...);
		elseif s.click then
			r,v = call(s, 'click', x, y, ...);
		end
		if r == nil and v == nil then
			return nil, true
		end
		return r,v
	end
	return f(s, cmd, x, y, ...)
end)
-- vim:ts=4
