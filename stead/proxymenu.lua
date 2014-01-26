stead.menu_prefix = '   '

local mpar = function(v, vv, rc) 		
	if stead.type(v) == 'string' or stead.type(vv) == 'string' then
		return stead.par(stead.space_delim, v, vv);
	elseif v == true or vv == true then
		return true
	end
	return rc
end

local call = function(o, m, ...)
	local rc = nil
	local v, r = stead.call(o, m, ...); 
	if r == false or v == false then
		rc = false
	elseif r or v then 
		rc = true 
	end
	return v, r, rc
end

stead.obj_proxy = function(o, act, use_mode, used_act, useit_act)
	local v = {};
	v.proxy_type = true;

	local d = stead.dispof(o);

	if stead.type(d) == 'string' then
		v.nam = stead.menu_prefix..d;
	end

	if inv():srch(o) then
		v.nam = txtem(v.nam);
	end

	if not v.nam then
		v.nam = true
	end

	v.pobj = o;
	v.pact = act;
	v.use_mode = use_mode;
	v.used_act = used_act;
	v.useit_act = useit_act;

	v.save = function(self, name, h, need)
		if need then
			h:write(stead.string.format(name.." = stead.obj_proxy(%s, %s, %s, %s, %s);\n", 
				stead.tostring(self.pobj), 
				stead.tostring(self.pact),  
				stead.tostring(self.use_mode),
				stead.tostring(self.used_act),
				stead.tostring(self.useit_act)));
		end
		stead.savemembers(h, self, name, false);
	end

	if use_mode then
		v.use = function(s, w)
			if w.proxy_type then
				local v, r, vv, rr, rc = false, ri
				local act = s.pact
				v, r, ri = call(game, 'before_'..act, s.pobj, w.pobj);
				rc = ri or rc
				if ri == false then 
					return v, false 
				end
				vv, r, ri = call(s.pobj, act, w.pobj);
				rc = ri or rc
				v = mpar(v, vv, rc);

				if ri == false then
					return v, false
				end

				if stead.type(s.used_act) == 'string' 
					and ri == nil then -- used only if use did nothing
					vv, r, ri = call(w.pobj, s.used_act, s.pobj);
					rc = ri or rc
					v = mpar(v, vv, rc);

					if ri == false then
						return v, false
					end
				end

				if ri then
					vv, rr, ri = call(game, 'after_'..act, s.pobj, w.pobj);
					rc = rc or ri
					v = mpar(v, vv, rc);

					if ri == false then
						return v, false
					end
				end

				if v == nil then
					v = stead.call(game, act, s.pobj, w.pobj);
				end

				return v, false;
			end
		end
	end

	v.inv = function(s)
		local v, r, vv, rr, rc = false, ri
		local act = s.pact
		if s.use_mode then
			act = s.useit_act
			if stead.type(act) ~= 'string' then
				return nil
			end
		end

		v, r, ri = call(game, 'before_'..act, s.pobj); 
		rc = rc or ri
		if ri == false then
			return v
		end

		vv, r, ri = call(s.pobj, act); 
		rc = rc or ri
		v = mpar(v, vv, rc)

		if ri == false then
			return v
		end

		if ri then
			vv, rr, ri = call(game, 'after_'..act, s.pobj); 
			rc = rc or ri
			v = mpar(v, vv, rc);
		end
		if v == nil then
			v = stead.call(game, act, s.pobj);
		end

		return v, rc;
	end

	if use_mode then
		return obj(v)
	end
	return menu(v)
end

stead.proxy_fill_objs = function(s, w, act, use_mode, used_act, useit_act)
	local ii,i,o
	local rc = false
	for i,o,ii in stead.opairs(w) do
		o = stead.ref(o);
		if isObject(o) and not isDisabled(o) and o ~= s and not isPhrase(o) 
			and not o.proxy_type and not isStatus(o) then

			s.obj:add(stead.obj_proxy(o, act, use_mode, used_act, useit_act));
			if not isRoom(o) then
				stead.proxy_fill_objs(s, o.obj, act, use_mode, used_act, useit_act);
			end
			rc = true
		end
	end
	return rc
end 

local select_only = function(s)
	local k, o, i
	for k,o in stead.opairs(stead.me().obj) do
		o = stead.ref(o)
		if o.action_type and o._state and o ~= s then
			o:inv();
		end
	end
	stead.obj_tag(stead.me(), MENU_TAG_ID);
end


local proxy_menu = function(nam, act, _scene, _inv, _way, use_mode, used_act, useit_act, _ifhave)
	local v = { };
	if stead.type(act) ~= 'string' then
		error("Wrong parameter to proxy_menu.", 3)
	end
	if null[act] then
		error(stead.tostring(act).."is a reserved handler. Do not use it.", 3)
	end

	if used_act and null[used_act] then
		error(stead.tostring(used_act).."is a reserved handler. Do not use it.", 3)
	end

	if useit_act and null[useit_act] then
		error(stead.tostring(useit_act).."is a reserved handler. Do not use it.", 3)
	end

	v.action_type = true;
	v._state = false;
	v.nam = nam;
	v.disp = function(s)
		local n = stead.call(s, 'nam')
		if s._state then
			return txtu(txtnb(n));
		end
		return txtnb(n);
	end

	v.fill_scene = _scene;
	v.fill_inv = _inv;
	v.fill_ifhave = _ifhave;
	v.fill_way = _way;

	v.gen = function(s)
		local k,o,i
		local rc = false
		s.obj:zap();
		if s.fill_inv then
			rc = stead.proxy_fill_objs(s, inv(), act, use_mode, used_act, useit_act);
		end
		if not s.fill_ifhave or rc then
			if s.fill_scene then
				stead.proxy_fill_objs(s, stead.here().obj, act, use_mode, used_act, useit_act);
			end
		end
		if s.fill_way then
			stead.proxy_fill_objs(s, stead.here().way, act, use_mode, used_act, useit_act);
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
	for k, o in stead.opairs(stead.me().obj) do
		o = stead.ref(o)
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

obj_menu = function(nam, act, _scene, _inv, _way)
	return proxy_menu(nam, act, _scene, _inv, _way)
end

use_menu = function(nam, act, used_act, useit_act, _scene, _inv, _ifhave)
	return proxy_menu(nam, act, _scene, _inv, false, true, used_act, useit_act, _ifhave)
end

inv = function(s)
    return stead.me().inventory;
end

game.onuse = function(s, v, w) -- do not let use on non proxy obj
	if not v.proxy_type and not w.proxy_type then
		return
	end
	if not v.proxy_type or not w.proxy_type then
		return false
	end
end

player = stead.inherit(player, function(v)
	v.inv = function(s)
		gen_actions(s);
		return stead.player_inv(s);
	end
	v.inventory = list {}
	return v
end)

pl = player(pl) -- reinit
-- vim:ts=4
