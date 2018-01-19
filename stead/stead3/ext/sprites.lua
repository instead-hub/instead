local std = stead
local type = std.type
local instead = std.ref '@instead'

-- theme
instead.theme_var = instead_theme_var
instead.theme_name = instead_theme_name

local theme = std.obj {
	nam = '@theme';
	vars = {};
	reset_vars = {};
	{
		win = { gfx = {}};
		inv = { gfx = {}};
		menu = { gfx = {}};
		gfx = {};
		snd = {};
	};
}

function theme.restore(name)
	if type(name) ~= 'string' then
		std.err("Wrong parameter to theme.restore", 2)
	end
	local v = theme.vars[name]
	if not v then
		return
	end
	instead.theme_var(name, v);
end

function theme.set(name, val)
	if type(name) ~= 'string' or val == nil then
		std.err("Wrong parameter to theme.set", 2)
	end
	if not theme.reset_vars[name] then
		theme.reset_vars[name] = instead.theme_var(name)
	end
	instead.theme_var(name, std.tostr(val));
	theme.vars[name] = std.tostr(val);
end

function theme.reset(name)
	if type(name) ~= 'string' then
		std.err("Wrong parameter to theme.reset", 2)
	end
	local v = theme.reset_vars[name]
	if not v then
		return
	end
	instead.theme_var(name, v);
	theme.vars[name] = nil
	theme.reset_vars[name] = nil
end

function theme.name(...)
	return instead.theme_name(...);
end

function theme.get(...)
	return instead.theme_var(...);
end

function theme.win.reset()
	std.for_all(theme.reset, "win.x", "win.y", "win.w", "win.h",
		    "win.col.fg", "win.col.link", "win.col.alink",
		    "win.fnt.name", "win.fnt.size", "win.fnt.height");
end

function theme.win.geom(x, y, w, h)
	theme.set("win.x", x);
	theme.set("win.y", y);
	theme.set("win.w", w);
	theme.set("win.h", h);
end


function theme.win.color(fg, link, alink)
	theme.set("win.col.fg", fg);
	theme.set("win.col.link", link);
	theme.set("win.col.alink", alink);
end

function theme.win.font(name, size, height)
	theme.set("win.fnt.name", name);
	theme.set("win.fnt.size", size);
	theme.set("win.fnt.height", height);
end

function theme.win.gfx.reset()
	std.for_all(theme.reset, "win.gfx.up", "win.up.x", "win.up.y");
	std.for_all(theme.reset, "win.gfx.down", "win.down.x", "win.down.y");
end

function theme.win.gfx.up(pic, x, y)
	theme.set("win.gfx.up", pic);
	theme.set("win.up.x", x);
	theme.set("win.up.y", y);
end

function theme.win.gfx.down(pic, x, y)
	theme.set("win.gfx.down", pic);
	theme.set("win.down.x", x);
	theme.set("win.down.y", y);
end

function theme.inv.reset()
	std.for_all(theme.reset, "inv.x", "inv.y", "inv.w", "inv.h",
		    "inv.col.fg", "inv.col.link", "inv.col.alink",
		    "inv.fnt.name", "inv.fnt.size", "inv.fnt.height",
		    "inv.mode");
end

function theme.inv.geom(x, y, w, h)
	theme.set("inv.x", x);
	theme.set("inv.y", y);
	theme.set("inv.w", w);
	theme.set("inv.h", h);
end

function theme.inv.color(fg, link, alink)
	theme.set("inv.col.fg", fg);
	theme.set("inv.col.link", link);
	theme.set("inv.col.alink", alink);
end

function theme.inv.font(name, size, height)
	theme.set("inv.fnt.name", name);
	theme.set("inv.fnt.size", size);
	theme.set("inv.fnt.height", height);
end

function theme.inv.mode(mode)
	theme.set("inv.mode", mode);
end

function theme.inv.gfx.reset()
	std.for_all(theme.reset, "inv.gfx.up", "inv.up.x", "inv.up.y");
	std.for_all(theme.reset, "inv.gfx.down", "inv.down.x", "inv.down.y");
end

function theme.inv.gfx.up(pic, x, y)
	theme.set("inv.gfx.up", pic);
	theme.set("inv.up.x", x);
	theme.set("inv.up.y", y);
end

function theme.inv.gfx.down(pic, x, y)
	theme.set("inv.gfx.down", pic);
	theme.set("inv.down.x", x);
	theme.set("inv.down.y", y);
end

function theme.menu.reset()
	std.for_all(theme.reset, "menu.bw",
		    "menu.col.fg", "menu.col.bg", "menu.col.alpha",
		    "menu.col.link", "menu.col.alink",
		    "menu.fnt.name", "menu.fnt.size", "menu.fnt.height");
end

function theme.menu.bw(w)
	theme.set("menu.bw", w)
