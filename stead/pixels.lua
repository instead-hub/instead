require "sprites"

local pixels_mt = {
}

local sprite_mt = {
	__gc = function(s)
		stead.sprite_free(s.spr)
	end;
	__tostring = function(s)
		return s.spr
	end;
}

local font_mt = {
	__gc = function(s)
		stead.font_free(s.font)
	end;
	__tostring = function(s)
		return s.font
	end;
}

local get = function(s)
	if type(s) == 'userdata' then
		return s
	end
	return stead.tostr(s)
end

local pnew = function(p)
	if type(p) ~= 'userdata' then
		return
	end
	local t = getmetatable(p).__index 
	setmetatable(t, pixels_mt)
	return p
end

local fnew = function(f)
	if type(f) ~= 'string' then
		return
	end
	local fn = {
		font = f;
	}
	setmetatable(fn, font_mt)
	return fn
end

local new = function(s)
	if type(s) ~= 'string' then
		return
	end
	local sp = {
		spr = s;
	}
	setmetatable(sp, sprite_mt)
	return sp
end

local sprite_m = {
	alpha = function(self, alpha, ...)
		return new(stead.sprite_alpha(self.spr, alpha, ...));
	end;
	colorkey = function(self, color, ...)
		return stead.sprite_colorkey(self.spr, color, ...);
	end;
	dup = function(self, ...)
		return new(stead.sprite_dup(self.spr, ...));
	end;
	scale = function(self, xs, ys, smooth, ...)
		if smooth == nil then
			smooth = true -- default is on
		end
		return new(stead.sprite_scale(self.spr, xs, ys, smooth,...));
	end;
	rotate = function(self, angle, smooth, ...)
		if smooth == nil then
			smooth = true -- default is on
		end
		return new(stead.sprite_rotate(self.spr, angle, smooth, ...));
	end;
	size = function(self)
		return stead.sprite_size(self.spr);
	end;
	draw = function(self, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_draw(self.spr, 0, 0, -1, -1, get(fx), fy, fw, fh);
		end
		return stead.sprite_draw(self.spr, fx, fy, fw, fh, get(d), x, y, alpha);
	end;
	copy = function(self, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_copy(self.spr, 0, 0, -1, -1, get(fx), fy, fw, fh);
		end
		return stead.sprite_copy(self.spr, fx, fy, fw, fh, get(d), x, y, alpha);
	end;
	compose = function(self, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_compose(self.spr, 0, 0, -1, -1, get(fx), fy, fw, fh);
		end
		return stead.sprite_compose(self.spr, fx, fy, fw, fh, get(d), x, y, alpha);
	end;
	fill = function(self, x, y, w, h, col)
		if h == nil and col == nil then
			return stead.sprite_fill(self.spr, 0, 0, -1, -1, x);
		end 
		return stead.sprite_fill(self.spr, x, y, w, h, col);
	end;
	pixel = function(self, x, y, col, alpha)
		return stead.sprite_pixel(self.spr, x, y, col, alpha)
	end;
};

local font_m = {
	text = function(s, text, col, style, ...)
		return pnew(stead.sprite_pixels(stead.sprite_text(s.font, text, col, style, ...)))
	end;
	size = function(s, ...)
		return stead.sprite_text_size(s.font, ...);
	end;
}

local pixels_m = {
	dup = function(self)
		local w, h, s = self:size()
		local p = stead.sprite_pixels(w, h, s)
		if p then
			self:copy(p)
		end
		return pnew(p)
	end;
	sprite = function(self) -- make sprite from pixels
		return new(stead.sprite_load(self))
	end;
}

font_mt.__index = font_m
sprite_mt.__index = sprite_m
pixels_mt.__index = pixels_m

pixels = {
	nam = 'pixels';
	object_type = true;
	system_type = true;
	font = function(name, sz, ...)
		if not tonumber(sz) then
			error("No font size specified.", 2)
		end
		return fnew(stead.font_load(name, -sz, ...))
	end;
	sprite = function(...)
		return new(stead.sprite_load(...))
	end;
	new = function(...)
		return pnew(stead.sprite_pixels(...))
	end;
}
