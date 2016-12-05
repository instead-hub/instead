-- some stubs for tiny-instead
game.hinting = true

menu = obj
stat = obj

stead.get_music = function()
	return '', 0
end

stead.get_music_loop = function()
	return 0
end

stead.save_music = function(s)
end

stead.restore_music = function(s)
end

stead.set_music = function(s, count)
end

stead.set_music_fading = function(o, i)
end

stead.get_music_fading = function()
end

stead.stop_music = function()
end

stead.is_music = function()
	return false
end

function instead_sound()
	return false
end

stead.is_sound = instead_sound

stead.get_sound = function()
	return 
end

stead.get_sound_chan = function()
	return 0
end

stead.get_sound_loop = function()
	return 0
end

stead.stop_sound = function(chan, fo)
end

stead.add_sound = function(s, chan, loop)
end

stead.set_sound = function(s, chan, loop)
end

-- those are sill in global space
add_sound = stead.add_sound
set_sound = stead.set_sound
stop_sound = stead.stop_sound

get_sound = stead.get_sound
get_sound_loop = stead.get_sound_loop
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

stead.set_timer = function() end

stead.timer = function()
	return
end

stead.module_init(function(s) 
	timer = obj {
		nam = 'timer',
		get = function(s)
			return 0
		end,
		stop = function(s)
		end,
		del = function(s)
		end,
		set = function(s, v)
			return true
		end,
	};
end)
