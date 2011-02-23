go = function (self, where, back, noenter, noexit, nodsc)
	local was = self.where;
	local need_scene = false;
	local ret

	if not stead.in_goto_call then
		ret = function(rc) stead.in_goto_call = false return nil end
	else
		ret = function(rc) return rc end
	end

	stead.in_goto_call = true

	if where == nil then
		return nil, ret(false)
	end

	if not isRoom(ref(where)) then
		error ("Trying to go nowhere: "..where);
	end

	if not isRoom(ref(self.where)) then
		error ("Trying to go from nowhere: "..self.where);
	end

	if stead.in_entered_call or stead.in_onexit_call then
		error ("Do not use goto from left/entered action! Use exit/enter action instead:" .. self.where);
	end

	local v, r, jump;

	if not isVroom(ref(where)) and not stead.in_exit_call and not noexit then
		stead.in_exit_call = true -- to break recurse
		v,r = call(ref(self.where), 'exit', ref(where));
		stead.in_exit_call = nil
		if r == false or (stead.api_version >= "1.3.0" and v == false and r == nil) then
			return v, ret(r)
		end
		if self.where ~= was then
			where = deref(self.where) -- jump
			jump = true
		end
	end

	local res = v;
	v = nil;

	if not isVroom(ref(where)) then
		self.where = deref(where);
	end

	if not jump and not noenter then
		v, r = call(ref(where), 'enter', ref(was));
		if r == false or (stead.api_version >= "1.3.0" and v == false and r == nil) then
			self.where = was;
			return par('^^', res, v), ret(r)
		end
	end
	
	need_scene = true;
	if ref(where) ~= ref(self.where) then -- jump !!!
		need_scene = false;
	end

	res = par('^^',res,v);

	if not back then
		ref(where).__from__ = deref(was);
	end

	ret()

	PLAYER_MOVED = true
	if need_scene and not nodsc then
		NEED_SCENE = true
	end

	if not stead.in_goto_call  then
		local to = self.where
		if not noexit then
			self.where = was
			stead.in_onexit_call = true
			v = call(ref(was), 'left', ref(to));
			stead.in_onexit_call = false
			res = par('^^',res,v);
		end

		self.where = deref(to)

		if not noenter then
			stead.in_entered_call = true
			v = call(ref(to), 'entered', ref(was));
			stead.in_entered_call = false
			res = par('^^',res,v);
		end

		if tonumber(ref(to).__visited) then
			ref(to).__visited = ref(to).__visited + 1;
		elseif here().__visited == nil then
			ref(to).__visited = 1
		end

		if isDialog(ref(to)) then
			dialog_rescan(ref(to));
		end
	end
	return res;
end
stead.go = go

function player_go(self, where) -- cmd iface
	local w = ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	return self:goto(w);
end

function player_goto(self, where, ...) -- real work
	local v, r = stead.go(self, where, ...);
	return v, r;
end

function player_back(self) -- deprecated
	error ("Do not use me():back(). It's deprecated.", 2)
end

function back()
	if isDialog(here()) and not isDialog(from()) then
		return stead.goout();
	end
	return stead.goback();
end
stead.back = back

function goback()
	return me():goto(from(), true);
end
stead.goback = goback

function goto(what, back, noenter, noexit, nodsc, ...)
	return me():goto(what, back, noenter, noexit, nodsc, ...);
end
stead.goto = goto

function goin(what)
	return me():goto(what, false, false, true);
end
stead.goin = goin

function goout(what)
	if isRoom(ref(what)) then
		return me():goto(what, true, true, false, true);
	end
	return me():goto(from(), true, true, false, true);
end
stead.goout = goout

function visited(w)
	if not w then w = here() end
	w = ref(w)
	if w == nil then
		return nil;
	end
	if not isRoom(w) then
		error ("Wrong parameter to visited.", 2);
	end
	return w.__visited
end

iface.fmt = function(self, cmd, st, moved, r, av, objs, pv) -- st -- changed state (main win), move -- loc changed
	local l
	if st then
		av = txtem(av);
		pv = txtem(pv);
--		if not PLAYER_MOVED then
			r = txtem(r)
--		end
		if isForcedsc(here()) or NEED_SCENE then
			l = here():scene();
		end
	end
	if moved then
		vv = stead.fmt(stead.cat(stead.par(stead.scene_delim, r, av, l, objs, pv), '^'));
	else
		vv = stead.fmt(stead.cat(stead.par(stead.scene_delim, l, r, av, objs, pv), '^'));
	end
	return vv
end

stead.go = stead.hook(stead.go, function(f, ...)
	local r,v = f(...)
	if type(r) == 'string' and cctx() then 
		pr (r)
	end
	if stead.in_life_call then
		ACTION_TEXT = nil
	end
	if r == nil and v == nil then
		if cctx() then
			cctx().action = true
		else
			r = true
		end
	end
	return r,v
end)

iface.cmd = stead.hook(iface.cmd, function(f, ...)
	NEED_SCENE = nil
	return f(...)
end)

player  = stead.inherit(player, function(v)
	v.look = function(s)
		if game._time == 0 then
			return stead.goto(here(), false, false, true);
		end
		NEED_SCENE = true
	end
	return v
end)

pl = player(pl) -- reinit

-- vim:ts=4
