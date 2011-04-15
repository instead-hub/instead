stead.sound_load = sound_load
stead.sound_free = sound_free
stead.sounds_free = sounds_free
stead.sound_channel = sound_channel
stead.sound_volume = sound_volume
stead.sound_panning = sound_panning

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
		if type(s) ~= 'number' then
			return stead.is_sound()
		end
		return stead.sound_channel(s,...)
	end;
	pan = function(...)
		return stead.sound_panning(...)
	end;
	vol = function(...)
		return stead.sound_volume(...)
	end
}

stead.module_init(function()
	stead.sounds_free();
end)
