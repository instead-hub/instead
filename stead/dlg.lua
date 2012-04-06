-- stead.phrase_prefix = '--'
local tagpnext = function(a, k)
	if not k then
		if tonumber(a.tag) then
			return 1, dialog_phrase(a.s, a.tag)
		end
		k = {}
		local r,v 
		for r,v in opairs(a.s.obj) do
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

function phrase_seen(s, enb, ...)
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

function dialog_prem(s, ...)
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

function dialog_pon(self,...)
	return ponoff(self, true, ...);
end

function dialog_poff(self,...)
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
	stead.table.remove(here().__phr_stack, n)
	return true
end

function dialog_look(self)
	local i,n,v,ph,ii
	n = 1
	local start = phr_get(self)
	for i,ph,ii in opairs(self.obj) do
		if ii >= start then
			ph = stead.ref(ph);
			if not ph.dsc then
				break
			end
			if isPhrase(ph) and not isDisabled(ph) then
				ph.nam = tostring(n)
				if stead.phrase_prefix then
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

function dialog_rescan(self, from)
	local i,k,ph,ii, start
	k = 1
	local start
	if type(from) == 'number' then
		start = from
	else
		start = phr_get(self)
	end
	for i,ph,ii in opairs(self.obj) do
		if ii >= start then
			ph = stead.ref(ph);
			if not ph.dsc then
				break
			end
			if isPhrase(ph) and not isDisabled(ph) then
				ph.nam = tostring(k);
				k = k + 1;
			end
		end
	end
	if k == 1 then
		return false
	end
	return true
end

function dialog_enter(self)
	if not dialog_rescan(self) then
		return nil, false
	end
	self.__last_answer = false
	return nil, true
end

function dialog_current(self,...)
	return phr_get(self)
end

function dialog_empty(self, from)
	return not dialog_rescan(self, from);
end

function dialog_pjump(self, w)
	local ph, i = dialog_phrase(self, w)
	if not ph then
		return false
	end
	if not dialog_rescan(self, i) then
		return false
	end
	local n = #self.__phr_stack;
	if n == 0 then
		stead.table.insert(self.__phr_stack, i);
	else
		self.__phr_stack[n] = i
	end
	return true
end

function pjump(w)
	if not isDialog(here()) then
		return false
	end
	return here():pjump(w)
end

function dialog_pstart(self, w)
	if not w then 
		w = 1 
	end
	local ph, i = dialog_phrase(self, w)
	if not ph then
		return
	end
	self.__phr_stack = { i }
	return
end

function pstart(w)
	if not isDialog(here()) then
		return
	end
	here():pstart(w)
end

function dialog_psub(self, w)
	local ph, i = dialog_phrase(self, w)
	if not ph then
		return false
	end
	if not dialog_rescan(self, i) then
		return false
	end
	stead.table.insert(self.__phr_stack, i);
	return
end

function psub(w)
	if not isDialog(here()) then
		return false
	end
	return here():psub(w)
end

function dialog_pret(self)
	while true do
		if  not phr_pop(self) then
			break
		end
		if dialog_rescan(self) then
			break
		end
	end
	return
end

function pret()
	if not isDialog(here()) then
		return
	end
	return here():pret()
end

function phr(ask, answ, act)
	local i = 1
	local r = {}
	local dis = false
	
	if type(ask) ~= 'table' then -- old style
		local p = phrase ( { dsc = ask, ans = answ, code = act });
		return p
	end

	local v = ask

	if type(v[i]) == 'boolean' then
		dis = not v[i]
		i = i + 1
	end
	r.dsc = v[i]
	i = i + 1
	r.ans = v[i]
	i = i + 1
	r.code = v[i]
	r.always = v.always
	r.tag = v.tag
	r = phrase(r)
	if dis then
		r = r:disable()
	end
	return r;
end

function _phr(ask, answ, act) -- compat only?
	local p = phr(ask, answ, act);
	p:disable()
	return p
end

stead.phr = phr

