input.click = function(s, press, mb, x, y, px, py)
	if press and px then
		return "click "..px..','..py;
	end
end

game.action = hook(game.action, 
function(f, s, cmd, x, y, ...)
	if cmd == 'click' then
		if here().click then
			return call(here(), 'click', x, y, unpack(arg));
		end
		if game.click then
			return call(game, 'click', x, y, unpack(arg));
		end
		return nil,true
	end
	return f(s, cmd, x, y, unpack(arg))
end)

