use_proxy = function(o)
	local v = {};
	v.proxy_type = true;
	v.nam = '   '..call(ref(o), 'nam');
	if inv():srch(ref(o)) then
		v.nam = txtem(v.nam);
	end
	v.pobj = deref(o);
	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format(name.." = use_proxy(%q);\n", tostring(self.pobj)));
		end
		stead.savemembers(h, self, name,false);
	end

	if ref(o).use ~= nil then
		v.use = function(s, w)
			if ref(w).proxy_type then
				local v,r = call(ref(s.pobj), 'use', ref(w.pobj));
--				where(s):gen();
				return v,r;
			end
		end
		v.inv = function(s)
			local v,r = call(ref(s.pobj), 'use', nil);
--			where(s):gen();
			return v,r;
		end
	end

	if ref(o).used ~= nil then
		v.used = function(s, w)
			if ref(w).proxy_type then
				local v,r = call(ref(s.pobj), 'used', ref(w.pobj));
--				where(s):gen();
				return v,r;
			end
		end
	end
	return obj(v)
end

act_proxy = function(o, act)
	local v = {};
	v.proxy_type = true;
	v.nam = '   '..call(ref(o), 'nam');
	v.pobj = deref(o);
	v.pact = act;
	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format(name.." = act_proxy(%q, %q);\n", self.pobj, self.pact));
		end
		stead.savemembers(h, self, name,false);
	end

	if ref(o)[act] ~= nil then
		v.inv = function(s)
			local v, r;
			v,r = call(ref(s.pobj), act);
--			where(s):gen();
			return v,r;
		end
	end
	return menu(v)
end

fill_objs = function(s, w, act)
	local ii,i, o
	
	for i,o,ii in opairs(objs(w)) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) and o ~= s and not isPhrase(o) then
			local n = deref(o)
			if type(n) ~= 'string' then
				n = deref(w)..".obj["..tonumber(ii).."]";
			end
			if act == "use" then
				put(use_proxy(n), s);
			else
				put(act_proxy(n, act), s);
			end
			fill_objs(s, o, act);
		end
	end
end 

fill_inv = function(s, w, act)
	local i, o
	local rc = false
	for i,o in opairs(w) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) 
			and not o.proxy_type 
			and not isStatus(o) 
			and s ~= o and not o.action_type then
			local n = deref(o)
			if type(n) ~= 'string' then
				n = deref(w)..".obj["..tonumber(ii).."]";
			end

			if act == "use" then
				put(use_proxy(n), s);
			else
				put(act_proxy(n, act), s);
			end
			fill_inv(s, o.obj, act);
			rc = true
		end
	end
	return rc
end 

select_only = function(s)
	local k, o, i
	for k,o in opairs(me().obj) do
		o = ref(o)
		if o.action_type and o._state and o ~= s then
			o:inv();
		end
	end
	obj_tag(me(), MENU_TAG_ID);
end

actmenu = function(nam, act, _scene, _inv, _ifinvonly)
	local v = { };
	v.action_type = true;
	v._state = false;
	v._nam = nam;
	v.nam = function(s)
		if s._state then
			return txtu(s._nam);
		end
		return s._nam;
	end
	v._scene = _scene;
	v._inv = _inv;
	v.gen = function(s)
		local k,o,i
		local rc = false
		s.obj:zap();
		if s._inv then
			rc =fill_inv(s, inv(), act);
		end
		if not _ifinvonly or rc then
			if s._scene then
				fill_objs(s, here(), act);
			end
		end
		select_only(s);
	end
	v.inv = function(s)
		local i,o
		local k,v
		if not s._state then
			s:gen();
			s._state = true;
		else
			s._state = false;
			s.obj:zap();
		end
		return nil, true -- to say instead, do not redraw scene, only inv ;)
	end
	return menu(v);
end

function gen_actions(s)
	local k, o
	for k,o in opairs(me().obj) do
		o = ref(o)
		if o.action_type and o._state then
			o:gen();
		end
	end
end

pocket = function(nam) 
	local v = {}
	v.action_type = true;
	v._state = false;
	v._nam = nam;
	v.nam = function(s)
		if s._state then
			return txtu(s._nam);
		end
		return s._nam;
	end;
	v.gen = function(s)
		s.obj:zap();
		fill_inv(s, inv(), 'act');
--		put(act_proxy(o, 'inv'), s);
--		s.obj:cat(s.robj);
		select_only(s);
	end;
	v.inv = function(s)
		if not s._state then
			s:gen();
			s._state = true;
		else
			s.obj:zap();
			s._state = false;
		end
		return nil,true
	end;
	v.robj = list {};
	return menu(v);
end

menu_init = function(s)
	pl.inv = function(s)
		gen_actions(s);
		return player_inv(s);
	end
end