end

function theme.menu.color(fg, bg, alpha, link, alink)
	theme.set("menu.col.fg", fg);
	theme.set("menu.col.bg", bg);
	theme.set("menu.col.alpha", alpha);
	theme.set("menu.col.link", link);
	theme.set("menu.col.alink", alink);
end

function theme.menu.font(name, size, height)
	theme.set("menu.fnt.name", name);
	theme.set("menu.fnt.size", size);
	theme.set("menu.fnt.height", height);
end

function theme.menu.gfx.reset()
	std.for_all(theme.reset, "menu.gfx.button", "menu.button.x", "menu.button.y");
end

function theme.menu.gfx.button(b, x, y)
	theme.set("menu.gfx.button", b);
	theme.set("menu.button.x", x);
	theme.set("menu.button.y", y);
end;

function theme.gfx.reset()
	std.for_all(theme.reset, "scr.gfx.cursor.normal", "scr.gfx.cursor.use",
		    "scr.gfx.cursor.x", "scr.gfx.cursor.y",
		    "scr.gfx.mode", "scr.gfx.pad",
		    "scr.gfx.bg");
end

function theme.gfx.cursor(norm, use, x, y)
	theme.set("scr.gfx.cursor.normal", norm);
	theme.set("scr.gfx.cursor.use", use);
	theme.set("scr.gfx.cursor.x", x);
	theme.set("scr.gfx.cursor.y", y);
end

function theme.gfx.mode(mode)
	theme.set("scr.gfx.mode", mode);
end

function theme.gfx.pad(pad)
	theme.set("scr.gfx.pad", pad);
end

function theme.gfx.bg(bg)
	theme.set("scr.gfx.bg", bg);
end

function theme.snd.reset()
	theme.reset("snd.click");
end

function theme.snd.click(w)
	theme.set("snd.click", w);
end

-- sprites
instead.ticks = instead_ticks
instead.font_load = instead_font_load
instead.font_free = instead_font_free
instead.font_scaled_size = instead_font_scaled_size
instead.sprite_alpha = instead_sprite_alpha
instead.sprite_dup = instead_sprite_dup
instead.sprite_scale = instead_sprite_scale
instead.sprite_rotate = instead_sprite_rotate
instead.sprite_text = instead_sprite_text
instead.sprite_text_size = instead_sprite_text_size
instead.sprite_draw = instead_sprite_draw
instead.sprite_copy = instead_sprite_copy
instead.sprite_compose = instead_sprite_compose
instead.sprite_fill = instead_sprite_fill
instead.sprite_pixel = instead_sprite_pixel
instead.sprite_load = instead_sprite_load
instead.sprite_free = instead_sprite_free
instead.sprite_size = instead_sprite_size
instead.sprites_free = instead_sprites_free
instead.sprite_colorkey = instead_sprite_colorkey
instead.sprite_pixels = instead_sprite_pixels

instead.mouse_pos = instead_mouse_pos
instead.mouse_show = instead_mouse_show
instead.mouse_filter = instead_mouse_filter

instead.finger_pos = instead_finger_pos

instead.noise1 = instead_noise1
instead.noise2 = instead_noise2
instead.noise3 = instead_noise3
instead.noise4 = instead_noise4

instead.render_callback = instead_render_callback

instead.direct = instead_direct

std.busy = instead_busy

local spr = {
	__gc = function(s)
		instead.sprite_free(s.spr)
	end;
	__tostring = function(s)
		return s.spr
	end;
}

spr.__index = spr

local fnt = {
	__gc = function(s)
		instead.font_free(s.fnt)
	end;
	__tostring = function(s)
		return s.fnt
	end;
}

fnt.__index = fnt

local spr_get = function(s)
	if type(s) == 'string' then
		return s
	end
	return std.tostr(s)
end

function fnt:new(nam)
	if type(nam) ~= 'string' then
		std.err("Wrong argument to fnt:new(): "..std.tostr(nam), 2)
	end
	local o = {
		fnt = nam;
		__save = function() end;
	}
	std.setmt(o, self)
	return std.proxy(o)
end

function fnt:text(text, col, style, ...)
	return spr:new(instead.sprite_text(self.fnt, text, col, style, ...))
end

function fnt:size(...)
	return instead.sprite_text_size(self.fnt, ...);
end

function fnt:height(...)
	local _, h = self:size(...)
	return h
end

function spr:new(nam)
	if type(nam) ~= 'string' then
		std.err("Wrong argument to spr:new(): "..std.tostr(nam), 2)
	end
	local o = {
		spr = nam;
		__save = function() end;
	}
	std.setmt(o, self)
	return std.proxy(o)
end;

function spr:alpha(alpha, ...)
	return spr:new(instead.sprite_alpha(self.spr, alpha, ...));
end

