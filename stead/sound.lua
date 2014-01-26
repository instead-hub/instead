stead.sound_load = instead_sound_load
stead.sound_free = instead_sound_free
stead.sounds_free = instead_sounds_free
stead.sound_channel = instead_sound_channel
stead.sound_volume = instead_sound_volume
stead.sound_panning = instead_sound_panning

sound = {
	nam = 'sound';
	object_type = true;
	system_type = true;
	load = function(fname)
		return stead.sound_load(fname);
	end;
	free = function(key)
		return stead.sound_free(key);
	end;
	play = function(key, ...)
		return stead.add_sound(key, ...)
	end;
	stop = function(...)
		stead.stop_sound(...);
	end;
	playing = function(s,...)
		if stead.type(s) ~= 'number' then
			return stead.is_sound()
		end
		return stead.sound_channel(s,...)
	end;
	pan = function(c, l, r, ...)
		return stead.sound_panning(c, l, r, ...)
	end;
	vol = function(v, ...)
		return stead.sound_volume(v, ...)
	end
}

stead.module_init(function()
	stead.sounds_free();
end)
