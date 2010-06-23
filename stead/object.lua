obj = stead.inherit(obj, function(v)
	if v.use then
		v.use = stead.hook(v.use, function(f, s, on, ...)
			return f(s, ref(on), unpack(arg))
		end)
	end
	if v.used then
		v.used = stead.hook(v.used, function(f, s, by, ...)
			return f(s, ref(by), unpack(arg))
		end)
	end
	return v
end)

room = stead.inherit(room, function(v)
	if v.enter then
		v.enter = stead.hook(v.enter, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.entered then
		v.entered = stead.hook(v.entered, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.exit then
		v.exit = stead.hook(v.exit, function(f, s, to, ...)
			return f(s, ref(to), unpack(arg))
		end)
	end
	if v.left then
		v.left = stead.hook(v.left, function(f, s, to, ...)
			return f(s, ref(to), unpack(arg))
		end)
	end
	return v
end)

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
