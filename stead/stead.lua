stead = {
	ver = { maj = 0, min = 9, rev = 1, str = "0.9.1", }
}

-- merge strings with "space" as separator
function par(space,...)
	local i, res
	for i = 1, table.maxn(arg) do
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

-- add to not nill string any string
function cat(v,...)
	local i, res
	if not v then
		return nil
	end
	res = v;
	for i = 1, table.maxn(arg) do
		if type(arg[i]) == 'string' then
			res = res..arg[i];
		end 
	end
	return res;
end

function txtem(v)
	return iface:em(v)
end

function txtr(v)
	return iface:right(v)
end

function txtl(v)
	return iface:left(v)
end

function txtc(v)
	return iface:center(v)
end

function txtb(v)
	return iface:bold(v)
end

function txtu(v)
	return iface:under(v)
end

function fmt(...)
	local i, res
	if arg == nil then
		return false
	end
	for i=1,table.maxn(arg) do
		if type(arg[i]) == 'string' then
			local s = string.gsub(arg[i],'[\t ]+',' ');
			s = string.gsub(s, '[\n]+', ' ');
			s = string.gsub(s,'%^','\n');
 			res = par('',res,s);
		end
	end
	return res
end
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
		table.insert(ordered, i);
		max = max + 1;
	end
	table.sort(ordered);
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
		local s =  string.gsub(str,'[{}]','');
		return xref(s, self);
	end
	if not str then
		return
	end
	if not isObject(self) then
		return str;
	end
	local s = string.gsub(str,'{[^}]+}',xrefrep);
	return s;
end

function obj_look(self)
	local i, vv, o
	if isDisabled(self) then
		return
	end
	local v = call(self,'dsc');
	if game.hinting then
		v = obj_xref(self, v);
	elseif v then
		v = string.gsub(v, '[{}]','');
	end
	for i,o in opairs(self.obj) do
		o = ref(o);
		if isObject(o) then
			vv = obj_look(o);
			v = par(' ',v, vv); 
		end
	end
	return v;
-- iface:xref(v,self.nam);
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
--	if self._disabled == nil then
--		return
--	end
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
		h:write(name.." = obj {nam = '"..tostring(self.nam).."'}\n");
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
--			vv = cat(vv,'(',tostring(ref(self[i]).id),')');
			v = par(',', v, vv, obj_str(o));
--			obj_str(o);
		end
	end
	return v;
end

function obj(v)
	if v.nam == nil then
		error ("No object name in constructor.");
	end
	if v.look == nil then
		v.look = obj_look;
	end
	v.object_type = true;
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
	-- error "Ref to unknown object."
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

function list_check(self)
	local i, v, ii;
	for i,v,ii in opairs(self) do
		local o = ref(v);
		if not o then -- isObject(o) then -- compat
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
--		if isObject(o) and not isDisabled(o) then
		if o~= nil and not isDisabled(o) then
			vv = call(o, 'nam');
			vv = xref(vv, o);
--			vv = cat(vv,'(',tostring(ref(self[i]).id),')');
			v = par(',', v, vv);
		end
	end
	return v;
end


function list_add(self, name, pos)
	local nam
--	if type(name) ~= 'string' then
--		error "No string adding to list."
--	end
	nam = deref(name);
	if self:look(nam) then
		return nil
	end
	self.__modifyed__ = true;
	if tonumber(pos) then
		table.insert(self, tonumber(pos), nam);
		self[tonumber(pos)] = nam; -- for spare lists
	else
		table.insert(self, nam);
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
	self.__modifyed__ = true;
	self[i] = nam; -- for spare lists
	return true
end

function list_find(self, name)
	local n, v, ii
--	if type(name) ~= 'string' then
--		error "No string finding in list."
--	end
	for n,v,ii in opairs(self) do 
		if ref(v) == ref(name) then
			return ii; 
		end	
	end
	return nil
end

function list_save(self, name, h, need)
	if self.__modifyed__ then
		h:write(name.." = list({});\n");
		need = true;
	end
	savemembers(h, self, name, need);
end

function list_name(self, name)
	local n, o, ii
--	if type(name) ~= 'string' then
--		error "No string finding in list."
--	end
	for n,o,ii in opairs(self) do
		o = ref(o);
		if isObject(o) then
			local nam = call(o,'nam') ;
			if not isDisabled(o) and name == tostring(nam) then
				return ii;
			end
		end
	end
	return nil
