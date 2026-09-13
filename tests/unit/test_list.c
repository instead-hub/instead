#include "test.h"
#include "list.h"

struct item {
	int value;
	struct list_node n;
};

static void test_basic(void)
{
	struct list_head h;
	struct item a = { 1, { NULL, NULL } };
	struct item b = { 2, { NULL, NULL } };
	struct item c = { 3, { NULL, NULL } };
	struct item *it;

	list_head_init(&h);
	CHECK(list_empty(&h));
	CHECK(list_check(&h, NULL) == &h);

	list_add_tail(&h, &a.n);
	list_add_tail(&h, &b.n);
	list_add(&h, &c.n); /* c, a, b */

	CHECK(!list_empty(&h));
	CHECK_INT(list_top(&h, struct item, n)->value, 3);
	CHECK_INT(list_tail(&h, struct item, n)->value, 2);
	it = list_next(&h, &c, n);
	CHECK_INT(it->value, 1);
	it = list_next(&h, &a, n);
	CHECK_INT(it->value, 2);
	CHECK(list_next(&h, &b, n) == NULL);
	it = list_prev(&h, &b, n);
	CHECK_INT(it->value, 1);
	CHECK(list_prev(&h, &c, n) == NULL);

	it = list_pop(&h, struct item, n);
	CHECK_INT(it->value, 3);
	list_del(&b.n);
	CHECK_INT(list_tail(&h, struct item, n)->value, 1);
	list_del_from(&h, &a.n);
	CHECK(list_empty(&h));
}

static void test_append_prepend(void)
{
	struct list_head to, from;
	struct item a = { 1, { NULL, NULL } };
	struct item b = { 2, { NULL, NULL } };
	struct item c = { 3, { NULL, NULL } };
	struct item d = { 4, { NULL, NULL } };
	struct item *it;

	list_head_init(&to);
	list_head_init(&from);
	list_add_tail(&to, &a.n);
	list_add_tail(&to, &b.n);
	list_add_tail(&from, &c.n);
	list_add_tail(&from, &d.n);

	list_append_list(&to, &from);
	CHECK(list_empty(&from));
	CHECK_INT(list_top(&to, struct item, n)->value, 1);
	CHECK_INT(list_tail(&to, struct item, n)->value, 4);
	it = list_next(&to, &a, n);
	CHECK_INT(it->value, 2);
	it = list_next(&to, &b, n);
	CHECK_INT(it->value, 3);

	list_del(&c.n);
	list_del(&d.n);
	list_head_init(&from);
	list_add_tail(&from, &c.n);
	list_add_tail(&from, &d.n);

	list_prepend_list(&to, &from);
	CHECK(list_empty(&from));
	CHECK_INT(list_top(&to, struct item, n)->value, 3);
	CHECK_INT(list_tail(&to, struct item, n)->value, 2);
	it = list_next(&to, &d, n);
	CHECK_INT(it->value, 1);
	CHECK(list_entry_or_null(&to, to.n.next, offsetof(struct item, n)) == (void *)&c);
}

void test_list(void)
{
	test_basic();
	test_append_prepend();
}
