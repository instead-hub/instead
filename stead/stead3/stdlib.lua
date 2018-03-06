-- luacheck: read globals declare

local std = stead
local type = std.type
-- luacheck: read globals game
std.rawset(_G, 'std', stead)
-- luacheck: globals include
include = std.include
-- luacheck: globals loadmod
loadmod = std.loadmod
-- luacheck: globals rnd
rnd = std.rnd
-- luacheck: globals rnd_seed
rnd_seed = std.rnd_seed
-- luacheck: globals p
p = std.p
-- luacheck: globals pclr
pclr = std.pclr
-- luacheck: globals pr
pr = std.pr
-- luacheck: globals pn
pn = std.pn
-- luacheck: globals pf
pf = std.pf
-- luacheck: globals obj
obj = std.obj
-- luacheck: globals stat
stat = std.stat
-- luacheck: globals room
room = std.room
-- luacheck: globals menu
menu = std.menu
-- luacheck: globals dlg
dlg = std.dlg
-- luacheck: globals me
me = std.me
-- luacheck: globals here
here = std.here
-- luacheck: globals from
from = std.from
-- luacheck: globals new
new = std.new
-- luacheck: globals delete
delete = std.delete
-- nameof = std.nameof
-- dispof = std.dispof
-- titleof = std.titleof
-- luacheck: globals gamefile
gamefile = std.gamefile
-- luacheck: globals player
player = std.player
-- luacheck: globals dprint
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

-- luacheck: globals pfn
function pfn(f, ...)
	local a = {...}
	if type(f) == 'function' then
		return _pfn(function() end, f, ...)
	end
	return function()
		std.p(f, std.unpack(a))
	end
end

-- luacheck: globals from
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

-- luacheck: globals visits
function visits(w)
	if not w then return std.here():visits() end
	return std.object(walkroom(w)):visits()
end

-- luacheck: globals visited
function visited(w)
	if not w then return std.here():visited() end
	return std.object(walkroom(w)):visited()
end