end
function list_id(self, id)
	local n,o,ii
	for n,o,ii in opairs(self) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) and id == o.id then
			return ii;
		end
	end
end

function list_search(self, n)
	local i;
	i = self:look(n);
	if not i then
		i = self:name(n);
	end
	if not i and tonumber(n) then
		i = self:byid(tonumber(n));
		if not i then
			return nil
		end
	end
	if isDisabled(self[i]) then
		return nil;
	end
	return self[i], i;
end

function list_zap(self)
	local n,o,ii
	for n,o,ii in opairs(self) do
		self[ii] = nil;
	end
	self.__modifyed__ = true
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
	self.__modifyed__ = true
	v = table.remove(self, n);
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
		error ("Call on non table object:"..n);
	end
	if v[n] == nil then
		return nil,nil;
	end
	if type(v[n]) == 'string' then
		return v[n],true;
	end 
	if type(v[n]) == 'function' then
		return v[n](v, unpack(arg));
	end
	error ("Method not string nor function:"..tostring(n));
end

function room_scene(self)
	local v;
	v = iface:title(call(self,'nam'));
	v = par('^^', v, call(self,'dsc')); --obj_look(self));
	return cat(v,' ');
end

function room_look(self)
	local i, vv, o;
	for i,o in opairs(self.obj) do
		o = ref(o);
		if isObject(o) then
			vv = par(' ',vv, o:look());
		end
	end
	return cat(vv,' ');
end

function obj_search(v, n)
	local i;
	local o;
	if isDisabled(v) then
		return
	end
	o = v.obj:srch(n);
	if o then
		return o, v;
	end
	for i,o in opairs(v.obj) do
		o = ref(o);
		if isObject(o) then
			local r,rr = obj_search(o, n);
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
		h:write(name.." = room {nam = '"..tostring(self.nam).."'}\n");
	end
	savemembers(h, self, name, need);
end

function room(v) --constructor
	if v.nam == nil then
		error "No room name in constructor.";
	end
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
--	self._last = nil;
	if not dialog_rescan(self) then
		return nil, false
	end
	return nil, true
end

function dialog_scene(self)
	local v
	v = iface:title(call(self,'nam'));
	v = par('^^', v, call(self, 'dsc')); --obj_look(self));
--	if self._last then
--		v = par('^^', v, self._last);
--	else
--		v = par('^^', v, call(self, 'dsc'));
--	end
	return v;	
end

function dialog_look(self)
	local i,n,v,ph
	n = 1
	for i,ph in opairs(self.obj) do
		ph = ref(ph);
		if isPhrase(ph) and not isDisabled(ph) then
			if game.hinting then
				v = par('^', v, n..' - '..xref(call(ph,'dsc'),ph));
			else
				v = par('^', v, n..' - '..string.gsub(call(ph,'dsc'),'[{}]',''));
			end
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
--	self._last = call(ph, 'ans');
--	return self._last;
end


function dialog_phrase(self, num)
	if not tonumber(num) then
		return nil
	end
	return ref(self.obj[tonumber(num)]);
end

function ponoff(self, on, ...)
	local i, ph
	for i=1,table.maxn(arg) do
		ph = dialog_phrase(self, arg[i]);
		if isPhrase(ph) and not isRemoved(ph) then
			 if on then
				ph:enable();
			else 
				ph:disable();
			end
--			ph.__changed__ = true;
		end
	end
end

function dialog_prem(self, ...)
	local i, ph
	for i=1,table.maxn(arg) do
		ph = dialog_phrase(self, arg[i]);
		if isPhrase(ph) then
			ph:remove();
--			ph.__changed__=true
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
	local r = here();
	local ret;
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
			error ("Error while eval phrase action");
		end
	elseif type(ph.do_act) == 'function' then
		ret = ph.do_act(self, nam);
	end
	local last = call(ph, 'ans');
	if isDialog(here()) and not dialog_rescan(here()) then
		ret = par(' ', ret, me():back());
	end
	return par("^^", last, ret);
--	if dialog_getn(self, 1) == nil then
--		me():back();
--	end
--	self._last = call(ph, 'ans');
--	return self._last;
end

function phrase_save(self, name, h, need)
	if need then
		local m = " = phr('"
		if isDisabled(self) then
			m = " = _phr('"
		end
		h:write(name..m..tostring(self.dsc).."','"..tostring(self.ans).."','"..tostring(self.do_act).."');\n");
	end
	savemembers(h, self, name, false);
