require "theme"
sprite = {
	nam = 'sprites';
	object_type = true;
	system_type = true;

	ini = function(s)
		if type(s.init) == 'function' then
			sprites_free();
			sprite.init();
		end
	end;

	screen = function(s)
		if theme.get 'scr.gfx.mode' ~= 'direct' then
			return
		end
		return 'screen'
	end;
	font = function(font, size)
		return font_load(font, size);
	end;
	free_font = function(font)
		return font_free(key);
	end;
	alpha = function(name, alpha)
		return sprite_alpha(name, alpha);
	end;
	dup = function(name)
		return sprite_dup(name);
	end;
	scale = function(name, xs, ys)
		return sprite_scale(name, xs, ys);
	end;
	rotate = function(name, angle)
		return sprite_rotate(name, angle);
	end;
	text = function(font, text, col, style)
		return sprite_text(font, text, col, style);
	end;
	size = function(spr)
		return sprite_size(spr);
	end;
	text_size = function(fnt, spr)
		return sprite_text_size(fnt, spr);
	end;
	draw = function(s, fx, fy, fw, fh, d, x, y, alpha)
		if d == nil and x == nil and y == nil then
			return sprite_draw(s, 0, 0, 0, 0, fx, fy, fw, fh);
		end
		return sprite_draw(s, fx, fy, fw, fh, d, x, y, alpha);
	end;
	fill = function(d, x, y, w, h, col)
		if h == nil and col == nil then
			return sprite_fill(d, 0, 0, 0, 0, x);
		end 
		return sprite_fill(d, x, y, w, h, col);
	end;
	load = function(fname)
		return sprite_load(fname);
	end;
	free = function(key)
		return sprite_free(key);
	end;
}

stead.module_init(function()
	sprites_free();
end)
