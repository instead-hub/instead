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
	text_size = function(fnt, spr)
		return stead.sprite_text_size(fnt, spr);
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
		local t = 'blank:'..stead.tostr(w)..'x'..stead.tostr(h)
		return stead.sprite_load(t, ...)
	end;
	box = function(w, h, col, alpha, ...)
		local t = 'box:'..stead.tostr(w)..'x'..stead.tostr(h)
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
}

stead.module_init(function()
	stead.sprites_free();
end)
