-- luacheck: globals STANDALONE
-- luacheck: read globals instead
-- luacheck: globals io os debug
-- luacheck: read globals instead_realpath

local function sandbox()
	if STANDALONE or not instead.gamepath then -- not standalone or not sdl-instead
		return
	end
-- luacheck: no unused args
	local check_path = function(realpath, type, find, gsub, savepath, gamepath, path)
-- luacheck: unused args
		if not path then
			return false
		end
		path = realpath(path)
		if not path then
			return false
		end
		local spath = realpath(savepath)
		if not spath then
			return false
		end
		local s = find(path, spath..'/', 1, true)
		if s ~= 1 then
			spath = realpath(gamepath);
			if spath then
				s = find(path, spath..'/', 1, true)
			end
		end
		if s ~= 1 then
			return false
		end
		return true
	end

	local build_sandbox_open = function(realpath, error, type, find, gsub, savepath, gamepath)
		return stead.hook(io.open, function(f, path, acc, ...)
					  if type(acc) ~= 'string' or not find(acc, "[aw+]") then -- only write access
						  return f(path, acc, ...)
					  end
-- luacheck: no unused args
					  if not check_path(realpath, type, find, gsub, savepath, gamepath, path) then
-- luacheck: unused args
						  error ("Access denied (write): ".. path, 3);
						  return false
					  end
					  return f(path, acc, ...)
		end)
	end

	local build_sandbox_remove = function(realpath, error, type, find, gsub, savepath, gamepath)
		return stead.hook(os.remove, function(f, path, ...)
					  if type(path) ~= 'string' then
						  return f(path, ...)
					  end
					  if not check_path(realpath, type, find, gsub, savepath, gamepath, path) then
						  error ("Access denied (remove): ".. path, 3);
						  return false
					  end
					  return f(path, ...)
		end)
	end

	local build_sandbox_rename = function(realpath, error, type, find, gsub, savepath, gamepath)
		return stead.hook(os.rename, function(f, oldname, newname, ...)
					  if not check_path(realpath, type, find, gsub, savepath, gamepath, oldname) or
					  not check_path(realpath, type, find, gsub, savepath, gamepath, newname) then
						  error ("Access denied (rename): ".. oldname .. ', '.. newname, 3);
						  return false
					  end
					  return f(oldname, newname, ...)
		end)
	end

	local build_sandbox_output = function(realpath, error, type, find, gsub, savepath, gamepath)
		return stead.hook(io.output, function(f, path, ...)
					  if type(path) == 'string' and not check_path(realpath, type, find, gsub, savepath, gamepath, path) then
						  error ("Access denied (output): ".. path, 3);
						  return false
					  end
					  return f(path, ...)
		end)
	end

	io.open = build_sandbox_open(instead_realpath, error, type, string.find, string.gsub,
				     instead.savepath(), instead.gamepath());

	os.remove = build_sandbox_remove(instead_realpath, error, type, string.find, string.gsub,
					 instead.savepath(), instead.gamepath());

	os.rename = build_sandbox_rename(instead_realpath, error, type, string.find, string.gsub,
					 instead.savepath(), instead.gamepath());

	io.output = build_sandbox_output(instead_realpath, error, type, string.find, string.gsub,
					 instead.savepath(), instead.gamepath());

	os.execute = function(s)
		print ("Warning: trying to do os.execute: "..s);
	end

	io.popen = function(s)
		print ("Warning: trying to do io.popen: "..s);
	end

	os.tmpname = function(_)
		print ("Warning: trying to do os.tmpname");
	end

	if not stead.rawget(_G, 'DEBUG') then
		debug = nil
	end
	package.cpath = ""
	package.preload = {}
	package = nil
end

sandbox()