end

function phrase(o) --constructor
	local ret = o;
--	if not tonumber(num) then
--		error "phrase not numbered.";
--	end
	ret.nam = ''; -- for start
--	ret.key = tostring(num);
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
	return iface:inv(cat(self:str()));
end

function player_ways(self)
	return iface:ways(cat(ref(self.where).way:str()));
end

function player_objs(self)
	return iface:objs(cat(ref(self.where):str()));
end

function player_look(self)
	--return par('',ref(self.where):scene(), ref(self.where):look());
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

--function player_do(self, what, ...)
--	local v,r = call(ref(self.where),'act', what, unpack(arg));
--	if v == nil then
--		return nil, false
--	end
--	return v;
--end

function player_action(self, what, ...)
	local v,r,obj
	obj = ref(self.where):srch(what);
	if not obj then
		return nil; --player_do(self, what, unpack(arg));
	end
	v, r = player_take(self, what);
	if not v then
		v, r = call(ref(obj), 'act', unpack(arg));
		if not v and r ~= true then
			v, r = call(ref(game), 'act', obj, unpack(arg));
		end
	end
	return v, r;
end

function player_take(self, what)
	local v,r,obj,w
	obj,w = ref(self.where):srch(what);
	if not obj then
		return nil, false;
	end
	v,r = call(ref(obj), 'tak');
	if v and r ~= false then
		take(obj, w);
	end
	return v;
--	return cat(v,'^^');
end

function player_use(self, what, onwhat)
	local obj, obj2, v, vv, r;
	obj = self:srch(what);
	if not obj then
		return game.err, false;
	end
	if onwhat == nil then
		v, r = call(ref(obj),'inv');
		if not v and r ~= true then
			v, r = call(game, 'inv', obj);
		end
		return v, r;
	end
	obj2 = ref(self.where):srch(onwhat);
	if not obj2 then
		obj2 = self:srch(onwhat);
	end
	if not obj2 or obj2 == obj then
		return game.err, false;
	end
	v, r = call(ref(obj), 'use', obj2);
	if r ~= false then
		vv = call(ref(obj2), 'used', obj);
	end
	if not v and not vv then
		v, r = call(game, 'use', obj, obj2);
	end
	return par(' ', v, vv);
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
	if where == nil then
		return nil,false
	end
	if not isRoom(ref(where)) then
		error ("Trying to go nowhere: "..where);
	end
	if not isRoom(ref(self.where)) then
		error ("Trying to go from nowhere: "..self.where);
	end
	local v, r;
--	if not isDialog(ref(self.where)) then
		v,r = call(ref(self.where), 'exit', where);
		if r == false then
			return v, r
		end

--	if ref(was) ~= ref(self.where) then -- jump !!!
--		where = self.where;
--		was = where;
--	end

--	end
	local res = v;
	v = nil;
	if not back or not isDialog(ref(self.where)) or isDialog(ref(where)) then
		v, r = call(ref(where), 'enter', self.where);
		if r == false then
			return v, r
		end
		need_scene = true;
		if ref(was) ~= ref(self.where) then -- jump !!!
			where = deref(self.where);
			need_scene = false;
		end
	end
	res = par('^^',res,v);
	if not back then
		ref(where).__from__ = deref(self.where);
	end	
	if need_scene then
		self.where = deref(where);
		return par('^^',res,ref(where):scene());
	end
	self.where = deref(where);
	return res;
end


function player_goto(self, where)
	local v = go(self, where, false);
	return v;
end
function player_go(self, where)
	local w = ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	local v = go(self, w, false);
	return v;
end

function player_save(self, name, h)
	h:write(tostring(name)..'.where = "'..deref(self.where)..'";\n');
	savemembers(h, self, name, false);
end

function player(v)
	if v.nam == nil then
		error "No player name in constructor.";
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
	
	for i,o in opairs(self.lifes) do
		local vv
		local pre
		o = ref(o);
		if not isDisabled(o) then
			vv,pre = call(o,'life');
			if not pre then
				v = par(' ',v, vv);
			else
				av = par(' ', av, vv);
			end
		end
	end
	return v, av;
end

check_list = function(k, v)
	if v.check == nil or not v:check() then
		error ("error in list: "..k);
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
	for_each(v, k, check_list, isList)
end

