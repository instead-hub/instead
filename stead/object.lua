local function onevent(ev, ...)
	local vv, r
	if stead.api_version >= "1.6.3" then
		vv, r = stead.call(game, ev, ...);
		if r == false then
			return vv, false
		end
		if vv == false then
			return nil, false
		end
		return vv
	end
end

function player_action(self, what, ...)
	local v,r,obj,vv

	if isXaction(what) then -- already xact
		obj = what
	else
		obj = _G[what];
	end
	if not isXaction(obj) then
		obj = stead.ref(self.where):srch(what);
	end
	if not obj then
		return stead.call(game, 'action', what, ...); --player_do(self, what, ...);
	end
	vv, r = onevent('onact', obj, ...);
	if vv == false then
		return
	end
	if r == false then 
		return vv
	end
	v, r = player_take(self, what, ...);
	if type(vv) == 'string' then
		v = stead.par(stead.space_delim, vv, v);
	end
	if not v and not r then
		v, r = stead.call(obj, 'act', ...);
		if not v and not r then
			v, r = stead.call(game, 'act', obj, ...);
		end
	end
	return v, r;
end


function player_use(self, what, onwhat, ...)
	local obj, obj2, v, vv, r;
	local scene_use_mode = false

	obj = _G[what];

	if isXaction(obj) then -- use xact is act
		return self:action(obj, onwhat, ...)
	end

	obj = self:srch(what); -- in inv?

	if not obj then -- no
		obj = stead.ref(self.where):srch(what); -- in scene?
		if not obj then -- no!
			return game.err, false;
		end
		scene_use_mode = true -- scene_use_mode!
	end

	obj = stead.ref(obj);
	if onwhat == nil then -- only one?
		if scene_use_mode then
			return self:action(what, ...); -- call act
		end
		vv, r = onevent('oninv', obj, ...);
		if vv == false then
			return
		end
		if r == false then
			return vv
		end
		v, r = stead.call(obj, 'inv', ...); -- call inv
		if type(vv) == 'string' then
			v = stead.par(stead.space_delim, vv, v);
		end
		if not v and not r then
			v, r = stead.call(game, 'inv', obj, ...);
		end
		return v, r;
	end
	obj2 = stead.ref(self.where):srch(onwhat); -- in scene?
	if not obj2 then
		obj2 = self:srch(onwhat); -- in inv?
	end
	if not obj2 then
		return game.err, false;
	end
	
	obj2 = stead.ref(obj2)

	if not scene_use_mode or isSceneUse(obj) then
		vv, r = onevent('onuse', obj, obj2, ...);
		if vv == false then
			return
		end
		if r == false then 
			return vv 
		end
		v, r = stead.call(obj, 'use', obj2, ...);
		if type(vv) == 'string' then
			v = stead.par(stead.space_delim, vv, v);
		end
		if r ~= false then
			vv = stead.call(obj2, 'used', obj, ...);
			if type(vv) == 'string' then
				v = stead.par(stead.space_delim, v, vv);
			end
		end
	end
	if not v and vv and stead.api_version >= "1.7.1" then
		v = true
	end
	if not v then
		v, r = stead.call(game, 'use', obj, obj2, ...);
	end
--	if v == nil and stead.api_version >= "1.3.5" then
--		return true
--	end
	return v
end

function vobj_save(self, name, h, need)
	local w = stead.deref(self.where)
	local dsc = self.dsc
	
	if need then
		h:write(stead.string.format("%s  = vobj(%s,%s,%s);\n",
			name, 
			stead.tostring(self.nam), 
			stead.tostring(dsc), 
			stead.tostring(w)));
	end
	stead.savemembers(h, self, name, false);
end

function vobj_act(self, ...)
	local o, r = here():srch(self); -- self.nam
	if stead.ref(o) and stead.ref(o).where then
		return stead.walk(stead.ref(o).where);
	end
	return stead.call(stead.ref(r),'act', self.nam, ...);
end

