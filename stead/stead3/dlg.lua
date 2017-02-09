local std = stead
local type = std.type
local table = std.table

std.phrase_prefix = '-- '

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
		v.dlg_onenter = v.onenter
		v.onenter = nil
		v.__stack = {}
		if type(v.obj) == 'table' then
			for i = 1, #v.obj do
				if not std.is_obj(v.obj[i]) then
					v.obj[i] = std.phr(v.obj[i])
				end
			end
		end
		v = std.room(v)
		std.setmt(v, s)
		return v
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
	onact = function(s, w) -- show dsc by default
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
	onenter = function(s, ...)
		s.__llact = false
		s.__stack = {}
		s.current = nil
		s:for_each(function(s) s:open() end) -- open all phrases
		local r, v = std.call(s, s.dlg_onenter, ...)
		if v == false then
			return r, v
		end
		local rr, vv = s:push()
		if not vv then
			std.err("Wrong dialog: "..std.tostr(s), 2)
		end
		return std.par(std.scene_delim, r, rr), v
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

		if c:disabled() then -- select always enables phrase
			c:enable()
		end
		s.current = c
		return c
	end;
	display = function(s)
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
					d = o:xref(d)
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
		if not s.always then
			s:close()
		end
		local cur = std.here().current

		local r, v = std.call(s, 'ph_act', ...)

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
		for i = 1, #s.obj do
			local o = s.obj[i]
			o = o:__alias()
			o:check()
		end
	end;
}, std.menu or std.obj)
