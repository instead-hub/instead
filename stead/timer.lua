game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_timer' then
		local r,v
		if here().timer then
			r,v = call(here(), 'timer');
		elseif s.timer then
			r,v = call(s, 'timer');
		end
		if r == nil and v == nil then
			return nil, true
		end
		return r,v
	end
	return f(s, cmd, ...);
end)

stead.module_init(function()
	timer.callback = function(s)
		return 'user_timer'
	end
end)

-- vim:ts=4
