-- some stubs for tiny-instead
-- fake game.gui
-- stat, menu
-- fake audio
-- fake input

if API == 'stead3' then
	dofile 'tiny3.lua'
	local instead = std '@instead'
	instead.savepath = function() return "./" end
	std.mod_start(function()
		std.mod_init(function()
			std.rawset(_G, 'instead', instead)
			require "ext/sandbox"
		end)
		local mp = std.ref '@metaparser'
		if mp then
			mp.msg.CUTSCENE_MORE = '^'..mp.msg.CUTSCENE_HELP
		end
	end)
	return
end

dofile 'tiny2.lua'
