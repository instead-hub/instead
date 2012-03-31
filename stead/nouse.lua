game.use = function(s, w, o)
	local r, v
	r, v = call(w, 'nouse', o);
	if not r and not v then
		r, v = call(o, 'noused', w);
	end
	if not r and not v then
		r, v = call(s, 'nouse', w, o)
	end
	return r,v
end
