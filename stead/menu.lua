stead.menu_prefix = '   '
stead.obj_proxy = function(o, act, use_mode, reverse)
	local v = {};
	v.proxy_type = true;
	v.nam = stead.menu_prefix..stead.nameof(o);
	if inv():srch(o) then
		v.nam = txtem(v.nam);
	end

	v.pobj = o;
	v.pact = act;
	v.use_mode = use_mode;
	v.reverse = reverse

	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format(name.." = stead.obj_proxy(%s, %s, %s, %s);\n", 
				stead.tostring(self.pobj), 
				stead.tostring(self.pact),  
				stead.tostring(self.use_mode),
				stead.tostring(self.reverse)));
		end
		stead.savemembers(h, self, name, false);
	end

	if use_mode then
		local f = function(s, w)
			if w.proxy_type then
				local v, r, vv, rr
				v, r = stead.call(game, 'before_'..act, s.pobj);
				if r == false or v == false then
					return v
				end
				vv, r = stead.call(s.pobj, act, w.pobj);
				v = stead.par(stead.space_delim, v, vv);
				if r ~= false and v ~= false then
					vv, rr = stead.call(game, 'after_'..act, s.pobj. w.pobj);
				end
				v = stead.par(stead.space_delim, v, vv);
				if not v then -- false or nil
					v = stead.call(game, act, s.pobj, w.pobj);
				end
				return v, false;
			end
		end
		if reverse then
			v.used = f
		else
			v.use = f
		end
	end

	v.inv = function(s)
		local v, r, vv, rr
		v, r = stead.call(game, 'before_'..act, s.pobj);
		if r == false or v == false then
			return v
		end
		vv, r = stead.call(s.pobj, act);
		v = stead.par(stead.space_delim, v, vv);
		if r ~= false and v ~= false then
			vv, rr = stead.call(game, 'after_'..act, s.pobj);
		end
		v = stead.par(stead.space_delim, v, vv);
		if not v then -- false or nil
			v = stead.call(game, act, s.pobj);
		end
		return v, r;
	end

	if use_mode then
		return obj(v)
	end
	return menu(v)
end

fill_objs = function(s, w, act, use_mode, reverse)
	local ii,i,o
	for i,o,ii in opairs(w) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) and o ~= s and not isPhrase(o) 
			and not o.proxy_type and not isStatus(o) then

			s.obj:add(stead.obj_proxy(o, act, use_mode, reverse));

			fill_objs(s, o.obj, act, use_mode, reverse);
		end
	end
end 

local select_only = function(s)
	local k, o, i
	for k,o in opairs(me().obj) do
		o = ref(o)
		if o.action_type and o._state and o ~= s then
			o:inv();
		end
	end
	obj_tag(me(), MENU_TAG_ID);
end

proxy_menu = function(nam, act, use_mode, reverse, _scene, _inv, _ifhave)
	local v = { };
	v.action_type = true;
	v._state = false;
	v.nam = nam;
	v.disp = function(s)
		if s._state then
			return txtu(txtnb(s.nam));
		end
		return txtnb(s.nam);
	end
	v._scene = _scene;
	v._inv = _inv;
	v.gen = function(s)
		local k,o,i
		local rc = false
		s.obj:zap();
		if s._inv then
			rc = fill_objs(s, inv(), act, use_mode, reverse);
		end
		if not _ifhave or rc then
			if s._scene then
				fill_objs(s, here().obj, act, use_mode, reverse);
			end
		end
		select_only(s);
	end
	v.inv = function(s)
		local i,o
		local k,v
		s._state = not s._state 
		if s._state then
			s:gen();
		else
			s.obj:zap();
		end
		return nil, true -- to say instead, do not redraw scene, only inv ;)
	end
	return menu(v);
end

local function gen_actions(s)
	local k, o
	for k, o in opairs(me().obj) do
		o = ref(o)
		if o.action_type and o._state then
			o:gen();
		end
	end
end

act_menu = function(nam, act)
	local v = { };
	v.action_type = true;
	v.nam = nam;
	v.gen = function(s)
	end
	v.inv = function(s)
		local v, r 
		v, r = stead.call(game, act);
		return v, r
	end
	return menu(v);
end

inventory = list { };

inv = function(s)
    return inventory;
end

player  = stead.inherit(player, function(v)
	v.inv = function(s)
		gen_actions(s);
		return player_inv(s);
	end
	return v
end)

pl = player(pl) -- reinit
-- vim:ts=4
