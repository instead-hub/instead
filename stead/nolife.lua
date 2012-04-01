game.life = stead.hook(game.life, function(f, ...)
	if stead.call_bool(here(), 'nolife') then
		return
	end
	return f(...)
end)
