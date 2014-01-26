-- stead.phrase_prefix = '--'
local function isValid(p)
	return isPhrase(p) and p.dsc and (p.ans or p.code)
end

local function isDelimiter(p)
	return isPhrase(p) and not isDisabled(p) and (p.dsc == nil and p.ans == nil and p.code == nil)
end

local tagpnext = function(a, k)
	if not k then
		if isPhrase(a.tag) then
			return 1, a.tag
		end
		if stead.type(a.tag) == 'number' then
			local r = a.s:phrase(a.tag)
			if r then return 1, r end
			return
		end
		k = {}
		local r,v 
		for r,v in stead.opairs(a.s.obj) do
			v = stead.ref(v)
			if isPhrase(v) and v.tag == a.tag then
				stead.table.insert(k, v)
			end
		end
		k.i = 0
		k.n = stead.table.maxn(k)
	end
	if k == 1 or k.i >= k.n then
		return nil
	end
	k.i = k.i + 1
	return k, k[k.i]
end

local function phrases(s, tag)
	local a = { s = s, tag = tag }
	return tagpnext, a, nil;
end

stead.phrase_seen = function(s, enb, ...)
	local i, ph, k
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self);
	end
	for i=1,stead.table.maxn(a) do
		local r
		for k, ph in phrases(s, a[i]) do
			r = isPhrase(ph) and not isRemoved(ph) and not ph:disabled();
			if r then 
				break 
			end
		end
		if enb then r = not r end
		if r then return false end
	end
	return true
end

local function ponoff(s, on, ...)
	local i, ph, k
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self)
	end
	for i=1,stead.table.maxn(a) do
		for k, ph in phrases(s, a[i]) do
			if isPhrase(ph) and not isRemoved(ph) then
				if on then
					ph:enable();
				else 
					ph:disable();
				end
			end
		end
	end
end

stead.dialog_prem = function(s, ...)
	local i, ph, k
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self);
	end
	for i=1,stead.table.maxn(a) do
		for k, ph in phrases(s, a[i]) do
			if isPhrase(ph) then
				ph:remove();
			end
		end
	end
end

stead.dialog_pon = function(self,...)
	return ponoff(self, true, ...);
end

stead.dialog_poff = function(self,...)
	return ponoff(self, false, ...);
end

local function phr_get(self)
	local n = #self.__phr_stack;
	if n == 0 then return 1 end
	return self.__phr_stack[n];
end

local function phr_pop(self)
	local n = #self.__phr_stack;
	if n <= 1 then return false end
	stead.table.remove(stead.here().__phr_stack, n)
	return true
end

local function call_empty(self)
	local ph = self:phrase(phr_get(self))
	local r 
	if not isPhrase(ph) or isDisabled(ph) or not ph.empty then
		self:pret()
		return
	end
	r = stead.call(ph, "empty")
	if stead.type(r) == 'string' then
		stead.p(r)
	end
	return r
end

local function call_enter(ph)
	local r, n
	if not isPhrase(ph) or isDisabled(ph) then
		return
	end
	if isValid(ph) then
		return
	end
	r = stead.call(ph, 'dsc')
	if stead.type(r) == 'string' then
		stead.p(r)
	end
end

local function dialog_reset(self)
	local i,ph
	for i,ph in stead.pairs(self.obj) do
		if isPhrase(ph) then
			ph.nam = ''
		end
	end
end

stead.dialog_look = function(self)
	local i,n,v,ph,ii
	n = 1
	local start = phr_get(self)
	dialog_reset(self)
	for i,ph,ii in stead.opairs(self.obj) do
		if ii >= start then
			if ii ~= start and isDelimiter(ph) then
				break
			end
			if isPhrase(ph) and not isDisabled(ph) and isValid(ph) then
				ph.nam = stead.tostr(n)
				if stead.type(stead.phrase_prefix) == 'string' then
					v = stead.par('^', v, stead.cat(stead.phrase_prefix, ph:look()));
				else
					v = stead.par('^', v, txtnm(n, ph:look()))
				end
				n = n + 1
			end
		end
	end
	return v;
end

stead.dialog_rescan = function(self, naming, from)
	local i,k,ph,ii, start
	k = 0
	if stead.type(from) == 'number' then
		start = from
	elseif stead.type(from) == 'string' then
		ph, start = self:phrase(from)
	else
		start = phr_get(self)
	end
	for i,ph,ii in stead.opairs(self.obj) do
		if ii >= start then
			if ii ~= start and isDelimiter(ph) then
				break
			end
			if isPhrase(ph) and not isDisabled(ph) and isValid(ph) then
				k = k + 1;
				if naming then
					ph.nam = stead.tostr(k)
				end
			end
		end
	end
	if k == 0 then
		return false
	end
	return k
