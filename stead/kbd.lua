game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_kbd' then
		local r,v;
		if stead.here().kbd then
			r,v = stead.call(stead.here(), 'kbd', 
			input.key_event.down, input.key_event.key);
		elseif s.kbd then
			r,v = stead.call(s, 'kbd', 
			input.key_event.down, input.key_event.key);
		end
		if r == nil and v == nil and not stead.api_atleast(1, 3, 5) then
			return nil, true
		end
		return r,v
	end
	return f(s, cmd, ...);
end)

stead.module_init(function()
	input.key = stead.hook(input.key, function(f, s, down, key, ...)
		if input._key_hooks[key] then
			input.key_event = { key = key, down = down };
			return 'user_kbd'
		end
		return f(s, down, key, ...)
	end)
	input._key_hooks = {}
end)

stead.hook_keys = function(...)
	local i
	local a = {...};
	for i = 1, stead.table.maxn(a) do
		input._key_hooks[tostring(a[i])] = true;
	end
end

stead.unhook_keys = function(...)
	local i
	local a = {...};
	for i = 1, stead.table.maxn(a) do
		input._key_hooks[tostring(a[i])] = nil;
	end
end

hook_keys = stead.hook_keys
unhook_keys = stead.unhook_keys
-- vim:ts=4
