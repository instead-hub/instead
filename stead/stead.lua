stead = {
	version = "1.2.0",
	api_version = "1.1.6", -- last version before 1.2.0
	table = table,
	string = string,
	math = math,
	io = io,
	os = os,
	call_top = 0,
	cctx = { txt = nil, self = nil },
--	functions = {}, -- code blocks
	timer = function()
		if type(timer) == 'table' and type(timer.callback) == 'function' then
			return timer:callback();
		end
		return
	end,
	input = function(event, ...)
		if type(input) ~= 'table' then
			return
		end
		if event == 'kbd' then
			if type(input.key) == 'function' then
				return input:key(unpack(arg)); -- pressed, event
			end
		elseif event == 'mouse' then
			if type(input.click) == 'function' then
				return input:click(unpack(arg)); -- pressed, x, y, mb
			end
		end
		return
	end,
	modules_ini = {},
	module_init = function(f, ...)
		if type(f) ~= 'function' then
			error ("Wrong parameter to mod_init.", 2);
		end
		stead.table.insert(stead.modules_ini, f);
		f();
	end
}

instead = stead;

module_init = stead.module_init;

function stead.getcmd(str)
	local a = {}
	local n = 1
	local cmd;
	local i,k = stead.string.find(str,'[a-zA-Z0-9_]+', k);
	if not i or not k then
		cmd = str;
	else
		cmd = stead.string.sub(str, i, k);
	end
	stead.cmd = cmd
	if cmd == 'load' or cmd == 'save' then
		a[1] = strip(stead.string.sub(str, k + 1));
		stead.args = a;
		return cmd, a
	end
	while i do
		k = k + 1;
		i,k = stead.string.find(str,'[^,]+', k);
		if not i then
			break
		end
		a[n] = strip(stead.string.sub(str, i, k));
		n = n + 1;
	end
	stead.args = a;
	return cmd, a
end

function cctx()
	return stead.cctx[stead.call_top];
end

function callpush(v, ...)
	stead.call_top = stead.call_top + 1;
	stead.cctx[stead.call_top] = { txt = nil, self = v, action = false };
	args = arg
	arg1 = arg[1]
	arg2 = arg[2]
	arg3 = arg[3]
	arg4 = arg[4]
	arg5 = arg[5]
	arg6 = arg[6]
	arg7 = arg[7]
	arg8 = arg[8]
	arg9 = arg[9]
	-- dirty but clean and fast :)
	self = v
end

function callpop()
	stead.cctx[stead.call_top] = nil;
	stead.call_top = stead.call_top - 1;
	if stead.call_top < 0 then
		error ("callpush/callpop mismatch")
	end 
	arg1 = nil
	arg2 = nil
	arg3 = nil
	arg4 = nil
	arg5 = nil
	arg6 = nil
	arg7 = nil
	arg8 = nil
	arg9 = nil
	self = nil
end

function pclr()
	cctx().txt = nil
end
stead.pclr = pclr

function pget()
	return cctx().txt;
end
stead.pget = pget
function p(...)
	local i
	for i = 1, stead.table.maxn(arg) do
		cctx().txt = stead.par('',cctx().txt, tostring(arg[i]));
	end
	cctx().txt = stead.cat(cctx().txt, ' ');
end
stead.p = p
function pr(...)
	local i
	for i = 1, stead.table.maxn(arg) do
		cctx().txt = stead.par('',cctx().txt, tostring(arg[i]));
	end
end
stead.pr = pr
function pn(...)
	p(unpack(arg));
	cctx().txt = stead.par('',cctx().txt,'^');
end
stead.pn = pn
-- merge strings with "space" as separator
function par(space,...)
	local i, res
	for i = 1, stead.table.maxn(arg) do
		if type(arg[i]) == 'string' then
			if res == nil then
				res = ""
			else
				res = res..space;
			end
			res = res..arg[i];
		end 
	end
	return res;
end
stead.par = par
-- add to not nill string any string
function cat(v,...)
	local i, res
	if not v then
		return nil
	end
	res = v;
	for i = 1, stead.table.maxn(arg) do
		if type(arg[i]) == 'string' then
			res = res..arg[i];
		end 
	end
	return res;
end
stead.cat = cat;

function txtnb(v)
	if type(v) ~= 'string' then return nil; end
	return iface:nb(v);
end

