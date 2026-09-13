#include "test.h"
#include "tinymt32.h"

#include <math.h>

/* official TinyMT32 test vectors, see
 * https://github.com/MersenneTwister-Lab/TinyMT (tinymt/check32.out.txt)
 */
static void test_seed1_uint32(void)
{
	static const Uint32 expected[10] = {
		2545341989u, 981918433u, 3715302833u, 2387538352u, 3591001365u,
		3820442102u, 2114400566u, 2196103051u, 2783359912u, 764534509u
	};
	tinymt32_t r;
	int i;

	r.mat1 = TINYMT32_MAT1;
	r.mat2 = TINYMT32_MAT2;
	r.tmat = TINYMT32_TMAT;
	tinymt32_init(&r, 1);
	for (i = 0; i < 10; i++)
		CHECK_INT(tinymt32_generate_uint32(&r), expected[i]);
}

static void test_init_by_array(void)
{
	static const float expected[5] = {
		0.0132459f, 0.2083899f, 0.1457998f, 0.1144078f, 0.6173239f
	};
	Uint32 key[1] = { 1 };
	tinymt32_t r;
	int i;

	r.mat1 = TINYMT32_MAT1;
	r.mat2 = TINYMT32_MAT2;
	r.tmat = TINYMT32_TMAT;
	tinymt32_init_by_array(&r, key, 1);
	for (i = 0; i < 5; i++)
		CHECK(fabsf(tinymt32_generate_float(&r) - expected[i]) < 1e-5f);
}

static void test_determinism(void)
{
	tinymt32_t a, b;
	int i;

	a.mat1 = b.mat1 = TINYMT32_MAT1;
	a.mat2 = b.mat2 = TINYMT32_MAT2;
	a.tmat = b.tmat = TINYMT32_TMAT;
	tinymt32_init(&a, 12345);
	tinymt32_init(&b, 12345);
	for (i = 0; i < 16; i++)
		CHECK(tinymt32_generate_uint32(&a) == tinymt32_generate_uint32(&b));
}

void test_tinymt(void)
{
	test_seed1_uint32();
	test_init_by_array();
	test_determinism();
}
