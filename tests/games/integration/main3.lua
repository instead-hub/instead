-- $Name:Integration tests$

-- Lua-level integration tests: this game is loaded by the real interpreter
-- (tests/integration.sh) and exercises the instead_* bindings.

local checks = 0
local failed = 0

local function fail(msg)
	failed = failed + 1
	print("INTEGRATION FAIL: " .. msg)
end

local function check(name, cond)
	checks = checks + 1
	if not cond then
		fail(name)
	end
end

local function check_eq(name, got, want)
	checks = checks + 1
	if got ~= want then
		fail(name .. ": got " .. tostring(got) .. ", expected " .. tostring(want))
	end
end

-- src/instead_paths.c
check("instead_gamespath is a function", type(instead_gamespath) == "function")
local gamespath = instead_gamespath()
check("gamespath is a non-empty string", type(gamespath) == "string" and #gamespath > 0)
check("gamespath is absolute", gamespath:sub(1, 1) == "/")
check("gamespath ends with games",
	gamespath:gsub("/+$", ""):sub(-5) == "games")

check("instead_exepath is a function", type(instead_exepath) == "function")
local exepath = instead_exepath()
check("exepath is a non-empty string", type(exepath) == "string" and #exepath > 0)

check("instead_savepath is a function", type(instead_savepath) == "function")
local savepath = instead_savepath()
check("savepath is a non-empty string", type(savepath) == "string" and #savepath > 0)

-- src/instead/instead.c
check("instead_gamepath", instead_realpath(".") == instead_gamepath())

local steadpath = instead_steadpath()
check("steadpath is absolute", type(steadpath) == "string" and steadpath:sub(1, 1) == "/")
check("steadpath contains stead", steadpath:find("stead", 1, true) ~= nil)

-- src/instead_timer.c: schedule and cancel before it can fire
check("instead_timer is a function", type(instead_timer) == "function")
instead_timer(1000)
instead_timer()

-- src/instead_bits.c
check_eq("bit_or", bit_or(0xF0, 0x0F), 0xFF)
check_eq("bit_and", bit_and(0xF0, 0x0F), 0)
check_eq("bit_shl", bit_shl(1, 4), 16)
check_eq("bit_not", bit_not(0), 4294967295)

-- random (instead_* -> mt_random)
instead_srandom(12345)
local rnd = instead_random()
instead_srandom(12345)
check_eq("instead_random is deterministic", instead_random(), rnd)

-- utf8 helpers
check_eq("utf8_len", utf8_len("привет"), 6)
check_eq("utf8_char", utf8_char("привет", 1), "п")
check_eq("utf8_char ascii", utf8_char("hello", 2), "e")

check_eq("table_get_maxn", table_get_maxn({ 1, 2, 3 }), 3)

print("INTEGRATION CHECKS " .. checks)
if failed == 0 then
	print("INTEGRATION OK")
else
	print("INTEGRATION FAILED " .. failed)
end
io.stdout:flush()
