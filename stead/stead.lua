stead = {
	version = "1.8.3",
	api_version = "1.1.6", -- last version before 1.2.0
	table = table,
	delim = ',',
	busy = instead_busy,
	scene_delim = "^^",
	space_delim = ' ',
	string = string,
	math = math,
	ticks = instead_ticks,
	mouse_pos = instead_mouse_pos,
	mouse_filter = instead_mouse_filter, 
	menu_toggle = instead_menu_toggle,
	set_timer = instead_timer,
	next = next,
	io = io,
	os = os,
	readdir = instead_readdir,
	call_top = 0,
	call_ctx = { txt = nil, self = nil },
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
				return input:key(...); -- pressed, event
			end
		elseif event == 'mouse' then
			if type(input.click) == 'function' then
				return input:click(...); -- pressed, x, y, mb
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

stead.last_act = function(s)
	local r = game.__last_act

	if s ~= nil and s ~= true then
		game.__last_act = s
	end

	return r
end

stead.need_scene = function(s)
	if s == nil then
		NEED_SCENE = true
	else
		NEED_SCENE = s
	end
end

if not stead.busy then
	stead.busy = function(v)
	end
end

if _VERSION == "Lua 5.1" then
	stead.eval = loadstring
	stead.unpack = unpack
else
	stead.eval = load
	stead.unpack = table.unpack
	stead.table.maxn = table_get_maxn
end

instead = stead;

function stead.getcmd(str)
	local a = {}
	local n = 1
	local cmd;
	local i,k = stead.string.find(str, '[a-zA-Z0-9_]+');
	if not i or not k then
		cmd = str;
	else
		cmd = stead.string.sub(str, i, k);
	end
	stead.cmd = cmd
	if cmd == 'load' or cmd == 'save' then
		a[1] = stead.strip(stead.string.sub(str, k + 1));
		stead.args = a;
		return cmd, a
	end
	while i do
		k = k + 1;
		i,k = stead.string.find(str,'[^,]+', k);
		if not i then
			break
		end
		a[n] = stead.strip(stead.string.sub(str, i, k));
		n = n + 1;
	end
	stead.args = a;
	return cmd, a
end

stead.tostring = function(v)
	if isCode(v) then
		v = stead.string.format("code %q", stead.functions[v].code);
	elseif type(v) == 'string' then
		v = stead.string.format("%q", v);
	elseif v == nil or type(v) == 'boolean' or type(v) == 'number' then
		v = tostring(v);
	elseif type(v) == 'table' and type(stead.deref(v)) == 'string' then
		v = stead.deref(v);
	else
		v = nil
	end
	return v
end

stead.cctx = function()
	return stead.call_ctx[stead.call_top];
end

stead.callpush = function(v, ...)
	stead.call_top = stead.call_top + 1;
	stead.call_ctx[stead.call_top] = { txt = nil, self = v, action = false };
	args = {...};
	arg1 = args[1]
	arg2 = args[2]
	arg3 = args[3]
	arg4 = args[4]
	arg5 = args[5]
	arg6 = args[6]
	arg7 = args[7]
	arg8 = args[8]
	arg9 = args[9]
	-- dirty but clean and fast :)
	self = v
end

stead.clearargs = function()
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

stead.callpop = function()
	stead.call_ctx[stead.call_top] = nil;
	stead.call_top = stead.call_top - 1;
	if stead.call_top < 0 then
		error ("callpush/callpop mismatch")
	end 
	stead.clearargs()
end

stead.pclr = function()
	stead.cctx().txt = nil
end

stead.pget = function()
	return stead.cctx().txt;
end

stead.p = function(...)
	local i
	local a = {...}
	for i = 1, stead.table.maxn(a) do
		stead.cctx().txt = stead.par('', stead.cctx().txt, tostring(a[i]));
	end
	stead.cctx().txt = stead.cat(stead.cctx().txt, stead.space_delim);
end

stead.pr = function(...)
	local i
	local a = {...}
	for i = 1, stead.table.maxn(a) do
		stead.cctx().txt = stead.par('', stead.cctx().txt, tostring(a[i]));
	end
end

stead.pn = function(...)
	p(...);
	stead.cctx().txt = stead.par('', stead.cctx().txt,'^');
end

-- merge strings with "space" as separator
stead.par = function(space,...)
	local i, res
	local a = {...};
	for i = 1, stead.table.maxn(a) do
		if type(a[i]) == 'string' then
			if res == nil then
				res = ""
			else
				res = res..space;
			end
			res = res..a[i];
		end 
	end
	return res;
end
-- add to not nill string any string
stead.cat = function(v,...)
	local i, res
	if not v then
		return nil
	end
	res = v;
	local a = {...}
	for i = 1, stead.table.maxn(a) do
		if type(a[i]) == 'string' then
			res = res..a[i];
		end 
	end
	return res;
end

function txtnb(v)
	if type(v) ~= 'string' then return nil; end
	return iface:nb(v);
end

