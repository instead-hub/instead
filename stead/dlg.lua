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
				v = stead.par('^', v, txtnm(n, ph:look()));
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

function dialog_current(self,...)
	return phr_get(self)
end

function dialog_empty(self, from)
	return not dialog_rescan(self, from);
end

function pjump(w)
	if not isDialog(here()) or type(w) ~= 'number' then
		return
	end
	if not dialog_rescan(here(), w) then
		return false
	end
	local n = #here().__phr_stack;
	if n == 0 then
		stead.table.insert(here().__phr_stack, w);
	else
		here().__phr_stack[n] = w
	end
	return true
end

function pstart(w)
	if not isDialog(here()) then
		return
	end
	if type(w) ~= 'number' then
		w = 1
	end
	here().__phr_stack = { w }
end

function psub(w)
	if not isDialog(here()) or type(w) ~= 'number' then
		return
	end
	if not dialog_rescan(here(), w) then
		return false
	end
	stead.table.insert(here().__phr_stack, w);
	return true
end

function pret()
	if not isDialog(here()) then
		return
	end
	return phr_pop(here())
end

local function dialog_phr2obj(self)
	local k, v, n, i
	if type(self.phr) ~= 'table' then
		return
	end
	n = 0
	for k,v in ipairs(self.phr) do
		if k == 1 and type(v) == 'string' or type(v) == 'function' then
			-- self.dsc = v
		elseif type(v) == 'table' then
			local q, a, c, on
			on = true;
			i = 1
			if type(v[i]) == 'number' then
				n = v[1]
				i = i + 1
			else
				n = n + 1
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
			if self.obj[n] then
				error ("Error in phr structure (numbering).", 4);
			end
			self.obj[n] = p
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
	v = f(v, ...)
	v.__phr_stack = { 1 }
	dialog_phr2obj(v);
	return v
end)