end

stead.dialog_enter = function(self)
	if self:empty(self) then
		return nil, false
	end
	return nil, true
end

stead.dialog_current = function(self, w, ...)
	local r = phr_get(self)
	if w then
		local ph, i = self:phrase(w)
		if ph then
			self.__phr_stack = { i }
		end
	end
	return r
end

stead.dialog_curtag = function(self, w, ...)
	local p = self:phrase(phr_get(self))
	if w then
		self:current(w)
	end
	if not isPhrase(p) then
		return
	end
	return p.tag
end

stead.dialog_empty = function(self, from)
	return (stead.dialog_rescan(self, false, from) == false)
end

stead.dialog_visible = function(self, from)
	local r = stead.dialog_rescan(self, false, from);
	if not r then r = 0 end
	return r
end

stead.dialog_pjump = function(self, w)
	local ph, i = self:phrase(w)
	if not ph then
		return
	end
	local n = #self.__phr_stack;
	if n == 0 then
		stead.table.insert(self.__phr_stack, i);
	else
		self.__phr_stack[n] = i
	end
	call_enter(ph)
	stead.cctx().action = true
	return
end

function pjump(w)
	if not isDialog(stead.here()) then
		return false
	end
	return stead.here():pjump(w)
end

stead.dialog_pstart = function(self, w)
	if not w then 
		w = 1 
	end
	local ph, i = self:phrase(w)
	if not ph then
		return
	end
	self.__phr_stack = { i }
	call_enter(ph)
	stead.cctx().action = true
	return
end

function pstart(w)
	if not isDialog(stead.here()) then
		return
	end
	stead.here():pstart(w)
end

stead.dialog_psub = function(self, w)
	local ph, i = self:phrase(w)
	if not ph then
		return
	end
	stead.table.insert(self.__phr_stack, i);
	call_enter(ph)
	stead.cctx().action = true
	return
end

function psub(w)
	if not isDialog(stead.here()) then
		return false
	end
	return stead.here():psub(w)
end

stead.dialog_pret = function(self)
	if not phr_pop(self) then
		return
	end
	stead.cctx().action = true
	if not self:empty() then
		return
	end
	call_empty(self)
	return
end

function pret()
	if not isDialog(stead.here()) then
		return
	end
	return stead.here():pret()
end

function phr(ask, answ, act)
	local i = 1
	local dis = false
	
	if stead.type(ask) ~= 'table' then -- old style
		local p = phrase ( { dsc = ask, ans = answ, code = act });
		return p
	end

	local v = ask

	if stead.type(v[1]) == 'number' then -- just skip number
		i = i + 1
	end

	if stead.type(v[i]) == 'boolean' then
		dis = not v[i]
		i = i + 1
	end

	if v.dsc == nil then
		v.dsc = v[i];
		i = i + 1
	end
	if v.ans == nil then
		v.ans = v[i];
		i = i + 1
	end
	if v.code == nil and (stead.type(v[i]) == 'function' or stead.type(v[i]) == 'string') then
		v.code = v[i];
	end
	v = phrase(v)
	if dis then
		v = v:disable()
	end
	return v;
end

function _phr(ask, answ, act) -- compat only?
	local p = phr(ask, answ, act);
	p:disable()
	return p
end

stead.phr = phr

stead.phrase_save = function(self, name, h, need)
	if need then
		local m = " = phrase {"
		local post = '}\n'
		if isDisabled(self) then
			post = "}:disable()\n"
		end
		m = stead.string.format("%s%s", name, m);
		if self.dsc then
			m = m..stead.string.format("dsc = %s, ", stead.tostring(self.dsc));
		end

		if self.ans then
			m = m..stead.string.format("ans = %s, ", stead.tostring(self.ans));
		end

		if self.code then
			m = m..stead.string.format("code = %s, ", stead.tostring(self.code));
		end

		if self.tag then
			m = m..stead.string.format("tag = %s, ", stead.tostring(self.tag));
		end

		if self.always then
			m = m..stead.string.format("always = %s, ", stead.tostring(self.always));
		end

		if self.empty then
			m = m..stead.string.format("empty = %s, ", stead.tostring(self.empty));
		end

		h:write(m..post);
	end
	stead.savemembers(h, self, name, false);
