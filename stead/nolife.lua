game.life = stead.hook(game.life, function(f, ...)
	if stead.call_bool(game, 'nolife') then
		return
	end
	if stead.call_bool(here(), 'nolife') then
		return
	end
	return f(...)
end)
