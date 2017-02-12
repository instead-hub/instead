local std = stead
local type = std.type
_ = std.ref
std.rawset(_G, 'std', stead)
include = std.include
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
walk = std.walk
walkin = std.walkin
walkout = std.walkout
new = std.new
delete = std.delete
nameof = std.nameof
dispof = std.dispof
titleof = std.titleof
gamefile = std.gamefile

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

function object(w)
	local o
	if std.is_tag(w) then
		o = std.here():lookup(w)
		if not o then
			o = std.ways():lookup(w)
		end
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

for_all = std.for_all

function seen(w, ww)
	local wh
	ww = ww or std.here()
	if not std.is_obj(ww, 'list') then
		wh = std.ref(ww)
	else
		return ww:seen(w)
	end
	if not std.is_obj(wh) then
		std.err("Wrong 2-nd argument to seen(): "..std.tostr(ww), 2)
	end
	return wh:seen(w)
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

function way(w, wh)
	return lookup(w, ways(wh))
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

function objs(ww)
	local wh
	ww = ww or std.here()
	wh = std.ref(ww)
	if not std.is_obj(wh) then
		std.err("Wrong 2-nd argument to objs(): "..std.tostr(ww), 2)
	end
	return wh.obj
end

function search(w, ...)
	return std.me():search(std.object(w), ...)
end

function have(w, ...)
	return std.me():have(std.object(w), ...)
end

function inroom(w, ...)
	return std.object(w):inroom(std.object(w), ...)
end

function where(w, ...)
	return std.object(w):where(std.object(w), ...)
end

function closed(w)
	return std.object(w):closed()
end

function disabled(w)
	return std.object(w):enabled()
end

function enable(w)
	return std.object(w):enable()
end

function disable(w)
	return std.object(w):enable()
end

function open(w)
	return std.object(w):open()
end

function close(w)
	return std.object(w):close()
end

function actions(w, t)
	return std.object(w):actions(t)
end

function pop(w)
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

function push(w)
	local wh = std.here()
	if not std.is_obj(wh, 'dlg') then
		std.err("Call push() in non-dialog object: "..std.tostr(wh), 2)
	end
	local r, v = ww:push(w)
	if std.cctx() and type(r) == 'string' then
		std.p(r)
	end
	return r, v
end

function empty(w)
	if not w then
		return std.here():empty()
	end
	return std.object(w):empty(w)
end

function lifeon(w, ...)
	return std.game:lifeon(w and std.object(w), ...)
end

function lifeoff(w, ...)
	return std.game:lifeoff(w and std.object(w), ...)
end

function live(...)
	return std.game:live(...)
end

function change_pl(...)
	return std.game:set_pl(...)
end

function player_moved()
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
		wh = std.ref(wh)
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

function place(w, wh)
	return __place(w, wh, true)
end

function put(w, wh)
	return __place(w, wh, false)
end

function take(w)
	return place(w, std.ref(std.me()):inventory())
end

function drop(w, wh)
	return place(w, wh)
end

local xact = (std.menu or std.obj) {
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
		pl = std.ref 'pl',
	}
end)
