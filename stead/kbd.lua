game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_kbd' then
		local r,v = call(s, 'kbd', 
			input.key_event.down, input.key_event.key);
		if r == nil and v == nil then
			return nil, true-- nothing to do
		end
		return r,v
	end
	return f(s, cmd, unpack(arg));
end)

stead.module_init(function()
	input.key = stead.hook(input.key, function(f, s, down, key, ...)
		if input._key_hooks[key] then
			input.key_event = { key = key, down = down };
			return 'user_kbd'
		end
		return f(s, down, key, unpack(arg))
	end)
	input._key_hooks = {}
end)

function hook_keys(...)
	local i
	for i = 1, stead.table.maxn(arg) do
		input._key_hooks[tostring(arg[i])] = true;
	end
end

function unhook_keys(...)
	local i
	for i = 1, stead.table.maxn(arg) do
		input._key_hooks[tostring(arg[i])] = nil;
	end
end

-- vim:ts=4
