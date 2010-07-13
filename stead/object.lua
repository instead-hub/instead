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
	
	obj = ref(obj)
	obj2 = ref(obj2)

	if not scene_use_mode or isSceneUse(obj) then
		v, r = call(obj, 'use', obj2, unpack(arg));
		if r ~= false then
			vv = call(obj2, 'used', obj, unpack(arg));
		end
	end
	if not v and not vv then
		v, r = call(game, 'use', obj, obj2, unpack(arg));
	end
	return stead.par(' ', v, vv);
end

function vobj_save(self, name, h, need)
	local w = deref(self.where)
	local dsc = self.dsc
	
	if need then
		h:write(stead.string.format("%s  = vobj(%s,%s,%s);\n",
			name, 
			stead.tostring(self.nam), 
			stead.tostring(dsc), 
			stead.tostring(w)));
	end
	savemembers(h, self, name, false);
end

function vobj_act(self, ...)
	local o, r = here():srch(self); -- self.nam
	if ref(o) and ref(o).where then
		return goto(ref(o).where);
	end
	return call(ref(r),'act', self.nam, unpack(arg));
end

function vobj_used(self, ...)
	local o, r = here():srch(self.nam);
	return call(ref(r),'used', self.nam, unpack(arg));
end

function vobj_use(self, ...)
	local o, r = here():srch(self.nam);
	return call(ref(r),'use', self.nam, unpack(arg));
end

function vobj(name, dsc, w)
	return obj{ nam = tostring(name), 
		vobject_type = true,
		dsc = dsc, 
		where = deref(w), 
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
		where = deref(w), 
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
		local o = ref(v);
		if not o then -- isObject(o) then -- compat
			error ("No object: "..tostring(v))
			return false
		end
		if isObject(deref(v)) then-- no named object!
			local n = stead.string.format("%s[%d]", name, ii);
			v = allocator:new(n);
			self[ii] = v;
			v.auto_allocated = true;
			for_each(v, n, check_list, isList, deref(v));
		else
			self[ii] = o;
		end
	end
	return true; 
end

function list_add(self, name, pos)
	local nam
	nam = ref(name);
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
	nam = ref(name);
	self.__modified__ = true;
	self[i] = nam; -- for spare lists
	return true
end

function list_concat(self, other, pos)
	local n,o,ii
	for n,o,ii in opairs(other) do
		o = ref(o);
		if pos == nil then
			self:add(o);
		else 
			self:add(o, pos);
			pos = pos + 1;
		end
	end
end

function list_str(self)
	local i, v, vv, o;
	for i,o in opairs(self) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) then
			vv = nil
			if game.gui then
				vv = call(o, 'disp');
			end
			if type(vv) ~= 'string' then
				vv = call(o, 'nam');
			end
			vv = xref(vv, o);
			v = stead.par(',', v, vv);
		end
	end
	return v;
end

function list_save(self, name, h, need)
	if self.__modified__ then
		local i,v
		for i,v in ipairs(self) do -- dump auto objs
			if isObject(v) and v.auto_allocated then
				v:save(v.key_name, h, true, true);
			end
		end
		h:write(name.." = list({});\n");
		need = true;
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
		if isObject(o) and not isDisabled(o) then
			vv = nil
			if game.gui then
				vv = call(o, 'disp');
			end
			if type(vv) ~= 'string' then
				vv = call(o, 'nam');
			end
			vv = xref(vv, o);
			v = stead.par(',', v, vv, obj_str(o));
		end
	end
	return v;
end

function path(w, wh) -- search in way, disabled too
	if not wh then
		wh = here();
	else
		wh = ref(wh);
	end
	local o = ways(wh):srch(w, true);
	o = ref(o);
	if isRoom(o) then
		return o
	end
	return nil
end

game.lifes = list(game.lifes)
stead:init(); -- reinit ob

-- vim:ts=4
