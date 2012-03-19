stead.menu_prefix = '   '

stead.obj_proxy = function(o, act, use_mode, used_act, useit_act)
	local v = {};
	v.proxy_type = true;
	v.nam = stead.menu_prefix..stead.nameof(o);
	if inv():srch(o) then
		v.nam = txtem(v.nam);
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
				local v, r, vv, rr, rc = false
				local act = s.pact
				local useit_mode = false

				if s == w and type(s.useit_act) == 'string' then
					useit_mode = true
					act = s.useit_act
				end

				v, r = stead.call(game, 'before_'..act, s.pobj, w.pobj);
				if r == false or v == false then 
					return v, false 
				end
				if r == true or v == true then 
					rc = true 
				end

				if s.pobj[act] then
					vv, r = stead.call(s.pobj, act, w.pobj);
					v = stead.par(stead.space_delim, v, vv);
					if r == false or vv == false then
						return v, false
					end
					if vv == true or r == true then 
						rc = true 
					end
				end
				if not useit_mode and type(s.used_act) == 'string' 
					and w.pobj[s.used_act] and vv == nil then -- used only if use did nothing
					vv, r = stead.call(w.pobj, s.used_act, s.pobj);
					v = stead.par(stead.space_delim, v, vv);
					if r == false or vv == false then
						return v, false
					end
					if vv == true or r == true then 
						rc = true 
					end
				end
				
				vv, rr = stead.call(game, 'after_'..act, s.pobj, w.pobj);
				v = stead.par(stead.space_delim, v, vv);

				if rr == false or vv == false then return v, false end
				if vv == true or rr == true then rc = true end

				if not v and not rc then-- false or nil
					v = stead.call(game, act, s.pobj, w.pobj);
				end
				return v, false;
			end
		end
	end

	v.inv = function(s)
		local v, r, vv, rr, rc = false
		v, r = stead.call(game, 'before_'..act, s.pobj);
		if r == false or v == false then
			return v
		end
		if r == true or v == true then
			rc = true
		end
		if s.pobj[act] then
			vv, r = stead.call(s.pobj, act);
			v = stead.par(stead.space_delim, v, vv);
			if r == false or vv == false then
				return v
			end
			if r == true or vv == true then
				rc = true
			end
			vv, rr = stead.call(game, 'after_'..act, s.pobj);
			v = stead.par(stead.space_delim, v, vv);
			if vv == true or rr == true then
				rc = true
			end
		end
		if not v and not rc then -- false or nil
			v = stead.call(game, act, s.pobj);
		end
		return v;
	end

	if use_mode then
		return obj(v)
	end
	return menu(v)
end

fill_objs = function(s, w, act, use_mode, used_act, useit_act)
	local ii,i,o
	local rc = false
	for i,o,ii in opairs(w) do
		o = ref(o);
		if isObject(o) and not isDisabled(o) and o ~= s and not isPhrase(o) 
			and not o.proxy_type and not isStatus(o) then

			s.obj:add(stead.obj_proxy(o, act, use_mode, used_act, useit_act));

			fill_objs(s, o.obj, act, use_mode, used_act, useit_act);
			rc = true
		end
	end
	return rc
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


local proxy_menu = function(nam, act, _scene, _inv, _way, use_mode, used_act, useit_act, _ifhave)
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
			rc = fill_objs(s, inv(), act, use_mode, used_act, useit_act);
		end
		if not _ifhave or rc then
			if s._scene then
				fill_objs(s, here().obj, act, use_mode, used_act, useit_act);
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

obj_menu = function(nam, act, _scene, _inv, _way)
	return proxy_menu(nam, act, _scene, _inv, _way)
end

use_menu = function(nam, act, used_act, useit_act, _scene, _inv, _ifhave)
	return proxy_menu(nam, act, _scene, _inv, false, true, used_act, useit_act, _ifhave)
end

inv = function(s)
    return me().inventory;
end

player  = stead.inherit(player, function(v)
	v.inv = function(s)
		gen_actions(s);
		return player_inv(s);
	end
	v.inventory = list {}
	return v
end)

pl = player(pl) -- reinit
-- vim:ts=4
