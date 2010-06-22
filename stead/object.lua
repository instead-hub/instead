obj = inherit(obj, function(v)
	if v.use then
		v.use = hook(v.use, function(f, s, on, ...)
			return f(s, ref(on), unpack(arg))
		end)
	end
	if v.used then
		v.used = hook(v.used, function(f, s, by, ...)
			return f(s, ref(by), unpack(arg))
		end)
	end
	return v
end)

room = inherit(room, function(v)
	if v.enter then
		v.enter = hook(v.enter, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.entered then
		v.entered = hook(v.entered, function(f, s, from, ...)
			return f(s, ref(from), unpack(arg))
		end)
	end
	if v.exit then
		v.exit = hook(v.exit, function(f, s, to, ...)
			return f(s, ref(to), unpack(arg))
		end)
	end
	if v.left then
		v.left = hook(v.left, function(f, s, to, ...)
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
		w = "\""..w:format("%q").."\""
	end
	if need then
		h:write(name.." = vobj(\""..tostring(self.nam):format("%q").."\",\""..tostring(self.dsc):format("%q").."\","..w..");\n");
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

function vobj(name, dsc, w)
	return obj{ nam = tostring(name), 
		dsc = dsc, 
		where = deref(w), 
		act = vobj_act, 
		used = vobj_used, 
		save = vobj_save };
end

function vway(name, dsc, w)
--	o.object_type = true;
	return  obj{ nam = tostring(name), 
		dsc = dsc, 
		act = vobj_act, 
		where = deref(w), 
		used = vobj_used, 
		save = vobj_save };
end
