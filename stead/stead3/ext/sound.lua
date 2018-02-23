-- raw interface to sound

local std = stead

local instead = std.ref '@instead'

-- luacheck: globals instead_sound_load
-- luacheck: globals instead_sound_free
-- luacheck: globals instead_sounds_free
-- luacheck: globals instead_sound_channel
-- luacheck: globals instead_sound_volume
-- luacheck: globals instead_sound_panning
-- luacheck: globals instead_sound_load_mem
-- luacheck: globals instead_music_callback
-- luacheck: globals instead_sound

instead.sound_load = instead_sound_load
instead.sound_free = instead_sound_free
instead.sounds_free = instead_sounds_free
instead.sound_channel = instead_sound_channel
instead.sound_volume = instead_sound_volume
instead.sound_panning = instead_sound_panning
instead.sound_load_mem = instead_sound_load_mem
instead.music_callback = instead_music_callback
instead.is_sound = instead_sound


function instead.get_music()
	return instead.__music, instead.__music_loop
end

function instead.set_music(mus, loop)
	instead.__music = mus or nil
	instead.__music_loop = loop or 0
end

function instead.get_music_fading()
	return instead.__music_fadeout, instead.__music_fadein
end

function instead.set_music_fading(o, i)
	if not i then i = o end
	if o == 0 or not o then o = -1 end
	if i == 0 or not i then i = -1 end
	instead.__music_fadeout = o
	instead.__music_fadein = i
end

function instead.finish_music()
	if (instead.__music_loop or 0) == 0 then
		return false
	end
	instead.__music_loop = -1
	return true
end

function instead.get_sound()
	return instead.__sound, instead.__sound_channel, instead.__sound_loop
end

function instead.add_sound(s, chan, loop)
	if type(s) ~= 'string' then
		std.err("Wrong parameter to instead.add_sound()", 2)
	end
	if type(instead.__sound) ~= 'string' then
		return instead.set_sound(s, chan, loop)
	end
	if std.tonum(chan) then
		s = s..'@'..std.tostr(chan);
	end
	if std.tonum(loop) then
		s = s..','..std.tostr(loop)
	end
	instead.set_sound(instead.__sound..';'..s, instead.__sound_channel, instead.__sound);
end

function instead.set_sound(sound, chan, loop)
	instead.__sound = sound
	instead.__sound_loop = loop or 1
	instead.__sound_channel = chan or -1
end

function instead.stop_sound(chan, fo)
	local str = '@-1'

	if (chan and type(chan) ~= 'number') or (fo and type(fo) ~= 'number') then
		std.err("Wrong parameter to instead.stop_sound", 2)
	end

	if chan then
		str = '@'..std.tostr(chan)
	end

	if fo then
		str = str .. ',' .. std.tostr(fo)
	end
	return instead.add_sound(str);
end

function instead.stop_music()
	instead.set_music(nil, -1);
end

std.mod_done(function(_)
	instead.music_callback() -- halt music mixer
	instead.stop_music()
	instead.stop_sound() -- halt all
--	instead.sounds_free();
end)

local sounds = {}

std.mod_cmd(function(_)
	if std 'game':time() > 0 then
		sounds = {}
		instead.set_sound(); -- empty sound
	end
end)

-- aliases
local snd = {
	__gc = function(s)
		instead.sound_free(s.snd)
	end;
	__tostring = function(s)
		return s.snd
	end
}
snd.__index = snd;

function snd:play(...)
	if self.snd then
		instead.add_sound(self.snd, ...)
	end
end

function snd:new(a, b, t)
	local o = {
		__save = function() end;
	}
	if type(a) == 'string' then
		o.snd = instead.sound_load(a);
	elseif type(t) == 'table' then
		o.snd = instead.sound_load_mem(a, b, t) -- hz, channel, t
	end
--	if not o.snd then
--		return
--	end
	std.setmt(o, self)
	return std.proxy(o)
end

local sound = std.obj {
	nam = '@snd';
}

sound.set = instead.set_sound
sound.play = instead.add_sound
sound.stop = instead.stop_sound
sound.music = function(mus, loop)
	if mus == nil and loop == nil then
		return instead.get_music()
	end
	return instead.set_music(mus, loop)
end

sound.stop_music = instead.stop_music
sound.music_fading = function(o, i)
	if o == nil and i == nil then
		return instead.get_music_fading()
	end
	return instead.set_music_fading(o, i)
end

function sound.new(...)
	local s = snd:new(...)
	std.table.insert(sounds, s) -- avoid __gc in this step
	return s
end

function sound.music_callback(...)
	return instead.music_callback(...)
end

function sound.free(key)
	return instead.sound_free(key);
end

function sound.music_playing()
	return instead.__music ~= nil and instead.__music_loop ~= -1
end

function sound.playing(s,...)
	if type(s) ~= 'number' then
		return instead.is_sound()
	end
	return instead.sound_channel(s,...)
end

function sound.pan(c, l, r, ...)
	return instead.sound_panning(c, l, r, ...)
end

function sound.vol(v, ...)
	return instead.sound_volume(v, ...)
end
