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
		if here().click then
			return call(here(), 'click', x, y, unpack(arg));
		end
		if s.click then
			return call(s, 'click', x, y, unpack(arg));
		end
		return nil,true
	end
	return f(s, cmd, x, y, unpack(arg))
end)
-- vim:ts=4
