-- call here().click or game.click on pic clicks

input.click = function(s, press, mb, x, y, px, py)
	if press and px then
		return "__mclick__ "..mb..','..px..','..py;
	end
end

game.action = function(s, cmd,...)
	if cmd == '__mclick__' then
		local r
		local v 
		v, r = call(here(), 'click', unpack(arg));
		if not v and r ~= true then
			v, r = call(ref(game), 'click', unpack(arg));
		end
		if not v then
			return nil, true
		end
		return v, r
	end
	return nil,true
end

