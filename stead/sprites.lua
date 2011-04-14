require "theme"

stead.font_load = font_load
stead.font_free = font_free
stead.font_scaled_size = font_scaled_size
stead.sprite_alpha = sprite_alpha
stead.sprite_dup = sprite_dup
stead.sprite_scale = sprite_scale
stead.sprite_rotate = sprite_rotate
stead.sprite_text = sprite_text
stead.sprite_text_size = sprite_text_size
stead.sprite_draw = sprite_draw
stead.sprite_copy = sprite_copy
stead.sprite_fill = sprite_fill
stead.sprite_load = sprite_load
stead.sprite_free = sprite_free
stead.sprite_size = sprite_size
stead.sprites_free = sprites_free

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
	font = function(font, size)
		return stead.font_load(font, size);
	end;
	free_font = function(font)
		return stead.font_free(key);
	end;
	font_height = function(font)
		local w,h = stead.sprite_text_size(font)
		return h
	end;
	alpha = function(name, alpha)
		return stead.sprite_alpha(name, alpha);
	end;
	dup = function(name)
		return stead.sprite_dup(name);
	end;
	scale = function(name, xs, ys)
		return stead.sprite_scale(name, xs, ys);
	end;
	rotate = function(name, angle)
		return stead.sprite_rotate(name, angle);
	end;
	text = function(font, text, col, style)
		return stead.sprite_text(font, text, col, style);
	end;
	size = function(spr)
		return stead.sprite_size(spr);
	end;
	text_size = function(fnt, spr)
		return stead.sprite_text_size(fnt, spr);
	end;
	draw = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_draw(s, 0, 0, 0, 0, fx, fy, fw, fh);
		end
		return stead.sprite_draw(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	copy = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return stead.sprite_copy(s, 0, 0, 0, 0, fx, fy, fw, fh);
		end
		return stead.sprite_copy(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	fill = function(d, x, y, w, h, col)
		if h == nil and col == nil then
			return stead.sprite_fill(d, 0, 0, 0, 0, x);
		end 
		return stead.sprite_fill(d, x, y, w, h, col);
	end;
	load = function(fname)
		return stead.sprite_load(fname);
	end;
	free = function(key)
		return stead.sprite_free(key);
	end;
}

stead.module_init(function()
	stead.sprites_free();
end)