end

local function dialog_phr2obj(self)
	local k, v, n
	local aliases = {}
	if stead.type(self.phr) ~= 'table' then
		return
	end
	n = 0
	for k, v in ipairs(self.phr) do
		if stead.type(v) == 'table' then
			if stead.type(v[1]) == 'number' then
				n = v[1]
			else
				n = n + 1
			end
			local p = stead.phr(v)
			if self.obj[n] then
				error ("Error in phr structure (numbering).", 4);
			end
			if v.alias then
				p.alias = v.alias
				stead.table.insert(aliases, n);
			end
			self.obj[n] = p
		else
			error ("Error in phr structure (wrong item).", 4);
		end
	end
	for k, v in ipairs(aliases) do
		local ph = self:phrase(self.obj[v].alias)
		if not ph then
			error ("Wrong alias in dlg.", 3);
		end
		self.obj[v] = ph
	end
end

stead.dialog_phrase = function(self, num)
	if not num then
		return
	end
	if isPhrase(num) then
		return num
	end
	if stead.type(num) ~= 'number' then
		local k,v,i
		for k,v,i in stead.opairs(self.obj) do
			v = stead.ref(v)
			if isPhrase(v) and v.tag == num then
				return v, i
			end
		end
		return nil
	end
	return stead.ref(self.obj[num]), num;
end

stead.phrase_action = function(self)
	local ph = self;
	local r, ret;
	local empty

	if isDisabled(ph) then
		return nil, false
	end
-- here it is
	if not ph.always then
		ph:disable(); -- /* disable it!!! */
	end
	local last = stead.call(ph, 'ans');

	if stead.type(ph.code) == 'string' then
		local f = stead.eval(ph.code);
		if f ~= nil then
			ret = f();
		else
			error ("Error while eval phrase action:"..ph.code);
		end
	elseif stead.type(ph.code) == 'function' then
		ret = ph.code(self);
	end

	if ret == nil then ret = stead.pget(); end

	if last == true or ret == true then
		r = true;
	end

	if isDialog(stead.here()) and stead.here():empty() then
		empty = call_empty(stead.here());
	end

	local wh = stead.here();

	while isDialog(wh) and wh:empty() and stead.from(wh) ~= wh do
		wh = stead.from(wh)
	end

	if wh ~= stead.here() then
		ret = stead.par(stead.scene_delim, ret, stead.back(wh));
	end
	
	ret = stead.par(stead.space_delim, last, empty, ret);

	if ret == nil then
		return r -- hack?
	end
	return ret
end

function dlg(v) --constructor
	v.dialog_type = true;
	if v.ini == nil then
		v.ini = function(s)
			stead.dialog_rescan(s, true);
		end
	end
	if v.enter == nil then
		v.enter = stead.dialog_enter;
	end
	if v.look == nil then
		v.look = stead.dialog_look;
	end
	if v.scene == nil then
		v.scene = stead.dialog_scene;
	end
	if v.pon == nil then
		v.pon = stead.dialog_pon;
	end
	if v.poff == nil then
		v.poff = stead.dialog_poff;
	end
	if v.prem == nil then
		v.prem = stead.dialog_prem;
	end
	if v.pseen == nil then
		v.pseen = stead.dialog_pseen;
	end
	if v.punseen == nil then
		v.punseen = stead.dialog_punseen;
	end
	if v.empty == nil then
		v.empty = stead.dialog_empty;
	end

	if v.visible == nil then
		v.visible = stead.dialog_visible;
	end

	if v.current == nil then
		v.current = stead.dialog_current
	end

	if v.curtag == nil then
		v.curtag = stead.dialog_curtag
	end

	if v.pstart == nil then
		v.pstart = stead.dialog_pstart
	end
	if v.pjump == nil then
		v.pjump = stead.dialog_pjump
	end
	if v.pret == nil then
		v.pret = stead.dialog_pret
	end
	if v.psub == nil then
		v.psub = stead.dialog_psub
	end
	if v.phrase == nil then
		v.phrase = stead.dialog_phrase
	end

	if v.dsc == nil then
		v.dsc = function(s)
			if stead.player_moved() then
				stead.last_act(false)
			elseif stead.type(ACTION_TEXT) ~= 'string' and stead.last_act() then
				stead.p(stead.last_act())
			end
		end
	end

	v = room(v);

	v.__phr_stack = { 1 }
	dialog_phr2obj(v);

	return v;
end
