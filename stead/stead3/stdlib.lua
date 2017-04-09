local std = stead
local type = std.type
std.rawset(_G, 'std', stead)
include = std.include
loadmod = std.loadmod
rnd = std.rnd
rnd_seed = std.rnd_seed
p = std.p
pr = std.pr
pn = std.pn
pf = std.pf
obj = std.obj
stat = std.stat
room = std.room
menu = std.menu
dlg = std.dlg
me = std.me
here = std.here
from = std.from
new = std.new
delete = std.delete
-- nameof = std.nameof
-- dispof = std.dispof
-- titleof = std.titleof
gamefile = std.gamefile
player = std.player
dprint = std.dprint

local function _pfn(f1, f2, ...)
	local a = {...}
	if type(f2) == 'string' then
		return function()
			f1()
			std.p(f2, std.unpack(a))
		end
	end
	if type(f2) ~= 'function' then
		return f1()
	end
	return function(f3, ...)
		return _pfn(function()
			f1()
			f2(std.unpack(a))
		end, f3, ...)
	end
end

function pfn(f, ...)
	local a = {...}
	if type(f) == 'function' then
		return _pfn(function() end, f, ...)
	end
	return function()
		std.p(f, std.unpack(a))
	end
end

function from(ww)
	local wh
	ww = ww or std.here()
	wh = std.ref(ww)
	if not std.is_obj(wh, 'room') then
		std.err("Wrong argument to from(): "..std.tostr(wh), 2)
	end
	return wh:from()
end;

local function walkroom(w)
	if std.is_tag(w) then
		local ww = std.here().way:lookup(w)
		if not ww then
			std.err("Can not found tag: "..std.tostr(w), 3)
		end
		return ww
	end
	return w
end

function visits(w)
	if not w then return std.here():visits() end
	return std.object(walkroom(w)):visits()
end

function visited(w)
	if not w then return std.here():visited() end
	return std.object(walkroom(w)):visited()
end

