char *instead_gui_lua = "game.hinting = true;\n\
game.showlast = true;\n\
iface.xref = function(self, str, obj)\n\
--		return '@'..str..'{'..obj..'}';\n\
		local o = ref(here():srch(obj));\n\
		local cmd=''\n\
		if not o then \n\
			o = ref(ways():srch(obj));\n\
			if o then\n\
				cmd = 'go ';\n\
			end\n\
		end\n\
		if not o then\n\
			o = ref(inv():srch(obj));\n\
		end\n\
		if not isObject(o) or not o.id then\n\
			return str;\n\
		end\n\
		return cat('<a:'..cmd..'0'..tostring(o.id)..'>',str,'</a>');\n\
end;\n\
iface.title = function(self, str)\n\
		return nil\n\
end;\n\
iface.em = function(self, str)\n\
		if str == nil then return nil; end; \n\
		return cat('<i>',str,'</i>');\n\
end;\n\
iface.right = function(self, str)\n\
		if str == nil then return nil; end; \n\
		return cat('<r>',str,'</r>');\n\
end;\n\
iface.left = function(self, str)\n\
		if str == nil then return nil; end; \n\
		return cat('<l>',str,'</l>');\n\
end;\n\
iface.center = function(self, str)\n\
		if str == nil then return nil; end; \n\
		return cat('<c>',str,'</c>');\n\
end;\n\
iface.bold = function(self, str)\n\
		if str == nil then return nil; end; \n\
		return cat('<b>',str,'</b>');\n\
end;\n\
\n\
iface.inv = function(self, str)\n\
	if str then\n\
		return string.gsub(str,',','^');\n\
	end\n\
	return str\n\
end;\n\
\n\
iface.ways = function(self, str)\n\
	if str then\n\
		return '<c>'..string.gsub(str,',',' | ')..'</c>';\n\
	end\n\
	return str\n\
end;";

