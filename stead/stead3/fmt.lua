local std = stead
local type = std.type
local iface = std.ref '@iface'

-- luacheck: globals fmt

fmt = std.obj {
	nam = '@format';
	para = false;
	nopara = '_';
	para_space = '    ';
	quotes = true;
	dash = true;
	filter = nil;
}

std.obj {
	nam = '$fmt';
	act = function(_, w, ...)
		if type(w) ~= 'string' or not fmt[w] then
			return w
		end
		return fmt[w](...)
	end
}

std.format = function(r, state)
	local utf8 = (std.ref 'game'.codepage == 'UTF-8' or std.ref 'game'.codepage == 'utf-8')

	if type(r) ~= 'string' then
		return r
	end

	if type(fmt.filter) == 'function' then
		r = fmt.filter(r, state)
	end

	if utf8 then
		if fmt.dash then
			r = r:gsub("%-%-%-*", { ['--'] = '—' })
			r = r:gsub("[ \t]+—", iface:nb('').."—"); -- do not break dash
			r = r:gsub("^([^ \t]*)—[ \t]+", "%1— ");
			r = r:gsub("(\n[^ \t]*)—[ \t]+", "%1— ");
		end
		if fmt.quotes then
			r = r:gsub('_"','«'):gsub('"_',"»");
			r = r:gsub('"([^"]*)"','«%1»');
			r = r:gsub(',,','„'):gsub("''",'”');
		end
	end
	if not state then
		return r
	end

	if fmt.para then
		r = r:gsub('\n([^\n])', '\001%1'):gsub('\001[ \t]*'..fmt.nopara,'\n'):gsub('\001[ \t]*', '\n'..iface:nb(fmt.para_space));
		r = r:gsub('^[ \t]*', '\001'):gsub('\001[ \t]*'..fmt.nopara,''):gsub('\001[ \t]*', iface:nb(fmt.para_space));
	end
	return r
end

function fmt.em(str)
	return iface:em(str)
end

function fmt.c(str)
	return iface:center(str)
end

function fmt.j(str)
	return iface:just(str)
end

function fmt.l(str)
	return iface:left(str)
end

function fmt.r(str)
	return iface:right(str)
end

function fmt.b(str)
	return iface:bold(str)
end

function fmt.top(str)
	return iface:top(str)
end

function fmt.bottom(str)
	return iface:bottom(str)
end

function fmt.middle(str)
	return iface:middle(str)
end

function fmt.nb(str)
	return iface:nb(str)
end

function fmt.anchor(str)
	return iface:anchor(str)
end

function fmt.img(str)
	return iface:img(str)
end

function fmt.imgl(str)
	return iface:imgl(str)
end

function fmt.imgr(str)
	return iface:imgr(str)
end

function fmt.u(str)
	return iface:under(str)
end

function fmt.st(str)
	return iface:st(str)
end

function fmt.tab(str, al)
	return iface:tab(str, al)
end

function fmt.y(str, al)
	return iface:y(str, al)
end
