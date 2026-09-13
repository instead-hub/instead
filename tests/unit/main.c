#include "test.h"

int tests_checks = 0;
int tests_failures = 0;

void test_util(void);
void test_list(void);
void test_tinymt(void);
void test_cache(void);
void test_utils(void);
void test_bits(void);

int main(void)
{
	test_util();
	test_list();
	test_tinymt();
	test_cache();
	test_utils();
	test_bits();

	printf("%d checks, %d failures\n", tests_checks, tests_failures);
	return tests_failures ? 1 : 0;
}
