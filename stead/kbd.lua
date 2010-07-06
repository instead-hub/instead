game.action = stead.hook(game.action, function(f, s, cmd, ...)
	if cmd == 'user_kbd' then
		local r,v = call(game, 'kbd', 
			input.key_event.down, input.key_event.key);
		if r == nil and v == nil then
			return nil, true-- nothing to do
		end
		return r,v
	end
	return f(cmd, unpack(arg));
end)

input.key = stead.hook(input.key, function(f, s, down, key, ...)
	local k,v
	for k,v in ipairs(input.key_hooks) do
		if v == key then
			input.key_event = { key = key, down = down };
			return 'user_kbd'
		end
	end
	return f(s, down, key, unpack(arg))
end)

function hook_keys(...)
	local i
	for i = 1, stead.table.maxn(arg) do
		stead.table.insert(input.key_hooks, tostring(arg[i]));
	end
end

stead.module_init(function()
	input.key_hooks = {}
end)

-- vim:ts=4

