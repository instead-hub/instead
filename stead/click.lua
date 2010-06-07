input.click = function(s, press, mb, x, y, px, py)
	if press and px then
		return "click "..px..','..py;
	end
end

game.action = hook(game.action, 
function(f, s, cmd, x, y, ...)
	if cmd == 'click' then
		return call(here(), 'click', x, y);
	end
	if f then return f(s, cmd, x, y, unpack(arg)) end
end)

