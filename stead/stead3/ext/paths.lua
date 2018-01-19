local std = stead
local instead = std.ref '@instead'
-- luacheck: read globals instead_savepath
-- luacheck: read globals instead_gamepath
-- luacheck: read globals instead_exepath
-- luacheck: globals io
instead.savepath = instead_savepath
std.savepath = instead_savepath
instead.gamepath = instead_gamepath
instead.exepath = instead_exepath

io.open = stead.hook(io.open, function(f, path, acc, ...)
	if std.type(path) == 'string' and path:sub(1, 1) ~= '/' and not
		(path:sub(2, 2) == ':' and path:find("^[A-Z]", 1) == 1) then
		path = instead.gamepath()..'/'..path
	end
	return f(path, acc, ...)
end)
