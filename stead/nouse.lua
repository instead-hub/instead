game.use = function(s, w, o)
	if w.nouse then
		return call(w, 'nouse');
	end
	if o.noused then
		return call(o, 'noused');
	end
	return call(s, 'nouse')
end
