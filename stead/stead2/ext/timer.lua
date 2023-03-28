stead.set_timer = instead_timer

stead.timer = function()
	if stead.type(timer) == 'table' and stead.type(timer.callback) == 'function' then
		return timer:callback();
	end
	return
end

stead.module_done(function(s) 
	timer:stop() 
end)

local function compat_api()
	if not stead.api_atleast(1, 7, 1) then
		set_timer = instead_timer
	end
end

stead.module_start(function(load)
	if compat_api and not load then
		compat_api()
		compat_api = nil
	end
end)

stead.module_init(function(s) 
	timer = obj { -- timer calls stead.timer callback 
		nam = 'timer',
		ini = function(s)
			if stead.tonum(s._timer) ~= nil and stead.type(stead.set_timer) == 'function' then
				stead.set_timer(s._timer);
			end
		end,
		get = function(s)
			if stead.tonum(s._timer) == nil then
				return 0
			end
			return stead.tonum(s._timer);
		end,
		stop = function(s)
			return s:set(-1);
		end,
		del = function(s)
			return s:set(-1);
		end,
		set = function(s, v)
			s._timer = stead.tonum(v);
			if stead.type(stead.set_timer) ~= 'function' then
				return false
			end
			stead.set_timer(v)
			return true
		end,
		--[[ 	callback = function(s)
			end, ]]
	};
end)