function vobj_used(self, ...)
	local o, r = here():srch(self.nam);
	return stead.call(stead.ref(r),'used', self.nam, ...);
end

function vobj_use(self, ...)
	local o, r = here():srch(self.nam);
	return stead.call(stead.ref(r),'use', self.nam, ...);
end

function vobj(name, dsc, w)
	return obj{ nam = tostring(name), 
		vobject_type = true,
		dsc = dsc, 
		where = stead.deref(w), 
		act = vobj_act, 
		used = vobj_used, 
		use = vobj_use,
		save = vobj_save };
end

function vway(name, dsc, w)
--	o.object_type = true;
	return  obj{ nam = tostring(name), 
		vobject_type = true,
		dsc = dsc, 
		act = vobj_act, 
		where = stead.deref(w), 
		used = vobj_used,
		use = vobj_use, 
		save = vobj_save };
end

function isVobject(v)
	return (type(v) == 'table') and (v.vobject_type)
end

function list_check(self, name) -- force using of objects, instead refs
	local i, v, ii;
	for i,v,ii in opairs(self) do
		local o = stead.ref(v);
		if not isObject(o) then 
			error ("No object: "..name.."["..tostring(ii).."]".." ("..tostring(type(v))..")")
			return false
		end
		if (v.auto_allocated and not stead.ref(v.key_name)) -- renew
			or (isObject(stead.deref(v)) and not v._dynamic_type) then -- no named object!
			local n = stead.string.format("%s[%d]", name, ii);
			v = allocator:new(n, n);
			self[ii] = v;
			v.auto_allocated = true;
			for_each(v, n, stead.check_list, isList, stead.deref(v));
		else
			self[ii] = o;
		end
	end
	return true; 
end

function list_add(self, name, pos)
	local nam = name
	if stead.initialized then
		nam = stead.ref(name);
	end
	if not nam then
		error ("Add wrong object to list: "..tostring(name), 2);
	end
	if self:look(nam) then
		return nil
	end
	self.__modified__ = true;
	if isObject(stead.deref(nam)) then
		nam._dynamic_type = true
	end
	if tonumber(pos) then
		stead.table.insert(self, tonumber(pos), nam);
		self[tonumber(pos)] = nam; -- for spare lists
	else
		stead.table.insert(self, nam);
	end
	return true
end

function list_set(self, name, pos)
	local nam = name
	local i = tonumber(pos);
	if not i then
		return nil
	end
	if stead.initialized then
		nam = stead.ref(name);
	end
	if not nam then
		error ("Set wrong object in list: "..tostring(name), 2);
	end
	if isObject(stead.deref(nam)) then
		nam._dynamic_type = true
	end
	self.__modified__ = true;
	self[i] = nam; -- for spare lists
	return true
end

function list_concat(self, other, pos)
	local n,o,ii
	for n,o,ii in opairs(other) do
		o = stead.ref(o);
		if pos == nil then
			self:add(o);
		else 
			self:add(o, pos);
			pos = pos + 1;
		end
	end
end

stead.delim = '|'

function list_str(self)
	local i, v, vv, o;
	for i,o in opairs(self) do
		o = stead.ref(o);
		if isObject(o) and not isDisabled(o) then
			vv = stead.dispof(o)
			vv = stead.xref(vv, o);
			v = stead.par(stead.delim, v, vv);
		end
	end
	return v;
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
		o = stead.ref(o);
		if isObject(o) and not isDisabled(o) then
			vv = stead.dispof(o)
			vv = stead.xref(vv, o);
			v = stead.par(stead.delim, v, vv, obj_str(o));
		end
	end
	return v;
end

function path(w, wh) -- search in way, disabled too
	if not wh then
		wh = here();
	else
		wh = stead.ref(wh);
	end
	local o = ways(wh):srch(w, true);
	o = stead.ref(o);
	if isRoom(o) then
		return o
	end
	return nil
end

game.lifes = list(game.lifes)
stead:init(); -- reinit ob

-- vim:ts=4
