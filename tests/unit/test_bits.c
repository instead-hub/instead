#include "test.h"
#include "instead/instead.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* minimal engine stubs: instead_bits.c only needs these two */
static lua_State *test_L = NULL;

int instead_api_register(const luaL_Reg *api)
{
	if (!test_L)
		return -1;
#if LUA_VERSION_NUM >= 502
	lua_pushglobaltable(test_L);
	luaL_setfuncs(test_L, api, 0);
#else
	luaL_register(test_L, "_G", api);
#endif
	lua_pop(test_L, 1);
	return 0;
}

int instead_extension(struct instead_ext *ext)
{
	if (!ext)
		return -1;
	if (ext->init)
		return ext->init();
	return 0;
}

extern int instead_bits_init(void);

static int push_expr(const char *expr)
{
	char buf[256];

	snprintf(buf, sizeof(buf), "return %s", expr);
	if (luaL_dostring(test_L, buf) != 0) {
		tests_checks++;
		tests_failures++;
		printf("FAIL lua %s: %s\n", expr, lua_tostring(test_L, -1));
		lua_pop(test_L, 1);
		return -1;
	}
	return 0;
}

static long long eval_int(const char *expr)
{
	long long v;

	if (push_expr(expr) != 0)
		return 0;
	v = (long long)lua_tointeger(test_L, -1);
	lua_pop(test_L, 1);
	return v;
}

void test_bits(void)
{
	test_L = luaL_newstate();
	CHECK(test_L != NULL);
	luaL_openlibs(test_L);
	CHECK_INT(instead_bits_init(), 0);

	CHECK_INT(eval_int("bit_and(0xF0, 0x0F)"), 0);
	CHECK_INT(eval_int("bit_or(0xF0, 0x0F)"), 0xFF);
	CHECK_INT(eval_int("bit_xor(0xFF, 0x0F)"), 0xF0);
	CHECK_INT(eval_int("bit_shl(1, 4)"), 16);
	CHECK_INT(eval_int("bit_shr(256, 4)"), 16);
	CHECK_INT(eval_int("bit_not(0)"), 0xFFFFFFFFLL);
	CHECK_INT(eval_int("bit_div(7, 2)"), 3);
	CHECK_INT(eval_int("bit_idiv(-7, 2)"), -3);
	CHECK_INT(eval_int("bit_mod(7, 2)"), 1);
	CHECK_INT(eval_int("bit_mul(3, 4)"), 12);
	CHECK_INT(eval_int("bit_imul(-3, 4)"), -12);
	CHECK_INT(eval_int("bit_sub(7, 4)"), 3);
	CHECK_INT(eval_int("bit_add(3, 4)"), 7);
	CHECK_INT(eval_int("bit_signed(-1)"), -1);
	CHECK_INT(eval_int("bit_unsigned(-1)"), 0xFFFFFFFFLL);

	/* optional arguments */
	CHECK_INT(eval_int("bit_and()"), 0);
	CHECK_INT(eval_int("bit_div(7)"), 7);
	CHECK_INT(eval_int("bit_mod(7)"), 0);

	/* division by zero returns no value */
	CHECK_INT(eval_int("select('#', bit_div(5, 0))"), 0);
	CHECK_INT(eval_int("select('#', bit_idiv(5, 0))"), 0);
	CHECK_INT(eval_int("select('#', bit_mod(5, 0))"), 0);

	lua_close(test_L);
	test_L = NULL;
}
