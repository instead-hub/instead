#include "test.h"
#include "xoshiro128.h"

/* reference vectors generated with the public domain xoshiro128**
 * reference implementation (Blackman, Vigna, 2018), state initialized
 * with splitmix32; see http://prng.di.unimi.it/
 */
static void test_seed1_uint32(void)
{
	static const Uint32 expected[10] = {
		393288148u, 2174103013u, 3814759091u, 2092745082u, 1865176206u,
		2179171167u, 3207394750u, 2858353069u, 559075315u, 3395495274u
	};
	xoshiro128_t r;
	int i;

	xoshiro128_init(&r, 1);
	for (i = 0; i < 10; i++)
		CHECK_INT(xoshiro128_next(&r), expected[i]);
}

static void test_seed0_uint32(void)
{
	static const Uint32 expected[10] = {
		1789933344u, 44971166u, 2521387044u, 3848737593u, 1138324114u,
		749234105u, 1899511038u, 1995189375u, 3629653958u, 19166872u
	};
	xoshiro128_t r;
	int i;

	xoshiro128_init(&r, 0);
	for (i = 0; i < 10; i++)
		CHECK_INT(xoshiro128_next(&r), expected[i]);
}

static void test_double(void)
{
	xoshiro128_t r;
	int i;

	xoshiro128_init(&r, 12345);
	for (i = 0; i < 1000; i++) {
		double v = xoshiro128_double(&r);
		CHECK(v >= 0.0 && v < 1.0);
	}
}

static void test_determinism(void)
{
	xoshiro128_t a, b;
	int i;

	xoshiro128_init(&a, 12345);
	xoshiro128_init(&b, 12345);
	for (i = 0; i < 16; i++)
		CHECK(xoshiro128_next(&a) == xoshiro128_next(&b));
}

void test_xoshiro(void)
{
	test_seed1_uint32();
	test_seed0_uint32();
	test_double();
	test_determinism();
}
