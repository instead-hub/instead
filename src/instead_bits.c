/*
 * Copyright 2009-2017 Peter Kosyh <p.kosyh at gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "externals.h"
#include "instead/instead.h"

static int luaB_bit_and(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a & b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_or(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a | b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_xor(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a ^ b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_shl(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a << b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_shr(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a >> b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_not(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int r = ~a;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_div(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 1);
	unsigned int r;
	if (b) {
		r = a / b;
		lua_pushinteger(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_idiv(lua_State *L) {
	int a = luaL_optinteger(L, 1, 0);
	int b = luaL_optinteger(L, 2, 1);
	int r;
	if (b) {
		r = a / b;
		lua_pushinteger(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_mod(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 1);
	unsigned int r;
	if (b) {
		r = a % b;
		lua_pushinteger(L, r);
		return 1;
	}
	return 0;
}

static int luaB_bit_mul(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a * b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_imul(lua_State *L) {
	int a = luaL_optinteger(L, 1, 0);
	int b = luaL_optinteger(L, 2, 0);
	int r = a * b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_sub(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a - b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_add(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	unsigned int b = luaL_optinteger(L, 2, 0);
	unsigned int r = a + b;
	lua_pushinteger(L, r);
	return 1;
}

static int luaB_bit_unsigned(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	lua_pushinteger(L, a);
	return 1;
}

static int luaB_bit_signed(lua_State *L) {
	unsigned int a = luaL_optinteger(L, 1, 0);
	lua_pushinteger(L, (int)a);
	return 1;
}

const luaL_Reg bits_funcs[] = {
	{"bit_or", luaB_bit_or},
	{"bit_and", luaB_bit_and},
	{"bit_xor", luaB_bit_xor},
	{"bit_shl", luaB_bit_shl},
	{"bit_shr", luaB_bit_shr},
	{"bit_not", luaB_bit_not},
	{"bit_div", luaB_bit_div},
	{"bit_idiv", luaB_bit_idiv},
	{"bit_mod", luaB_bit_mod},
	{"bit_mul", luaB_bit_mul},
	{"bit_imul", luaB_bit_imul},
	{"bit_sub", luaB_bit_sub},
	{"bit_add", luaB_bit_add},
	{"bit_signed", luaB_bit_signed},
	{"bit_unsigned", luaB_bit_unsigned},

	{NULL, NULL}
};

static int bits_init(void)
{
	return instead_api_register(bits_funcs);
}

static struct instead_ext ext = {
	.init = bits_init,
};

int instead_bits_init(void)
{
	return instead_extension(&ext);
}