function for_everything(f, ...)
	local is_ok = function(s)
		return true
	end
	for_each(_G, '_G', f, is_ok, unpack(arg))
end

function do_ini(self)
	local v='',vv
	local function call_key(k, v)
		v.key_name = k;
	end
	local function call_ini(k, v)
	--	v.key_name = k;
		v = call(v, 'ini');
		v = cat(v, "^^");
	end

	math.randomseed(tonumber(os.date("%m%d%H%M%S")))
	rnd(1); rnd(1); rnd(1); -- Lua bug?

	for_each_object(call_key);
	for_each_object(check_object);

	game.pl = deref(game.pl);
	game.where = deref(game.where);

	for_each(game, "game", check_list, isList)
	
	for_each_object(call_ini);

	me():tag();
	if not self.showlast then
		self._lastdisp = nil;
	end
	return par('',v, self._lastdisp); --par('^^',v);
end

function game_ini(self)
	local v,vv
	v = do_ini(self);
	vv = iface:title(call(self,'nam'));
	vv = par('^^', vv, call(self,'dsc'));
	return par("^^", vv, v);
end

function game(v)
	if v.nam == nil then
		error "No game name in constructor.";
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


function for_each(o, n, f, fv, ...)
	local k,v
	if type(o) ~= 'table' then
		return
	end

	for k,v in pairs(o) do
		if type(v) == 'table' and fv(v) then
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
			if isForSave(k) then
				need2 = true;
			end
			
			if type(k) == 'string' then
				savevar(h, v, name.."."..k, need or need2);
			else
				savevar(h, v, name.."["..k.."]", need or need2)
			end
		end
	end
end

function savevar (h, v, n, need)
	local r,f
	
	if v == nil or type(v)=="userdata" or
			 type(v)=="function" then 
		return 
	end

--	if string.find(n, '_') ==  1 or string.match(n,'^%u') then
--		need = true;
--	end

	if type(v) == "string" then
		if not need then 
			return
		end
		h:write(string.format("%s=%q\n",n,v))
		return;
	end
 	
	if type(v) == "table" then
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


function save_object(key, value, h)
	savevar(h, value, key, false);
	return true;
end

function game_save(self, name, file) 
	local h;
	if file ~= nil then
		file:write(name..".pl = '"..deref(self.pl).."'\n");
		savemembers(file, self, name, false);
		return nil, true
	end
	if name == nil then
		return nil, false
	end
	h = io.open(name,"w");
	if not h then
		return nil, false
	end
	h:write("-- $Name: "..call(here(),'nam').."$\n");
--	for_each_room(save_object, h);
	for_each_object(save_object, h);
--	for_each_player(save_object, h);
	save_object('game', self, h);
	clearvar(_G);
	h:flush();
	h:close();
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
		return do_ini(self);
	end
	return nil, false
end


function game_step(self)
	self._time = self._time + 1;
	return self:life(self);
end


game = game {
	nam = "INSTEAD -- Simple Text Adventure interpreter v"..stead.ver.str.." '2009 by Peter Kosyh",
	dsc = [[
Commands:^
    look(or just enter), act <on what> (or just what), use <what> [on what], go <where>,^
    back, inv, way, obj, quit, save <fname>, load <fname>.]],
	pl ='pl',
	showlast = true, 
};
function strip(s)
	local s = tostring(s);
	s = string.gsub(s, '^[ \t]*', '');
	s = string.gsub(s, '[ \t]*$', '');
	return s;
