stead.sound_load = sound_load
stead.sound_free = sound_free
stead.sounds_free = sounds_free
stead.sound_channel = sound_channel
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
	channel = function(...)
		return stead.sound_channel(...)
	end
}

stead.module_init(function()
	stead.sounds_free();
end)
