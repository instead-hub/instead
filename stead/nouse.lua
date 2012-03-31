game.use = function(s, w, o)
	if w.nouse then
		return call(w, 'nouse', o);
	end
	if o.noused then
		return call(o, 'noused', w);
	end
	return call(s, 'nouse', w, o)
end
