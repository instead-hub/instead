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
	for_each_object(save_object, h);
	save_object('game', game, h);
	clearvar(_G);
	game._snapshots = old
	game._snapshots[nr] = h.txt;
end

function restore_snapshot(nr)
	if not tonumber(nr) then nr = 0 end
	local ss = game._snapshots
	if not ss[nr] then return nil, true end -- nothing todo
	stead:init();
	game.lifes:zap();
	dofile('main.lua');
	if type(init) == 'function' then -- no hooks here!!!
		init();
	end
	local f, err = loadstring(ss[nr]);
	if not f then return end
	local i,r = f();
	game._snapshots = ss
	if r then
		return nil, false
	end
	i = do_ini(game);
	RAW_TEXT = true
	return i;
end

function delete_snapshot(nr)
	if not tonumber(nr) then nr = 0 end
	game._snapshots[nr] = nil
end
