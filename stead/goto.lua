go = function (self, where, back, forceenter)
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

	if not isVroom(ref(where)) and not stead.in_exit_call then
		stead.in_exit_call = true -- to break recurse
		v,r = call(ref(self.where), 'exit', ref(where));
		stead.in_exit_call = nil
		if r == false then
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

	if not jump and (not back or forceenter or 
		not isDialog(ref(was)) or isDialog(ref(where))) then
		v, r = call(ref(where), 'enter', ref(was));
		if r == false then
			self.where = was;
			return par('^^', res, v), ret(r)
		end

		need_scene = true;
		if ref(where) ~= ref(self.where) then -- jump !!!
			need_scene = false;
		end
	end

	res = par('^^',res,v);

	if not back then
		ref(where).__from__ = deref(was);
	end

	ret()

	if not stead.in_goto_call  then
		local to = self.where
		self.where = was

		stead.in_onexit_call = true
		v = call(ref(was), 'left', ref(to));
		stead.in_onexit_call = false
		res = par('^^',res,v);

		self.where = deref(to)

		stead.in_entered_call = true
		v = call(ref(to), 'entered', ref(was));
		stead.in_entered_call = false
		res = par('^^',res,v);
		if tonumber(ref(to).__visited) then
			ref(to).__visited = ref(to).__visited + 1;
		elseif here().__visited == nil then
			ref(to).__visited = 1
		end
		if isDialog(ref(to)) then
			dialog_rescan(ref(to));
		end
	end
	PLAYER_MOVED = true
	if need_scene then -- or isForcedsc(ref(where)) then -- i'am not sure...
		NEED_SCENE = true
--		return par('^^',res,ref(where):scene());
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
	local v, r = stead.go(self, where, unpack(arg));
	return v, r;
end

function player_back(self) -- deprecated
	error ("Do not use me():back(). It's deprecated.", 2)
end

function back()
	return me():goto(from(), true);
end

function goback()
	return me():goto(from(), true, true);
end

game.ini = stead.hook(game.ini,function(f, ...)
	if isRoom(here()) then
		here().__visited = 1
	end
	return f(unpack(arg))
end)

function visited(w)
	if not w then w = here() end
	w = ref(w)
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
		if not PLAYER_MOVED then
			r = txtem(r)
		end
		if isForcedsc(here()) or NEED_SCENE then
			l = here():scene();
		end
	end
	if moved then
		vv = stead.fmt(stead.cat(stead.par("^^", r, av, l, objs, pv), '^'));
	else
		vv = stead.fmt(stead.cat(stead.par("^^", l, r, av, objs, pv), '^'));
	end
	return vv
end

stead.go = stead.hook(stead.go, function(f, ...)
	local r,v = f(unpack(arg))
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
	return f(unpack(arg))
end)

player  = stead.inherit(player, function(v)
	v.look = function(s)
		NEED_SCENE = true
	end
	return v
end)

pl = player(pl) -- reinit

-- vim:ts=4
