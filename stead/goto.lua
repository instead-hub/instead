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

	if not isRoom(stead.ref(where)) then
		error ("Trying to go nowhere: "..where);
	end

	if not isRoom(stead.ref(self.where)) then
		error ("Trying to go from nowhere: "..self.where);
	end

	if stead.in_entered_call or stead.in_onexit_call then
		error ("Do not use goto from left/entered action! Use exit/enter action instead:" .. self.where);
	end

	local v, r, jump;

	if not isVroom(stead.ref(where)) and not stead.in_exit_call and not noexit then
		stead.in_exit_call = true -- to break recurse
		v,r = stead.call(stead.ref(self.where), 'exit', stead.ref(where));
		stead.in_exit_call = nil
		if r == false or (stead.api_version >= "1.3.0" and v == false and r == nil) then
			return v, ret(r)
		end
		if self.where ~= was then
			where = stead.deref(self.where) -- jump
			jump = true
		end
	end

	local res = v;
	v = nil;

	if not isVroom(stead.ref(where)) then
		self.where = stead.deref(where);
	end

	if not jump and not noenter then
		v, r = stead.call(stead.ref(where), 'enter', stead.ref(was));
		if r == false or (stead.api_version >= "1.3.0" and v == false and r == nil) then
			self.where = was;
			return par(stead.scene_delim, res, v), ret(r)
		end
	end
	
	need_scene = true;
	if stead.ref(where) ~= stead.ref(self.where) then -- jump !!!
		need_scene = false;
	end

	res = par(stead.scene_delim, res, v);

	if not back then
		stead.ref(where).__from__ = stead.deref(was);
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
			v = stead.call(stead.ref(was), 'left', stead.ref(to));
			stead.in_onexit_call = false
			res = par(stead.scene_delim, res, v);
		end

		self.where = stead.deref(to)

		if not noenter then
			stead.in_entered_call = true
			v = stead.call(stead.ref(to), 'entered', stead.ref(was));
			stead.in_entered_call = false
			res = par(stead.scene_delim, res, v);
		end

		if tonumber(stead.ref(to).__visited) then
			stead.ref(to).__visited = stead.ref(to).__visited + 1;
		elseif here().__visited == nil then
			stead.ref(to).__visited = 1
		end

		if isDialog(stead.ref(to)) then
			dialog_rescan(stead.ref(to));
		end
	end
	return res;
end
stead.go = go

function player_go(self, where) -- cmd iface
	local w = stead.ref(self.where).way:srch(where);
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
	if isRoom(stead.ref(what)) then
		return me():goto(what, true, true, false, true);
	end
	return me():goto(from(), true, true, false, true);
end
stead.goout = goout

function visited(w)
	if not w then w = here() end
	w = stead.ref(w)
	if w == nil then
		return nil;
	end
	if not isRoom(w) then
		error ("Wrong parameter to visited.", 2);
	end
	return w.__visited
end

function visits(w)
	local n = visited(w)
	if not n then n = 0 end
	return n
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
	if type(r) == 'string' and stead.cctx() then 
		pr (r)
	end
	if stead.in_life_call then
		ACTION_TEXT = nil
	end
	if r == nil and v == nil then
		if stead.cctx() then
			stead.cctx().action = true
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
		if not stead.started then
			game:start()
			stead.started = true
		end
		if game._time == 0 then
			return stead.goto(here(), false, false, true);
		end
		NEED_SCENE = true
		if stead.api_version >= "1.3.5" then
			return true -- force action
		end
	end
	return v
end)

pl = player(pl) -- reinit

-- vim:ts=4
