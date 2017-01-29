require 'kbd'

stead.module_init(function()
	hook_keys('1','2','3','4','5','6','7','8','9','0');
end)

game.kbd = stead.hook(game.kbd, function(f, s, down, key, ...)
	if down and key >= '0' and key <= '9' then
		if isDialog(stead.here()) then
			if key == '0' then key = '10' end
			local p = stead.here():srch(key);
			if p and stead.nameof(p) == key then
				return stead.call(p, 'act');
			end
		end
		return
	end
	return f(s, down, key, ...);
end)

-- vim:ts=4
