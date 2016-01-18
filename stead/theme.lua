theme = {
	nam = 'theme';
	object_type = true;
	system_type = true;
	save = function(self, name, h, need)
		local k,v
		if self.name() ~= '.' then
			return
		end
		for k,v in stead.pairs(self.vars) do
			h:write(stead.string.format("theme.set(%q, %q);\n", k, v))
		end
	end;
	reset = function(...)
		local i
		local a = {...};
		for i = 1, stead.table.maxn(a) do
			local name = a[i]
			local v
			if name then
				v = theme.reset_vars[name]
				if v then
					instead_theme_var(name, v);
					theme.vars[name] = nil
					theme.reset_vars[name] = nil
				end
			end
		end
	end;
	set = function(name, val)
		if val and name then
			if not theme.reset_vars[name] then
				theme.reset_vars[name] = instead_theme_var(name)
			end
			instead_theme_var(name, stead.tostr(val));
			theme.vars[name] = stead.tostr(val);
		end
	end;
	name = function()
		return instead_theme_name(name);
	end;
	get = function(name)
		return instead_theme_var(name);
	end;
	win = {
		reset = function()
			theme.reset("win.x", "win.y", "win.w", "win.h",
				"win.col.fg", "win.col.link", "win.col.alink",
				"win.fnt.name", "win.fnt.size", "win.fnt.height");
		end;
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
			reset = function()
				theme.reset("win.gfx.up", "win.up.x", "win.up.y");
				theme.reset("win.gfx.down", "win.down.x", "win.down.y");
			end;
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
		reset = function()
			theme.reset("inv.x", "inv.y", "inv.w", "inv.h",
				"inv.col.fg", "inv.col.link", "inv.col.alink",
				"inv.fnt.name", "inv.fnt.size", "inv.fnt.height",
				"inv.mode");
		end;
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
			reset = function()
				theme.reset("inv.gfx.up", "inv.up.x", "inv.up.y");
				theme.reset("inv.gfx.down", "inv.down.x", "inv.down.y");
			end;
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
		reset = function()
			theme.reset("menu.bw",
				"menu.col.fg", "menu.col.bg", "menu.col.alpha",
				"menu.col.link", "menu.col.alink",
				"menu.fnt.name", "menu.fnt.size", "menu.fnt.height");
		end;
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
			reset = function()
				theme.reset("menu.gfx.button", "menu.button.x", "menu.button.y");
			end;
			button = function(b, x, y)
				theme.set("menu.gfx.button", b);
				theme.set("menu.button.x", x);
				theme.set("menu.button.y", y);
			end;
		};
	};
	gfx = {
		reset = function()
			theme.reset("scr.gfx.cursor.normal", "scr.gfx.cursor.use",
				"scr.gfx.cursor.x", "scr.gfx.cursor.y",
				"scr.gfx.mode", "scr.gfx.pad",
				"scr.gfx.bg");
		end;
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
		reset = function()
			theme.reset("snd.click");
		end;
		click = function(w)
			theme.set("snd.click", w);
		end
	}
}

theme.vars = {}
theme.reset_vars = {}
