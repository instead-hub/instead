require "timer"
require "theme"

function fading(where, step, bg)
	fading_room.where = where
	fading_room.step = step
	fading_room._bg = bg
	goto 'fading_room'
end

game.fading = stead.hook(game.fading, 
	function(f, s, ...)
		local r,v = f(s, unpack(arg))
		if r and ( here() == fading_room or from() == fading_room ) then
			return true, fading_room.step
		end
		return r,v
	end)

fading_room = room {
	nam = '';
	var { where = 'main' };
	var { step = 4 };
	entered = function(s)
		if s._bg then
			s._saved_bg = theme.get 'scr.gfx.bg';
			theme.gfx.bg(s._bg);
		end
		s._timer = timer:get();
		timer:set(1);
	end;
	exit = function(s)
		if s._bg then
			theme.gfx.bg(s._saved_bg)
		end
	end;
	timer = function(s)
		timer:set(s._timer);
		goto(s.where)
	end
}
