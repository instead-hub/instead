-- stead.phrase_prefix = '--'
local function isReaction(ph)
	return ph.ans ~= nil or ph.do_act ~= nil
end

local function phr_call(self, w)
	local r = true
	local ph = dialog_phrase(self, w);
	if isPhrase(ph) and not isDisabled(ph) and not isReaction(ph) then
		r = stead.call(ph, 'dsc')
		if type(r) ~= 'string' then
			r = true
		else
			p (r)
		end
	end
	return r
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
			if isPhrase(ph) and not isDisabled(ph) and isReaction(ph) then
				if stead.phrase_prefix then
					v = stead.par('^', v, stead.cat(stead.phrase_prefix, ph:look()));
				else
					v = stead.par('^', v, txtnm(n, ph:look()));
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
			if isPhrase(ph) and not isDisabled(ph) and isReaction(ph) then
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
	if type(w) ~= 'number' then
		return false
	end
	if not dialog_rescan(self, w) then
		return false
	end
	local n = #self.__phr_stack;
	if n == 0 then
		stead.table.insert(self.__phr_stack, w);
	else
		self.__phr_stack[n] = w
	end
	return phr_call(self, w)
end

function pjump(w)
	if not isDialog(here()) then
		return false
	end
	return here():pjump(w)
end

function dialog_pstart(self, w)
	if type(w) ~= 'number' then
		w = 1
	end
	self.__phr_stack = { w }
	return phr_call(self, w)
end

function pstart(w)
	if not isDialog(here()) then
		return
	end
	here():pstart(w)
end

function dialog_psub(self, w)
	if type(w) ~= 'number' then
		return false
	end
	if not dialog_rescan(self, w) then
		return false
	end
	stead.table.insert(self.__phr_stack, w);
	return phr_call(self, w)
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
	return phr_call(self, phr_get(self))
end

function pret()
	if not isDialog(here()) then
		return
	end
	return here():pret()
end

local function dialog_phr2obj(self)
	local k, v, n, i
	if type(self.phr) ~= 'table' then
		return
	end
	n = 0
	for k,v in ipairs(self.phr) do
		if type(v) == 'table' then
			local q, a, c, on
			on = true;
			i = 1
			local nn = {}
			q = 0
			while type(v[i]) == 'number' do
				n = v[i]
				if n > q then
					q = n
				end
				i = i + 1
				stead.table.insert(nn, n)
			end
			if #nn == 0 then
				n = n + 1
				stead.table.insert(nn, n)
			else
				n = q -- maximum index
			end
			if type(v[i]) == 'boolean' then
				on = v[i]
				i = i + 1
			end
			q = v[i]
			i = i + 1
			a = v[i]
			i = i + 1
			c = v[i]
			local p
			if on then
				p = stead.phr(q, a, c);
			else
				p = stead._phr(q, a, c);
			end
			for q,a in ipairs(nn) do
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
	ph:disable(); -- /* disable it!!! */

	local last = stead.call(ph, 'ans');

	here().__last_answer = last;
	
	if type(ph.do_act) == 'string' then
		local f = stead.eval(ph.do_act);
		if f ~= nil then
			ret = f();
		else
			error ("Error while eval phrase action.");
		end
	elseif type(ph.do_act) == 'function' then
		ret = ph.do_act(self);
	end

	if ret == nil then ret = stead.pget(); end

	if last == true or ret == true then
		r = true;
	end

	while not dialog_rescan(here()) and phr_pop(here())  do -- do returns

	end

	local wh = here();

	while isDialog(wh) and not dialog_rescan(wh) and stead.from(wh) ~= wh do
		wh = stead.from(wh)
	end

	if wh ~= here() then
		ret = stead.par(stead.space_delim, ret, stead.back(wh));
	end
	
	ret = stead.par(stead.scene_delim, last, ret);
	
	if ret == nil then
		return r -- hack?
	end
	return ret
end

dlg = stead.hook(dlg, 
function(f, v, ...)
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
	v = f(v, ...)
	v.__last_answer = false
	v.__phr_stack = { 1 }
	dialog_phr2obj(v);
	return v
end)
