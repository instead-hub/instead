theme = {
	nam = 'theme';
	object_type = true;
	system_type = true;
	save = function(self, name, h, need)
		local k,v
		for k,v in pairs(self.vars) do
			h:write(stead.string.format("theme.set(%q, %q);\n", k, v))
		end
	end;
	set = function(name, val)
		if val and name then
			theme_var(name, tostring(val));
			theme.vars[name] = tostring(val);
		end
	end;
	get = function(name)
		return theme_var(name);
	end;
	win = {
		geom = function(x, y, w, h)
			theme.set("win.x", x);
			theme.set("win.y", y);
			theme.set("win.w", w);
			theme.set("win.h", h);
		end;
		color = function(fg, link, alink)
			theme.set("win.col.fg", fg);
			theme.set("win.col.link", link);
			theme.set("win.col.alink", alink);
		end;
		font = function(name, size, height)
			theme.set("win.fnt.name", name);
			theme.set("win.fnt.size", size);
			theme.set("win.fnt.height", height);
		end;
		gfx = {
			up = function(pic, x, y)
				theme.set("win.gfx.up", pic);
				theme.set("win.up.x", x);
				theme.set("win.up.y", y);
			end;
			down = function(pic, x, y)
				theme.set("win.gfx.down", pic);
				theme.set("win.down.x", x);
				theme.set("win.down.y", y);
			end;
		};
	};
	inv = {
		geom = function(x, y, w, h)
			theme.set("inv.x", x);
			theme.set("inv.y", y);
			theme.set("inv.w", w);
			theme.set("inv.h", h);
		end;
		color = function(fg, link, alink)
			theme.set("inv.col.fg", fg);
			theme.set("inv.col.link", link);
			theme.set("inv.col.alink", alink);
		end;
		font = function(name, size, height)
			theme.set("inv.fnt.name", name);
			theme.set("inv.fnt.size", size);
			theme.set("inv.fnt.height", height);
		end;
		mode = function(mode)
			theme.set("inv.mode", mode);
		end;
		gfx = {
			up = function(pic, x, y)
				theme.set("inv.gfx.up", pic);
				theme.set("inv.up.x", x);
				theme.set("inv.up.y", y);
			end;
			down = function(pic, x, y)
				theme.set("inv.gfx.down", pic);
				theme.set("inv.down.x", x);
				theme.set("inv.down.y", y);
			end;
		};
	};
	menu = {
		bw = function(w)
			theme.set("menu.bw", w);
		end;
		color = function(fg, bg, alpha, link, alink)
			theme.set("menu.col.fg", fg);
			theme.set("menu.col.bg", bg);
			theme.set("menu.col.alpha", alpha);
			theme.set("menu.col.link", link);
			theme.set("menu.col.alink", alink);
		end;
		font = function(name, size, height)
			theme.set("menu.fnt.name", name);
			theme.set("menu.fnt.size", size);
			theme.set("menu.fnt.height", height);
		end;
		gfx = {
			button = function(b, x, y)
				theme.set("menu.gfx.button", b);
				theme.set("menu.button.x", x);
				theme.set("menu.button.y", y);
			end;
		};
	};
	gfx = {
		cursor = function(norm, use, x, y)
			theme.set("scr.gfx.cursor.normal", norm);
			theme.set("scr.gfx.cursor.use", use);
			theme.set("scr.gfx.cursor.x", x);
			theme.set("scr.gfx.cursor.y", y);
		end;
		mode = function(mode)
			theme.set("scr.gfx.mode", mode);
		end;
		pad = function(pad)
			theme.set("scr.gfx.pad", pad);
		end;
		bg = function(bg)
			theme.set("scr.gfx.bg", bg);
		end;
	};
	snd = {
		click = function(w)
			theme.set("snd.click", w);
		end
	}
}

theme.vars = {}
