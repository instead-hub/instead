game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_event' then
		local r,v;
		r, v = stead.call(stead, 'event', ... );
		return r,v
	end
	return f(s, cmd, ...);
end)

stead.module_init(function()
	input.event = stead.hook(input.event, function(f, s, ev, ...)
		if type(stead.event) == 'function' then
			return 'user_event,'..tostring(ev)
		end
		return f(s, ev, ...)
	end)
end)

-- vim:ts=4
