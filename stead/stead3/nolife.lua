local std = stead

std.mod_step(function(state)
	if not state then
		return
	end
	if std.here().nolife and std.game:live() then
		std.game:lifeoff()
	elseif not std.game:live() and not std.here().nolife then
		std.game:lifeon()
	end
end)

std.mod_done(function()
	if not std.game:live() then
		std.game:lifeon()
	end
end)
