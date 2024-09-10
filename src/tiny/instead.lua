-- pure Lua instead (stead3 api)

-- metaparser example:
--
-- local instead = require "instead":new()
-- print(instead:start())
-- for l in io.lines() do
--   print(instead:parser(l))
-- end

local env = {
  __super = false,
  __mods_loaded__ = {},
  package = { path = './stead/stead3/?.lua;'..package.path },
  debug = debug,
  type = type,
  setfenv = setfenv,
  getfenv = getfenv,
  rawset = rawset,
  rawget = rawget,
  rawequal = rawequal,
  setmetatable = setmetatable,
  getmetatable = getmetatable,
  table = table,
  math = math,
  string = string,
  pairs = pairs,
  ipairs = ipairs,
  io = io,
  os = os,
  tonumber = tonumber,
  tostring = tostring,
  print = print,
  collectgarbage = collectgarbage,
  error = error,
  sys = sys,
  PLATFORM = 'unix',
  DEBUG = false,
}

if not table.clone then
  function table.clone(src)
    local dst = {}
    if type(src) ~= 'table' then return src end
    for k, _ in pairs(src) do
      dst[table.clone(k)] = table.clone(src[k])
    end
    return dst
  end
end

if not string.split then
  function string.split(self, n, sep, rexp)
    if type(n) ~= 'number' then
      sep, rexp, n = n, sep, false
    end
    if not sep then
      rexp = true
      sep = "[ \t]+"
    end
    local ret = {}
    if self:len() == 0 then
      return ret
    end
    n = n or -1
    local idx, start = 1, 1
    local s, e = self:find(sep, start, not rexp)
    while s and n ~= 0 do
      ret[idx] = self:sub(start, s - 1)
      idx = idx + 1
      start = e + 1
      s, e = self:find(sep, start, not rexp)
      n = n - 1
    end
    ret[idx] = self:sub(start)
    return ret
  end
end

function env.instead_gamepath()
  return "./"
end

local loadf = loadfile

local function make_loadfile(n, env)
  if setfenv then
    setfenv(1, env)
  end
  return loadf(n, "t", env)
end

local function make_dofile(n, env)
  local r, e = make_loadfile(n, env)
  if setfenv then
    setfenv(1, env)
  end
  if not r then
    error(e..'\n'..debug.traceback())
  end
  return r()
end

local function make_require(n, env)
  local mods = env.__mods_loaded__
  if mods[n] then
    return mods[n]
  end
  local pathes = env.package.path:split(";")
  for _, p in ipairs(pathes) do
    local name = p:gsub("%?", n)
    local f = io.open(name, "r")
    if f then
      f:close()
      mods[n] = make_dofile(name, env) or true
      break
    end
  end
  if not mods[n] then
    error("Can't load module: "..tostring(n), 2)
  end
  return mods[n]
end


local function init(env, f)
  local _ENV = env
  if setfenv then
    setfenv(1, _ENV)
  end
  require("stead")
  require("tiny3")
  stead:init()
  std.mod_start(function()
    local mp = std.ref '@metaparser'
    if mp then
      VerbExtend ({
        "#MetaSave",
        "*:MetaSave",
      }, mp)
      VerbExtend ({
        "#MetaLoad",
        "*:MetaLoad",
      }, mp)
      rawset(mp, 'MetaSave', function(self, w)
        __super:save(w)
      end)
      rawset(mp, 'MetaLoad', function(self, w)
        __super.need_load = w
      end)
      rawset(instead, 'restart', function()
        __super.need_restart = true
      end)
    end
  end)
  dofile(f or "main3.lua")
end

local boot = {}

function boot:newenv()
  local new = table.clone(env) -- mt can't be used :(
  new.dofile = function(f) return make_dofile(f, new) end
  new.require = function(f) return make_require(f, new) end
  new.loadfile = function(f) return make_loadfile(f, new) end
  new._G = new
  init(new)
  return new
end

function boot:start()
  self.env.game:ini()
  return self.env.iface:cmd 'look'
end

function boot:restart()
  self.env = boot:newenv()
  self.env.__super = self
  return self:start()
end

function boot:load(file)
  self.env = boot:newenv()
  self.env.__super = self
  self.env.game:ini()
  return self.env.iface:cmd(string.format('load %s', file))
end

function boot:save(file)
  return self.env.iface:cmd(string.format('save %s', file))
end

function boot:cmd(cmd)
  return self.env.iface:cmd(cmd)
end

function boot:parser(cmd)
  return self:cmd(string.format("@metaparser %q", cmd))
end

function boot:new()
  local new = boot:newenv()
  local o = { env = new }
  new.__super = o
  self.__index = self
  setmetatable(o, self)
  return o
end

return boot
