-- example module
require "sprites"
require "theme"

function font(name, size, scale)
	if scale == nil then
		scale = true
	end
	local v = obj {
		nam = 'styler';
		fname = name;
		size = size;
		cache = { };
		list = { };
		scaled = scale;
		cache_get = function(s, w, color, t)
			local k = w..color..tostring(t)
			if s.cache[k].time ~= -1 then
				s.cache[k].time = stead.time()
			end
			return s.cache[k]
		end;
		cache_clear = function(self, age)
			local k, v
			local new_list = {}
			if not age then 
				age = 0 
			end
			for k,v in ipairs(self.list) do
				if v.time ~= -1 and stead.time() - v.time >= age then
					sprite.free(v.img);
				else
					table.insert(new_list, v)
				end
			end
			self.list = {}
			self.cache = {}
			for k, v in ipairs(new_list) do
				self:cache_add(v.word, v.color, v.t, v.img, v.time)
			end
		end;
		life = function(s)
			if player_moved() then
				s:cache_clear(2)
			end
		end;
		txt = function(st, txt, color, t)
			local s, e;
			local ss = 1
			local res = ''
			if not color then
				color = theme.get 'win.col.fg'
			end
			if not t then
				t = 0
			end
			while true do
				s, e = txt:find("[ \t]+", ss);
				local w = txt:sub(ss, s);
				if w and w ~= '' then
					st:cache_add(w, color, t)
					res = res .. img(st:cache_get(w, color, t).img);
				end
				if not e then break end
				ss = e + 1
				res = res .. ' ';
			end	
			return res;
		end;
		cache_add = function(s, w, color, t, key, time)
			local k = w..color..tostring(t)
			if not s.cache[k] then
				s.cache[k] = {}
				s.cache[k].img = sprite.text(s.font, w, color, t);
				if key and key ~= s.cache[k].img then
					stead.sprite_dup(s.cache[k].img, key);
					sprite.free(s.cache[k].img);
					s.cache[k].img = key
				end
				s.cache[k].word = w;
				s.cache[k].color = color;
				s.cache[k].t = t;
				s.cache[k].time = stead.time();
				stead.table.insert(s.list, s.cache[k]);
			end
			if not stead.initialized and not time then
				time = -1
			end
			if time then
				s.cache[k].time = time
			else
				s.cache[k].time = stead.time(); -- renew time
			end
		end;
		save = function(self, name, h, need)
			h:write(stead.string.format("%s = font(%q, %d, %s);\n", name, self.fname, self.size, stead.tostring(self.scaled)))
			local k, v
			for k,v in ipairs(self.list) do
				h:write(stead.string.format("%s:cache_add(%q, %q, %d, %q, %d);\n", name, v.word, v.color, v.t, v.img, v.time))
			end
		end;
	}
	if v.scaled then
		size = sprite.font_scaled_size(size)
	end
	v.font = sprite.font(name, size);
	lifeon(v);
	return v;
end
