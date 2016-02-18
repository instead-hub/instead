local function onevent(ev, ...)
	local vv, r
	if stead.api_atleast(1, 6, 3) then
		vv, r = stead.call(game, ev, ...);
		if r == false then
			return vv, false
		end
		if vv == false then
			return nil, false
		end
		return vv
	end
end

local go = function (self, where, back, noenter, noexit, nodsc)
	local was = self.where;
	local need_scene = false;
	local ret

	if not stead.in_walk_call then
		ret = function(rc) stead.in_walk_call = false return nil end
	else
		ret = function(rc) return rc end
	end

	stead.in_walk_call = true

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
		error ("Do not use walk from left/entered action! Use exit/enter action instead:" .. self.where);
	end

	local v, r, jump;

	if not isVroom(stead.ref(where)) and not stead.in_exit_call and not noexit then
		stead.in_exit_call = true -- to break recurse
		v,r = stead.call(stead.ref(self.where), 'exit', stead.ref(where));
		stead.in_exit_call = nil
		if r == false or (stead.api_atleast(1, 3, 0) and v == false and r == nil) then
			return v, ret(r)
		end
		if stead.api_atleast(2, 4, 0) then
			jump = PLAYER_MOVED
		else
			jump = (self.where ~= was)
		end
		if jump then
			where = stead.deref(self.where) -- jump
		end
	end

	local res = v;
	v = nil;

	if not isVroom(stead.ref(where)) then
		self.where = stead.deref(where);
	end

	if not jump and not noenter then
		v, r = stead.call(stead.ref(where), 'enter', stead.ref(was));
		if r == false or (stead.api_atleast(1, 3, 0) and v == false and r == nil) then
			self.where = was;
			return stead.par(stead.scene_delim, res, v), ret(r)
		end
	end
	
	if stead.api_atleast(2, 4, 0) then
		need_scene = not PLAYER_MOVED;
	else
		need_scene = not (stead.ref(where) ~= stead.ref(self.where))
	end

	res = stead.par(stead.scene_delim, res, v);

	if not back then
		stead.ref(where).__from__ = stead.deref(was);
	end

	ret()

	PLAYER_MOVED = true
	if need_scene and not nodsc then
		NEED_SCENE = true
	end

	if not stead.in_walk_call  then
		local to = self.where
		if not noexit then
			self.where = was
			stead.in_onexit_call = true
			v = stead.call(stead.ref(was), 'left', stead.ref(to));
			stead.in_onexit_call = false
			res = stead.par(stead.scene_delim, res, v);
		end

		self.where = stead.deref(to)

		if not noenter then
			stead.in_entered_call = true
			v = stead.call(stead.ref(to), 'entered', stead.ref(was));
			stead.in_entered_call = false
			res = stead.par(stead.scene_delim, res, v);
		end

		if stead.tonum(stead.ref(to).__visited) then
			stead.ref(to).__visited = stead.ref(to).__visited + 1;
		elseif stead.here().__visited == nil then
			stead.ref(to).__visited = 1
		end
		if not stead.api_atleast(1, 6, 3) and isDialog(stead.ref(to)) then
			stead.dialog_rescan(stead.ref(to))
		end
	end
	return res;
end

stead.player_go = function(self, where) -- cmd iface
	local w = stead.ref(self.where).way:srch(where);
	if not w then
		return nil,false
	end
	return self:walk(w);
end

stead.player_walk = function(self, where, ...) -- real work
	local v, r, vv;
	vv, r = onevent('onwalk', stead.ref(where), ...);
	if vv == false then
		return
	end
	if r == false then 
		return vv 
	end
	v, r = stead.go(self, where, ...);
	if stead.type(vv) == 'string' then
		v = stead.par(stead.space_delim, vv, v);
	end
	return v, r;
end

stead.player_back = function(self) -- deprecated
	error ("Do not use stead.me():back(). It's deprecated.", 2)
end

stead.back = function(w)
	if isDialog(stead.here()) and not isDialog(stead.from()) then
		return stead.walkout(w);
	end
	return stead.walkback(w);
end
back = stead.back

stead.walkback = function(w)
	if isRoom(stead.ref(w)) then
		return stead.me():walk(w, true);
	end
	return stead.me():walk(stead.from(), true);
end
walkback = stead.walkback

stead.walk = function(what, back, noenter, noexit, nodsc, ...)
	return stead.me():walk(what, back, noenter, noexit, nodsc, ...);
end
walk = stead.walk

stead.walkin = function(what)
	return stead.me():walk(what, false, false, true);
end
walkin = stead.walkin

stead.walkout = function(what)
	if isRoom(stead.ref(what)) then
		return stead.me():walk(what, true, true, false, true);
	end
	return stead.me():walk(stead.from(), true, true, false, true);
end
walkout = stead.walkout

function visited(w)
	if not w then w = stead.here() end
	w = stead.ref(w)
	if w == nil then
		return nil;
	end
	if not isRoom(w) then
		error ("Wrong parameter to visited.", 2);
	end
	return w.__visited
end
stead.visited = visited

function visits(w)
	local n = stead.visited(w)
	if not n then n = 0 end
	return n
end
stead.visits = visits

iface.fmt = function(self, cmd, st, moved, r, av, objs, pv) -- st -- changed state (main win), move -- loc changed
	local l, vv
	if st then
		av = txtem(av);
		pv = txtem(pv);
--		if not PLAYER_MOVED then
			r = txtem(r)
--		end
		if isForcedsc(stead.here()) or NEED_SCENE then
			l = stead.here():scene();
		end
	end
	if moved then
		vv = stead.fmt(stead.cat(stead.par(stead.scene_delim, r, l, av, objs, pv), '^'));
	else
		vv = stead.fmt(stead.cat(stead.par(stead.scene_delim, l, r, av, objs, pv), '^'));
	end
	return vv
end

stead.go = function(...)
	local r,v = go(...)
	if stead.type(r) == 'string' and stead.cctx() then 
		stead.pr (r)
	end
--	if stead.in_life_call then
--		ACTION_TEXT = nil
--	end
	if r == nil and v == nil then
		if stead.cctx() then
			stead.cctx().action = true
		else
			r = true
		end
	end
	return r,v
end

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
			return stead.walk(stead.here(), false, false, true);
		end
		NEED_SCENE = true
		if stead.api_atleast(1, 3, 5) then
			return true -- force action
		end
	end
	return v
end)

pl = player(pl) -- reinit
-- vim:ts=4
