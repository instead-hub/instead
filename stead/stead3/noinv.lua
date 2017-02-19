local std = stead

std.mod_step(function(state)
	if not state then
		return
	end
	if std.here().noinv and not std.me().__inv_disabled then
		std.me():inventory():disable()
		std.me().__inv_disabled = true
	elseif std.me().__inv_disabled and not std.here().noinv then
		std.me():inventory():enable()
		std.me().__inv_disabled = nil
	end
end)

std.mod_done(function()
	if std.me().__inv_disabled then
		std.me():inventory():enable()
		std.me().__inv_disabled = nil
	end
end)
