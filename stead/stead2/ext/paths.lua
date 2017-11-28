-- nothing todo in stead2 ;)
local instead_gamepath = instead_gamepath
io.open = stead.hook(io.open, function(f, path, acc, ...)
	if stead.type(path) == 'string' and path:sub(1, 1) ~= '/' and not
		(path:sub(2, 2) == ':' and path:find("^[A-Z]", 1) == 1) then
		path = instead_gamepath()..'/'..path
	end
	return f(path, acc, ...)
end)
