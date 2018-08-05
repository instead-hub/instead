local MAKE_SNAPSHOT
game._snapshots = {}
stead.make_snapshot = function(nr)
	if not stead.tonum(nr) then nr = 0 end
	local h = { };
	h.txt = ''
	h.write = function(s, ...)
		local a = {...};
		for i = 1, stead.table.maxn(a) do
			s.txt = s.txt .. stead.tostr(a[i]);
		end
	end
	local old = game._snapshots; game._snapshots = nil
	stead.do_savegame(game, h);
	game._snapshots = old
	game._snapshots[nr] = h.txt;
end

function isSnapshot(nr)
	if not stead.tonum(nr) then nr = 0 end
	return (game._snapshots[nr] ~= nil)
end

stead.restore_snapshot = function (nr)
	if not stead.tonum(nr) then nr = 0 end
	local ss = game._snapshots
	if not ss[nr] then return nil, true end -- nothing todo
	local i,v

	if stead.api_atleast(1, 7, 1) then
		stead.gamereset("main.lua", true);
	else
		stead.gamefile("main.lua", true);
	end

	local f, err = stead.eval(ss[nr]..' ');
	if not f then return end
	local i,r = f();
	game._snapshots = ss
	if r then
		return nil, false
	end

	i = stead.do_ini(game, true);

	if stead.api_atleast(1, 7, 1) then
		i = game:start()
		stead.rawset(_G, 'PLAYER_MOVED',  true) -- force fading
	else -- legacy
		if not game.showlast then
			stead.last_disp(false)
		end
		i = stead.cat('', stead.last_disp())
	end
	stead.rawset(_G, 'RAW_TEXT', true)
--	delete_snapshot(nr);
	if stead.cctx() then
		stead.pr(i)
	end
	return i;
end

stead.delete_snapshot = function(nr)
	if not stead.tonum(nr) then nr = 0 end
	game._snapshots[nr] = nil
end

function make_snapshot(nr)
	if stead.type(nr) ~= 'number' then
		nr = 0
	end
	MAKE_SNAPSHOT = nr
end

function restore_snapshot(nr)
	return stead.restore_snapshot(nr)
end

function delete_snapshot(nr)
	return stead.delete_snapshot(nr);
end

iface.cmd = stead.hook(iface.cmd, function(f, ...)
	local r,v = f(...);
	if MAKE_SNAPSHOT ~= nil then
		stead.make_snapshot(MAKE_SNAPSHOT);
		MAKE_SNAPSHOT = nil
	end
	return r,v
end)
-- vim:ts=4
