game._snapshots = {}

function make_snapshot(nr)
	if not tonumber(nr) then nr = 0 end
	local h = { };
	h.txt = ''
	h.write = function(s, ...)
		local i
		for i = 1, stead.table.maxn(arg) do
			s.txt = s.txt .. tostring(arg[i]);
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

function restore_snapshot(nr)
	if not tonumber(nr) then nr = 0 end
	local ss = game._snapshots
	if not ss[nr] then return nil, true end -- nothing todo
	local i,v

	gamefile("main.lua", true);
	stead.pclr();
--	if type(init) == 'function' then -- no hooks here!!!
--		init();
--	end
	local f, err = loadstring(ss[nr]);
	if not f then return end
	local i,r = f();
	game._snapshots = ss
	if r then
		return nil, false
	end
	i = do_ini(game, true);
	RAW_TEXT = true
	delete_snapshot(nr);
	if cctx() then
		pr(i)
	end
	return i;
end

function delete_snapshot(nr)
	if not tonumber(nr) then nr = 0 end
	game._snapshots[nr] = nil
end
-- vim:ts=4
