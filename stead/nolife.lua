game.life = stead.hook(game.life, function(f, ...)
	if stead.call_bool(stead.here(), 'nolife') then
		return
	end
	return f(...)
end)
