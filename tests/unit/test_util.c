#include "test.h"
#include "util.h"
#include "tinymt32.h"

#include <stdlib.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static void test_get_utf8(void)
{
	unsigned long sym = 0;

	CHECK_INT(get_utf8("A", &sym), 1);
	CHECK_INT(sym, 'A');
	CHECK_INT(get_utf8("\xD0\xAF", &sym), 2); /* U+042F я */
	CHECK_INT(sym, 0x42F);
	CHECK_INT(get_utf8("\xE2\x82\xAC", &sym), 3); /* U+20AC € */
	CHECK_INT(sym, 0x20AC);
	CHECK_INT(get_utf8("\xF0\x9F\x98\x80", &sym), 4); /* U+1F600 */
	CHECK_INT(sym, 0x1F600);
	CHECK_INT(get_utf8("\xD0" "A", &sym), 1); /* broken continuation */
	CHECK_INT(get_utf8("\x80", &sym), 1); /* stray continuation */
	CHECK_INT(get_utf8("", &sym), 0);
	CHECK_INT(get_utf8("A", NULL), 1);
}

static void test_is_cjk(void)
{
	CHECK_INT(is_cjk(0x2E7F), 0);
	CHECK_INT(is_cjk(0x2E80), 1);
	CHECK_INT(is_cjk(0x3040), 1);
	CHECK_INT(is_cjk(0x30FF), 1);
	CHECK_INT(is_cjk(0x4E00), 1);
	CHECK_INT(is_cjk(0x9FFF), 1);
	CHECK_INT(is_cjk(0xA000), 1);
	CHECK_INT(is_cjk(0xA4CF), 1);
	CHECK_INT(is_cjk(0xA4D0), 0);
	CHECK_INT(is_cjk(0xD7AF), 1);
	CHECK_INT(is_cjk(0xD7B0), 0);
	CHECK_INT(is_cjk(0xF900), 1);
	CHECK_INT(is_cjk(0xFB00), 0);
	CHECK_INT(is_cjk('A'), 0);
}

static void test_is_space_empty(void)
{
	CHECK_INT(is_space(' '), 1);
	CHECK_INT(is_space('\t'), 1);
	CHECK_INT(is_space('\n'), 0);
	CHECK_INT(is_space('x'), 0);

	CHECK_INT(is_empty(NULL), 1);
	CHECK_INT(is_empty(""), 1);
	CHECK_INT(is_empty(" \t "), 1);
	CHECK_INT(is_empty(" x "), 0);
	CHECK_INT(is_empty("\n"), 0);
}

static void test_strip(void)
{
	char s1[] = "  hi \t ";
	char s2[] = "   ";
	char s3[] = "x";
	char s4[] = " x";

	CHECK_STR(strip(s1), "hi");
	CHECK_STR(strip(s2), "");
	CHECK_STR(strip(s3), "x");
	CHECK_STR(strip(s4), "x");
}

static void test_tolow(void)
{
	char s[] = "Hello World 123";

	tolow(s);
	CHECK_STR(s, "hello world 123");
}

static void test_strlowcmp(void)
{
	CHECK(strlowcmp("AbC", "abc") == 0);
	CHECK(strlowcmp("abc", "abd") < 0);
	CHECK(strlowcmp("ABD", "abc") > 0);
	CHECK(strlowcmp("", "") == 0);
}

static void test_unix_path(void)
{
	char p[] = "C:\\games\\instead\\main.lua";

	unix_path(p);
	CHECK_STR(p, "C:/games/instead/main.lua");
	unix_path(NULL); /* must not crash */
}

static void test_paths(void)
{
	char *p;

	p = getfilepath("/games", "main.lua");
	CHECK_STR(p, "/games/main.lua");
	free(p);

	p = getfilepath("/games/", "main.lua");
	CHECK_STR(p, "/games/main.lua");
	free(p);

	p = getfilepath(NULL, "main.lua");
	CHECK_STR(p, "main.lua");
	free(p);

	p = getfilepath("", "main.lua");
	CHECK_STR(p, "main.lua");
	free(p);

	p = getfilepath("dir", NULL);
	CHECK_STR(p, "dir/");
	free(p);

	p = getpath("dir", "main.lua");
	CHECK_STR(p, "dir/main.lua/");
	free(p);
}

static void test_absolute_path(void)
{
	CHECK_INT(is_absolute_path("/usr/share"), 1);
	CHECK_INT(is_absolute_path("usr/share"), 0);
	CHECK_INT(is_absolute_path(""), 0);
	CHECK_INT(is_absolute_path(NULL), 0);
}

static void test_getrealpath(void)
{
	char buf[PATH_MAX];

	CHECK(getrealpath(NULL, NULL) == NULL);
	CHECK_STR(getrealpath("/", buf), "/");
}

static void test_random(void)
{
	tinymt32_t ref;
	int i;

	ref.mat1 = TINYMT32_MAT1;
	ref.mat2 = TINYMT32_MAT2;
	ref.tmat = TINYMT32_TMAT;
	tinymt32_init(&ref, 42);
	mt_random_seed(42);
	for (i = 0; i < 4; i++)
		CHECK_INT(mt_random(), tinymt32_generate_uint32(&ref));
	CHECK(mt_random_double() == tinymt32_generate_32double(&ref));
}

void test_util(void)
{
	test_get_utf8();
	test_is_cjk();
	test_is_space_empty();
	test_strip();
	test_tolow();
	test_strlowcmp();
	test_unix_path();
	test_paths();
	test_absolute_path();
	test_getrealpath();
	test_random();
}