function phrase_save(self, name, h, need)
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
		h:write(m..post);
	end
	stead.savemembers(h, self, name, false);
end

local function dialog_phr2obj(self)
	local k, v, n, q, a

	if type(self.phr) ~= 'table' then
		return
	end

	n = 0

	for k,v in ipairs(self.phr) do
		if type(v) == 'table' then
			local nn = {}

			while type(v[1]) == 'number' do
				stead.table.insert(nn, v[1])
				stead.table.remove(v, 1)
			end

			stead.table.sort(nn);

			local p = stead.phr(v)
			if #nn == 0 then
				n = n + 1
				stead.table.insert(nn, n)
			else
				n = nn[#nn] -- maximum index
			end

			for q, a in ipairs(nn) do
				if self.obj[a] then
					error ("Error in phr structure (numbering).", 4);
				end
				self.obj[a] = p
			end
		else
			error ("Error in phr structure (wrong item).", 4);
		end
	end
end

function dialog_phrase(self, num)
	if not num then
		return
	end
	if not tonumber(num) then
		local k,v,i
		for k,v,i in opairs(self.obj) do
			v = stead.ref(v)
			if isPhrase(v) and v.tag == num then
				return v, i
			end
		end
		return nil
	end
	num = tonumber(num)
	return stead.ref(self.obj[num]), num;
end

function dialog_last(self, v)
	local r = self.__last_answer
	if v ~= nil then
		self.__last_answer = v
	end
	return r
end

function phrase_action(self)
	local ph = self;
	local r, ret;

	if isDisabled(ph) then
		return nil, false
	end
-- here it is
	if not ph.always then
		ph:disable(); -- /* disable it!!! */
	end

	local last = stead.call(ph, 'ans');

	here().__last_answer = last;
	
	if type(ph.code) == 'string' then
		local f = stead.eval(ph.code);
		if f ~= nil then
			ret = f();
		else
			error ("Error while eval phrase action.");
		end
	elseif type(ph.code) == 'function' then
		ret = ph.code(self);
	end

	if ret == nil then ret = stead.pget(); end

	if last == true or ret == true then
		r = true;
	end

	while isDialog(here()) and not dialog_rescan(here()) and phr_pop(here())  do -- do returns

	end

	local wh = here();

	while isDialog(wh) and not dialog_rescan(wh) and stead.from(wh) ~= wh do
		wh = stead.from(wh)
	end

	if wh ~= here() then
		ret = stead.par(stead.scene_delim, ret, stead.back(wh));
	end
	
	ret = stead.par(stead.scene_delim, last, ret);

	here().__last_answer = ret;

	if ret == nil then
		return r -- hack?
	end
	return ret
end

function dlg(v) --constructor
	v.dialog_type = true;
	if v.ini == nil then
		v.ini = dialog_enter;
	end
	if v.enter == nil then
		v.enter = dialog_enter;
	end
	if v.look == nil then
		v.look = dialog_look;
	end
	if v.scene == nil then
		v.scene = dialog_scene;
	end
	if v.pon == nil then
		v.pon = dialog_pon;
	end
	if v.poff == nil then
		v.poff = dialog_poff;
	end
	if v.prem == nil then
		v.prem = dialog_prem;
	end
	if v.pseen == nil then
		v.pseen = dialog_pseen;
	end
	if v.punseen == nil then
		v.punseen = dialog_punseen;
	end
	if v.empty == nil then
		v.empty = dialog_empty;
	end

	if v.current == nil then
		v.current = dialog_current
	end
	if v.last == nil then
		v.last = dialog_last
	end
	if v.pstart == nil then
		v.pstart = dialog_pstart
	end
	if v.pjump == nil then
		v.pjump = dialog_pjump
	end
	if v.pret == nil then
		v.pret = dialog_pret
	end
	if v.psub == nil then
		v.psub = dialog_psub
	end
	if v.phrase == nil then
		v.phrase = dialog_phrase
	end

	v = room(v);

	v.__last_answer = false
	v.__phr_stack = { 1 }
	dialog_phr2obj(v);

	return v;
end
