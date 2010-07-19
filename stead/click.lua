input.click = stead.hook(input.click, 
function(f, s, press, mb, x, y, px, py, ...)
	if press and px then
		return "click "..px..','..py;
	end
	return f(s, press, mb, x, y, px, py, unpack(arg))
end)

game.action = stead.hook(game.action, 
function(f, s, cmd, x, y, ...)
	if cmd == 'click' then
		local r,v
		if here().click then
			r,v = call(here(), 'click', x, y, unpack(arg));
		elseif s.click then
			r,v = call(s, 'click', x, y, unpack(arg));
		end
		if r == nil and v == nil then
			return nil, true
		end
		return r,v
	end
	return f(s, cmd, x, y, unpack(arg))
end)
-- vim:ts=4
