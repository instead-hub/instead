local std = stead

std.mod_step(function()
	if std.me():moved() then
		if std.here().nolife and std.game:live() then
			std.game:lifeoff()
		elseif not std.game:live() then
			std.game:lifeon()
		end
	end
end)

std.mod_done(function()
	if not std.game:live() then
		std.game:lifeon()
	end
end)
