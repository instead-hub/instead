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

pl.use = player_use;

function vobj_save(self, name, h, need)
	local w
	w = deref(self.where)
	if w == nil then
		w = "nil"
	else
		w = stead.string.format("%q", w)
	end
	if need then
		h:write(stead.string.format("%s  = vobj(%q,%q,%s);\n",
			name, tostring(self.nam), tostring(self.dsc), w));
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
