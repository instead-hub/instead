require "theme"

stead.font_load = instead_font_load
stead.font_free = instead_font_free
stead.font_scaled_size = instead_font_scaled_size
stead.sprite_alpha = instead_sprite_alpha
stead.sprite_dup = instead_sprite_dup
stead.sprite_scale = instead_sprite_scale
stead.sprite_rotate = instead_sprite_rotate
stead.sprite_text = instead_sprite_text
stead.sprite_text_size = instead_sprite_text_size
stead.sprite_draw = instead_sprite_draw
stead.sprite_copy = instead_sprite_copy
stead.sprite_compose = instead_sprite_compose
stead.sprite_fill = instead_sprite_fill
stead.sprite_pixel = instead_sprite_pixel
stead.sprite_load = instead_sprite_load
stead.sprite_free = instead_sprite_free
stead.sprite_size = instead_sprite_size
stead.sprites_free = instead_sprites_free
stead.sprite_colorkey = instead_sprite_colorkey
stead.sprite_pixels = instead_sprite_pixels

local get = function(s)
	if type(s) == 'userdata' then
		return s
	end
	return stead.tostr(s)
end


local sprite_mt = {
	__gc = function(s)
		stead.sprite_free(s.spr)
	end;
	__tostring = function(s)
		return s.spr
	end;
}

local new = function(s)
	if type(s) ~= 'string' then
		return
	end
	local sp = {
--		nam = s;
		spr = s;
--		object_type = true;
		save = function() end;
	}
	setmetatable(sp, sprite_mt)
	return sp
end

local font_mt = {
	__gc = function(s)
		stead.font_free(s.fnt)
	end;
	__tostring = function(s)
		return s.fnt
	end;
}

local fnew = function(f)
	if type(f) ~= 'string' then
		return
	end
	local fn = {
--		nam = f;
		fnt = f;
--		object_type = true;
		save = function() end;
	}
	setmetatable(fn, font_mt)
	return fn
end

local font_m = {
	text = function(s, text, col, style, ...)
		return new(stead.sprite_text(s.fnt, text, col, style, ...))
	end;
	size = function(s, ...)
		return stead.sprite_text_size(s.fnt, ...);
	end;
}

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

sprite_mt.__index = sprite_m
font_mt.__index = font_m

local screen = new('screen')

sprite = {
	nam = 'sprites';
	object_type = true;
	system_type = true;

	screen = function(s)
		if theme.get 'scr.gfx.mode' ~= 'direct' then
			return
		end
		return 'screen'
	end;

	font_scaled_size = function(size)
		return stead.font_scaled_size(size);
	end;
	font = function(font, size, ...)
		return stead.font_load(font, size, ...);
	end;
	free_font = function(font)
		return stead.font_free(font);
	end;
	font_height = function(font)
		local w,h = stead.sprite_text_size(font)
		return h
	end;
	alpha = function(name, alpha, ...)
		return stead.sprite_alpha(name, alpha, ...);
	end;
	colorkey = function(name, color, ...)
		return stead.sprite_colorkey(name, color, ...);
	end;
	dup = function(name, ...)
		return stead.sprite_dup(name, ...);
	end;
	scale = function(name, xs, ys, smooth, ...)
		if smooth == nil then
			smooth = true -- default is on
		end
		return stead.sprite_scale(name, xs, ys, smooth,...);
	end;
	rotate = function(name, angle, smooth, ...)
		if smooth == nil then
			smooth = true -- default is on
		end
		return stead.sprite_rotate(name, angle, smooth, ...);
	end;
	text = function(font, text, col, style, ...)
		return stead.sprite_text(font, text, col, style, ...);
	end;
	size = function(spr)
		return stead.sprite_size(spr);
	end;
	text_size = function(fnt, ...)
		return stead.sprite_text_size(fnt, ...);
	end;
	draw = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_draw(s, 0, 0, -1, -1, fx, fy, fw, fh);
		end
		return stead.sprite_draw(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	copy = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_copy(s, 0, 0, -1, -1, fx, fy, fw, fh);
		end
		return stead.sprite_copy(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	compose = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_compose(s, 0, 0, -1, -1, fx, fy, fw, fh);
		end
		return stead.sprite_compose(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	fill = function(d, x, y, w, h, col)
		if h == nil and col == nil then
			return stead.sprite_fill(d, 0, 0, -1, -1, x);
		end 
		return stead.sprite_fill(d, x, y, w, h, col);
	end;
	pixel = function(d, x, y, col, alpha)
		return stead.sprite_pixel(d, x, y, col, alpha)
	end;
	load = function(fname, ...)
		return stead.sprite_load(fname, ...);
	end;
	blank = function(w, h, ...)
		local t = 'blank:'..stead.tostr(stead.math.floor(w))..'x'..stead.tostr(stead.math.floor(h))
		return stead.sprite_load(t, ...)
	end;
	box = function(w, h, col, alpha, ...)
		local t = 'box:'..stead.tostr(stead.math.floor(w))..'x'..stead.tostr(stead.math.floor(h))
		if col then
			t = t..','..stead.tostr(col)
		end
		if alpha then
			t = t..','..stead.tostr(alpha)
		end
		return stead.sprite_load(t, ...)
	end;
	free = function(key)
		return stead.sprite_free(key);
	end;
-- new api
	new = function(w, h, ...)
		if stead.tonum(w) and stead.tonum(h) then
			local t = 'blank:'..stead.tostr(stead.math.floor(w))..'x'..stead.tostr(stead.math.floor(h))
			return new(stead.sprite_load(t))
		end
		return new(stead.sprite_load(w, h, ...))
	end;
	fnt = function(name, sz, ...)
		if not tonumber(sz) then
			error("No font size specified.", 2)
		end
		return fnew(stead.font_load(name, sz, ...))
	end;
	scr = function()
		return screen
	end;
	direct = function(v)
		local ov = theme.get('scr.gfx.mode') == 'direct'
		if v then
			if ov then
				return true
			end
			theme.set ('scr.gfx.mode', 'direct')
			return theme.get('scr.gfx.mode') == 'direct'
		elseif v == false then
			if ov then
				theme.reset ('scr.gfx.mode')
			end
			return true
		end
		return ov
	end;
}

stead.module_done(function()
	stead.sprites_free();
end)

local function compat_api()
	if not stead.api_atleast(1, 7, 1) then
		font_load = instead_font_load
		font_free = instead_font_free
		font_scaled_size = instead_font_scaled_size

		sprite_alpha = instead_sprite_alpha
		sprite_dup = instead_sprite_dup
		sprite_scale = instead_sprite_scale
		sprite_rotate = instead_sprite_rotate
		sprite_text = instead_sprite_text
		sprite_text_size = instead_sprite_text_size
		sprite_draw = instead_sprite_draw
		sprite_copy = instead_sprite_copy
		sprite_compose = instead_sprite_compose
		sprite_fill = instead_sprite_fill
		sprite_pixel = instead_sprite_pixel
		sprite_load = instead_sprite_load
		sprite_free = instead_sprite_free
		sprite_size = instead_sprite_size
		sprites_free = instead_sprites_free
	end
end

stead.module_start(function(load)
	if compat_api and not load then
		compat_api()
		compat_api = nil
	end
end)
