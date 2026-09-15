#include "test.h"
#include "utils.h"

#include <stdlib.h>

/* the real sdl_path() lives in unix.c/windows.c */
char *sdl_path(char *path)
{
	return path;
}

/* utils.c references idf, but the tests below do not exercise it */
idff_t idf_open(idf_t idf, const char *fname)
{
	(void)idf;
	(void)fname;
	return NULL;
}

int idf_seek(idff_t fil, int offset, int whence)
{
	(void)fil;
	(void)offset;
	(void)whence;
	return -1;
}

int idf_close(idff_t fil)
{
	(void)fil;
	return -1;
}

char *idf_gets(idff_t idf, char *b, int size)
{
	(void)idf;
	(void)b;
	(void)size;
	return NULL;
}

static int cmd_called = 0;

static int cmd_fn(const char *v, void *p)
{
	(void)v;
	(void)p;
	cmd_called++;
	return 0;
}

static void test_process_cmd(void)
{
	static struct parser cmds[] = {
		{ "test", cmd_fn, NULL, 0 },
		{ NULL, NULL, NULL, 0 },
	};
	char n[] = " test ";
	char v[] = " value ";

	CHECK_INT(process_cmd(n, v, cmds), 0);
	CHECK_INT(cmd_called, 1);
	CHECK_INT(process_cmd("unknown", "value", cmds), -1);
	CHECK_INT(cmd_called, 1);
}

static void test_escape(void)
{
	char *e, *p = NULL;

	e = encode_esc_string("a b\"c'd\\e");
	CHECK_STR(e, "a\\ b\\\"c\\'d\\\\e");
	free(e);
	CHECK(encode_esc_string(NULL) == NULL);

	CHECK_INT(parse_esc_string("a\\nb", &p), 0);
	CHECK_STR(p, "a\nb");
	CHECK_INT(parse_esc_string("x\\\\y", &p), 0);
	CHECK_STR(p, "x\\y");
	CHECK_INT(parse_esc_string("a\\$b", &p), 0);
	CHECK_STR(p, "a$b");
	CHECK_INT(parse_esc_string("a\\rb", &p), 0);
	CHECK_STR(p, "a\nb");
	CHECK_INT(parse_esc_string("a\\qb", &p), 0);
	CHECK_STR(p, "aqb");

	/* parse_string frees the previous value */
	CHECK_INT(parse_string("hello", &p), 0);
	CHECK_STR(p, "hello");
	CHECK_INT(parse_string("world", &p), 0);
	CHECK_STR(p, "world");
	free(p);
}

static void test_find_in_esc(void)
{
	const char *s = "a\\;b;c";
	const char *nul;

	CHECK(find_in_esc(s, "\\;") == s + 4);
	/* no unescaped separator: points at the terminating NUL */
	nul = find_in_esc("abc\\", ";");
	CHECK(nul != NULL && *nul == '\0');
	CHECK(find_in_esc("", ";") == NULL);
}

static void test_parse_numbers(void)
{
	int i = 0;
	float f = 0;

	CHECK_INT(parse_int("10", &i), 0);
	CHECK_INT(i, 10);
	CHECK_INT(parse_int("0x1f", &i), 0);
	CHECK_INT(i, 31);
	CHECK_INT(parse_int("10x", &i), -1);

	CHECK_INT(parse_float("1.5", &f), 0);
	CHECK(f > 1.49f && f < 1.51f);
	CHECK_INT(parse_float("x", &f), -1);
}

static void test_parse_path(void)
{
	char *p = NULL;

	CHECK_INT(parse_path("a\\b", &p), 0);
	CHECK_STR(p, "a/b");
	CHECK_INT(parse_path("", &p), 0);
	CHECK_STR(p, "");
	free(p);
}

static void test_hex(void)
{
	unsigned char data[4] = { 0x00, 0xab, 0x7f, 0xff };
	unsigned char out[4] = { 0xaa, 0xaa, 0xaa, 0xaa };
	char hex[9] = { 0 };

	data2hex(data, (int)sizeof(data), hex);
	/* low nibble goes first */
	CHECK_STR(hex, "00baf7ff");
	CHECK_INT(hex2data(hex, out, (int)sizeof(out)), 8);
	CHECK(memcmp(data, out, sizeof(data)) == 0);
	CHECK_INT(hex2data("zz", out, 1), 0);
}

void test_utils(void)
{
	test_process_cmd();
	test_escape();
	test_find_in_esc();
	test_parse_numbers();
	test_parse_path();
	test_hex();
}
