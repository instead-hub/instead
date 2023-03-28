stead.get_music = function()
	return game._music, game._music_loop;
end

stead.get_music_loop = function()
	return game._music_loop;
end

stead.save_music = function(s)
	if s == nil then
		s = self
	end
	s.__old_music__ = stead.get_music();
	s.__old_loop__ = stead.get_music_loop();
end

stead.restore_music = function(s)
	if s == nil then
		s = self
	end
	stead.set_music(s.__old_music__, s.__old_loop__);
end

stead.set_music = function(s, count)
	game._music = s;
	if not stead.tonum(count) then
		game._music_loop = 0;
	else
		game._music_loop = stead.tonum(count);
	end
end

stead.set_music_fading = function(o, i)
	if o and o == 0 then o = -1 end
	if i and i == 0 then i = -1 end

	game._music_fadeout = o
	if not i then
		game._music_fadein = o
	else
		game._music_fadein = i
	end
end

stead.get_music_fading = function()
	return game._music_fadeout, game._music_fadein
end

stead.stop_music = function()
	stead.set_music(nil, -1);
end

stead.is_music = function()
	return game._music ~= nil and game._music_loop ~= -1
end

if instead_sound == nil then
	function instead_sound()
		return false -- sdl-instead export own function
	end
end
stead.is_sound = instead_sound

stead.get_sound = function()
	return game._sound, game._sound_channel, game._sound_loop, game._sound_fading;
end

stead.get_sound_chan = function()
	return game._sound_channel
end

stead.get_sound_loop = function()
	return game._sound_loop
end

stead.get_sound_fading = function()
	return game._sound_fading
end

stead.stop_sound = function(chan, fo)
	if not stead.tonum(chan) then
		if stead.tonum(fo) then
			stead.set_sound('@-1,'..stead.tostr(fo));
		else
			stead.set_sound('@-1');
		end
		return
	end
	if stead.tonum(fo) then
		stead.add_sound('@'..stead.tostr(chan)..','..stead.tostr(fo));
	else
		stead.add_sound('@'..stead.tostr(chan));
	end
end

stead.add_sound = function(s, chan, loop,fading)
	if stead.type(s) ~= 'string' then
		return
	end
	if stead.type(game._sound) == 'string' then
		if stead.tonum(chan) then
			s = s..'@'..stead.tostr(chan);
		end
		if stead.tonum(loop) then
			s = s..','..stead.tostr(loop)
		end
		if stead.tonum(fading) then
			s = s..','..stead.tostr(fading)
		end
		stead.set_sound(game._sound..';'..s, stead.get_sound_chan(), stead.get_sound_loop(),stead.get_sound_fading);
	else
		stead.set_sound(s, chan, loop,fading);
	end
end

stead.set_sound = function(s, chan, loop,fading)
	game._sound = s;
	if not stead.tonum(loop) then
		game._sound_loop = 1;
	else
		game._sound_loop = stead.tonum(loop);
	end
if not stead.tonum(fading) then
		game._sound_fading = 0;
	else
		game._sound_fading = stead.tonum(fading);
	end
	if not stead.tonum(chan) then
		game._sound_channel = -1;
	else
		game._sound_channel = stead.tonum(chan);
	end
end

stead.module_done(function(s) 
	stead.stop_music();
	stead.stop_sound();
end)

stead.module_cmd(function(s) 
	stead.set_sound(); -- empty sound
end)

-- those are sill in global space
add_sound = stead.add_sound
set_sound = stead.set_sound
stop_sound = stead.stop_sound

get_sound = stead.get_sound
get_sound_loop = stead.get_sound_loop
get_sound_fadding =stead.get_sound_fading
get_sound_chan = stead.get_sound_chan

get_music = stead.get_music
get_music_fading = stead.get_music_fading
get_music_loop = stead.get_music_loop

set_music = stead.set_music
set_music_fading = stead.set_music_fading
stop_music = stead.stop_music

save_music = stead.save_music
restore_music = stead.restore_music

is_music = stead.is_music

local function compat_api()
	if not stead.api_atleast(1, 7, 1) then
		is_sound = instead_sound
		sound_load = instead_sound_load
		sound_free = instead_sound_free
		sounds_free = instead_sounds_free
		sound_channel = instead_sound_channel
		sound_volume = instead_sound_volume
		sound_panning = instead_sound_panning
	end
end

stead.module_start(function(load)
	if compat_api and not load then
		compat_api()
		compat_api = nil
	end
end)
