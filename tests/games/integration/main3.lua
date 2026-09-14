-- $Name:Integration tests$

-- Lua-level integration tests: this game is loaded by the real interpreter
-- (tests/integration.sh) and exercises the instead_* bindings.
-- Assets: assets/test.png, assets/test.wav.

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

-- random (instead_* -> instead_random)
instead_srandom(12345)
local rnd = instead_random()
instead_srandom(12345)
check_eq("instead_random is deterministic", instead_random(), rnd)

-- utf8 helpers
check_eq("utf8_len", utf8_len("привет"), 6)
check_eq("utf8_char", utf8_char("привет", 1), "п")
check_eq("utf8_char ascii", utf8_char("hello", 2), "e")

check_eq("table_get_maxn", table_get_maxn({ 1, 2, 3 }), 3)

-- src/instead_sprites.c
local gp = instead_gamepath()
local png = gp .. "/assets/test.png"

local orig = instead_sprite_load(png, "itest/orig")
check_eq("sprite load", orig, "itest/orig")
check("missing sprite", instead_sprite_load(gp .. "/assets/nope.png") == nil)

local ow, oh = instead_sprite_size(orig)
check("sprite size", ow > 0 and oh > 0 and ow == oh)
local r, g, b, a = instead_sprite_pixel(orig, 0, 0)
check("sprite pixel", r == 255 and g == 0 and b == 0 and a == 255)

local dup = instead_sprite_dup(orig, "itest/dup")
check_eq("sprite dup", dup, "itest/dup")
instead_sprite_pixel(dup, 0, 0, "#00ff00")
local dr, dg, db, da = instead_sprite_pixel(dup, 0, 0)
check("sprite pixel set", dr == 0 and dg == 255 and db == 0 and da == 255)
local orr, org, orb, ora = instead_sprite_pixel(orig, 0, 0)
check("sprite pixel is per copy", orr == 255 and org == 0 and orb == 0 and ora == 255)

local fill = instead_sprite_dup(orig, "itest/fill")
check("sprite fill", instead_sprite_fill(fill, 0, 0, -1, -1, "#0000ff"))
local fr, fg, fb, fa = instead_sprite_pixel(fill, 0, 0)
check("sprite fill pixel", fr == 0 and fg == 0 and fb == 255 and fa == 255)

local big = instead_sprite_scale(orig, 2, 2, false, "itest/big")
check_eq("sprite scale", big, "itest/big")
local bw, bh = instead_sprite_size(big)
check("sprite scale size", bw >= ow and bh >= oh)

check("sprite rotate", instead_sprite_rotate(orig, 90, false, "itest/rot") == "itest/rot")

local alpha = instead_sprite_alpha(orig, 128, "itest/alpha")
check_eq("sprite alpha", alpha, "itest/alpha")
local ar, ag, ab, aa = instead_sprite_pixel(alpha, 0, 0)
check("sprite alpha pixel", ar == 255 and ag == 0 and ab == 0 and aa > 0 and aa < 255)

instead_sprite_colorkey(dup, "#ff00ff")
check("sprite copy", instead_sprite_copy(orig, 0, 0, -1, -1, dup))
check("sprite compose", instead_sprite_compose(orig, 0, 0, -1, -1, dup))

instead_sprite_free("itest/big")
check("sprite free", instead_sprite_size("itest/big") == nil)

-- fonts/text (uses the theme font shipped with the interpreter)
local fontpath = instead_realpath(".") .. "/themes/default/sans.ttf"
local fh = io.open(fontpath, "rb")
if fh then
	fh:close()
	local font = instead_font_load(fontpath, 16, "itest/font")
	check_eq("font load", font, "itest/font")
	if font then
		local tw, th = instead_sprite_text_size(font, "ABC")
		check("text size", tw > 0 and th > 0)
		local txt = instead_sprite_text(font, "ABC", "#ffffff", 0, "itest/text")
		check_eq("text sprite", txt, "itest/text")
		local sw, sh = instead_sprite_size(txt)
		check("text sprite size", sw > 0 and sh > 0)
		instead_font_free(font)
	end
else
	print("INTEGRATION SKIP: theme font not found")
end

-- src/instead_sound.c
local wav = gp .. "/assets/test.wav"

local snd = instead_sound_load(wav)
check_eq("sound load", snd, wav)
check("missing sound", instead_sound_load(gp .. "/assets/nope.wav") == nil)
check("is_sound is boolean", type(instead_sound()) == "boolean")

local vol = instead_sound_volume(-1)
check("sound volume query", type(vol) == "number")
check_eq("sound volume set", instead_sound_volume(vol), vol)
instead_sound_panning(0, 128, 128)

local mem = instead_sound_load_mem(22050, 1,
	{ 0.1, 0.2, 0.3, 0.4, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })
check("sound load_mem", mem ~= nil)

instead_sound_free(snd)
if mem then
	instead_sound_free(mem)
end
instead_sounds_free()
instead_sprites_free()

print("INTEGRATION CHECKS " .. checks)
if failed == 0 then
	print("INTEGRATION OK")
else
	print("INTEGRATION FAILED " .. failed)
end
io.stdout:flush()
