#ifndef INSTEAD_TESTS_TEST_H
#define INSTEAD_TESTS_TEST_H

#include <stdio.h>
#include <string.h>

extern int tests_checks;
extern int tests_failures;

#define CHECK(cond) do { \
	tests_checks++; \
	if (!(cond)) { \
		tests_failures++; \
		printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
	} \
} while (0)

#define CHECK_INT(got, want) do { \
	long long _got = (long long)(got); \
	long long _want = (long long)(want); \
	tests_checks++; \
	if (_got != _want) { \
		tests_failures++; \
		printf("FAIL %s:%d: %s == %lld, expected %lld\n", \
			__FILE__, __LINE__, #got, _got, _want); \
	} \
} while (0)

#define CHECK_STR(got, want) do { \
	const char *_got = (got); \
	const char *_want = (want); \
	tests_checks++; \
	if (!_got || !_want || strcmp(_got, _want) != 0) { \
		tests_failures++; \
		printf("FAIL %s:%d: %s == \"%s\", expected \"%s\"\n", \
			__FILE__, __LINE__, #got, \
			_got ? _got : "(null)", _want ? _want : "(null)"); \
	} \
} while (0)

#endif
