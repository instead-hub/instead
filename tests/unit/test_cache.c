#include "test.h"
#include "cache.h"

static int freed = 0;

static void free_cb(void *p)
{
	(void)p;
	freed++;
}

static void test_hash(void)
{
	int a = 0, b = 0;

	CHECK(hash_string("abc") == hash_string("abc"));
	CHECK(hash_string("abc") != hash_string("abd"));
	CHECK(hash_addr(&a) == hash_addr(&a));
	CHECK(hash_addr(&a) != hash_addr(&b));
}

static void test_cache_basic(void)
{
	cache_t c = cache_init(2, free_cb);
	int a = 1, b = 2, d = 3;
	int i;

	CHECK(c != NULL);
	CHECK_INT(cache_add(c, "a", &a), 0);
	CHECK_INT(cache_add(c, "b", &b), 0);
	CHECK_INT(cache_add(c, "c", &d), 0);
	CHECK_INT(cache_add(c, "a", &b), -1); /* busy entry */
	CHECK(cache_lookup(c, "a") == &a);
	CHECK(cache_lookup(c, "b") == &b);
	CHECK(cache_lookup(c, "c") == &d);
	CHECK(cache_lookup(c, "x") == NULL);
	CHECK(cache_get(c, "a") == &a);
	CHECK_INT(cache_have(c, &a), 0);
	CHECK_INT(cache_have(c, &d), 0);

	/* release 'a', shrink evicts it: size 3 > max_size 2 */
	CHECK_INT(cache_forget(c, &a), 0); /* used 2 -> 1 */
	CHECK_INT(cache_forget(c, &a), 0); /* used 1 -> 0, now unused */
	CHECK_INT(cache_forget(c, &a), -1); /* already unused */
	cache_shrink(c);
	CHECK_INT(freed, 1);
	CHECK(cache_lookup(c, "a") == NULL);
	CHECK(cache_lookup(c, "b") == &b);

	/* re-adding allocates a fresh entry */
	CHECK_INT(cache_add(c, "a", &a), 0);
	CHECK_INT(freed, 1);
	CHECK(cache_get(c, "a") == &a);
	CHECK(cache_get(c, "a") == &a);
	CHECK_INT(cache_forget(c, &a), 0);
	cache_shrink(c);
	CHECK_INT(freed, 1); /* still in use once */

	for (i = 0; i < 64; i++) {
		static int values[64];
		char name[16];
		snprintf(name, sizeof(name), "key%d", i);
		CHECK_INT(cache_add(c, name, &values[i]), 0);
	}
	CHECK(cache_lookup(c, "key63") != NULL);

	cache_zap(c);
	CHECK(cache_lookup(c, "a") == NULL);
	CHECK(cache_lookup(c, "key63") == NULL);
	CHECK_INT(freed, 1); /* live entries are owned by the caller */
	cache_free(c);
}

void test_cache(void)
{
	test_hash();
	test_cache_basic();
}
