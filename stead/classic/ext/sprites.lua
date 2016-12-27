stead.ticks = instead_ticks
stead.mouse_pos = instead_mouse_pos
stead.mouse_show = instead_mouse_show
stead.finger_pos = instead_finger_pos
stead.mouse_filter = instead_mouse_filter
stead.busy = instead_busy

mouse_pos = stead.mouse_pos
mouse_filter = stead.mouse_filter

get_ticks = stead.ticks

local function compat_api()
	if not stead.api_atleast(1, 7, 1) then
		stead_busy = instead_busy
		theme_var = instead_theme_var
		theme_name = instead_theme_name
		get_themespath = instead_themespath
	end
end

stead.module_start(function(load)
	if compat_api and not load then
		compat_api()
		compat_api = nil
	end
end)