function walk(w, ...)
	local r, v = std.me():walk(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

function life_walk(w, ...)
	game:reaction(false)
	game:events(false, false)
	return walk(w, ...)
end

function walkin(w, ...)
	local r, v = std.me():walkin(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

function walkout(w, ...)
	local r, v = std.me():walkout(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

std.walkout = walkout
std.walkin = walkin
std.walk = walk

function object(w)
	local o
	if std.is_tag(w) then
		o = std.here():lookup(w)
		if not o then
			o = std.me():lookup(w)
		end
		if not o then
			std.err("Wrong tag: "..w, 3)
		end
		return o
	end
	o = std.ref(w)
	if not o then
		std.err("Wrong object: "..std.tostr(w), 3)
	end
	return o
end
std.object = object
_ = std.object

for_all = std.for_all

function seen(w, ww)
	local wh
	ww = ww or std.here()
	if not std.is_obj(ww, 'list') then
		wh = std.ref(ww)
	else
		return ww:srch(w)
	end
	if not std.is_obj(wh) then
		std.err("Wrong 2-nd argument to seen(): "..std.tostr(ww), 2)
	end
	return wh:srch(w)
end

function lookup(w, ww)
	local wh
	ww = ww or std.here()
	if not std.is_obj(ww, 'list') then
		wh = std.ref(ww)
	else
		return ww:lookup(w)
	end
	wh = std.ref(ww)
	if not std.is_obj(wh) and not std.is_obj(wh, 'list') then
		std.err("Wrong 2-nd argument to lookup(): "..std.tostr(ww), 2)
	end
	return wh:lookup(w)
end

function ways(ww)
	local wh
	ww = ww or std.here()
	wh = std.ref(ww)
	if not std.is_obj(wh, 'room') then
		std.err("Wrong 2-nd argument to ways(): "..std.tostr(ww), 2)
	end
	return wh.way
end
std.ways = ways

function objs(ww)
	local wh
	ww = ww or std.here()
	wh = std.ref(ww)
	if not std.is_obj(wh) then
		std.err("Wrong 2-nd argument to objs(): "..std.tostr(ww), 2)
	end
	return wh.obj
end

function inspect(w, ...)
	return std.me():inspect(std.object(w), ...)
end

function have(w, ...)
	return std.me():have(std.object(w), ...)
end

function inroom(w, ...)
	return std.object(w):inroom(...)
end

function where(w, ...)
	return std.object(w):where(...)
end

function closed(w)
	return std.object(w):closed()
end

function disabled(w)
	return std.object(w):disabled()
end

function enable(w)
	return std.object(w):enable()
end

function disable(w)
	return std.object(w):disable()
end

function open(w)
	return std.object(w):open()
end

function close(w)
	return std.object(w):close()
end

function actions(w, t, v)
	return std.object(w):actions(t, v)
end

function pop(w, ww)
	local wh = std.here()
	if not std.is_obj(wh, 'dlg') then
		std.err("Call pop() in non-dialog object: "..std.tostr(wh), 2)
	end
	local r, v = wh:pop(w)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

function push(w, ww)
	local wh = std.here()
	if not std.is_obj(wh, 'dlg') then
		std.err("Call push() in non-dialog object: "..std.tostr(wh), 2)
	end
	local r, v = wh:push(w)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

function empty(w, ...)
	if not w then
		return std.here():empty()
	end
	return std.object(w):empty(...)
end

function lifeon(w, ...)
	return std 'game':lifeon(w and std.object(w), ...)
end

function lifeoff(w, ...)
	return std 'game':lifeoff(w and std.object(w), ...)
end

function live(...)
	return std 'game':live(...)
end

function change_pl(...)
	return std 'game':set_pl(...)
end

function player_moved(pl)
	pl = pl or std.me()
	pl = std.ref(pl)
	if not std.is_obj(pl, 'player') then
		std.err("Wrong argument to player_moved(): "..std.tostr(pl))
	end
	return std.me():moved()
end

function inv(pl)
	pl = pl or std.me()
	pl = std.ref(pl)
	if not std.is_obj(pl, 'player') then
		std.err("Wrong argument to inv(): "..std.tostr(pl))
	end
	return pl:inventory()
end

function remove(w, wh)
	local o = std.object(w)
	if not w then
		std.err("Wrong argument to remove(): "..std.tostr(w), 2)
	end
	wh = wh and std.object(wh)
	return o:remove(wh)
end

function purge(w)
	local o = std.object(w)
	if not w then
		std.err("Wrong argument to purge(): "..std.tostr(w), 2)
	end
	return o:purge()
end

local function __place(w, wh, remove)
	local o = std.object(w)
	if not o then
		std.err("Wrong argument to place(): "..std.tostr(w), 3)
	end
	if remove then
		o:remove() -- remove object from everywhere
	end
	wh = wh or std.here()
	if type(wh) ~= 'table' then
		wh = std.object(wh)
	end
	if o:type 'player' then
		if not std.is_obj(wh) then
			std.err("Wrong 2-nd argument to place(): "..std.tostr(wh), 3)
		end
		o:walk(wh, false, false)
		return o
	end
	if std.is_obj(wh) then
		wh.obj:add(o)
	elseif std.is_obj(wh, 'list') then
		wh:add(o)
	else
		std.err("Wrong 2-nd argument to place(): "..std.tostr(wh), 3)
	end
	return o
end

function replace(w, ww, wh)
	local o = std.object(w)
	if not o then
		std.err("Wrong argument to replace(): "..std.tostr(w), 2)
	end
	local oo = std.object(ww)
	if not oo then
		std.err("Wrong argument to replace(): "..std.tostr(ww), 2)
	end
	if not wh then -- replace all
		local l = {}
		o:where(l)
		for k, v in std.ipairs(l) do
			v.obj:replace(o, oo)
			if std.is_obj(v, 'room') then
				v.way:replace(o, oo)
			end
		end
		return oo
	end
	if type(wh) ~= 'table' then
		wh = std.object(wh)
	end
	if std.is_obj(wh) then
		local ob, l = wh:lookup(o)
		if l then
			l:replace(o, oo)
			return oo
		end
	elseif std.is_obj(wh, 'list') then
		wh:replace(o, oo)
		return oo
	else
		std.err("Wrong 3-rd argument to replace(): "..std.tostr(wh), 3)
	end
end

function place(w, wh)
	return __place(w, wh, true)
end

function put(w, wh)
	return __place(w, wh, false)
end

function take(w)
	local o = std.object(w)
	if o then
		o:actions('take', 1 + o:actions 'take')
	end
	return place(w, std.ref(std.me()):inventory())
end

function drop(w, wh)
	local o = std.object(w)
	if o then
		o:actions('drop', 1 + o:actions 'drop')
	end
	return place(w, wh)
end

function path(t)
	if type(t) ~= 'table' then
		std.err("Wrong path argument. Use {} as path argument.", 2)
	end
	local n, s, w = t[1], t[2], t[3]
	if not w then
		s, w = n, s
		n = nil
	end
	return room {
		nam = n or t.nam;
		before = s;
		disp = function(s)
			if disabled(s.walk) or closed(s.walk) then
				return false
			end
			if s.after ~= nil and visited(w) then
				return std.call(s, 'after')
			end
			return std.call(s, 'before')
		end;
		after = t.after;
		walk = w;
		onwalk = function(s, f)
			if disabled(s.walk) or closed(s.walk) then
				return false
			end
			if type(s.walk) == 'function' then
				walk(s.walk())
			else
				walk(s.walk)
			end
			return false
		end
	}
end

function time(...)
	return std.ref 'game':time(...)
end

local xact = std.obj {
	nam = '@';
	{
		methods = {}
	};
	act = function(s, w, ...)
		if type(s.methods[w]) ~= 'function' then
			std.err ("Wrong method to xact", 2)
		end
		return s.methods[w](...)
	end
}

std.rawset(_G, 'xact', xact.methods)

std.mod_init(function()
	declare {
		game = std.ref 'game',
		pl = std.ref 'player',
	}
end)
