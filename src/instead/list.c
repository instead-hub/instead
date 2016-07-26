/* Licensed under BSD-MIT - see LICENSE file for details */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void list_head_init(struct list_head *h)
{
	h->n.next = h->n.prev = &h->n;
}

void list_add(struct list_head *h, struct list_node *n)
{
	n->next = h->n.next;
	n->prev = &h->n;
	h->n.next->prev = n;
	h->n.next = n;
	(void)list_debug(h);
}

void list_add_tail(struct list_head *h, struct list_node *n)
{
	n->next = &h->n;
	n->prev = h->n.prev;
	h->n.prev->next = n;
	h->n.prev = n;
	(void)list_debug(h);
}

int list_empty(const struct list_head *h)
{
	(void)list_debug(h);
	return h->n.next == &h->n;
}

void list_del(struct list_node *n)
{
	(void)list_debug_node(n);
	n->next->prev = n->prev;
	n->prev->next = n->next;
#ifdef CCAN_LIST_DEBUG
	/* Catch use-after-del. */
	n->next = n->prev = NULL;
#endif
}

void list_del_from(struct list_head *h, struct list_node *n)
{
#ifdef CCAN_LIST_DEBUG
	{
		/* Thorough check: make sure it was in list! */
		struct list_node *i;
		for (i = h->n.next; i != n; i = i->next)
			assert(i != &h->n);
	}
#endif /* CCAN_LIST_DEBUG */

	/* Quick test that catches a surprising number of bugs. */
	assert(!list_empty(h));
	list_del(n);
}

const void *list_top_(const struct list_head *h, size_t off)
{
	if (list_empty(h))
		return NULL;
	return (const char *)h->n.next - off;
}

const void *list_pop_(const struct list_head *h, size_t off)
{
	struct list_node *n;

	if (list_empty(h))
		return NULL;
	n = h->n.next;
	list_del(n);
	return (const char *)n - off;
}

const void *list_tail_(const struct list_head *h, size_t off)
{
	if (list_empty(h))
		return NULL;
	return (const char *)h->n.prev - off;
}

void list_append_list(struct list_head *to,
				    struct list_head *from)
{
	struct list_node *from_tail = list_debug(from)->n.prev;
	struct list_node *to_tail = list_debug(to)->n.prev;

	/* Sew in head and entire list. */
	to->n.prev = from_tail;
	from_tail->next = &to->n;
	to_tail->next = &from->n;
	from->n.prev = to_tail;

	/* Now remove head. */
	list_del(&from->n);
	list_head_init(from);
}

void list_prepend_list(struct list_head *to,
				     struct list_head *from)
{
	struct list_node *from_tail = list_debug(from)->n.prev;
	struct list_node *to_head = list_debug(to)->n.next;

	/* Sew in head and entire list. */
	to->n.next = &from->n;
	from->n.prev = &to->n;
	to_head->prev = from_tail;
	from_tail->next = to_head;

	/* Now remove head. */
	list_del(&from->n);
	list_head_init(from);
}

void *list_node_to_off_(struct list_node *node, size_t off)
{
	return (void *)((char *)node - off);
}

struct list_node *list_node_from_off_(void *ptr, size_t off)
{
	return (struct list_node *)((char *)ptr + off);
}

void *list_entry_or_null(const struct list_head *h,
				       const struct list_node *n,
				       size_t off)
{
	if (n == &h->n)
		return NULL;
	return (char *)n - off;
}

static void *corrupt(const char *abortstr,
		     const struct list_node *head,
		     const struct list_node *node,
		     unsigned int count)
{
	if (abortstr) {
		fprintf(stderr,
			"%s: prev corrupt in node %p (%u) of %p\n",
			abortstr, node, count, head);
		abort();
	}
	return NULL;
}

struct list_node *list_check_node(const struct list_node *node,
				  const char *abortstr)
{
	const struct list_node *p, *n;
	int count = 0;

	for (p = node, n = node->next; n != node; p = n, n = n->next) {
		count++;
		if (n->prev != p)
			return corrupt(abortstr, node, n, count);
	}
	/* Check prev on head node. */
	if (node->prev != p)
		return corrupt(abortstr, node, node, 0);

	return (struct list_node *)node;
}

struct list_head *list_check(const struct list_head *h, const char *abortstr)
{
	if (!list_check_node(&h->n, abortstr))
		return NULL;
	return (struct list_head *)h;
}
