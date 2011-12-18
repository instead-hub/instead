-- example module
require "timer"
require "theme"

function fading_goto(where, step, bg)
	fading_room.where = where
	fading_room.step = game.gui.fading;
	if step then fading_room.step = step end
	fading_room._bg = bg
	return walk 'fading_room'
end

game.fading = stead.hook(game.fading, 
	function(f, s, ...)
		local r,v = f(s, unpack(arg))
		if r and fading_room.active then
			if from() == fading_room then
				fading_room.active = false
			end
			return true, fading_room.step
		end
		return r,v
	end)

fading_room = room {
	nam = true;
	var { active = false };
	var { where = 'main' };
	var { step = 4 };
	pic = true;
	entered = function(s)
		s.active = true;
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
		walk(s.where)
	end
}
