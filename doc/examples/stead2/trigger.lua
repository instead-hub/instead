-- example module
function trigger(act, cond)
	local v = {}
	v.nam = 'trigger';
	if not act then
		error("Wrong parameter to trigger.", 2)
	end
	v.act = act;
	if not cond then
		cond = true
	elseif type(cond) == 'string' then
		cond = stead.eval('return '..cond);
	end
	v.cond = cond;
	v.life = function(s)
		if stead.call_bool(s, 'cond') then
			s:off()
			local r,v = stead.call(s, 'act')
			if r then
				return r, true
			end
			return r,v
		end
	end

	v.state = function(s)
		return s._state;
	end

	v.on = function(s, p)
		s._state = true
		stead.lifeon(s, p)
		return s
	end

	v.off = function(s)
		s._state = false
		stead.lifeoff(s)
		return s
	end

	return obj(v)
end