-- luacheck: globals walk
function walk(w, ...)
	local r, v = std.me():walk(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

-- luacheck: globals life_walk
function life_walk(w, ...)
	game:reaction(false)
	game:events(false, false)
	return walk(w, ...)
end

-- luacheck: globals walkin
function walkin(w, ...)
	local r, v = std.me():walkin(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

-- luacheck: globals walkout
function walkout(w, ...)
	local r, v = std.me():walkout(walkroom(w), ...)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

-- luacheck: globals walkback
function walkback(w, ...)
	return walkout(w, false, ...)
end

std.walkout = walkout
std.walkin = walkin
std.walk = walk

local function object(w)
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

-- luacheck: globals _
_ = std.object

-- luacheck: globals for_all
for_all = std.for_all

-- luacheck: globals seen
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

-- luacheck: globals lookup
function lookup(w, ww)
	local wh
	ww = ww or std.here()
	if not std.is_obj(ww, 'list') then
		wh = std.ref(ww)
	else
		return ww:lookup(w)
	end
	if not std.is_obj(wh) and not std.is_obj(wh, 'list') then
		std.err("Wrong 2-nd argument to lookup(): "..std.tostr(ww), 2)
	end
	return wh:lookup(w)
end

-- luacheck: globals ways
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

-- luacheck: globals objs
function objs(ww)
	local wh
	ww = ww or std.here()
	wh = std.ref(ww)
	if not std.is_obj(wh) then
		std.err("Wrong 2-nd argument to objs(): "..std.tostr(ww), 2)
	end
	return wh.obj
end

-- luacheck: globals inspect
function inspect(w, ...)
	return std.me():inspect(std.object(w), ...)
end

-- luacheck: globals have
function have(w, ...)
	return std.me():have(std.object(w), ...)
end

-- luacheck: globals inroom
function inroom(w, ...)
	return std.object(w):inroom(...)
end

-- luacheck: globals where
function where(w, ...)
	return std.object(w):where(...)
end

-- luacheck: globals closed
function closed(w)
	return std.object(w):closed()
end

-- luacheck: globals disabled
function disabled(w)
	return std.object(w):disabled()
end

-- luacheck: globals enable
function enable(w)
	return std.object(w):enable()
end

-- luacheck: globals disable
function disable(w)
	return std.object(w):disable()
end

-- luacheck: globals open
function open(w)
	return std.object(w):open()
end

-- luacheck: globals close
function close(w)
	return std.object(w):close()
end

-- luacheck: globals actions
function actions(w, t, v)
	return std.object(w):actions(t, v)
end

-- luacheck: globals pop
function pop(w, ww)
	local wh = ww or std.here()
	if not std.is_obj(wh, 'dlg') then
		std.err("Call pop() in non-dialog object: "..std.tostr(wh), 2)
	end
	local r, v = wh:pop(w)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

-- luacheck: globals push
function push(w, ww)
	local wh = ww or std.here()
	if not std.is_obj(wh, 'dlg') then
		std.err("Call push() in non-dialog object: "..std.tostr(wh), 2)
	end
	local r, v = wh:push(w)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

-- luacheck: globals empty
function empty(w, ...)
	if not w then
		return std.here():empty()
	end
	return std.object(w):empty(...)
end

-- luacheck: globals lifeon
function lifeon(w, ...)
	return std 'game':lifeon(w and std.object(w), ...)
end

-- luacheck: globals lifeoff
function lifeoff(w, ...)
	return std 'game':lifeoff(w and std.object(w), ...)
end

-- luacheck: globals live
function live(...)
	return std 'game':live(...)
end

-- luacheck: globals change_pl
function change_pl(w, ...)
	return std 'game':set_pl(w and std.object(w), ...)
end

-- luacheck: globals player_moved
function player_moved(pl)
	pl = pl or std.me()
	pl = std.ref(pl)
	if not std.is_obj(pl, 'player') then
		std.err("Wrong argument to player_moved(): "..std.tostr(pl))
	end
	return std.me():moved()
end

-- luacheck: globals inv
function inv(pl)
	pl = pl or std.me()
	pl = std.ref(pl)
	if not std.is_obj(pl, 'player') then
		std.err("Wrong argument to inv(): "..std.tostr(pl))
	end
	return pl:inventory()
end

-- luacheck: globals remove
function remove(w, wh)
	local o = std.object(w)
	if not w then
		std.err("Wrong argument to remove(): "..std.tostr(w), 2)
	end
	wh = wh and std.object(wh)
	return o:remove(wh)
end

-- luacheck: globals purge
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

-- luacheck: globals replace
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
		for _, v in std.ipairs(l) do
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
		local _, l = wh:lookup(o)
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

-- luacheck: globals place
function place(w, wh)
	return __place(w, wh, true)
end

-- luacheck: globals put
function put(w, wh)
	return __place(w, wh, false)
end

-- luacheck: globals take
function take(w)
	local o = std.object(w)
	if o then
		o:actions('take', 1 + o:actions 'take')
	end
	return place(w, std.me():inventory())
end

-- luacheck: globals drop
function drop(w, wh)
	local o = std.object(w)
	if o then
		o:actions('drop', 1 + o:actions 'drop')
	end
	return place(w, wh)
end

-- luacheck: globals path
path = std.class({
	__path_type = true;
	new = function(_, t)
		if type(t) ~= 'table' then
			std.err("Wrong path argument. Use {} as path argument.", 2)
		end
		local n, s, w = t[1], t[2], t[3]

		if not w then
			s, w = n, s
			n = nil
		end

		local new = {
			before = s;
			walk = w;
		}

		for k, v in std.pairs(t) do
			if type(k) == 'string' then
				new[k] = v
			end
		end

		new.nam = n or new.nam

		return std.room(new)
	end;
	disp = function(s)
		local w = s.walk
		if type(w) == 'function' then
			w = w()
		end
		if disabled(w) or closed(w) then
			return false
		end
		if s.after ~= nil and visited(w) then
			return std.call(s, 'after')
		end
		return std.call(s, 'before')
	end;
	onwalk = function(s, _)
		local w = s.walk
		if type(w) == 'function' then
			w = w()
		end
		if not disabled(w) and not closed(w) then
			walk(w)
		end
		return false
	end;
}, std.room)

-- luacheck: globals time
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
