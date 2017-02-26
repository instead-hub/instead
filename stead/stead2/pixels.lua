require "sprites"

local pixels_mt = {
}

local font_mt = {
	__gc = function(s)
		stead.font_free(s.fnt)
	end;
	__tostring = function(s)
		return s.fnt
	end;
}

local pnew = function(p)
	if stead.type(p) ~= 'userdata' then
		return
	end
	local t = getmetatable(p).__index 
	setmetatable(t, pixels_mt)
	return p
end

local fnew = function(f)
	if stead.type(f) ~= 'string' then
		return
	end
	local fn = {
		fnt = f;
		save = function() end;
	}
	setmetatable(fn, font_mt)
	return fn
end

local font_m = {
	text = function(s, text, col, style, ...)
		return pnew(stead.sprite_pixels(stead.sprite_text(s.fnt, text, col, style, ...)))
	end;
	size = function(s, ...)
		return stead.sprite_text_size(s.fnt, ...);
	end;
}

local pixels_m = {
	save = function() end;
	dup = function(self)
		local w, h, s = self:size()
		local p = stead.sprite_pixels(w, h, s)
		if p then
			self:copy(p)
		end
		return pnew(p)
	end;
}

font_mt.__index = font_m
pixels_mt.__index = pixels_m

pixels = {
	nam = 'pixels';
	object_type = true;
	system_type = true;
	fnt = function(name, sz, ...)
		if not stead.tonum(sz) then
			error("No font size specified.", 2)
		end
		return fnew(stead.font_load(name, -sz, ...))
	end;
	new = function(...)
		return pnew(stead.sprite_pixels(...))
	end;
}