function img(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:img(v);
end

function imgl(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:imgl(v);
end

function imgr(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:imgr(v);
end

function txtem(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:em(v)
end

function txtst(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:st(v)
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

function txttab(v,a)
	return iface:tab(v, a)
end

function txtj(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:just(v)
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

function txttop(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:top(v)
end

function txtbottom(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:bottom(v)
end

function txtmiddle(v)
	if type(v) ~= 'string' then return nil; end; 
	return iface:middle(v)
end

stead.fmt = function(...)
	local i, res
	local a = {...};

	for i=1,stead.table.maxn(a) do
		if type(a[i]) == 'string' then
			local s = stead.string.gsub(a[i],'[\t ]+', stead.space_delim);
			s = stead.string.gsub(s, '[\n]+', stead.space_delim);
			s = stead.string.gsub(s, '\\?[\\^]', { ['^'] = '\n', ['\\^'] = '^', ['\\\\'] = '\\'} );
			res = stead.par('', res, s);
		end
	end
	return res
end

-- integer lists
local inext = function(t, k)
	local v
	k, v = stead.next(t, k);
	while k and not tonumber(k) do
		k, v = stead.next(t, k);
	end
	if not tonumber(k) then
		return nil
	end
	return k, v
end

local ilist = function(s, var)
	return inext, s, nil;
end

local ordered_i = function(t)
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

local onext = function(t, k)
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

function opairs(s)
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

function isXaction(v)
	return (type(v) == 'table') and (v.xaction_type)
end


stead.obj_xref = function(self,str)
	function xrefrep(str)
		local s = stead.string.gsub(str,'[\001\002]','');
		return stead.xref(s, self);
	end
	if not str then
		return
	end
	if not isObject(self) then
		return str;
	end
	local s = stead.string.gsub(str, '\\?[\\{}]', 
		{ ['{'] = '\001', ['}'] = '\002', [ '\\{' ] = '{', [ '\\}' ] = '}' }):gsub('\001([^\002]+)\002', xrefrep):gsub('[\001\002]', { ['\001'] = '{', ['\002'] = '}' });
	return s;
end

stead.obj_look = function(self)
	local i, vv, o
	if isDisabled(self) then
		return
	end
	local v = stead.call(self,'dsc');
	if game.hinting then
		v = self:xref(v);
	elseif v then
		v = stead.string.gsub(v, '[{}]','');
	end
	for i,o in opairs(self.obj) do
		o = stead.ref(o);
		if isObject(o) then
			vv = stead.obj_look(o);
			v = stead.par(stead.space_delim, v, vv); 
		end
	end
	return v;
end


stead.obj_remove = function(self)
	self._disabled = -1;
	return self
end

stead.obj_disable = function(self)
	self._disabled = true;
	return self
end

stead.obj_enable = function(self)
	self._disabled = false;
	return self
end

stead.obj_disabled = function(self)
	return (self._disabled == true);
end

stead.obj_enable_all = function(s)
	if not isObject(s) then
		return
	end
	objs(s):enable_all();
end

stead.obj_disable_all = function(s)
	if not isObject(s) then
		return
	end
	objs(s):disable_all();
end

stead.obj_save = function(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: object "..name.." can not be saved!");
		return
	end
	stead.savemembers(h, self, name, need);
end

stead.obj_str = function(self)
	local i, v, vv, o;
	if not isObject(self) then
		return
	end
	if isDisabled(self) then
		return 
	end
	for i,o in opairs(self.obj) do
		o = stead.ref(o);
		if o~= nil and not isDisabled(o) then -- isObject is better, but compat layer must be ok
			vv = stead.call(o, 'nam');
			vv = stead.xref(vv, o);
			v = stead.par(',', v, vv, stead.obj_str(o));
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
		v.xref = stead.obj_xref;
	end

	if v.look == nil then
		v.look = stead.obj_look;
	end
	if v.enable == nil then
		v.enable = stead.obj_enable;
	end
	if v.disable == nil then
		v.disable = stead.obj_disable;
	end
	if v.disabled == nil then
		v.disabled = stead.obj_disabled;
	end
	if v.enable_all == nil then
		v.enable_all = stead.obj_enable_all;
	end
	if v.disable_all == nil then
		v.disable_all = stead.obj_disable_all;
	end
	if v.remove == nil then
		v.remove = stead.obj_remove;
	end
	if v.obj == nil then
		v.obj = {};
	end
	if v.srch == nil then
		v.srch = stead.obj_search;
	end
	if v.str == nil then
		v.str = stead.obj_str;
	end
	v.obj = list(v.obj);
	if v.save == nil then
		v.save = stead.obj_save;
	end
	return v
end


function stead.ref(n, nofunc) -- ref object by name
	if type(n) == 'string' then
		if type(_G[n]) == 'table' then -- fastest path
			return _G[n];
		end
		local f = stead.eval('return '..n);
		if f then
			return stead.ref(f(), nofunc);
		end
		return nil;
	end
	if type(n) == 'table' then
		return n;
	end
	if type(n) == 'function' and not nofunc then
		local r,v = pcall(n);
		if not r then
			return nil
		end
		return stead.ref(v);
	end
	return nil
end

function stead.deref(n)
	if type(n) == 'string' then
		return n
	end
	
	if type(n) == 'table' and type(n.key_name) == 'string' then
		return n.key_name
	end
	return n
end

stead.list_check = function(self, name)
	local i, v, ii;
	for i,v,ii in opairs(self) do
		local o = stead.ref(v);
		if not o then -- isObject(o) then -- compat
			error ("No object: "..name.."["..tostring(ii).."]".." ("..tostring(type(v))..")")
			return false
		end
		if stead.deref(v) then
			self[ii] = stead.deref(v);
		end
	end
	return true; 
end

stead.list_str = function(self)
	local i, v, vv, o;
	for i,o in opairs(self) do
		o = stead.ref(o);
		if o~= nil and not isDisabled(o) then
			vv = stead.call(o, 'nam');
			vv = stead.xref(vv, o);
			v = stead.par(',', v, vv);
		end
	end
	return v;
end


stead.list_add = function(self, name, pos)
	local nam
	nam = stead.deref(name);
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

stead.list_set = function(self, name, pos)
	local nam
	local i = tonumber(pos);
	if not i then
		return nil
	end
	nam = stead.deref(name);
	self.__modified__ = true;
	self[i] = nam; -- for spare lists
	return true
end

stead.list_find = function(self, name)
	local n, v, ii
	for n,v,ii in opairs(self) do 
		if stead.ref(v) == stead.ref(name, true) then -- do not call func while search
			return ii; 
		end	
	end
	return nil
end

stead.list_disable_all = function(s)
	local k,v
	for k,v in opairs(s) do
		local o = stead.ref(v);
		if isObject(o) then
			o:disable()
		end
	end
end

stead.list_enable_all = function(s)
	local k,v
	for k,v in opairs(s) do
		local o = stead.ref(v);
		if isObject(o) then
			o:enable()
		end
	end
end

stead.list_enable = function(s, w)
	local o, i = stead.list_search(s, w, true);
	o = stead.ref(o);
	if isObject(o) then
		o:enable()
		return o
	end
end

stead.list_disable = function(s, w)
	local o, i = stead.list_search(s, w, true);
	o = stead.ref(o);
	if isObject(o) then
		o:disable()
		return o
	end
end

stead.list_save = function(self, name, h, need)
	if self.__modifyed__ or self.__modified__ then -- compat
		h:write(name.." = list({});\n");
		need = true;
	end
	stead.savemembers(h, self, name, need);
end

stead.list_name = function(self, name, dis)
	local n, o, ii
	for n,o,ii in opairs(self) do
		o = stead.ref(o);
		if isObject(o) then
			local nam = stead.call(o,'nam') ;
			if ( not isDisabled(o) or dis ) and name == tostring(nam) then
				return ii;
			end
		end
	end
	return nil
end
stead.list_id = function(self, id, dis)
	local n,o,ii
	for n,o,ii in opairs(self) do
		o = stead.ref(o);
		if dis or not isDisabled(o) then
			if isObject(o) and id == o.id then
				return ii;
			end
		end
	end
end

stead.list_search = function(self, n, dis)
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
	if not dis and isDisabled(stead.ref(self[i])) then
		return nil;
	end
	return self[i], i;
end

stead.list_zap = function(self)
	local n,o,ii
	for n,o,ii in opairs(self) do
		self[ii] = nil;
	end
	self.__modified__ = true
	return self
end

stead.list_concat = function(self, other, pos)
	local n,o,ii
	for n,o,ii in opairs(other) do
		o = stead.ref(o);
		if pos == nil then
			self:add(stead.deref(o));
		else 
			self:add(stead.deref(o), pos);
			pos = pos + 1;
		end
	end
end

stead.list_del = function(self, name)
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

stead.list_purge = function(self, name)
	local v,n
	v, n = self:srch(name, true);
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

stead.list_replace = function(self, name, name2)
	local o, ii
	o, ii = self:srch(name);
	if ii then
		self:set(name2, ii);
	else
		self:add(name2);
	end
	return ii;
end

function list(v)
	v.list_type = true;
	v.add = stead.list_add;
	v.set = stead.list_set;
	v.cat = stead.list_concat;
	v.zap = stead.list_zap;
	v.del = stead.list_del;
	v.purge = stead.list_purge;
	v.replace = stead.list_replace;
	v.look = stead.list_find;
	v.name = stead.list_name;
	v.byid = stead.list_id;
	v.srch = stead.list_search;
	v.str = stead.list_str;
	v.check = stead.list_check;
	v.save = stead.list_save;
	v.enable = stead.list_enable;
	v.disable = stead.list_disable;
	v.enable_all = stead.list_enable_all;
	v.disable_all = stead.list_disable_all;
	return v;
end

function isList(v)
	return (type(v) == 'table') and (v.list_type == true)
end

stead.call = function(v, n, ...)
	if type(v) ~= 'table' then
		error ("Call on non table object:"..tostring(n), 2);
	end
	if v[n] == nil then
		return nil,nil;
	end
	if type(v[n]) == 'string' then
		return v[n];
	end
	if type(v[n]) == 'function' then
		stead.callpush(v, ...)
		local a,b = v[n](v, ...);
		-- boolean, nil
		if type(a) == 'boolean' and b == nil then
			b, a = a, stead.pget()
			if a == nil then
				if stead.cctx().action then
					a = true
				else
					a = b
					b = nil
				end
			end 
		elseif a == nil and b == nil then
			a = stead.pget()
			b = nil
		end
		if a == nil and b == nil and stead.cctx().action then
			a = true
		end
		stead.callpop()
		return a,b
	end
	if type(v[n]) == 'boolean' then
		return v[n]
	end
	error ("Method not string nor function:"..tostring(n), 2);
end

stead.call_bool = function(v, n, ...)
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
		stead.callpush(v, ...)
		local r,v = v[n](v, ...);
		stead.callpop();
		return r,v;
	end
	return true; -- not nil
end

stead.call_value = function(v, n, ...)
	if type(v) ~= 'table' then
		error ("Call value on non table object:"..n, 2);
	end
	
	if v[n] == nil then
		return nil
	end	
	
	if type(v[n]) ~= 'function' then
		return v[n];
	end
	stead.callpush(v, ...)
	local r,v = v[n](v, ...);
	stead.callpop();
	return r,v;
end

stead.room_scene = function(self)
	local v;
	v = iface:title(stead.call(self,'nam'));
	v = stead.par(stead.scene_delim, v, stead.call(self,'dsc')); --obj_look(self));
	return stead.cat(v, stead.space_delim);
end

stead.room_look = function(self)
	local i, vv, o;
	for i,o in opairs(self.obj) do
		o = stead.ref(o);
		if isObject(o) then
			vv = stead.par(stead.space_delim, vv, o:look());
		end
	end
	return stead.cat(vv, stead.space_delim);
end

stead.obj_search = function(v, n, dis)
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
		o = stead.ref(o);
		if isObject(o) then
			local r,rr = stead.obj_search(o, n, dis);
			if r then
				return r, rr;
			end
		end
	end
	return;
end

stead.room_save = function(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: room "..name.." can not be saved!");
		return
	end
	stead.savemembers(h, self, name, need);
end

function room(v) --constructor
--	if v.nam == nil then
--		error ("No room name in constructor.", 2);
--	end
	if v.scene == nil then
		v.scene = stead.room_scene;
	end
	if v.look == nil then
		v.look = stead.room_look;
	end
	if v.save == nil then
		v.save = stead.room_save;
	end
	v.location_type = true;
	if v.way == nil then
		v.way = { };
	end
	v.way = list(v.way);
	v = obj(v);
	return v;
end

stead.dialog_enter = function(self)
	if not stead.dialog_rescan(self) then
		return nil, false
	end
	return nil, true
end

stead.dialog_scene = function(self)
	local v
	v = iface:title(stead.call(self,'nam'));
	v = stead.par(stead.scene_delim, v, stead.call(self, 'dsc')); --obj_look(self));
	return v;
end

stead.dialog_look = function(self)
	local i,n,v,ph
	n = 1
	for i,ph in opairs(self.obj) do
		ph = stead.ref(ph);
		if isPhrase(ph) and not isDisabled(ph) then
			v = stead.par('^', v, txtnm(n, ph:look()));
			n = n + 1
		end
	end
	return v;
end

stead.dialog_rescan = function(self)
	local i,k,ph
	k = 1
	for i,ph in opairs(self.obj) do
		ph = stead.ref(ph);
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

stead.dialog_empty = function(self)
	return not stead.dialog_rescan(self);
end

stead.dialog_phrase = function(self, num)
	if not tonumber(num) then
		if isPhrase(stead.ref(num)) then
			return stead.ref(num);
		end
		return nil
	end
	return stead.ref(self.obj[tonumber(num)]);
end

stead.phrase_seen = function(s, enb, ...)
	local i, ph
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self);
	end
	for i=1,stead.table.maxn(a) do
		ph = stead.dialog_phrase(s, a[i]);
		local r = not isPhrase(ph) or isRemoved(ph) or ph:disabled();
		if not enb then r = not r end
		if r then return false end
	end
	return true
end

stead.dialog_pseen = function(s, ...)
	return stead.phrase_seen(s, true, ...);
end

stead.dialog_punseen = function(s, ...)
	return stead.phrase_seen(s, false, ...);
end

local function ponoff(s, on, ...)
	local i, ph
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self)
	end
	for i=1,stead.table.maxn(a) do
		ph = stead.dialog_phrase(s, a[i]);
		if isPhrase(ph) and not isRemoved(ph) then
			if on then
				ph:enable();
			else 
				ph:disable();
			end
		end
	end
end

stead.dialog_prem = function(s, ...)
	local i, ph
	local a = {...}
	if stead.table.maxn(a) == 0 then
		stead.table.insert(a, stead.cctx().self);
	end
	for i=1,stead.table.maxn(a) do
		ph = stead.dialog_phrase(s, a[i]);
		if isPhrase(ph) then
			ph:remove();
		end
	end
end

stead.dialog_pon = function(self,...)
	return ponoff(self, true, ...);
end

stead.dialog_poff = function(self,...)
	return ponoff(self, false, ...);
end

function dlg(v) --constructor
	v.dialog_type = true;
	if v.ini == nil then
		v.ini = stead.dialog_enter;
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
	v = room(v);
	return v;
end

stead.phrase_action = function(self)
	local ph = self;
	local r, ret;

	if isDisabled(ph) then
		return nil, false
	end
-- here it is
	ph:disable(); -- /* disable it!!! */

	local last = stead.call(ph, 'ans');

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

	local wh = stead.here();

	while isDialog(wh) and not stead.dialog_rescan(wh) and stead.from(wh) ~= wh do
		wh = stead.from(wh)
	end

	if wh ~= stead.here() then
		ret = stead.par(stead.space_delim, ret, stead.back(wh));
	end
	
	ret = stead.par(stead.scene_delim, last, ret);
	
	if ret == nil then
		return r -- hack?
	end
	return ret
end

stead.phrase_save = function(self, name, h, need)
	if need then
		local m = " = phr("
		if isDisabled(self) then
			m = " = _phr("
		end
		h:write(stead.string.format("%s%s%s,%s,%s);\n", 
			name, m, 
			stead.tostring(self.dsc), 
			stead.tostring(self.ans), 
			stead.tostring(self.do_act)));
	end
	stead.savemembers(h, self, name, false);
end

stead.phrase_look = function(self, n)
	if isDisabled(self) then
		return
	end
	local v = stead.call(self, 'dsc');
	if type(v) ~= 'string' then return; end
	if game.hinting then
		return self:xref('{'..v..'}');
	end
	return v;
end

function phrase(o) --constructor
	local ret = o;
	ret.look = stead.phrase_look;
	ret.nam = ''; -- for start
	ret.phrase_type = true;
	ret.act = stead.phrase_action;
	ret.save = stead.phrase_save;
	ret = obj(ret);
	return ret;
end

function _phr(ask, answ, act)
	local p = phrase ({ dsc = ask, ans = answ, do_act = act });
	p:disable();
	return p;
end

function phr(ask, answ, act)
	local p = phrase ({ dsc = ask, ans = answ, do_act = act });
--	p:enable();
	return p;
end

stead.player_inv = function(self)
	return iface:inv(stead.cat(self:str()));
end

stead.player_ways = function(self)
	return iface:ways(stead.cat(stead.ref(self.where).way:str()));
end

stead.player_objs = function(self)
	return iface:objs(stead.cat(stead.ref(self.where):str()));
end

stead.player_look = function(self)
	return stead.ref(self.where):scene();
end

stead.obj_tag = function(self, id)
	local k,v

	if isDisabled(self) then
		return id
	end
	
	for k,v in opairs(self.obj) do
		v = stead.ref(v);
		if isObject(v) and not isDisabled(v) then
			id = id + 1;
			v.id = id;
			id = stead.obj_tag(v, id);
		end
	end
	return id;
end

stead.player_tagall = function(self)
	local id, k, v;
	id = 0;

	id = stead.obj_tag(stead.here(), id);
	id = stead.obj_tag(stead.me(), id);

	for k,v in opairs(ways()) do
		v = stead.ref(v);
		if isRoom(v) and not isDisabled(v) then
			id = id + 1;
			v.id = id;
		end
	end
end

stead.player_action = function(self, what, ...)
	local v,r,obj
	obj = stead.ref(self.where):srch(what);
	if not obj then
		return stead.call(stead.ref(game), 'action', what, ...); --player_do(self, what, ...);
	end
	v, r = stead.player_take(self, what, ...);
	if not v then
		v, r = stead.call(stead.ref(obj), 'act', ...);
		if not v and r ~= true then
			v, r = stead.call(stead.ref(game), 'act', obj, ...);
		end
	end
	return v, r;
end

stead.player_take = function(self, what, ...)
	local v,r,obj,w
	obj,w = stead.ref(self.where):srch(what);
	if not obj then
		return nil, false;
	end
	v,r = stead.call(stead.ref(obj), 'tak', ...);
	if v and r ~= false then
		take(obj, w);
	end
	return v;
end

stead.player_use = function(self, what, onwhat, ...)
	local obj, obj2, v, vv, r;
	local scene_use_mode = false

	obj = self:srch(what); -- in inv?
	if not obj then -- no
		obj = stead.ref(self.where):srch(what); -- in scene?
		if not obj then -- no!
			return game.err, false;
		end
		scene_use_mode = true -- scene_use_mode!
	end
	if onwhat == nil then -- only one?
		if scene_use_mode then
			return self:action(what, ...); -- call act
		else
			v, r = stead.call(stead.ref(obj),'inv', ...); -- call inv
		end
		if not v and r ~= true then
			v, r = stead.call(game, 'inv', obj, ...);
		end
		return v, r;
	end
	obj2 = stead.ref(self.where):srch(onwhat); -- in scene?
	if not obj2 then
		obj2 = self:srch(onwhat); -- in inv?
	end
	if not obj2 or obj2 == obj then
		return game.err, false;
	end
	if not scene_use_mode or isSceneUse(stead.ref(obj)) then
		v, r = stead.call(stead.ref(obj), 'use', obj2, ...);
		if r ~= false then
			vv = stead.call(stead.ref(obj2), 'used', obj, ...);
		end
	end
	if not v and not vv then
		v, r = stead.call(game, 'use', obj, obj2, ...);
	end
	return stead.par(stead.space_delim, v, vv);
end

stead.player_back = function(self)
	local where = stead.ref(self.where);
	if where == nil then
		return nil,false
	end
	return stead.go(self, where.__from__, true);
end

stead.go = function(self, where, back)
	local was = self.where;
	local need_scene = false;	
	local ret

	if not stead.in_walk_call then
		ret = function(rc) stead.in_walk_call = false return nil end
	else
		ret = function(rc) return rc end
	end

	stead.in_walk_call = true

	if where == nil then
		return nil,ret(false)
	end
	if not isRoom(stead.ref(where)) then
		error ("Trying to go nowhere: "..where, 2);
	end
	if not isRoom(stead.ref(self.where)) then
		error ("Trying to go from nowhere: "..self.where, 2);
	end

	if stead.in_entered_call or stead.in_onexit_call then
		error ("Do not use walk from onexit/entered action! Use exit/enter action instead:" .. self.where, 2);
	end

	local v, r;
	if not isVroom(stead.ref(where)) and not stead.in_exit_call then
		stead.in_exit_call = true -- to break recurse
		v,r = stead.call(stead.ref(self.where), 'exit', where);
		stead.in_exit_call = nil
		if r == false then
			return v, ret(r)
		end
	end

	local res = v;

	v = nil;
	if not back or not isDialog(stead.ref(self.where)) or isDialog(stead.ref(where)) then
		v, r = stead.call(stead.ref(where), 'enter', self.where);
		if r == false then
			return v, ret(r)
		end
		need_scene = true;
		if stead.ref(was) ~= stead.ref(self.where) then -- jump !!!
			where = stead.deref(self.where);
			need_scene = false;
		end
	end
	res = stead.par(stead.scene_delim, res, v);

	if not back then
		stead.ref(where).__from__ = stead.deref(self.where);
	end

	self.where = stead.deref(where);

	ret();

	PLAYER_MOVED = true
	if need_scene then -- or isForcedsc(stead.ref(where)) then -- i'am not sure...
		return stead.par(stead.scene_delim, res, stead.ref(where):scene());
	end
	return res;
end

stead.player_walk = function(self, where, ...)
	local v, r = stead.go(self, where, ...);
	return v, r;
end

stead.player_go = function(self, where)
	local w = stead.ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	local v, r = stead.go(self, w, false);
	return v, r;
end

stead.player_save = function(self, name, h)
	h:write(tostring(name)..".where = '"..stead.deref(self.where).."';\n");
	stead.savemembers(h, self, name, false);
end

function player(v)
	if v.nam == nil then
		error ("No player name in constructor.", 2);
	end
	if v.where == nil then
		v.where = 'main';
	end
	if v.tag == nil then
		v.tag = stead.player_tagall;
	end
	if v.walk == nil then
		v.walk = stead.player_walk;
	end
	if v.go == nil then
		v.go = stead.player_go;
	end
	if v.ways == nil then
		v.ways = stead.player_ways;
	end
	if v.back == nil then
		v.back = stead.player_back;
	end
	if v.look == nil then
		v.look = stead.player_look;
	end
	if v.inv == nil then
		v.inv = stead.player_inv;
	end
	if v.use == nil then
		v.use = stead.player_use;
	end
	if v.action == nil then
		v.action = stead.player_action;
	end
	if v.save == nil then
		v.save = stead.player_save;
	end
	if v.objs == nil then
		v.objs = stead.player_objs;
	end
	v.player_type = true;
	return obj(v);
end

stead.game_life = function(self)
	local i,o
	local av,v
	local was_moved
	stead.in_life_call = true;
	stead.lifes_op = {}; -- lifes to on/off
	stead.PLAYER_MOVED = PLAYER_MOVED
	for i,o in opairs(self.lifes) do
		local vv
		local pre
		o = stead.ref(o);
		if not isDisabled(o) then
			PLAYER_MOVED = false
			vv,pre = stead.call(o, 'life');
			if PLAYER_MOVED then -- clear life output, but not current
				av = nil
				v = nil
				was_moved = true
				ACTION_TEXT = vv;
			elseif pre then
				av = stead.par(stead.space_delim, av, vv);
			else
				v = stead.par(stead.space_delim, v, vv);
			end
		end
	end
	PLAYER_MOVED = was_moved
	if not PLAYER_MOVED then PLAYER_MOVED = stead.PLAYER_MOVED end
	stead.PLAYER_MOVED = nil
	stead.in_life_call = false;
	for i,o in ipairs(stead.lifes_op) do
		if o[1] then
			stead.lifeon(o[2], o[3]);
		else
			stead.lifeoff(o[2]);
		end
	end
	stead.lifes_op = nil;
	return v, av;
end

stead.player_moved = function()
	return PLAYER_MOVED or stead.PLAYER_MOVED
end

stead.check_list = function(k, v, p)
	if v.check == nil or not v:check(stead.string.format("%s[%q]", p, k)) then
		error ("error in list: "..stead.object..'.'..k);
	end
end

stead.check_room = function(k, v)
	if v.obj == nil then
		error("no obj in room:"..k);
	end
	if v.way == nil then
		error("no way in room:"..k);
	end
end

stead.check_player = function(k, v)
	v.where = stead.deref(v.where);
end

stead.check_object = function(k, v)
	if not v.nam then
		error ("No name in "..k);
	end
	if isRoom(v) then
		stead.check_room(k, v);
	end
	if isPlayer(v) then
		stead.check_player(k, v);
	end
	for_each(v, k, stead.check_list, isList, stead.deref(v))
end

function for_everything(f, ...)
	local is_ok = function(s)
		return true
	end
	for_each(_G, '_G', f, is_ok, ...)
end

local compat_api = function()
	if stead.compat_api then
		return
	end

	if stead.api_version < "1.6.0" then
		if not go then
			go = stead.go
		end
		if not goin then
			goin = walkin
		end
		if not goout then
			goout = walkout
		end
		if not goback then
			goback = walkback
		end
		if not _G["goto"] then
			if _VERSION == "Lua 5.1" then -- 5.1 lua
				_G["goto"] = walk
			end
		end
	end

	if stead.api_version < "1.7.1" then
		if not goin then
			goin = function() error ("Please use 'walkin' instead 'goin'.", 2) end
		end
		if not goout then
			goout = function() error ("Please use 'walkout' instead 'goout'.", 2) end
		end
		if not goback then
			goback = function() error ("Please use 'walkback' instead 'goback'.", 2) end
		end
		if not _G["goto"] then
			if _VERSION == "Lua 5.1" then -- 5.1 lua
				_G["goto"] = function() error ("Please use 'walk' instead 'goto'.", 2) end
			end
		end

		get_savepath = instead_savepath
		get_gamepath = instead_gamepath
		get_steadpath = instead_steadpath
		get_themespath = instead_themespath
		get_gamespath = instead_gamespath

		theme_var = instead_themevar
		theme_name = instead_theme_name

		is_sound = instead_sound

		set_timer = instead_timer

		menu_toggle = instead_menu_toggle
		stead_busy = instead_busy
		readdir = instead_readdir

		sound_load = instead_sound_load
		sound_free = instead_sound_free
		sounds_free = instead_sounds_free
		sound_channel = instead_sound_channel
		sound_volume = instead_sound_volume
		sound_panning = instead_sound_panning

		font_load = instead_font_load
		font_free = instead_font_free
		font_scaled_size = instead_font_scaled_size

		sprite_alpha = instead_sprite_alpha
		sprite_dup = instead_sprite_dup
		sprite_scale = instead_sprite_scale
		sprite_rotate = instead_sprite_rotate
		sprite_text = instead_sprite_text
		sprite_text_size = instead_sprite_text_size
		sprite_draw = instead_sprite_draw
		sprite_copy = instead_sprite_copy
		sprite_compose = instead_sprite_compose
		sprite_fill = instead_sprite_fill
		sprite_pixel = instead_sprite_pixel
		sprite_load = instead_sprite_load
		sprite_free = instead_sprite_free
		sprite_size = instead_sprite_size
		sprites_free = instead_sprites_free

		call = stead.call
		call_bool = stead.call_bool
		call_value = stead.call_value

		get_title = stead.get_title
		get_picture = stead.get_picture
		get_inv = stead.get_inv
		get_ways = stead.get_ways

		get_autosave = stead.get_autosave

		fmt = stead.fmt

		obj_tag = stead.obj_tag

		module_init = stead.module_init

		player_inv = stead.player_inv
		dialog_enter = stead.dialog_enter
	end

	if stead.api_version < "1.4.5" then
		stead.xref = function(...)
			return xref(...);
		end

		-- internals of call
		cctx = stead.cctx
		callpush = stead.callpush
		callpop = stead.callpop
		clearargs = stead.clearargs
		-- saving
		savemembers = stead.savemembers;
		savevar = stead.savevar
		clearvar = stead.clearvar

	--	do_ini = stead.do_ini
	--	do_savegame = stead.do_savegame
	end

	stead.compat_api = true
end

stead.do_ini = function(self, load)
	local v='',vv
	local function call_key(k, o)
		o.key_name = k;
	end
	local function call_codekey(k, o)
		stead.functions[o].key_name = k;
	end
	local function call_ini(k, o, ...)
		v = stead.par('', v, stead.call(o, 'ini', ...));
	end
	math.randomseed(os.time(os.date("*t")))
	stead.rnd(1); stead.rnd(2); stead.rnd(3); -- Lua bug?
	if type(game) ~= 'table' then
		error ("No valid 'game' object.");
	end
	if not isPlayer(stead.me()) then
		error ("No valid player.");
	end
	if not isRoom(stead.here()) then
		error ("No valid room.");
	end
	game.pl = stead.deref(game.pl);
	stead.me().where = stead.deref(stead.me().where);
--	game.where = stead.deref(game.where);
	if not load then
		compat_api()
		for_each_object(call_key);
		for_each_codeblock(call_codekey);
		for_each_object(stead.check_object);
		call_key("game", game);
		for_each(game, "game", stead.check_list, isList, stead.deref(game))
	end
	for_each_object(call_ini, load);
	stead.me():tag();
	if not self.showlast then
		self._lastdisp = nil;
	end
	stead.initialized = true
	return stead.par('',v, self._lastdisp); --stead.par('^^',v);
end

stead.game_ini = function(self)
	local v,vv
	v = stead.do_ini(self);
	vv = iface:title(stead.call(self,'nam'));
	vv = stead.par(stead.scene_delim, vv, stead.call(self,'dsc'));
	if type(init) == 'function' then
		init();
	end
--	if type(hooks) == 'function' then
--		hooks();
--	end
	return stead.par(stead.scene_delim, vv, v);
end

stead.game_start = function(s)
	if type(start) == 'function' then
		start() -- start function
	end
end

function game(v)
	if v.nam == nil then
		error ("No game name in constructor.", 2);
	end
	if v.pl == nil then
		v.pl = 'player';
	end
	if v.ini == nil then
		v.ini = stead.game_ini;
	end
	if v.start == nil then
		v.start = stead.game_start
	end
	if v.save == nil then
		v.save = stead.game_save;
	end
	if v.load == nil then
		v.load = stead.game_load;
	end
	if v.life == nil then
		v.life = stead.game_life;
	end
	if v.step == nil then
		v.step = stead.game_step;
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
	return stead.ref(game.lifes:srch(v));
end

function isEnableSave()
	if game.enable_save == nil or stead.get_autosave() then
		return true
	end
	return stead.call_bool(game, 'enable_save');
end

function isEnableAutosave()
	if game.enable_autosave == nil then
		return true
	end
	return stead.call_bool(game, 'enable_autosave');
end

function for_each(o, n, f, fv, ...)
	local call_list = {}
	local k,v
	if type(o) ~= 'table' then
		return
	end
	stead.object = n;

	for k,v in pairs(o) do
		if fv(v) then
			stead.table.insert(call_list, { k = k, v = v });
		end
	end

	for k, v in ipairs(call_list) do
		f(v.k, v.v, ...);
	end
end

function isCode(s)
	return type(s) == 'function' and type(stead.functions[s]) == 'table'
end
function for_each_codeblock(f,...)
	for_each(_G, '_G', f, isCode, ...)
end

function for_each_object(f,...)
	for_each(_G, '_G', f, isObject, ...)
end

function for_each_player(f,...)
	for_each(_G, '_G', f, isPlayer, ...)
end

function for_each_room(f,...)
	for_each(_G, '_G', f, isRoom, ...)
end

function for_each_list(f,...)
	for_each(_G, '_G', f, isList, ...)
end

stead.clearvar = function(v)
	local k,o
	for k,o in pairs(v) do
		if type(o) == 'table' and o.__visited__ ~= nil then
			o.__visited__ = nil
			o.auto_saved = nil
			stead.clearvar(o)
		end
	end
end

stead.savemembers = function(h, self, name, need)
	local k,v
	for k,v in pairs(self) do
		local need2
		if k ~= "__visited__" then
			need2 = false
			if isForSave(k, v, self) then
				need2 = true;
			end

			if type(k) == 'string' then
				stead.savevar(h, v, name..'['..stead.string.format("%q",k)..']', need or need2);
			elseif type(k) == 'number' then
				stead.savevar(h, v, name.."["..k.."]", need or need2)
			elseif type(k) == 'table' and type(k.key_name) == 'string' then
				stead.savevar(h, v, name.."["..k.key_name.."]", need or need2)
			end
		end
	end
end
-- savemembers = stead.savemembers;

stead.savevar = function(h, v, n, need)
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
			if v.auto_allocated and not v.auto_saved then
				v:save(v.key_name, h, false, true); -- here todo
			end
			if need then
				if stead.ref(v.key_name) == nil then
					v.key_name = 'null'
				end
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

		stead.savemembers(h, v, n, need);
		return;
	end

	if not need then
		return
	end
	h:write(n, " = ",tostring(v))
	h:write("\n") 
end
-- savevar = stead.savevar

stead.gamereset = function(file, forget)
	stead.clearargs()
	if forget then
		stead.stop_music();
		stead.stop_sound();
		timer:stop();
		if type(variables) == 'table' then
			local k,v
			for k,v in ipairs(variables) do
				_G[v] = nil
			end
			variables = nil
			variables_save = nil
		end
		init = function() -- null init function
		end
		start = function() -- null start function
		end
		for_each_object(function(k, o) -- destroy all objects
			if o.system_type then
				return
			end
			_G[k] = nil
		end);
		game._scripts = { }
		game.lifes:zap()
		game.scriptsforget = true
		-- anything else?
		stead:init();
	end
	dofile(file);
	game:ini();

	if #game._scripts == 0 or file ~= game._scripts[#game._scripts] then
		if #game._scripts ~= 0 or file ~= 'main.lua' then
			stead.table.insert(game._scripts, file);
		end
	end
end

stead.gamefile = function(file, forget)
	stead.gamereset(file, forget)
	if forget then
		game:start()
		stead.started = true
		return stead.walk(stead.here(), false, false, true);
	end
end


stead.do_savegame = function(s, h)
	stead.busy(true)
	local function save_object(key, value, h)
		stead.busy(true)
		stead.savevar(h, value, key, false);
		return true;
	end
	local function save_var(key, value, h)
		stead.busy(true)
		stead.savevar(h, value, key, isForSave(key, value, _G))
	end
	local forget = game.scriptsforget
	local i,v
	for i,v in ipairs(s._scripts) do
		h:write(stead.string.format("stead.gamereset(%q,%s)\n", 
			v, tostring(forget)))
		forget = nil
	end
	save_object('allocator', allocator, h); -- always first!
	for_each_object(save_object, h);
	save_object('game', self, h);
	for_everything(save_var, h);
--	save_object('_G', _G, h);
	stead.clearvar(_G);
	stead.busy(false)
end

stead.savename = function()
	return stead.call(stead.here(), 'nam');
end

stead.game_save = function(self, name, file) 
	local h;

	if file ~= nil then
		file:write(name..".pl = '"..stead.deref(self.pl).."'\n");
		stead.savemembers(file, self, name, false);
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
	local n = stead.savename()
	if type(n) == 'string' and n ~= "" then
		h:write("-- $Name: "..n:gsub("\n","\\n").."$\n");
	end
	stead.do_savegame(self, h);
	h:flush();
	h:close();
	game.autosave = false; -- we have only one try for autosave
	return nil;
end

stead.game_load = function(self, name) 
	if name == nil then
		return nil, false
	end
	local f, err = loadfile(name);
	if f then
		local i,r = f();
		if r then
			return nil, false
		end
		i, r = stead.do_ini(self, true);
		if not stead.started then
			game:start()
			stead.started = true
		end
		return i, r
	end
	return nil, false
end


stead.game_step = function(self)
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
	_scripts = {};
};

stead.strip = function(s)
	local s = tostring(s);
	s = stead.string.gsub(s, '^[ \t]*', '');
	s = stead.string.gsub(s, '[ \t]*$', '');
	return s;
end

function isForcedsc(v)
	local r,g
	r = stead.call_bool(v, 'forcedsc');
	if r then
		return true
	end
	g = stead.call_bool(game, 'forcedsc', v);
	return g and r ~= false
end

function isSceneUse(v)
	local o,g
	o = stead.call_bool(v, 'scene_use');
	if o then
		return true
	end
	g = stead.call_bool(game, 'scene_use', v);
	return g and o ~= false
end

iface = {
	anchor = function(self)
		return '';
	end;
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
	just = function(self, str)
		return str;
	end,
	top = function(self, str)
		return str;
	end,
	bottom = function(self, str)
		return str;
	end,
	middle = function(self, str)
		return str;
	end,
	tab = function(self, str, al)
		return '';
	end;
	bold = function(self, str)
		return str;
	end,
	under = function(self, str)
		return str;
	end,
	st = function(self, str)
		return str;
	end,
	enum = function(self, n, str)
		return n..' - '..str;
	end,
	xref = function(self, str, obj)
		local o = stead.ref(stead.here():srch(obj));
		if not o then 
			o = stead.ref(ways():srch(obj));
		end
		if not o then
			o = stead.ref(stead.me():srch(obj));
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
		local l, vv
		if st and not moved then
			if cmd ~= 'look' then
				av = txtem(av);
				pv = txtem(pv);
				r  = txtem(r);
				if isForcedsc(stead.here()) then
					l = stead.me():look();
				end
			end
		end
		vv = stead.fmt(stead.cat(stead.par(stead.scene_delim, l, r, av, objs, pv), '^'));
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
		stead.set_sound(); -- empty sound
		cmd,a = stead.getcmd(inp);
		if cmd == '' then cmd = 'look' end
--		stead.me():tag();
		local oldloc = stead.here();
		if cmd == 'look' then
			stead.state = true
			r,v = stead.me():look();
		elseif cmd == 'obj' then
			r,v = stead.me():objs();
		elseif cmd == 'inv' then
			r,v = stead.me():inv();
		elseif cmd == 'way' then
			r,v = stead.me():ways();
		elseif cmd == 'ls' then
			r = stead.par(stead.scene_delim, stead.me():objs(), stead.me():inv(), stead.me():ways());
			v = nil;
		elseif cmd == 'go' then
			stead.state = true
			r,v = stead.me():go(stead.unpack(a));
		elseif cmd == 'back' then
			stead.state = true
			r,v = stead.me():go(stead.from());
		elseif cmd == 'act' then
			stead.state = true
			r,v = stead.me():action(stead.unpack(a));
		elseif cmd == 'use' then
			stead.state = true
			r,v = stead.me():use(stead.unpack(a));
		elseif cmd == 'save' then
			r, v = game:save(stead.unpack(a));
		elseif cmd == 'load' then
			r, v = game:load(stead.unpack(a));
			if v ~= false and game.showlast then
				return r;
			end
		elseif cmd == 'wait' then -- nothing todo in game, skip tick
			v = nil;
			r = true;
			stead.state = true
		elseif cmd == 'nop' then -- inv only
			v = true;
			r = nil;
			stead.state = true
		else
			stead.state = true
			r,v = stead.me():action(cmd, stead.unpack(a));
		end
		-- here r is action result, v - ret code value	
		-- state -- game state changed
		if stead.state and r == nil and v == true then -- we do nothing
			return nil, true; -- menu
		end

		if stead.state and r == nil and v == nil and stead.api_version >= "1.3.5" then -- new goto
			return nil, false -- really nothing
		end

		if RAW_TEXT and v ~= false then
			return stead.cat(r, '\n'), true;
		end

		if v == false then
			return stead.cat(r, '\n'), false;
		end

		ACTION_TEXT = r; -- here, life methods can redefine this

		local av, pv -- av -- active lifes, pv -- background
		local vv

		if stead.state then
			pv,av = game:step();
			stead.me():tag();
			vv = stead.here():look();
		end

		vv = self:fmt(cmd, stead.state, (oldloc ~= stead.here()) or PLAYER_MOVED, 
			ACTION_TEXT, av, vv, pv);

		if stead.state then
			game._lastdisp = vv
			stead.last_act(ACTION_TEXT)
		end
		if vv == nil then -- nil is error
			vv = ''
		end
		return vv, true; -- action is here
	end, 
	shell = function(self)
		local inp, i, k, cmd, a, n;
		stead.me():tag();		
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
	return stead.ref(game.pl);
end
stead.me = me

function where(s)
	if not isObject(stead.ref(s)) then error("Wrong parameter to where.", 2); end
	if isPlayer(stead.ref(s)) then
		return stead.ref(stead.ref(s).where);
	end
	return stead.ref(stead.ref(s).__where__);
end

function here()
	return stead.ref(stead.me().where);
end
stead.here = here

function from(w)
	if w == nil then
		w = stead.here();
	else
		w = stead.ref(w);
	end
	return stead.ref(w.__from__);
end
stead.from = from

stead.time = function(s)
	local n = game._time;
	if type(s) == 'number' then
		game._time = s
	end
	return n
end

function inv()
	return stead.me().obj;
end

function objs(w)
	if not w then
		return stead.here().obj;
	else
		return stead.ref(w).obj;
	end
end

function ways(w)
	if not w then
		return stead.here().way;
	else
		return stead.ref(w).way;
	end
end

stead.xref = function(str, obj, ...)
	if type(str) ~= 'string' then return nil; end; 
	return iface:xref(str, obj, ...);
end
xref = stead.xref

function pseen(...)
	if not isDialog(stead.here()) then
		return
	end
	return stead.here():pseen(...);
end

function punseen(...)
	if not isDialog(stead.here()) then
		return
	end
	return stead.here():punseen(...);
end

function pon(...)
	if not isDialog(stead.here()) then
		return
	end
	stead.here():pon(...);
end

function poff(...)
	if not isDialog(stead.here()) then
		return
	end
	stead.here():poff(...);
end

function prem(...)
	if not isDialog(stead.here()) then
		return
	end
	stead.here():prem(...);
end

function lifeon(what, nr)
	if stead.in_life_call then
		stead.table.insert(stead.lifes_op, { true, what, nr });
		return
	end
	game.lifes:add(what, nr);
end
stead.lifeon = lifeon

function lifeoff(what)
	if stead.in_life_call then
		stead.table.insert(stead.lifes_op, { false, what });
		return
	end
	game.lifes:del(what);
end
stead.lifeoff = lifeoff

stead.allocator_save = function(s, name, h, need, auto)
	if s.auto_allocated and not auto then
		return
	end
	if need then
		if s.auto_allocated then -- in current realization always false
			local m = stead.string.format("allocator:new(%s, %s)\n", 
				stead.tostring(s.constructor),
				stead.tostring(s.constructor));
			h:write(m);
		else
			local m = stead.string.format(" = allocator:get(%s, %s)\n",
				stead.tostring(name),
				stead.tostring(s.constructor));
			h:write(name..m);
			if stead.api_version >= "1.3.0" then
				m = stead.string.format("stead.check_object(%s, %s)\n",
					stead.tostring(name),
					name);
				h:write(m);
			end
		end
	end
	stead.savemembers(h, s, name, false);
	if s.auto_allocated then
		s.auto_saved = true
	end
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

stead.vobj_save = function(self, name, h, need)
	local dsc = self.dsc;
	local w = stead.deref(self.where);
	
	if need then
		h:write(stead.string.format("%s  = vobj(%s, %s, %s, %s);\n",
			name, 
			stead.tostring(self.key), 
			stead.tostring(self.nam), 
			stead.tostring(dsc), 
			stead.tostring(w)));

	end
	stead.savemembers(h, self, name,false);
end

stead.vobj_act = function(self, ...)
	local o, r = stead.here():srch(self); -- self.nam
	if stead.ref(o) and stead.ref(o).where then
		return stead.walk(stead.ref(o).where);
	end
	return stead.call(stead.ref(r),'act', self.key, ...);
end

stead.vobj_used = function(self, ...)
	local o, r = stead.here():srch(self.nam);
	return stead.call(stead.ref(r),'used', self.key, ...);
end

function vobj(key, name, dsc, w)
	if not tonumber(key) then
		error ("vobj key must be number!", 2);
	end
	return obj{ key = key, nam = name, dsc = dsc, where = stead.deref(w), act = stead.vobj_act, used = stead.vobj_used, save = stead.vobj_save, obj = list({}) };
end

function vway(name, dsc, w)
--	o.object_type = true;
	return  obj{ key = -1, nam = name, dsc = dsc, act = stead.vobj_act, where = stead.deref(w), used = stead.vobj_used, save = stead.vobj_save, obj = list({}), };
end

stead.vroom_save = function(self, name, h, need)
	if need then
		local t = stead.string.format("%s = vroom(%s, %q);\n",
			name, stead.tostring(self.nam), 
				stead.deref(self.where))
		h:write(t);
	end
	stead.savemembers(h, self, name,false);
end

stead.vroom_enter = function(self, ...)
	return stead.walk(self.where);
end

function isVroom(v)
	return (type(v) == 'table') and (v.vroom_type)
end

function vroom(name, w)
	if w == nil then
		error("Wrong parameter to vroom.", 2);
	end
	return room { vroom_type = true, nam = name, where = stead.deref(w), enter = stead.vroom_enter, save = stead.vroom_save, };
end

function walk(what)
	local v,r=stead.me():walk(what);
	stead.me():tag();
	return v,r;
end
stead.walk = walk;

function back()
	return stead.me():back();
end
stead.back = back;

stead.rnd = function(m)
	if not m then
		return math.random();
	end
	return math.random(m);
end

function taken(obj)
	if isObject(stead.ref(obj)) and stead.ref(obj)._taken then
		return true
	end
	return false;
end

function remove(obj, from)
	local o,w
	from = stead.ref(from)
	if from then
		if isList(from) then
			return from:del(obj)
		end
		o,w = from:srch(obj);
	else
		o,w = stead.here():srch(obj);
	end
	if w then
		stead.ref(w).obj:del(obj);
	end
	o = stead.ref(o);
	if not isObject(o) then
		o = stead.ref(obj);
	end
	if isObject(o) then
		o.__where__ = nil;
	end
	return o
end
stead.remove = remove

function purge(obj, from)
	local o,w
	from = stead.ref(from)
	if from then
		if isList(from) then
			return from:purge(obj)
		end
		o,w = from:srch(obj, true);
	else
		o,w = stead.here():srch(obj, true);
	end
	if w then
		stead.ref(w).obj:purge(obj);
	end
	o = stead.ref(o);
	if not isObject(o) then
		o = stead.ref(obj);
	end
	if isObject(o) then
		o.__where__ = nil;
	end
	return o
end
stead.purge = purge

function taketo(obj, wh, pos)
	local o = remove(obj, wh);
	if not isObject(o) then
		error ("Trying to take wrong object.", 2);
	end
	inv():add(obj, pos);
	o._taken = true
	wh = stead.deref(stead.me())
	if type(wh) == 'string' then
		o.__where__ = wh;
	end
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
	local o = stead.ref(obj);
	if not isObject(o) then
		error ("Trying to put wrong object.", 2);
	end
	if not w then
		wh = stead.deref(stead.here());
		w = stead.here();
	else
		wh = stead.deref(w);
		w = stead.ref(w);
	end
	if isList(w) then
		w:add(obj, pos);
	else
		w.obj:add(obj, pos);
	end
	if type(wh) == 'string' then
		o.__where__ = wh;
	end
	return o;
end


function put(obj, w)
	return stead.placeto(obj, w);
end

function putf(obj, w)
	return stead.placeto(obj, w, 1);
end

place = put
placef = putf
placeto = putto
stead.placeto = placeto

function replace(obj, obj2, from)
	local o,w,i
	if not isObject(stead.ref(obj2)) then
		error ("Wrong parameter to replace.", 2);
	end
	from = stead.ref(from)
	if from then
		if isList(from) then
			from:replace(obj, obj2);
			return stead.ref(obj)
		end
		o,w = from:srch(obj);
	else
		o,w = stead.here():srch(obj);
	end
	if w then
		stead.ref(w).obj:replace(o, obj2);
		stead.ref(obj2).__where__ = stead.deref(w);
	else
		stead.placeto(obj2, from);
	end
	o = stead.ref(o);
	if not isObject(o) then
		o = stead.ref(obj);
	end
	if isObject(o) then
		o.__where__ = nil;
	end
	return o;
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
		wh = stead.here();
	else
		wh = stead.ref(wh);
	end
	local o,w = wh:srch(obj);
	o = stead.ref(o);
	if isObject(o) then
		return o,w
	end
	return nil
end

function exist(obj, wh)
	if not wh then
		wh = stead.here();
	else
		wh = stead.ref(wh);
	end
	local o,w = wh:srch(obj, true);
	o = stead.ref(o);
	if isObject(o) then
		return o,w
	end
	return nil
end

function have(obj)
	local o = inv():srch(obj);
	o = stead.ref(o);
	if isObject(o) then
		return o
	end
	return nil
end

function moveto(obj, there, from, pos)
	stead.remove(obj, from);
	stead.placeto(obj, there, pos);
	return stead.ref(obj);
end
stead.moveto = moveto

function move(obj, there, from)
	return stead.moveto(obj, there, from);
end

function movef(obj, there, from)
	return stead.moveto(obj, there, from, 1);
end

stead.get_picture = function()
	local s = stead.call(stead.here(),'pic');
	if not s then
		s = stead.call(game, 'pic');
	end
	return s;
end

stead.get_title = function()
	local s = stead.call(stead.here(),'nam');
	return s;
end

stead.get_music = function()
	return game._music, game._music_loop;
end

stead.get_music_loop = function()
	return game._music_loop;
end

stead.save_music = function(s)
	if s == nil then
		s = self
	end
	s.__old_music__ = stead.get_music();
	s.__old_loop__ = stead.get_music_loop();
end

stead.restore_music = function(s)
	if s == nil then
		s = self
	end
	stead.set_music(s.__old_music__, s.__old_loop__);
end

stead.set_music = function(s, count)
	game._music = s;
	if not tonumber(count) then
		game._music_loop = 0;
	else
		game._music_loop = tonumber(count);
	end
end

stead.set_music_fading = function(o, i)
	if o and o == 0 then o = -1 end
	if i and i == 0 then i = -1 end

	game._music_fadeout = o
	if not i then
		game._music_fadein = o
	else
		game._music_fadein = i
	end
end

stead.get_music_fading = function()
	return game._music_fadeout, game._music_fadein
end

stead.stop_music = function()
	stead.set_music(nil, -1);
end

stead.is_music = function()
	return game._music ~= nil and game._music_loop ~= -1
end

if instead_sound == nil then
	function instead_sound()
		return false -- sdl-instead export own function
	end
end
stead.is_sound = instead_sound

if instead_savepath == nil then
	function instead_savepath()
		return "./"
	end
end

function autosave(slot)
	game.autosave = true;
	game.autosave_slot = slot;
end

stead.get_autosave = function()
	return game.autosave, game.autosave_slot
end

stead.get_sound = function()
	return game._sound, game._sound_channel, game._sound_loop;
end

stead.get_sound_chan = function()
	return game._sound_channel
end

stead.get_sound_loop = function()
	return game._sound_loop
end

stead.stop_sound = function(chan)
	if not tonumber(chan) then
		stead.set_sound('@');
		return
	end
	stead.add_sound('@'..tostring(chan));
end

stead.add_sound = function(s, chan, loop)
	if type(s) ~= 'string' then
		return
	end
	if type(game._sound) == 'string' then
		if tonumber(chan) then
			s = s..'@'..tostring(chan);
		end
		if tonumber(loop) then
			s = s..','..tostring(loop)
		end
		stead.set_sound(game._sound..';'..s, stead.get_sound_chan(), stead.get_sound_loop());
	else
		stead.set_sound(s, chan, loop);
	end
end

stead.set_sound = function(s, chan, loop)
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
	local o = stead.ref(p);
	if type(stead.deref(p)) ~= 'string' or not o then
		error ("Wrong player name in change_pl...", 2);
	end
	game.pl = stead.deref(p);
	return stead.walk(o.where, false, true, true); -- no call enter/exit
end

function disabled(o)
	return isDisabled(stead.ref(o))
end

function disable(o)
	o = stead.ref(o)
	if isObject(o) then
		o:disable()
	end
	return o
end

function enable(o)
	o = stead.ref(o)
	if isObject(o) then
		o:enable()
	end
	return o
end

function disable_all(o)
	o = stead.ref(o)
	if isObject(o) or isList(o) then
		o:disable_all()
	end
	return o
end

function enable_all(o)
	o = stead.ref(o)
	if isObject(o) or isList(o) then
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

stead.inherit = function(o, f)
	return function(...)
		return f(o(...))
	end
end
inherit = stead.inherit

stead.hook = function(o, f)
	return function(...)
		local ff
		if type(o) ~= 'function' then
			ff = function(s)
				return o;
			end
		else
			ff = o
		end
		return f(ff, ...)
	end
end
hook = stead.hook

function nameof(v)
	if isObject(v) then
		local r = stead.call(v, 'nam');
		return r
	end
end

stead.nameof = nameof

stead.dispof = function(v)
	if isObject(v) then
		local r
		if game.gui then
			r = stead.call(v, 'disp')
		end
		if r == nil then
			r = stead.call(v, 'nam');
		end
		return r
	end
end

function stead_version(v)
	if not tostring(v) then
		return
	end
	if stead.version < v then
		error ([[The game requires instead engine of version ]] ..v.. [[ or higher.
		http://instead.googlecode.com]], 2)
	end
	stead.api_version = v
	if v >= "1.2.0" then
		require ("walk")
		require ("vars")
		require ("object")
	end
	if v >= "1.6.3" then
		require ("dlg")
	end
end
instead_version = stead_version

function code(v)
	local f = stead.eval(v)
	if not f then
		error ("Wrong script: "..tostring(v), 2);
	end
	stead.functions[f] = { f = f, code = v };
	return f;
end
stead.code = code

--- here the game begins
stead.objects = function(s)
	null = obj {
		nam = 'null';
	}

	input = obj { -- input object
		system_type = true,
		nam = 'input',
	--[[	key = function(s, down, key)
			return
		end, ]]
	--[[	click = function(s, down, mb, x, y, [ px, py ] )
			return
		end ]]
	};

	timer = obj { -- timer calls stead.timer callback 
		nam = 'timer',
		ini = function(s)
			if tonumber(s._timer) ~= nil and type(stead.set_timer) == 'function' then
				stead.set_timer(s._timer);
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
			if type(stead.set_timer) ~= 'function' then
				return false
			end
			stead.set_timer(v)
			return true
		end,
		--[[ 	callback = function(s)
			end, ]]
	};

	allocator = obj {
		nam = 'allocator',
		get = function(s, n, c)
			if isObject(stead.ref(n)) and stead.api_version >= "1.3.0" then -- static?
				return stead.ref(n);
			end
			local v = stead.ref(c);
			if not v then
				error ("Null object in allocator: "..tostring(c));
			end
			v.key_name = n;
			v.save = stead.allocator_save;
			v.constructor = c;
			return v
		end,
		delete = function(s, w)
			w = stead.ref(w);
			if type(w.key_name) ~= 'string' then
				return
			end
			local f = stead.eval(w.key_name..'= nil;');
			if f then
				f();
			end
		end,
		new = function(s, n, key)
			local v = stead.ref(n);
			if type(v) ~= 'table' or type(n) ~= 'string' then
				error ("Error in new.", 2);
			end
			v.save = stead.allocator_save;
			v.constructor = n;
			if key then
				s.objects[key] = v
				v.key_name = stead.string.format('allocator["objects"][%s]', stead.tostring(key));
			else
				stead.table.insert(s.objects, v);
				v.key_name = 'allocator["objects"]['..stead.table.maxn(s.objects)..']';
			end
			if stead.api_version >= "1.3.0" then
				stead.check_object(v.key_name, v)
			end
			return v
		end,
		objects = {
			save = function(self, name, h, need)
				stead.savemembers(h, self, name, true);
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
	stead.initialized = false
	stead.started = false
	stead:objects();
	s.functions = {} -- code blocks
	local k,v
	for k,v in ipairs(s.modules_ini) do
		v();
	end
end
stead:init();

-- those are sill in global space
add_sound = stead.add_sound
set_sound = stead.set_sound
stop_sound = stead.stop_sound

get_sound = stead.get_sound
get_sound_loop = stead.get_sound_loop
get_sound_chan = stead.get_sound_chan

get_music = stead.get_music
get_music_fading = stead.get_music_fading
get_music_loop = stead.get_music_loop

set_music = stead.set_music
set_music_fading = stead.set_music_fading
stop_music = stead.stop_music

save_music = stead.save_music
restore_music = stead.restore_music

is_music = stead.is_music

ref = stead.ref
deref = stead.deref

mouse_pos = stead.mouse_pos
mouse_filter = stead.mouse_filter

get_ticks = stead.ticks

pclr = stead.pclr
pget =  stead.pget
p = stead.p
pr = stead.pr
pn = stead.pn
par = stead.par
cat = stead.cat
player_moved = stead.player_moved
rnd = stead.rnd;
gamefile = stead.gamefile
time = stead.time

-- vim:ts=4