function spr:colorkey(color, ...)
	instead.sprite_colorkey(self.spr, color, ...);
	return self
end

function spr:dup(...)
	return spr:new(instead.sprite_dup(self.spr, ...));
end

function spr:scale(xs, ys, smooth, ...)
	if smooth == nil then
		smooth = true -- default is on
	end
	return spr:new(instead.sprite_scale(self.spr, xs, ys, smooth,...));
end

function spr:rotate(angle, smooth, ...)
	if smooth == nil then
		smooth = true -- default is on
	end
	return spr:new(instead.sprite_rotate(self.spr, angle, smooth, ...));
end

function spr:size()
	return instead.sprite_size(self.spr);
end

function spr:draw(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_draw(self.spr, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_draw(self.spr, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function spr:copy(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_copy(self.spr, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_copy(self.spr, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function spr:compose(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_compose(self.spr, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_compose(self.spr, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function spr:fill(x, y, w, h, col)
	if h == nil and col == nil then
		instead.sprite_fill(self.spr, 0, 0, -1, -1, x);
		return self
	end
	instead.sprite_fill(self.spr, x, y, w, h, col);
	return self
end

function spr:pixel(x, y, col, alpha)
	if not col then
		return instead.sprite_pixel(self.spr, x, y, col, alpha)
	end
	instead.sprite_pixel(self.spr, x, y, col, alpha)
	return self
end

local screen = spr:new 'screen'

local sprite = {
	nam = '@sprite';
}

function sprite.new(w, h, ...)
	if std.tonum(w) and std.tonum(h) then
		local t = 'blank:'..std.tostr(std.math.floor(w))..'x'..std.tostr(std.math.floor(h))
		return spr:new(instead.sprite_load(t))
	end
	return spr:new(instead.sprite_load(w, h, ...))
end

function sprite.fnt(name, sz, ...)
	if not std.tonum(sz) then
		std.err("No font size specified in sprite:fnt().", 2)
	end
	return fnt:new(instead.font_load(name, sz, ...))
end

function sprite.scr()
	return screen
end

function sprite.direct(v)
	return instead.direct(v)
end

function sprite.font_scaled_size(size)
	return instead.font_scaled_size(size);
end

local render_cb = nil
function sprite.render_callback(fn)
	local old = render_cb
	render_cb = fn
	instead.render_callback(render_cb)
	return old
end

std.obj(sprite)

instead.sprite_pixels = instead_sprite_pixels

local pfnt = {
}

pfnt.__index = pfnt
std.setmt(pfnt, fnt)

local pxl = {
}

pxl.__index = pxl

function pxl:dup()
	local w, h, s = self:size()
	local p = instead.sprite_pixels(w, h, s)
	if p then
		self:copy(p)
	end
	return self:new(p)
end

function pxl:sprite()
	return sprite.new(self)
end

function pxl:draw_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_draw(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_draw(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:copy_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_copy(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_copy(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:compose_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_compose(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_compose(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:scale(...)
	return pxl:new(self:new_scaled(...))
end

function pxl:rotate(...)
	return pxl:new(self:new_rotated(...))
end

local function poly(self, fn, t, ...)
	if type(t) ~= 'table' then
		std.err("Wrong argument to :poly()", 3)
	end
	if #t < 4 then
		return
	end
	local n = #t
	for i = 1, n, 2 do
		if i == n - 1 then
			fn(self, t[i], t[i+1], t[1], t[2], ...);
		else
			fn(self, t[i], t[i+1], t[i+2], t[i+3], ...);
		end
	end
end

function pxl:poly(t, ...)
	poly(self, self.line, t, ...)
end

function pxl:polyAA(t, ...)
	poly(self, self.lineAA, t, ...)
end

function pxl:new(p)
	if type(p) ~= 'userdata' then
		return
	end
	local t = getmetatable(p).__index
	setmetatable(t, self)
	return p
end

function pfnt:new(nam)
	return fnt.new(self, nam)
end

function pfnt:text(text, col, style, ...)
	local s = self
	return pxl:new(instead.sprite_pixels(instead.sprite_text(s.fnt, text, col, style, ...)))
end

local pixels = {
	nam = '@pixels';
}

function pixels.fnt(name, sz, ...)
	if not std.tonum(sz) then
		std.err("No font size specified.", 2)
	end
	return pfnt:new(instead.font_load(name, -sz, ...))
end

function pixels.new(...)
	return pxl:new(instead.sprite_pixels(...))
end

std.obj (pixels)

local rnd_seed = 1980 + 1978

stead.mod_init(function()
	rnd_seed = (std.os.time(stead.os.date("*t")) + rnd_seed + instead.ticks())
	std.rnd_seed(rnd_seed)
end)

stead.mod_done(function()
	sprite.render_callback() -- stop render
--	instead.sprites_free();
end)
