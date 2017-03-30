local std = stead
local type = std.type
local table = std.table

std.phrase_prefix = '-- '
std.phrase_show = true

local function phr_prefix(d, nr)
	if type(std.phrase_prefix) == 'string' then
		d = std.phrase_prefix .. d
	elseif type(std.phrase_prefix) == 'function' then
		d = std.phrase_prefix(nr) .. d
	end
	return d
end

std.dlg = std.class({
	__dlg_type = true;
	new = function(s, v)
		if v.current == nil then
			v.current = false
		end
		v.dlg_enter = v.enter
		v.enter = nil
		v.__stack = {}
		if type(v.phr) == 'table' then
			if not v.obj then v.obj = {} end
			if type(v.obj) == 'table' then
				table.insert(v.obj, 1, v.phr)
			end
		end
		v = std.room(v)
		std.setmt(v, s)
		v:__recreate()
		return v
	end;
	__recreate = function(s)
		for i = 1, #s.obj do
			if not std.is_obj(s.obj[i]) then
				s.obj[i] = std.phr(s.obj[i])
			end
		end
	end;
	with = function(self, ...)
		std.room.with(self, ...)
		self:__recreate()
		return self
	end;
	scene = function(s)
		local title, dsc, lact
		title = iface:title(std.titleof(s))
		dsc = std.call(s, 'dsc')
		if not std.me():moved() then
			s.__lact = std.game:lastreact() or s.__lact
			lact = iface:em(s.__lact)
		end
		return std.par(std.scene_delim, title or false, lact or false, dsc)
	end;
	ph_onact = function(s, w) -- show dsc by default
		if not std.phrase_show then
			return
		end
		local r, v = std.call(w, 'dsc')
		if type(r) == 'string' then
			return phr_prefix(r)
		end
		return r, v
	end;
	empty = function(s, w)
		if not w then
			if not s.current then
				return true
			end
			return s.current:empty()
		end
		w = s:lookup(w)
		if not w then
			return true
		end
		return w:empty()
	end;
	enter = function(s, ...)
		s.__llact = false
		s.__stack = {}
		s.current = nil
		s:for_each(function(s) s:open() end) -- open all phrases
		local r, v = std.call(s, 'dlg_enter', ...)
		if std.here() ~= s or #s.__stack > 0 then
			return r, v
		end
		local rr, vv = s:push(s.current)
		if not vv then
			std.err("Wrong dialog: "..std.tostr(s), 2)
		end
		return std.par(std.scene_delim, r or false, rr or false), v
	end;
	push = function(s, p)
		local c = s.current
		local r = s:select(p)
		local t
		if r ~= false then
			if c then
				table.insert(s.__stack, c)
			end
			if r.dsc ~= nil and r.ph_act == nil and r.next == nil then -- no rection
				t = std.call(r, 'dsc')
			end
			if s.current ~= r or std.me():moved() then
				return t
			end
			if r:empty() then
				local tt, vv = s:pop()
				t = std.par(std.scene_delim, t or false, tt or false)
				if not vv then
					tt = std.walkout(s:from())
					t = std.par(std.scene_delim, t or false, tt or false)
				end
			end
		end
		return t, r ~= false
	end;
	reset = function(s, phr)
		s.__stack = {}
		return s:push(phr)
	end;
	pop = function(s, phr)
		if #s.__stack == 0 then
			return false
		end

		if phr then
			local l = {}
			for i = 1, #s.__stack do
				table.insert(l, s.__stack[i])
				if s.stack[i] == phr then
					break
				end
			end
			s.__stack = l
		end
		local p
		while #s.__stack > 0 do
			p = table.remove(s.__stack, #s.__stack) -- remove top
			p = s:select(p)
			if not p then
				return false
			end
			if p:empty() then
				local r, v = std.call(p, 'onempty')
				if v then
					return r, p
				end
			else
				return false, p
			end
		end
		return false
	end;
	select = function(s, p)
		if #s.obj == 0 then
			return false
		end
		if not p then -- get first one
			p = s.obj[1]
		end

		local c = s:lookup(p)

		if not c then
			std.err("Wrong dlg:select argumant: "..std.tostr(p), 2)
		end

		c:select()

		-- if c:disabled() then -- select always enables phrase
		--	c:enable()
		-- end
		s.current = c
		return c
	end;
	srch = function(s, w)
		local oo = s.current -- lookup in current
		if not oo then
			return
		end
		local r, l, i = s:lookup(w)
		if not r then
			return
		end
		if not std.is_obj(r, 'phr') then -- simple object
			return std.room.srch(s, w)
		end
		w = oo.obj:for_each(function(v) -- aliases
			v = v:__alias()
			if not v:visible() then
				return
			end
			if v == r then
				return v
			end
		end)
		if not w then
			return
		end
		return r, l, i
	end;
	display = function(s)
		local deco = std.call(s, 'decor'); -- static decorations
		return std.par(std.scene_delim, deco or false, s:ph_display())
	end;
	ph_display = function(s)
		local r, nr
		nr = 1
		local oo = s.current
		if not oo then -- nothing to show
			return
		end

		oo:select() -- to recheck all

		for i = 1, #oo.obj do
			local o = oo.obj[i]
			o = o:__alias()
			if o:visible() then
				if r then
					r = r .. '^'
				end
				local d = std.call(o, 'dsc')
				if type(d) == 'string' then
					d = phr_prefix(d, nr)
					d = o:__xref(d, true)
					r = (r or '').. d
					nr = nr + 1
				end
			end
		end
		return r
	end;
}, std.room)

std.phr = std.class({
	__phr_type = true;
	new = function(s, v)
		local disabled
		local a = v
		local o = {
			obj = {}
		}
		for i = 1, #a do
			local v = a[i]
			if i == 1 and type(v) == 'boolean' then
				if not v then
					disabled = true
				else
					o.always = true
				end
			elseif type(v) == 'table' then
				if not std.is_obj(v, 'phr') then
					v = s:new(v)
				end
				table.insert(o.obj, v)
			elseif o.tag == nil and v ~= nil and std.is_tag(v) then
				o.tag = v
			elseif o.dsc == nil and v ~= nil then
				o.dsc = v
			elseif o.act == nil and v ~= nil then
				o.act = v
			end
		end

		for k, v in std.pairs(a) do
			if type(k) == 'string' then
				o[k] = v
			end
		end

--		if o.act == nil then
--			std.err("Wrong phrase (no act)", 2)
--		end
		o.ph_act = o.act
		o.act = nil

		disabled = disabled or (o.hidden == true)

		o = std.obj(o)
		std.setmt(o, s)
		if disabled then o = o:disable() end
		return o
	end,
	__alias = function(s)
		if s.alias ~= nil then
			local ss = std.here():lookup(s.alias)
			if not std.is_obj(ss) then
				std.err("Wrong alias: "..std.tostr(s.alias), 3)
			end
			s = ss
		end
		return s
	end;
	check = function(s)
--		s = s:__alias()
		if type(s.cond) == 'function' then
			if s:cond() then
				s:enable()
			else
				s:disable()
			end
		end
	end;
	empty = function(s)
		for i = 1, #s.obj do
			local o = s.obj[i]
			if not o:disabled() and not o:closed() then
				return false
			end
		end
		return true
	end;
	visible = function(s)
		return not s:disabled() and not s:closed()
	end;
	act = function(s, ...)
		local n = s
--		s = s:__alias()
		local onact, v = std.call(std.here(), 'ph_onact', s)
		if not v then
			return onact
		end
		local w = s:where()
		if w and w.only then -- only one choice
			for i = 1, #w.obj do
				local o = w.obj[i]
				if not o.always then
					o:close()
				end
			end
		elseif not s.always then
			s:close()
		end
		local cur = std.here().current

		local r, v = std.call(s, 'ph_act', ...)

		r = std.par(std.scene_delim, onact or false, r or false), v

		if std.me():moved() or cur ~= std.here().current then
			return r, v
		end

		if std.is_tag(s.next) then
			n = s.next
		end

		cur:select() -- conditions

		local t

		local rr, vv = std.here():push(n)
		if not vv then
			t = std.walkout(std.here():from())
		end
		return std.par(std.scene_delim, r or false, rr or false, t or false), v
	end,
	select = function(s)
		s:close()
		for i = 1, #s.obj do
			local o = s.obj[i]
			o = o:__alias()
			o:check()
		end
	end;
}, std.obj)
