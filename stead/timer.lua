game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_timer' then
		return call(s, 'timer');
	end
	return f(s, cmd, unpack(arg));
end)

timer.callback = function(s)
	return 'user_timer'
end

-- vim:ts=4
