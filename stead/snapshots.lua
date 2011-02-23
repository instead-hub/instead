game._snapshots = {}
stead.make_snapshot = function(nr)
	if not tonumber(nr) then nr = 0 end
	local h = { };
	h.txt = ''
	h.write = function(s, ...)
		local i
		local a = {...};
		for i = 1, stead.table.maxn(a) do
			s.txt = s.txt .. tostring(a[i]);
		end
	end
	local old = game._snapshots; game._snapshots = nil
	do_savegame(game, h);
	game._snapshots = old
	game._snapshots[nr] = h.txt;
end

function isSnapshot(nr)
	if not tonumber(nr) then nr = 0 end
	return (game._snapshots[nr] ~= nil)
end

stead.restore_snapshot = function (nr)
	if not tonumber(nr) then nr = 0 end
	local ss = game._snapshots
	if not ss[nr] then return nil, true end -- nothing todo
	local i,v

	stead.gamefile("main.lua", true);

	local f, err = loadstring(ss[nr]..' ');
	if not f then return end
	local i,r = f();
	game._snapshots = ss
	if r then
		return nil, false
	end
	i = do_ini(game, true);
	RAW_TEXT = true
--	delete_snapshot(nr);
	if cctx() then
		pr(i)
	end
	return i;
end

stead.delete_snapshot = function(nr)
	if not tonumber(nr) then nr = 0 end
	game._snapshots[nr] = nil
end

function make_snapshot(nr)
	if type(nr) ~= 'number' then
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
	if v == nil then return r end
	return r,v
end)
-- vim:ts=4