function img(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:img(v);
end

function txtem(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:em(v)
end

function txtr(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:right(v)
end

function txtl(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:left(v)
end

function txtc(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:center(v)
end

function txtb(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:bold(v)
end

function txtu(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:under(v)
end

function txtnm(n, v)
	if type(v) ~= 'string' or not tonumber(n) then return nil; end
	return iface:enum(n, v);
end

fmt = function(...)
	local i, res
	if arg == nil then
		return false
	end
	for i=1,stead.table.maxn(arg) do
		if type(arg[i]) == 'string' then
			local s = stead.string.gsub(arg[i],'[\t ]+',' ');
			s = stead.string.gsub(s, '[\n]+', ' ');
			s = stead.string.gsub(s,'%^','\n');
 			res = stead.par('',res,s);
		end
	end
	return res
end
stead.fmt = fmt

-- integer lists
function inext(t, k)
	local v
	k, v = next(t, k);
	while k and not tonumber(k) do
		k, v = next(t, k);
	end
	if not tonumber(k) then
		return nil
	end
	return k, v
end

function ilist(s, var)
	return inext, s, nil;
end

function ordered_i(t)
	local ordered = {};
	local i,v, max;
	max = 0;
	for i,v in ilist(t) do
		stead.table.insert(ordered, i);
		max = max + 1;
	end
	stead.table.sort(ordered);
	ordered.i = 1;
	ordered.max = max;
	return ordered;
end

function onext(t, k)
	local v
	if not k then
		k = ordered_i(t);
	end
	if k.i > k.max then
		return nil
	end
	v = k[k.i]
	k.i = k.i + 1
	return k, t[v], v;
end

function opairs(s, var)
	return onext, s, nil;
end

function isPlayer(v)
	return (type(v) == 'table') and (v.player_type)
end

function isRoom(v)
	return (type(v) == 'table') and (v.location_type)
end

function isPhrase(v)
	return (type(v) == 'table') and (v.phrase_type)
end

function isDialog(v)
	return (type(v) == 'table') and (v.dialog_type)
end

function isDisabled(v)
	return (type(v) == 'table') and (v._disabled)
end

function isRemoved(v)
	return (type(v) == 'table') and (v._disabled == -1)
end

function isObject(v)
	return (type(v) == 'table') and (v.object_type)
end

function obj_xref(self,str)
	function xrefrep(str)
		local s = stead.string.gsub(str,'[{}]','');
		return xref(s, self);
	end
	if not str then
		return
	end
	if not isObject(self) then
		return str;
	end
	local s = stead.string.gsub(str,'{[^}]+}',xrefrep);
	return s;
end

function obj_look(self)
	local i, vv, o
	if isDisabled(self) then
		return
	end
	local v = call(self,'dsc');
	if game.hinting then
		v = self:xref(v);
	elseif v then
		v = stead.string.gsub(v, '[{}]','');
	end
	for i,o in opairs(self.obj) do
		o = ref(o);
		if isObject(o) then
			vv = obj_look(o);
			v = stead.par(' ',v, vv); 
		end
	end
	return v;
end


function obj_remove(self)
	self._disabled = -1;
	return self
end

function obj_disable(self)
	self._disabled = true;
	return self
end

function obj_enable(self)
	self._disabled = false;
	return self
end

function obj_enable_all(s)
	local k,v
	if not isObject(s) then
		return
	end
	for k,v in opairs(objs(s)) do
		local o = ref(v);
		if isObject(o) then
			o:enable()
		end
	end
end

function obj_disable_all(s)
	local k,v
	if not isObject(s) then
		return
	end
	for k,v in opairs(objs(s)) do
		local o = ref(v);
		if isObject(o) then
			o:disable()
		end
	end
end


function obj_save(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: object "..name.." can not be saved!");
		return
	end
	savemembers(h, self, name, need);
end

function obj_str(self)
	local i, v, vv, o;
	if not isObject(self) then
		return
	end
	if isDisabled(self) then
		return 
	end
	for i,o in opairs(self.obj) do
		o = ref(o);
		if o~= nil and not isDisabled(o) then -- isObject is better, but compat layer must be ok
			vv = call(o, 'nam');
			vv = xref(vv, o);
			v = stead.par(',', v, vv, obj_str(o));
		end
	end
	return v;
end

function obj(v)
	if v.nam == nil then
		if isRoom(v) then
			if isDialog(v) then
				error ("No dialog name in constructor.", 3);
			end
			error ("No room name in constructor.", 3);
		end
		error ("No object name in constructor.", 2);
	end
	v.object_type = true;

	if v.xref == nil then
		v.xref = obj_xref;
	end

	if v.look == nil then
		v.look = obj_look;
	end
	if v.enable == nil then
		v.enable = obj_enable;
	end
	if v.disable == nil then
		v.disable = obj_disable;
	end
	if v.enable_all == nil then
		v.enable_all = obj_enable_all;
	end
	if v.disable_all == nil then
		v.disable_all = obj_disable_all;
	end
	if v.remove == nil then
		v.remove = obj_remove;
	end
	if v.obj == nil then
		v.obj = {};
	end
	if v.srch == nil then
		v.srch = obj_search;
	end
	if v.str == nil then
		v.str = obj_str;
	end
	v.obj = list(v.obj);
	if v.save == nil then
		v.save = obj_save;
	end
	return v
end


function ref(n) -- ref object by name
	if type(n) == 'string' then
		local f = loadstring('return '..n);
		if f then
			return ref(f());
		end
		return nil;
	end
	if type(n) == 'table' then
		return n;
	end
	if type(n) == 'function' then
		return ref(n());
	end
	return nil
end

function deref(n)
	if type(n) == 'string' then
		return n
	end
	
	if type(n) == 'table' and type(n.key_name) == 'string' then
		return n.key_name
	end
	return n
end

function list_check(self, name)
	local i, v, ii;
	for i,v,ii in opairs(self) do
		local o = ref(v);
		if not o then -- isObject(o) then -- compat
			error ("No object: "..tostring(v))
			return false
		end
		if deref(v) then
			self[ii] = deref(v);
		end
	end
	return true; 
end

function list_str(self)
	local i, v, vv, o;
	for i,o in opairs(self) do
		o = ref(o);
		if o~= nil and not isDisabled(o) then
			vv = call(o, 'nam');
			vv = xref(vv, o);
			v = stead.par(',', v, vv);
		end
	end
	return v;
end


function list_add(self, name, pos)
	local nam
	nam = deref(name);
	if self:look(nam) then
		return nil
	end
	self.__modified__ = true;
	if tonumber(pos) then
		stead.table.insert(self, tonumber(pos), nam);
		self[tonumber(pos)] = nam; -- for spare lists
	else
		stead.table.insert(self, nam);
	end
	return true
end

function list_set(self, name, pos)
	local nam
	local i = tonumber(pos);
	if not i then
		return nil
	end
	nam = deref(name);
	self.__modified__ = true;
	self[i] = nam; -- for spare lists
	return true
end

function list_find(self, name)
	local n, v, ii
	for n,v,ii in opairs(self) do 
		if ref(v) == ref(name) then
			return ii; 
		end	
	end
	return nil
end

function list_save(self, name, h, need)
	if self.__modifyed__ or self.__modified__ then -- compat
		h:write(name.." = list({});\n");
		need = true;
	end
	savemembers(h, self, name, need);
end

function list_name(self, name, dis)
	local n, o, ii
	for n,o,ii in opairs(self) do
		o = ref(o);
		if isObject(o) then
			local nam = call(o,'nam') ;
			if ( not isDisabled(o) or dis ) and name == tostring(nam) then
				return ii;
			end
		end
	end
	return nil
end
function list_id(self, id, dis)
	local n,o,ii
	for n,o,ii in opairs(self) do
		o = ref(o);
		if dis or not isDisabled(o) then
			if isObject(o) and id == o.id then
				return ii;
			end
		end
	end
end

function list_search(self, n, dis)
	local i;
	i = self:look(n);
	if not i then
		i = self:name(n, dis);
	end
	if not i and tonumber(n) then
		i = self:byid(tonumber(n), dis);
		if not i then
			return nil
		end
	end
	if not dis and isDisabled(ref(self[i])) then
		return nil;
	end
	return self[i], i;
end

function list_zap(self)
	local n,o,ii
	for n,o,ii in opairs(self) do
		self[ii] = nil;
	end
	self.__modified__ = true
	return self
end

function list_concat(self, other, pos)
	local n,o,ii
	for n,o,ii in opairs(other) do
		o = ref(o);
		if pos == nil then
			self:add(deref(o));
		else 
			self:add(deref(o), pos);
			pos = pos + 1;
		end
	end
end

function list_del(self, name)
	local v,n
	v, n = self:srch(name);
	if n == nil then
		return nil;
	end
	self.__modified__ = true
	v = stead.table.remove(self, n);
	if not v then
		v = self[n];
		self[n] = nil -- for spare lists
	end
	return v
end

function list(v)
	v.list_type = true;
	v.add = list_add;
	v.set = list_set;
	v.cat = list_concat;
	v.zap = list_zap;
	v.del = list_del;
	v.look = list_find;
	v.name = list_name;
	v.byid = list_id;
	v.srch = list_search;
	v.str = list_str;
	v.check = list_check;
	v.save = list_save;
	return v;
end

function isList(v)
	return (type(v) == 'table') and (v.list_type == true)
end

function call(v, n, ...)
	if type(v) ~= 'table' then
		error ("Call on non table object:"..n, 2);
	end
	if v[n] == nil then
		return nil,nil;
	end
	if type(v[n]) == 'string' then
		return v[n];
	end
	if type(v[n]) == 'function' then
		callpush(v, unpack(arg))
		local a,b = v[n](v, unpack(arg));
		-- boolean, nil
		if type(a) == 'boolean' and b == nil then
			b, a = a, stead.pget()
			if a == nil then
				a = b
				b = nil
			end 
		elseif a == nil and b == nil then
			a = stead.pget()
			b = nil
		end
		if a == nil and b == nil and cctx().action then
			a = true
		end
		callpop()
		return a,b
	end
	if type(v[n]) == 'boolean' then
		return v[n]
	end
	error ("Method not string nor function:"..tostring(n), 2);
end

function call_bool(v, n, ...)
	if type(v) ~= 'table' then
		error ("Call bool on non table object:"..n, 2);
	end
	
	if v[n] == nil then
		return nil
	end	
	
	if v[n] == false then
		return false;
	end
	
	if type(v[n]) == 'function' then
		callpush(v, unpack(arg))
		local r = v[n](v, unpack(arg));
		callpop();
		return r;
	end
	return true; -- not nil
end

function room_scene(self)
	local v;
	v = iface:title(call(self,'nam'));
	v = stead.par('^^', v, call(self,'dsc')); --obj_look(self));
	return stead.cat(v,' ');
end

function room_look(self)
	local i, vv, o;
	for i,o in opairs(self.obj) do
		o = ref(o);
		if isObject(o) then
			vv = stead.par(' ',vv, o:look());
		end
	end
	return stead.cat(vv,' ');
end

function obj_search(v, n, dis)
	local i;
	local o;
	if not dis and isDisabled(v) then
		return
	end
	o = v.obj:srch(n, dis);
	if o then
		return o, v;
	end
	for i,o in opairs(v.obj) do
		o = ref(o);
		if isObject(o) then
			local r,rr = obj_search(o, n, dis);
			if r then
				return r, rr;
			end
		end
	end
	return;
end

function room_save(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: room "..name.." can not be saved!");
		return
	end
	savemembers(h, self, name, need);
end

function room(v) --constructor
--	if v.nam == nil then
--		error ("No room name in constructor.", 2);
--	end
	if v.scene == nil then
		v.scene = room_scene;
	end
	if v.look == nil then
		v.look = room_look;
	end
	if v.save == nil then
		v.save = room_save;
	end
	v.location_type = true;
	if v.way == nil then
		v.way = { };
	end
	v.way = list(v.way);
	v = obj(v);
	return v;
end

function dialog_enter(self)
	if not dialog_rescan(self) then
		return nil, false
	end
	return nil, true
end

function dialog_scene(self)
	local v
	v = iface:title(call(self,'nam'));
	v = stead.par('^^', v, call(self, 'dsc')); --obj_look(self));
	return v;
end

function dialog_look(self)
	local i,n,v,ph
	n = 1
	for i,ph in opairs(self.obj) do
		ph = ref(ph);
		if isPhrase(ph) and not isDisabled(ph) then
			v = stead.par('^', v, txtnm(n, ph:look()));
			n = n + 1
		end
	end
	return v;
end

function dialog_rescan(self)
	local i,k,ph
	k = 1
	for i,ph in opairs(self.obj) do
		ph = ref(ph);
		if isPhrase(ph) and not isDisabled(ph) then
			ph.nam = tostring(k);
			k = k + 1;
		end
	end
	if k == 1 then
		return false
	end
	return true
end


function dialog_phrase(self, num)
	if not tonumber(num) then
		if isPhrase(ref(num)) then
			return ref(num);
		end
		return nil
	end
	return ref(self.obj[tonumber(num)]);
end

function ponoff(s, on, ...)
	local i, ph
	if stead.table.maxn(arg) == 0 then
		stead.table.insert(arg, self);
	end
	for i=1,stead.table.maxn(arg) do
		ph = dialog_phrase(s, arg[i]);
		if isPhrase(ph) and not isRemoved(ph) then
			if on then
				ph:enable();
			else 
				ph:disable();
			end
		end
	end
end

function dialog_prem(s, ...)
	local i, ph
	if stead.table.maxn(arg) == 0 then
		stead.table.insert(arg, self);
	end
	for i=1,stead.table.maxn(arg) do
		ph = dialog_phrase(s, arg[i]);
		if isPhrase(ph) then
			ph:remove();
		end
	end
end

function dialog_pon(self,...)
	return ponoff(self, true, unpack(arg));
end

function dialog_poff(self,...)
	return ponoff(self, false, unpack(arg));
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
	v = room(v);
	return v;
end

function phrase_action(self)
	local ph = self;
	local r = nil;
	local ret = nil;
	if isDisabled(ph) then
		return nil, false
	end
-- here it is
	ph:disable(); -- /* disable it!!! */

	if type(ph.do_act) == 'string' then
		local f = loadstring(ph.do_act);
		if f ~= nil then
			ret = f();
		else
			error ("Error while eval phrase action.");
		end
	elseif type(ph.do_act) == 'function' then
		ret = ph.do_act(self, nam);
	end
	local last = call(ph, 'ans');
	if last == true or ret == true then
		r = true;
	end
	if isDialog(here()) and not dialog_rescan(here()) then
		ret = stead.par(' ', ret, me():back());
	end
	
	ret = stead.par("^^", last, ret);
	
	if ret == nil then
		return r -- hack?
	end
	
	return ret
end

function phrase_save(self, name, h, need)
	if need then
		local m = " = phr("
		if isDisabled(self) then
			m = " = _phr("
		end
		h:write(stead.string.format("%s%s%q,%q,%q);\n", 
			name, m, tostring(self.dsc), tostring(self.ans), tostring(self.do_act)));
	end
	savemembers(h, self, name, false);
end

function phrase_look(self, n)
	if isDisabled(self) then
		return
	end
	local v = call(self, 'dsc');
	if type(v) ~= 'string' then return; end
	if game.hinting then
		return self:xref('{'..v..'}');
	end
	return v;
end

function phrase(o) --constructor
	local ret = o;
	ret.look = phrase_look;
	ret.nam = ''; -- for start
	ret.phrase_type = true;
	ret.act = phrase_action;
	ret.save = phrase_save;
	ret = obj(ret);
	return ret;
end

function _phr(ask, answ, act)
	local p = phrase ( { dsc = ask, ans = answ, do_act = act });
	p:disable();
	return p;
end

function phr(ask, answ, act)
	local p =  phrase ( { dsc = ask, ans = answ, do_act = act });
--	p:enable();
	return p;
end

function player_inv(self)
	return iface:inv(stead.cat(self:str()));
end

function player_ways(self)
	return iface:ways(stead.cat(ref(self.where).way:str()));
end

function player_objs(self)
	return iface:objs(stead.cat(ref(self.where):str()));
end

function player_look(self)
	return ref(self.where):scene();
end

function obj_tag(self, id)
	local k,v

	if isDisabled(self) then
		return id
	end
	
	for k,v in opairs(self.obj) do
		v = ref(v);
		if isObject(v) and not isDisabled(v) then
			id = id + 1;
			v.id = id;
			id = obj_tag(v, id);
		end
	end
	return id;
end

function player_tagall(self)
	local id, k, v;
	id = 0;

	id = obj_tag(here(), id);
	id = obj_tag(me(), id);

	for k,v in opairs(ways()) do
		v = ref(v);
		if isRoom(v) and not isDisabled(v) then
			id = id + 1;
			v.id = id;
		end
	end
end

function player_action(self, what, ...)
	local v,r,obj
	obj = ref(self.where):srch(what);
	if not obj then
		return call(ref(game), 'action', what, unpack(arg)); --player_do(self, what, unpack(arg));
	end
	v, r = player_take(self, what, unpack(arg));
	if not v then
		v, r = call(ref(obj), 'act', unpack(arg));
		if not v and r ~= true then
			v, r = call(ref(game), 'act', obj, unpack(arg));
		end
	end
	return v, r;
end

function player_take(self, what, ...)
	local v,r,obj,w
	obj,w = ref(self.where):srch(what);
	if not obj then
		return nil, false;
	end
	v,r = call(ref(obj), 'tak', unpack(arg));
	if v and r ~= false then
		take(obj, w);
	end
	return v;
end

function player_use(self, what, onwhat, ...)
	local obj, obj2, v, vv, r;
	local scene_use_mode = false

	obj = self:srch(what); -- in inv?
	if not obj then -- no
		obj = ref(self.where):srch(what); -- in scene?
		if not obj then -- no!
			return game.err, false;
		end
		scene_use_mode = true -- scene_use_mode!
	end
	if onwhat == nil then -- only one?
		if scene_use_mode then
			return self:action(what, unpack(arg)); -- call act
		else
			v, r = call(ref(obj),'inv', unpack(arg)); -- call inv
		end
		if not v and r ~= true then
			v, r = call(game, 'inv', obj, unpack(arg));
		end
		return v, r;
	end
	obj2 = ref(self.where):srch(onwhat); -- in scene?
	if not obj2 then
		obj2 = self:srch(onwhat); -- in inv?
	end
	if not obj2 or obj2 == obj then
		return game.err, false;
	end
	if not scene_use_mode or isSceneUse(ref(obj)) then
		v, r = call(ref(obj), 'use', obj2, unpack(arg));
		if r ~= false then
			vv = call(ref(obj2), 'used', obj, unpack(arg));
		end
	end
	if not v and not vv then
		v, r = call(game, 'use', obj, obj2, unpack(arg));
	end
	return stead.par(' ', v, vv);
end

function player_back(self)
	local where = ref(self.where);
	if where == nil then
		return nil,false
	end
	return go(self, where.__from__, true);
end

function go(self, where, back)
	local was = self.where;
	local need_scene = false;	
	local ret

	if not stead.in_goto_call then
		ret = function(rc) stead.in_goto_call = false return nil end
	else
		ret = function(rc) return rc end
	end

	stead.in_goto_call = true

	if where == nil then
		return nil,ret(false)
	end
	if not isRoom(ref(where)) then
		error ("Trying to go nowhere: "..where, 2);
	end
	if not isRoom(ref(self.where)) then
		error ("Trying to go from nowhere: "..self.where, 2);
	end

	if stead.in_entered_call or stead.in_onexit_call then
		error ("Do not use goto from onexit/entered action! Use exit/enter action instead:" .. self.where, 2);
	end

	local v, r;
	if not isVroom(ref(where)) and not stead.in_exit_call then
		stead.in_exit_call = true -- to break recurse
		v,r = call(ref(self.where), 'exit', where);
		stead.in_exit_call = nil
		if r == false then
			return v, ret(r)
		end
	end

	local res = v;

	v = nil;
	if not back or not isDialog(ref(self.where)) or isDialog(ref(where)) then
		v, r = call(ref(where), 'enter', self.where);
		if r == false then
			return v, ret(r)
		end
		need_scene = true;
		if ref(was) ~= ref(self.where) then -- jump !!!
			where = deref(self.where);
			need_scene = false;
		end
	end
	res = stead.par('^^',res,v);

	if not back then
		ref(where).__from__ = deref(self.where);
	end

	self.where = deref(where);

	ret();

	PLAYER_MOVED = true
	if need_scene then -- or isForcedsc(ref(where)) then -- i'am not sure...
		return stead.par('^^',res,ref(where):scene());
	end
	return res;
end

function player_goto(self, where)
	local v, r = go(self, where, false);
	return v, r;
end
function player_go(self, where)
	local w = ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	local v, r = go(self, w, false);
	return v, r;
end

function player_save(self, name, h)
	h:write(tostring(name)..".where = '"..deref(self.where).."';\n");
	savemembers(h, self, name, false);
end

function player(v)
	if v.nam == nil then
		error ("No player name in constructor.", 2);
	end
	if v.where == nil then
		v.where = 'main';
	end
	if v.tag == nil then
		v.tag = player_tagall;
	end
	if v.goto == nil then
		v.goto = player_goto;
	end
	if v.go == nil then
		v.go = player_go;
	end
	if v.ways == nil then
		v.ways = player_ways;
	end
	if v.back == nil then
		v.back = player_back;
	end
	if v.look == nil then
		v.look = player_look;
	end
	if v.inv == nil then
		v.inv = player_inv;
	end
	if v.use == nil then
		v.use = player_use;
	end
	if v.action == nil then
		v.action = player_action;
	end
	if v.save == nil then
		v.save = player_save;
	end
	if v.objs == nil then
		v.objs = player_objs;
	end
	v.player_type = true;
	return obj(v);
end

function game_life(self)
	local i,o
	local av,v
	stead.in_life_call = true;
	stead.lifes_off = list {}; -- lifes to off 
	local m = PLAYER_MOVED
	for i,o in opairs(self.lifes) do
		local vv
		local pre
		o = ref(o);
		if not isDisabled(o) then
			PLAYER_MOVED = false
			vv,pre = call(o,'life');
			if pre or (PLAYER_MOVED and pre ~= false) then
				av = stead.par(' ', av, vv);
			else
				v = stead.par(' ',v, vv);
			end
		end
	end
	if not PLAYER_MOVED then PLAYER_MOVED = m end
	stead.in_life_call = false;
	for i,o in ipairs(stead.lifes_off) do
		lifeoff(o);
	end
	stead.lifes_off = nil;
	return v, av;
end

function check_list(k, v, p)
	if v.check == nil or not v:check(stead.string.format("%s[%q]", p, k)) then
		error ("error in list: "..stead.object..'.'..k);
	end
end

function check_room(k, v)
	if v.obj == nil then
		error("no obj in room:"..k);
	end
	if v.way == nil then
		error("no way in room:"..k);
	end
end

function check_player(k, v)
	v.where = deref(v.where);
end

function check_object(k, v)
	if not v.nam then
		error ("No name in "..k);
	end
	if isRoom(v) then
		check_room(k, v);
	end
	if isPlayer(v) then
		check_player(k, v);
	end
	for_each(v, k, check_list, isList, deref(v))
end

function for_everything(f, ...)
	local is_ok = function(s)
		return true
	end
	for_each(_G, '_G', f, is_ok, unpack(arg))
end

function do_ini(self, load)
	local v='',vv
	local function call_key(k, o)
		o.key_name = k;
	end
	local function call_codekey(k, o)
		stead.functions[o].key_name = k;
	end
	local function call_ini(k, o, ...)
		v = stead.par('', v, call(o, 'ini'), unpack(arg));
	end

	math.randomseed(tonumber(os.date("%m%d%H%M%S")))
	rnd(1); rnd(1); rnd(1); -- Lua bug?

	game.pl = deref(game.pl);
	game.where = deref(game.where);

	if not load then
		for_each_object(call_key);
		for_each_codeblock(call_codekey);
		for_each_object(check_object);
		call_key("game", game);
		for_each(game, "game", check_list, isList, deref(game))
	end
	for_each_object(call_ini, load);
	me():tag();
	if not self.showlast then
		self._lastdisp = nil;
	end
	return stead.par('',v, self._lastdisp); --stead.par('^^',v);
end

function game_ini(self)
	local v,vv
	v = do_ini(self);
	vv = iface:title(call(self,'nam'));
	vv = stead.par('^^', vv, call(self,'dsc'));
	if type(init) == 'function' then
		init();
	end
--	if type(hooks) == 'function' then
--		hooks();
--	end
	return stead.par("^^", vv, v);
end

function game(v)
	if v.nam == nil then
		error ("No game name in constructor.", 2);
	end
	if v.pl == nil then
		v.pl = 'player';
	end
	if v.ini == nil then
		v.ini = game_ini;
	end
	if v.save == nil then
		v.save = game_save;
	end
	if v.load == nil then
		v.load = game_load;
	end
	if v.life == nil then
		v.life = game_life;
	end
	if v.step == nil then
		v.step = game_step;
	end
	if v.lifes == nil then
		v.lifes = {};
	end
	v.lifes = list(v.lifes);
	v._time = 0;
	v._running = true;
	v.game_type = true;
	return v;
end

function live(v)
	return ref(game.lifes:srch(v));
end

function isEnableSave()
	if game.enable_save == nil or get_autosave() then
		return true
	end
	return call_bool(game, 'enable_save');
end

function for_each(o, n, f, fv, ...)
	local k,v
	if type(o) ~= 'table' then
		return
	end
	stead.object = n;
	for k,v in pairs(o) do
		if fv(v) then
			local i = tonumber(k);
			local nn
			if i then
				nn = n.."["..i.."]"
			else
				if n == '_G' then
					nn = k;
				else
					nn = n.."."..k;
				end
			end
			f(k, v, unpack(arg));
		end
	end
end

function isCode(s)
	return type(s) == 'function' and type(stead.functions[s]) == 'table'
end
function for_each_codeblock(f,...)
	for_each(_G, '_G', f, isCode, unpack(arg))
end

function for_each_object(f,...)
	for_each(_G, '_G', f, isObject, unpack(arg))
end

function for_each_player(f,...)
	for_each(_G, '_G', f, isPlayer, unpack(arg))
end

function for_each_room(f,...)
	for_each(_G, '_G', f, isRoom, unpack(arg))
end

function for_each_list(f,...)
	for_each(_G, '_G', f, isList, unpack(arg))
end

function clearvar (v)
	local k,o
	for k,o in pairs(v) do
		if type(o) == 'table' and o.__visited__ ~= nil then
			o.__visited__ = nil
			clearvar(o)
		end
	end
end

function savemembers(h, self, name, need)
	local k,v
	for k,v in pairs(self) do
		local need2
		if k ~= "__visited__" then
			need2 = false
			if isForSave(k, v, self) then
				need2 = true;
			end
			
			if type(k) == 'string' then
				savevar(h, v, name..'['..stead.string.format("%q",k)..']', need or need2);
			elseif type(k) ~= 'function' then
				savevar(h, v, name.."["..k.."]", need or need2)
			end
		end
	end
end

function savevar (h, v, n, need)
	local r,f
	if v == nil or type(v) == "userdata" or
			 type(v) == "function" then
		if isCode(v) and need then
			if type(stead.functions[v].key_name) == 'string' 
				and stead.functions[v].key_name ~= n then
				h:write(stead.string.format("%s=%s\n", n, stead.functions[v].key_name))
			else
				h:write(stead.string.format("%s=code %q\n", n, stead.functions[v].code))
			end
		end
--		if need then
--			error ("Variable "..n.." can not be saved!");
--		end 
		return 
	end

--	if stead.string.find(n, '_') ==  1 or stead.string.match(n,'^%u') then
--		need = true;
--	end

	if type(v) == "string" then
		if not need then 
			return
		end
		h:write(stead.string.format("%s=%q\n",n,v))
		return;
	end
 	
	if type(v) == "table" then
		if v == _G then return end
		if type(v.key_name) == 'string' and v.key_name ~= n then -- just xref
			if need then
				h:write(stead.string.format("%s = %s\n", n, v.key_name));
			end
			return
		end
		if v.__visited__ ~= nil then
			return
		end

		v.__visited__ = n;

		if type(v.save) == 'function' then
			v:save(n, h, need);
			return;
		end

		if need then
			h:write(n.." = {};\n");
		end

		savemembers(h, v, n, need);
		return;
	end

	if not need then
		return
	end
	h:write(n, " = ",tostring(v))
	h:write("\n") 
end



function game_save(self, name, file) 
	local h;
	local function save_object(key, value, h)
		savevar(h, value, key, false);
		return true;
	end

	local function save_var(key, value, h)
		savevar(h, value, key, isForSave(key, value, _G))
	end

	if file ~= nil then
		file:write(name..".pl = '"..deref(self.pl).."'\n");
		savemembers(file, self, name, false);
		return nil, true
	end

	if not isEnableSave() then
		return nil, false
	end

	if name == nil then
		return nil, false
	end
	h = stead.io.open(name,"w");
	if not h then
		return nil, false
	end
	local n = call(here(),'nam');
	if type(n) == 'string' and n ~= "" then
		h:write("-- $Name: "..n:gsub("\n","\\n").."$\n");
	end
	save_object('allocator', allocator, h); -- always first!
	for_each_object(save_object, h);
	save_object('game', self, h);
	for_everything(save_var, h);
--	save_object('_G', _G, h);
	clearvar(_G);
	h:flush();
	h:close();
	game.autosave = false; -- we have only one try for autosave
	return nil;
end

function game_load(self, name) 
	if name == nil then
		return nil, false
	end
	local f, err = loadfile(name);
	if f then
		local i,r = f();
		if r then
			return nil, false
		end
		return do_ini(self, true);
	end
	return nil, false
end


function game_step(self)
	self._time = self._time + 1;
	return self:life(self);
end


game = game {
	codepage = "UTF-8",
	nam = "INSTEAD -- Simple Text Adventure interpreter v"..stead.version.." '2009-2010 by Peter Kosyh",
	dsc = [[
Commands:^
    look(or just enter), act <on what> (or just what), use <what> [on what], go <where>,^
    back, inv, way, obj, quit, save <fname>, load <fname>.]],
	pl ='pl',
	showlast = true, 
};
function strip(s)
	local s = tostring(s);
	s = stead.string.gsub(s, '^[ \t]*', '');
	s = stead.string.gsub(s, '[ \t]*$', '');
	return s;
end

function isForcedsc(v)
	local r,g
	r = call_bool(v, 'forcedsc');
	if r then
		return true
	end
	g = call_bool(game, 'forcedsc', v);
	return g and r ~= false
end

function isSceneUse(v)
	local o,g
	o = call_bool(v, 'scene_use');
	if o then
		return true
	end
	g = call_bool(game, 'scene_use', v);
	return g and o ~= false
end

iface = {
	img = function(self, str)
		return '';
	end,
	nb = function(self, str)
		return str;
	end,
	em = function(self, str)
		return str;
	end,
	right = function(self, str)
		return str;
	end,
	left = function(self, str)
		return str;
	end,
	center = function(self, str)
		return str;
	end,
	bold = function(self, str)
		return str;
	end,
	under = function(self, str)
		return str;
	end,
	enum = function(self, n, str)
		return n..' - '..str;
	end,
	xref = function(self, str, obj)
		local o = ref(here():srch(obj));
		if not o then 
			o = ref(ways():srch(obj));
		end
		if not o then
			o = ref(me():srch(obj));
		end
		if not o or not o.id then
			return str;
		end
		return stead.cat(str,"("..tostring(o.id)..")");
	end,
	title = function(self, str)
		return "["..str.."]";
	end,
	objs = function(self, str)
		return str;
	end,
	ways = function(self, str)
		return str;
	end,
	inv = function(self, str)
		return str;
	end,
	text = function(self, str)
		if str then
			print(str);
		end
	end,
	fmt = function(self, cmd, st, moved, r, av, objs, pv) -- st -- changed state (main win), move -- loc changed
		local l
		if st and not moved then
			if cmd ~= 'look' then
				av = txtem(av);
				pv = txtem(pv);
				r  = txtem(r);
				if isForcedsc(here()) then
					l = me():look();
				end
			end
		end
		vv = stead.fmt(stead.cat(stead.par("^^", l, r, av, objs, pv), '^'));
		return vv
	end,
	cmd = function(self, inp)
		local r, v;
		v = false
		stead.state = false; -- changed state (main screen)
		local a = { };
		local cmd;
		RAW_TEXT = nil
		PLAYER_MOVED = nil
		cmd,a = stead.getcmd(inp);
		if cmd == '' then cmd = 'look' end
--		me():tag();
		local oldloc = here();
		if cmd == 'look' then
			stead.state = true
			r,v = me():look();
		elseif cmd == 'obj' then
			r,v = me():objs();
		elseif cmd == 'inv' then
			r,v = me():inv();
		elseif cmd == 'way' then
			r,v = me():ways();
		elseif cmd == 'ls' then
			r = stead.par('^^', me():objs(), me():inv(), me():ways());
			v = nil;
		elseif cmd == 'go' then
			stead.state = true
			r,v = me():go(unpack(a));
		elseif cmd == 'back' then
			stead.state = true
			r,v = me():go(from());
		elseif cmd == 'act' then
			stead.state = true
			r,v = me():action(unpack(a));
		elseif cmd == 'use' then
			stead.state = true
			r,v = me():use(unpack(a));
		elseif cmd == 'save' then
			r, v = game:save(unpack(a));
		elseif cmd == 'load' then
			r, v = game:load(unpack(a));
			if v ~= false and game.showlast then
				return r;
			end
		elseif cmd == 'wait' then
			v = nil;
			r = nil;
			stead.state = true
		elseif cmd == 'nop' then
			v = true;
			r = nil;
			stead.state = true
		else
			stead.state = true
			r,v = me():action(cmd, unpack(a));
		end
		-- here r is action result, v - ret code value	
		-- state -- game state changed
		if stead.state and r == nil and v == true then -- we do nothing
			return nil;
		end
		if RAW_TEXT then
			v = false
		end

		if v == false then
			return stead.cat(r, '\n'), false;
		end

		ACTION_TEXT = r; -- here, life methods can redefine this

		local av, pv -- av -- active lifes, pv -- background
		local vv

		if stead.state then
			pv,av = game:step();
			me():tag();
			vv = here():look();
		end

		vv = self:fmt(cmd, stead.state, (oldloc ~= here()) or PLAYER_MOVED, 
			ACTION_TEXT, av, vv, pv);

		if stead.state then
			game._lastdisp = vv
		end

		if vv == nil then -- nil is error
			return ''
		end
		return vv;
	end, 
	shell = function(self)
		local inp, i, k, cmd, a, n;
		me():tag();		
		while game._running do
			inp = stead.io.read("*l");
			if inp == 'quit' then
				break;
			end
			self:text(self:cmd(inp));
		end
	end
};


function me()
	return ref(game.pl);
end

function where(s)
	if isPlayer(ref(s)) then
		return ref(ref(s).where);
	end
	return ref(ref(s).__where__);
end

function here()
	return ref(me().where);
end

function from(w)
	if w == nil then
		w = here();
	else
		w = ref(w);
	end
	return ref(w.__from__);
end

function time()
	return game._time;
end
stead.time = time

function inv()
	return me().obj;
end

function objs(w)
	if not w then
		return here().obj;
	else
		return ref(w).obj;
	end
end

function ways(w)
	if not w then
		return here().way;
	else
		return ref(w).way;
	end
end

function xref(str, obj, ...)
	if type(str) ~= 'string' then return nil; end; 
	return iface:xref(str, obj, unpack(arg));
end

function pon(...)
	if not isDialog(here()) then
		return
	end
	dialog_pon(here(), unpack(arg));
end
function poff(...)
	if not isDialog(here()) then
		return
	end
	dialog_poff(here(), unpack(arg));
end
function prem(...)
	if not isDialog(here()) then
		return
	end
	dialog_prem(here(), unpack(arg));
end

function lifeon(what)
	game.lifes:add(what);
end

function lifeoff(what)
	if stead.in_life_call then
		stead.lifes_off:add(what);
		return
	end
	game.lifes:del(what);
end

function allocator_save(s, name, h, need)
	if need then
		local m = ' = allocator:get("'..name:gsub('"','\\"')..'","'..tostring(s.constructor):gsub('"','\\"')..'");';
		h:write(name..m..'\n');
	end
	savemembers(h, s, name, false);
end

function new(str)
	if type(str) ~= 'string' then
		error("Non string constructor in new.", 2);
	end
	return allocator:new(str);
end

function delete(v)
	allocator:delete(v);
end

timer = obj { -- timer calls stead.timer callback 
	nam = 'timer',
	ini = function(s)
		if tonumber(s._timer) ~= nil and type(set_timer) == 'function' then
			set_timer(s._timer);
		end
	end,
	get = function(s)
		if tonumber(s._timer) == nil then
			return 0
		end
		return tonumber(s._timer);
	end,
	stop = function(s)
		return s:set(0);
	end,
	del = function(s)
		return s:set(0);
	end,
	set = function(s, v)
		s._timer = tonumber(v);
		if type(set_timer) ~= 'function' then
			return false
		end
		set_timer(v)
		return true
	end,
--[[ 	callback = function(s)
	end, ]]
};

input = obj { -- input object
	nam = 'input',
--[[	key = function(s, down, key)
		return
	end, ]]
--[[	click = function(s, down, mb, x, y, [ px, py ] )
		return
	end ]]
};

function vobj_save(self, name, h, need)
	local dsc = self.dsc;
	local w = deref(self.where);
	
	if dsc == nil then
		dsc = 'nil';
	else
		dsc = stead.string.format("%q", tostring(dsc))
	end

	if w == nil then
		w = 'nil';
	else
		w = stead.string.format("%q", w)
	end
	
	if need then
		h:write(stead.string.format("%s  = vobj(%s, %q, %s, %s);\n",
			name, tostring(self.key), tostring(self.nam), dsc, w));

	end
	savemembers(h, self, name,false);
end

function vobj_act(self, ...)
	local o, r = here():srch(self); -- self.nam
	if ref(o) and ref(o).where then
		return goto(ref(o).where);
	end
	return call(ref(r),'act', self.key, unpack(arg));
end

function vobj_used(self, ...)
	local o, r = here():srch(self.nam);
	return call(ref(r),'used', self.key, unpack(arg));
end

function vobj(key, name, dsc, w)
	if not tonumber(key) then
		error ("vobj key must be number!", 2);
	end
	return obj{ key = key, nam = name, dsc = dsc, where = deref(w), act = vobj_act, used = vobj_used, save = vobj_save, obj = list({}) };
end

function vway(name, dsc, w)
--	o.object_type = true;
	return  obj{ key = -1, nam = name, dsc = dsc, act = vobj_act, where = deref(w), used = vobj_used, save = vobj_save, obj = list({}), };
end

function vroom_save(self, name, h, need)
	if need then
		h:write(name.." = vroom('"..self.nam.."','"..deref(self.where).."');\n");
	end
	savemembers(h, self, name,false);
end

function vroom_enter(self, ...)
	return go(me(), self.where, false);
end

function isVroom(v)
	return (type(v) == 'table') and (v.vroom_type)
end

function vroom(name, w)
	return room { vroom_type = true, nam = name, where = deref(w), enter = vroom_enter, save = vroom_save, };
end

function goto(what)
	local v,r=me():goto(what);
	me():tag();
	return v,r;
end
function back()
	return me():back();
end
function rnd(m)
	return math.random(m);
end

function taken(obj)
	if isObject(ref(obj)) and ref(obj)._taken then
		return true
	end
	return false;
end

function remove(obj, from)
	local o,w
	if from then
		o,w = ref(from):srch(obj);
	else
		o,w = here():srch(obj);
	end
	if w then
		ref(w).obj:del(obj);
	end
	o = ref(o);
	if not isObject(o) then
		o = ref(obj);
	end
	return o
end

function taketo(obj, wh, pos)
	local o = remove(obj, wh);
	if not isObject(o) then
		error ("Trying to take wrong object.", 2);
	end
	inv():add(obj, pos);
	o._taken = true
	return o
end

function take(obj, wh)
	return taketo(obj, wh);
end

function takef(obj, wh)
	return taketo(obj, wh, 1);
end

function putto(obj, w, pos)
	local wh
	local o = ref(obj);
	if not isObject(o) then
		error ("Trying to put wrong object.", 2);
	end
	if not w then
		wh = deref(here());
		w = here();
	else
		wh = deref(w);
		w = ref(w);
	end
	w.obj:add(obj, pos);
	if type(wh) == 'string' then
		o.__where__ = wh;
	end
	return o;
end


function put(obj, w)
	return putto(obj, w);
end

function putf(obj, w)
	return putto(obj, w, 1);
end

function drop(obj, w)
	local o = put(obj, w);
	if not isObject(o) then
		error ("Trying to drop wrong object.", 2);
	end
	inv():del(obj);
	o._taken = false
	return o;
end

function dropf(obj)
	local o = putf(obj);
	if not isObject(o) then
		error ("Trying to dropf wrong object.", 2);
	end
	inv():del(obj);
	o._taken = false
	return o;
end

function seen(obj, wh)
	if not wh then
		wh = here();
	else
		wh = ref(wh);
	end
	local o,w = wh:srch(obj);
	o = ref(o);
	if isObject(o) then
		return o,w
	end
	return nil
end

function exist(obj, wh)
	if not wh then
		wh = here();
	else
		wh = ref(wh);
	end
	local o,w = wh:srch(obj, true);
	o = ref(o);
	if isObject(o) then
		return o,w
	end
	return nil
end

function have(obj)
	local o = inv():srch(obj);
	o = ref(o);
	if isObject(o) then
		return o
	end
	return nil
end

function moveto(obj, there, from, pos)
	remove(obj, from);
	putto(obj, there, pos);
	return ref(obj);
end


function move(obj, there, from)
	return moveto(obj, there, from);
end

function movef(obj, there, from)
	return moveto(obj, there, from, 1);
end

function get_picture()
	local s = call(here(),'pic');
	if not s then
		s = call(game, 'pic');
	end
	return s;
end

function get_title()
	local s = call(here(),'nam');
	return s;
end

function get_music()
	return game._music, game._music_loop;
end

function get_music_loop()
	return game._music_loop;
end

function save_music(s)
	s.__old_music__ = get_music();
	s.__old_loop__ = get_music_loop();
end

function restore_music(s)
	set_music(s.__old_music__, s.__old_loop__);
end

function dec_music_loop()
	if game._music_loop == 0 then
		return 0
	end
	game._music_loop = game._music_loop - 1;
	if game._music_loop == 0 then
		game._music_loop = -1;
	end
	return game._music_loop;
end

function set_music(s, count)
	game._music = s;
	if not tonumber(count) then
		game._music_loop = 0;
	else
		game._music_loop = tonumber(count);
	end
end

function stop_music()
	set_music(nil, -1);
end

function is_music()
	return game._music ~= nil and game._music_loop ~= -1
end

if is_sound == nil then
	function is_sound()
		return false -- sdl-instead export own function
	end
end

if get_savepath == nil then
	function get_savepath()
		return "./"
	end
end

function autosave(slot)
	game.autosave = true;
	game.autosave_slot = slot;
end

function get_autosave()
	return game.autosave, game.autosave_slot
end

function get_sound()
	return game._sound, game._sound_channel, game._sound_loop;
end

function get_sound_chan()
	return game._sound_channel
end

function get_sound_loop()
	return game._sound_loop
end

function set_sound(s, chan, loop)
	game._sound = s;	
	if not tonumber(loop) then
		game._sound_loop = 1;
	else
		game._sound_loop = tonumber(loop);
	end

	if not tonumber(chan) then
		game._sound_channel = -1;
	else
		game._sound_channel = tonumber(chan);
	end
end

function change_pl(p)
	local o = ref(p);
	if type(deref(p)) ~= 'string' or not o then
		error ("Wrong player name in change_pl...", 2);
	end
	game.pl = deref(p);
	return goto(o.where);
end

function disabled(o)
	return isDisabled(ref(o))
end

function disable(o)
	o = ref(o)
	if isObject(o) then
		o:disable()
	end
	return o
end

function enable(o)
	o = ref(o)
	if isObject(o) then
		o:enable()
	end
	return o
end

function disable_all(o)
	o = ref(o)
	if isObject(o) then
		o:disable_all()
	end
	return o
end

function enable_all(o)
	o = ref(o)
	if isObject(o) then
		o:enable_all()
	end
	return o
end

function isForSave(k, v, s) -- k - key, v - value, s -- parent table
	if type(k) == 'function' then
		return false
	end
	if type(v) == 'function' or type(v) == 'userdata' then
		return false
	end
	return stead.string.find(k, '_') ==  1 or stead.string.match(k,'^%u')
end

function inherit(o, f)
	return function(...)
		return f(o(unpack(arg)))
	end
end
stead.inherit = inherit

function hook(o, f)
	return function(...)
		local ff
		if type(o) ~= 'function' then
			ff = function(s)
				return o;
			end
		else
			ff = o
		end
		return f(ff, unpack(arg))
	end
end
stead.hook = hook

function nameof(v)
	if isObject(v) then
		local r = call(v, 'nam');
		return r
	end
end

function stead_version(v)
	if not tostring(v) then
		return
	end
	if stead.version < v then
		error ([[The game requires instead engine of version ]] ..v.. [[ or higher. http://instead.googlecode.com]], 2)
	end
	stead.api_version = v
	if v >= "1.2.0" then
		require ("goto")
		require ("vars")
		require ("object")
	end
end
instead_version = stead_version

function code(v)
	local f = loadstring(v)
	if not f then
		error ("Wrong script: "..tostring(v), 2);
	end
	stead.functions[f] = { f = f, code = v };
	return f;
end
stead.code = code
--- here the game begins
stead.objects = function(s)
	allocator = obj {
		nam = 'allocator',
		get = function(s, n, c)
			local v = ref(c);
			if not v then
				error ("Null object in allocator: "..tostring(c));
			end
			v.key_name = n;
			v.save = allocator_save;
			v.constructor = c;
			return v
		end,
		delete = function(s, w)
			w = ref(w);
			if type(w.key_name) ~= 'string' then
				return
			end
			local f = loadstring(w.key_name..'= nil;');
			if f then
				f();
			end
		end,
		new = function(s, n)
			local v = ref(n);
			if type(v) ~= 'table' or type(n) ~= 'string' then
				error ("Error in new.", 2);
			end
			v.save = allocator_save;
			v.constructor = n;
			stead.table.insert(s.objects, v);
			v.key_name = 'allocator["objects"]['..stead.table.maxn(s.objects)..']';
			return v
		end,
		objects = {
			save = function(self, name, h, need)
				savemembers(h, self, name, true);
			end,
		},
	};

	pl = player {
		nam = "Incognito",
		where = 'main',
		obj = { }
	};

	main = room {
		nam = 'main',
		dsc = 'No main room defined.',
	};
end

stead.init = function(s)
	stead:objects();
	s.functions = {} -- code blocks
	local k,v
	for k,v in ipairs(s.modules_ini) do
		v();
	end
end
stead:init();
-- vim:ts=4
