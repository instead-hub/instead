-- brake strings by space
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


function p(...)
 	io.write(fmt(unpack(arg)))
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

function isPlayer(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.player_type then
		return true
	end
	return false
end

function isRoom(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.location_type then
		return true
	end
	return false
end

function isPhrase(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.phrase_type then
		return true
	end
	return false
end

function isDialog(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.dialog_type then
		return true
	end
	return false
end

function isDisabled(v)
	if type(v) ~= 'table' then
		return false
	end
	if v._disabled then
		return true
	end
	return false
end

function isRemoved(v)
	if type(v) ~= 'table' then
		return false
	end
	if v._disabled == -1 then
		return true
	end
	return false
end

function isObject(v)
	if type(v) ~= 'table' then
		return false
	end
	if v.object_type then
		return true
	end
	return false
end

function obj_xref(self,str)
	function xrefrep(str)
		local s =  string.gsub(str,'[{}]','');
		return iface:xref(s, self);
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
	local i, vv
	local v = call(self,'dsc');
	if isDisabled(self) then
		return
	end
	if game.hinting then
		v = obj_xref(self, v);
	elseif v then
		v = string.gsub(v, '[{}]','');
	end
	for i=1, table.maxn(self.obj) do
		local o = ref(self.obj[i]);
		vv = obj_look(o);
		v = par(' ',v, vv); 
	end
	return v;
-- iface:xref(v,self.nam);
end


function obj_remove(self)
	self._disabled = -1;
end

function obj_disable(self)
	self._disabled = true;
end

function obj_enable(self)
--	if self._disabled == nil then
--		return
--	end
	self._disabled = false;
end

function obj_save(self, name, h, need)
	local dsc;
	if need then
		h:write(name.." = obj {nam = '"..tostring(self.nam).."'}\n");
	end
	savemembers(h, self, name, need);
end

function obj_str(self)
	local i, v, vv;
	if not isObject(self) then
		return
	end
	if isDisabled(self) then
		return 
	end
	for i = 1,table.maxn(self.obj) do
		local o = ref(self.obj[i]);
		if not isDisabled(o) and isObject(o) then
			vv = call(o, 'nam');
			vv = iface:xref(vv, o);
--			vv = cat(vv,'(',tostring(ref(self[i]).id),')');
			v = par(',', v, vv, obj_str(o));
--			obj_str(o);
		end
	end
	return v;
end

function obj(v)
	if v.nam == nil then
		error "No object name in constructor.";
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
			return f();
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

function list_check(self)
	local i;
	for i = 1,table.maxn(self) do
		local o = ref(self[i]);
		if not o then
			return false
		end
		if not isObject(o) then
			return false
		end
	end
	return true; 
end

function list_str(self)
	local i, v, vv;
	for i = 1,table.maxn(self) do
		local o = ref(self[i]);
		if not isDisabled(o) then
			vv = call(o, 'nam');
			vv = iface:xref(vv, o);
--			vv = cat(vv,'(',tostring(ref(self[i]).id),')');
			v = par(',', v, vv);
		end
	end
	return v;
end


function list_add(self, name)
--	if type(name) ~= 'string' then
--		error "No string adding to list."
--	end
	if list_find(self, name) then
		return nil
	end
	self.__modifyed__ = true
	table.insert(self, name);
	return true
end

function list_find(self, name)
	local n
--	if type(name) ~= 'string' then
--		error "No string finding in list."
--	end
	for n=1,table.maxn(self) do 
		if ref(self[n]) == ref(name) then
			return n; 
		end	
	end
	return nil
end

function list_save(self, name, h, need)
	local k,v;
	if self.__modifyed__ then
		h:write(name.." = list({});\n");
		need = true;
	end
	savemembers(h, self, name, need);
end

function list_name(self, name)
	local n
--	if type(name) ~= 'string' then
--		error "No string finding in list."
--	end
	for n=1,table.maxn(self) do
		local o = ref(self[n]);
		local nam = call(o,'nam') ;
		if not isDisabled(o) and name == tostring(nam) then
			return n;
		end
	end
	return nil
end
function list_id(self, id)
	local n
	for n=1,table.maxn(self) do
		local o = ref(self[n]);
		if not isDisabled(o) and  id == o.id then
			return n;
		end
	end
end

function list_search(self, n)
	local i;
	i = list_find(self, n);
	if not i then
		i = list_name(self, n);
	end
	if not i and tonumber(n) then
		i = list_id(self, tonumber(n));
		if not i then
			return nil
		end
	end
	if isDisabled(self[i]) then
		return nil;
	end
	return self[i], i;
end

function list_del(self, name)
	local v,n
	v, n = list_search(self, name);
	if n == nil then
		return nil;
	end
	self.__modifyed__ = true
	return table.remove(self, n);
end

function list(v)
	v.add = list_add;
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
	local i, vv;
	for i = 1,table.maxn(self.obj) do
		local o = ref(self.obj[i]);
		vv = par(' ',vv, obj_look(o));
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
	for i=1, table.maxn(v.obj) do
		o = ref(v.obj[i]);
		local r,rr = obj_search(o, n);
		if r then
			return r, rr;
		end
	end
	return;
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
	v.location_type = true;
	if v.way == nil then
		v.way = { };
	end
	v.way = list(v.way);
	v = obj(v);
	return v;
end

function dialog_enter(self)
	local i
--	self._last = nil;
	if not dialog_rescan(self) then
		return nil, false
	end
	return nil, true
end

function dialog_scene(self)
	local i,n,v
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
	local i,n,v
	n=1
	for i=1,table.maxn(self.obj) do
		local ph = ref(self.obj[i]);
		if not isDisabled(ph) then
			if game.hinting then
				v = par('^', v, n..' - '..iface:xref(call(ph,'dsc'),ph));
			else
				v = par('^', v, n..' - '..string.gsub(call(ph,'dsc'),'[{}]',''));
			end
			n = n + 1
		end
	end
	return v;
end

function dialog_rescan(self)
	local i,k
	k = 1

	for i=1,table.maxn(self.obj) do
		local ph = ref(self.obj[i]);
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
		if ph ~= nil then
			if not isRemoved(ph) then
			 	if on then
					ph:enable();
				else 
					ph:disable();
				end
--				ph.__changed__ = true;
			end
		end
	end
end

function dialog_prem(self, ...)
	local i, ph
	for i=1,table.maxn(arg) do
		ph = dialog_phrase(self, arg[i]);
		if ph ~= nil then
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
	r._last = call(ph, 'ans');
	if isDialog(here()) and not dialog_rescan(here()) then
		ret = par(' ', ret, me():back());
	end
	return par("^^",r._last, ret);
--	if dialog_getn(self, 1) == nil then
--		me():back();
--	end
--	self._last = call(ph, 'ans');
--	return self._last;
end

function phrase_save(self, name, h, need)
	savemembers(h, self, name, need);
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
	p:enable();
	return p;
end

function player_inv(self)
	return iface:inv(cat(self.obj:str()));
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

	for k,v in ipairs(self.obj) do
		if ref(v) and not isDisabled(ref(v)) then
			id = id + 1;
			ref(v).id = id;
			id = obj_tag(ref(v), id);
		end
	end
	return id;
end

function player_tagall(self)
	local id = 0;
	local k, v;

	id = obj_tag(here(), id);

	for k,v in ipairs(inv()) do
		if ref(v) and not isDisabled(ref(v)) then
			id = id + 1;
			ref(v).id = id;
		end
	end
	for k,v in ipairs(ways()) do
		if isRoom(ref(v)) then
			id = id + 1;
			ref(v).id = id;
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
		if not v then
			v, r = call(ref(game), 'act', obj, unpack(arg));
		end
	end
	return v;
end

function player_take(self, what)
	local v,r,obj,w
	obj,w = ref(self.where):srch(what);
	if not obj then
		return nil, false;
	end
	v,r = call(ref(obj), 'tak');
	if v and r ~= false then
		ref(w).obj:del(obj);
		self.obj:add(obj);
		ref(obj)._taken = true
	end
	return v;
--	return cat(v,'^^');
end

function player_use(self, what, onwhat)
	local obj, obj2, v, vv, r;
	obj = self.obj:srch(what);
	if not obj then
		return nil, false;
	end
	if onwhat == nil then
		v, r = call(ref(obj),'inv');
		if not v then
			v, r = call(game, 'inv', obj);
		end
		return cat(v, ' '), r;
	end
	obj2 = ref(self.where):srch(onwhat);
	if not obj2 then
		obj2 = self.obj:srch(onwhat);
	end
	if not obj2 or obj2 == obj then
		return nil, false;
	end
	v, r = call(ref(obj), 'use', obj2);
	if r ~= false then
		vv = call(ref(obj2), 'used', obj);
	end
	if not v and not vv then
		v, r = call(game, 'use', obj, obj2);
	end
	return cat(par(' ', v, vv),' ');
end

function player_back(self)
	local where = ref(self.where);
	if where == nil then
		return nil,false
	end
	return go(self, where.__from__, true);
end

function go(self, where, back)
	local need_scene = false;	
	if where == nil then
		return nil,false
	end
	if not isRoom(ref(where)) then
		error ("Trying to go nowhere."..where);
	end
	if not isRoom(ref(self.where)) then
		error "Trying to go from nowhere."
	end
	local v, r;
--	if not isDialog(ref(self.where)) then
		v,r = call(ref(self.where), 'exit', where);
		if r == false then
			return v
		end
--	end
	local res = v;
	v = nil;
	if not back or not isDialog(ref(self.where)) or isDialog(ref(where)) then
		v, r = call(ref(where), 'enter', self.where);
		if r == false then
			return v
		end
		need_scene = true;
	end
	res = par('^^',res,v);
	if not back then
		ref(where).__from__ = self.where;
	end	
	if need_scene then
		self.where = where;
		return par('^^',res,ref(where):scene());
	end
	self.where = where;
	return res;
end


function player_goto(self, where)
	return go(self, where, false);
end
function player_go(self, where)
	local w = ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	return go(self, w, false);
end

function player_save(self, name, h)
	h:write(tostring(name)..'.where = "'..self.where..'";\n');
	savemembers(h, self, name, false);
end

function player(v)
	if v.nam == nil then
		error "No player name in constructor.";
	end
	if v.obj == nil then
		v.obj = { };
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
	v.obj = list(v.obj); -- inventory
	v.player_type = true;
	return v;
end

function game_life(self)
	local i, v;
	for i=1, table.maxn(self.lifes) do
		if not isDisabled(ref(self.lifes[i])) then
			v = par(' ',v,call(ref(self.lifes[i]),'life'));
		end
	end
	return v;
end

function check_room(k, v)
	if not v.nam then
		error ("No name in "..k);
	end
	if v.obj == nil then
		error("no obj in room:"..k);
	end
	if v.way == nil then
		error("no way in room:"..k);
	end
	if not v.obj:check() then
		error ("error in room (obj): "..k);
	end
	if not v.way:check() then
		error ("error in room (way): "..k);
	end
end

function do_ini(self)
	local v,vv
	local function call_ini(k, v)
		v = call(v, 'ini');
		v = cat(v, "^^");
	end
	math.randomseed(tonumber(os.date("%m%d%H%M%S")))
	for_each_object(call_ini);
--	for_each_room(call_ini);
	for_each_room(check_room);
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

function for_each_room(f,...)
	local k,v
	for k,v in pairs(_G) do
		if isRoom(v) then
			f(k,v, unpack(arg));
		end
	end
end

function for_each_object(f,...)
	local k,v
	for k,v in pairs(_G) do
		if isObject(v) then
			f(k,v, unpack(arg));
		end
	end
end

function for_each_player(f,...)
	local k,v
	for k,v in pairs(_G) do
		if isPlayer(v) then
			f(k,v, unpack(arg));
		end
	end
end


function clearvar (v)
	local r,f
	if type(v) ~= "table" then 
		return 
	end
	v.__visited__ = nil
	for r,f in pairs(v) do
		clearvar(r, f)
	end
end

function savemembers(h, self, name, need)
	local k,v
	for k,v in pairs(self) do
		local need2
		if k ~= "__visited__" then
			need2 = false
			if string.find(k, '_') ==  1 then
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

	if string.find(n, '_') ==  1 then
		need = true;
	end

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
		if type(v.save) == 'function' then
			v:save(n, h, need);
			return;
		end
		v.__visited__ = n;
		
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
		file:write(name..".pl = '"..self.pl.."'\n");
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
--	for_each_room(save_object, h);
	for_each_object(save_object, h);
	for_each_player(save_object, h);
	save_object('game', self, h);
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

function game_life(self)
	local i,v
	for i=1,table.maxn(self.lifes) do
		vv = call(ref(self.lifes[i]),'life');
		v = par(' ',v, vv);
	end
	return v;
end

function game_step(self)
	self._time = self._time + 1;
	return game_life(self);
end

version = "0.4.3";

game = game {
	nam = "INSTEAD -- Simple Text Adventure interpreter v"..version.." '2008 by Peter Kosyh",
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
	xref = function(self, str, obj)
--		return "@"..str.."{"..obj.."}";
		local o = ref(here():srch(obj));
		if not o then 
			o = ref(ways():srch(obj));
		end
		if not o then
			o = ref(inv():srch(obj));
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
		local r, v, vv,  i, k , cmd, n;
		local scene = false;
		local st = false;
		i,k = string.find(inp,'[a-zA-Z0-9_]+', k);
		if not i or not k then
			cmd = inp
		else
			cmd = string.sub(inp, i, k);
		end
		a = { };
		n = 1;
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
		if cmd == 'look' or cmd == '' then
			r,v = me():look();
			st = true;
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
--		self:text(r);
		if st and v ~= false then
			me():tag();		
			vv = par(" ",vv, game:step());
			vv = par("^^",here():look(), vv);
		end
		if v == false then
			return fmt(par("^^", vv, "Error.^"));
		end
		vv = fmt(cat(par("^^",r,vv),'^'));
		if st then
			game._lastdisp = vv
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

function here()
	return ref(me().where);
end

function from()
	return ref(ref(me().where).__from__);
end

function time()
	return game._time;
end

function inv()
	return me().obj;
end

function objs()
	return here().obj;
end

function ways()
	return here().way;
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
	if type(self.dsc) ~= 'string' then
		dsc = 'nil';
	else
		dsc = "[["..self.dsc.."]]";
	end
	if need then
		h:write(name.." = vobj("..tostring(self.key)..",[["..self.nam.."]],"..dsc..");\n");
	end
	savemembers(h, self, name,false);
end

function vobj_act(self, ...)
	local o, r = here():srch(self.nam);
	return call(ref(r),'act', self.key, unpack(arg));
end

function vobj_used(self, ...)
	local o, r = here():srch(self.nam);
	return call(ref(r),'used', self.key, unpack(arg));
end

function vobj(key, name, dsc)
	local o = { key = key, nam = name, dsc = dsc, act = vobj_act, used = vobj_used, save = vobj_save, obj = list({}), };
	if not tonumber(key) then
		error ("vobj key must be number!");
	end
--	o.object_type = true;
	o = obj(o);
	return o;
end
function goto(what)
	local v=me():goto(what);
	me():tag();
	return v;
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

function take(obj)
	local o,w = here():srch(obj);
	if w then
		ref(w).obj:del(obj);
	end
	me().obj:add(obj);
	ref(obj)._taken = true
end

function drop(obj)
	me().obj:del(obj);
	here().obj:add(obj);
	ref(obj)._taken = false
end

function seen(obj)
	if here().obj:srch(obj) then
		return true
	end
	return false
end

function have(obj)
	if me().obj:srch(obj) then
		return true
	end
	return false
end

function move(obj, there)
	local o,w = here():srch(obj);
	if w then
		ref(w).obj:del(obj);
	end
--	if not isRoom(ref(there)) then
--		error ("move error");
--	end
	ref(there).obj:add(obj);
end

function get_picture()
	local s = call(here(),'pic');
	return s;
end

function get_title()
	local s = call(here(),'nam');
	return s;
end
function get_music()
	return game._music;
end

function set_music(s)
	game._music = s;
end

-- here the game begins