end
iface = {
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
	xref = function(self, str, obj)
--		return "@"..str.."{"..obj.."}";
		local o = ref(here():srch(obj));
		if not o then 
			o = ref(ways():srch(obj));
		end
		if not o then
			o = ref(me():srch(obj));
		end
		if not o or not o.id then
			return str;
--			error ("Xref to nowhere:"..str);
		end
		return cat(str,"("..tostring(o.id)..")");
--		return str;
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
	cmd = function(self, inp)
		local r, v, vv, i, k, cmd;
		local scene = false;
		local st = false;
		local l;
		i,k = string.find(inp,'[a-zA-Z0-9_]+', k);
		if not i or not k then
			cmd = inp
		else
			cmd = string.sub(inp, i, k);
		end
		local a = { };
		local n = 1;
		while i do
			k = k + 1;
			i,k = string.find(inp,'[^,]+', k);
			if not i then
				break
			end
			a[n] = strip(string.sub(inp, i, k));
			n = n + 1;
		end
		v = false
--		me():tag();	
		local oldloc = here();	
		local look = false;
		if cmd == 'look' or cmd == '' then
			r,v = me():look();
			st = true;
			look = true;
		elseif cmd == 'obj' then
			r,v = me():objs();
		elseif cmd == 'inv' then
			r,v = me():inv();
		elseif cmd == 'way' then
			r,v = me():ways();
		elseif cmd == 'ls' then
			r = par('^^', me():objs(), me():inv(), me():ways());
			v = nil;
		elseif cmd == 'go' then
			r,v = me():go(unpack(a));
			st = true;
		elseif cmd == 'back' then
			r,v = me():go(from());
			st = true;
		elseif cmd == 'act' then
			r,v = me():action(unpack(a));
			st = true;	
		elseif cmd == 'use' then
			r,v = me():use(unpack(a));
			st = true;
		elseif cmd == 'save' then
			r, v = game:save(unpack(a));
		elseif cmd == 'load' then
			r, v = game:load(unpack(a));
			if v ~= false and game.showlast then
				return r;
			end
		else
			r,v = me():action(strip(inp));
			st = true;
		end
		-- here r is action result, v - ret code value	
		-- st -- game state changed
		if st and r == nil and v == true then -- we do nothing
			return nil;
		end
		
		ACTION_TEXT = r; -- here, life methods can redefine this
		
		if st and v ~= false then
			local av, pv -- av -- active lifes, pv -- background
			pv,av = game:step();
			vv = par(" ",vv, pv);
			me():tag();		
			if oldloc == here() and not look then
				if here().forcedsc == true then
					l,v = me():look();
				elseif game.forcedsc == true and here().forcedsc ~= false then
					l,v = me():look();
				end
				ACTION_TEXT = iface:em(ACTION_TEXT);
				vv = par("^^",iface:em(av), here():look(), iface:em(vv));
			else
				vv = par("^^",av, here():look(), vv);
			end
		end
		if v == false then
			return fmt(ACTION_TEXT);
		end
		vv = fmt(cat(par("^^",l,ACTION_TEXT,vv),'^'));
		if st then
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
			inp = io.read("*l");
			if inp == 'quit' then
				break;
			end
			self:text(self:cmd(inp));
		end
	end
};

pl = player {
	nam = "Incognito",
	where = 'main',
	obj = { }
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

function from()
	return ref(here().__from__);
end

function time()
	return game._time;
end

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

function xref(str, obj)
	return iface:xref(str, obj);
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
	game.lifes:del(what);
end

function vobj_save(self, name, h, need)
	local dsc;
	local w
	if type(self.dsc) ~= 'string' then
		dsc = 'nil';
	else
		dsc = "[["..self.dsc.."]]";
	end
	
	if type(deref(self.where)) ~= 'string' then
		w = 'nil';
	else
		w = "'"..deref(self.where).."'";
	end
	
	if need then
		h:write(name.." = vobj("..tostring(self.key)..",[["..self.nam.."]],"..dsc..","..w..");\n");
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
		error ("vobj key must be number!");
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

function vroom(name, w)
	return room { nam = name, where = deref(w), enter = vroom_enter, save = vroom_save, };
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

function take(obj, wh)
	local o = remove(obj, wh);
	if not isObject(o) then
		error "Trying to take wrong object.";
	end
	inv():add(obj);
	o._taken = true
	return o
end

function putto(obj, w, pos)
	local o = ref(obj);
	if not isObject(o) then
		error "Trying to put wrong object.";
	end
	if not w then
		here().obj:add(obj, pos);
		o.__where__ = deref(here());
	else
		ref(w).obj:add(obj, pos);
		o.__where__ = deref(w);
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
		error "Trying to drop wrong object:";
	end
	inv():del(obj);
	o._taken = false
	return o;
end

function dropf(obj)
	local o = putf(obj);
	if not isObject(o) then
		error "Trying to dropf wrong object:";
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
	local o,w = wh.obj:srch(obj);
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
	return game._music;
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

function change_pl(p)
	local o = ref(p);
	if type(deref(p)) ~= 'string' or not o then
		error "Wrong player name in change_pl...";	
	end
	game.pl = deref(p);
	return goto(o.where);
end

function disabled(o)
	return isDisabled(ref(o))
end

function isForSave(k)
	return string.find(k, '_') ==  1 or string.match(k,'^%u')
end
-- here the game begins
