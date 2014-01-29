/* Licensed under BSD-MIT - see LICENSE file for details */
#ifndef CCAN_LIST_H
#define CCAN_LIST_H

#include <assert.h>
#include <stddef.h>

/**
 * BUILD_ASSERT - assert a build-time dependency.
 * @cond: the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't be evaluated
 * by the compiler.  This can only be used within a function.
 *
 * Example:
 *	#include <stddef.h>
 *	...
 *	static char *foo_to_char(struct foo *foo)
 *	{
 *		// This code needs string to be at start of foo.
 *		BUILD_ASSERT(offsetof(struct foo, string) == 0);
 *		return (char *)foo;
 *	}
 */
#define BUILD_ASSERT(cond) \
	do { (void) sizeof(char [1 - 2*!(cond)]); } while(0)

/**
 * BUILD_ASSERT_OR_ZERO - assert a build-time dependency, as an expression.
 * @cond: the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't be evaluated
 * by the compiler.  This can be used in an expression: its value is "0".
 *
 * Example:
 *	#define foo_to_char(foo)					\
 *		 ((char *)(foo)						\
 *		  + BUILD_ASSERT_OR_ZERO(offsetof(struct foo, string) == 0))
 */
#define BUILD_ASSERT_OR_ZERO(cond) \
	(sizeof(char [1 - 2*!(cond)]) - 1)

/**
 * check_type - issue a warning or build failure if type is not correct.
 * @expr: the expression whose type we should check (not evaluated).
 * @type: the exact type we expect the expression to be.
 *
 * This macro is usually used within other macros to try to ensure that a macro
 * argument is of the expected type.  No type promotion of the expression is
 * done: an unsigned int is not the same as an int!
 *
 * check_type() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do is fail
 * to compile if the sizes of the types are unequal (a less complete check).
 *
 * Example:
 *	// They should always pass a 64-bit value to _set_some_value!
 *	#define set_some_value(expr)			\
 *		_set_some_value((check_type((expr), uint64_t), (expr)))
 */

/**
 * check_types_match - issue a warning or build failure if types are not same.
 * @expr1: the first expression (not evaluated).
 * @expr2: the second expression (not evaluated).
 *
 * This macro is usually used within other macros to try to ensure that
 * arguments are of identical types.  No type promotion of the expressions is
 * done: an unsigned int is not the same as an int!
 *
 * check_types_match() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do is fail
 * to compile if the sizes of the types are unequal (a less complete check).
 *
 * Example:
 *	// Do subtraction to get to enclosing type, but make sure that
 *	// pointer is of correct type for that member.
 *	#define container_of(mbr_ptr, encl_type, mbr)			\
 *		(check_types_match((mbr_ptr), &((encl_type *)0)->mbr),	\
 *		 ((encl_type *)						\
 *		  ((char *)(mbr_ptr) - offsetof(enclosing_type, mbr))))
 */
#if HAVE_TYPEOF
#define check_type(expr, type)			\
	((typeof(expr) *)0 != (type *)0)

#define check_types_match(expr1, expr2)		\
	((typeof(expr1) *)0 != (typeof(expr2) *)0)
#else
/* Without typeof, we can only test the sizes. */
#define check_type(expr, type)					\
	BUILD_ASSERT_OR_ZERO(sizeof(expr) == sizeof(type))

#define check_types_match(expr1, expr2)				\
	BUILD_ASSERT_OR_ZERO(sizeof(expr1) == sizeof(expr2))
#endif /* HAVE_TYPEOF */

/**
 * container_of - get pointer to enclosing structure
 * @member_ptr: pointer to the structure member
 * @containing_type: the type this member is within
 * @member: the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * Example:
 *	struct foo {
 *		int fielda, fieldb;
 *		// ...
 *	};
 *	struct info {
 *		int some_other_field;
 *		struct foo my_foo;
 *	};
 *
 *	static struct info *foo_to_info(struct foo *foo)
 *	{
 *		return container_of(foo, struct info, my_foo);
 *	}
 */
#define container_of(member_ptr, containing_type, member)		\
	 ((containing_type *)						\
	  ((char *)(member_ptr)						\
	   - container_off(containing_type, member))			\
	  + check_types_match(*(member_ptr), ((containing_type *)0)->member))

/**
 * container_off - get offset to enclosing structure
 * @containing_type: the type this member is within
 * @member: the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does
 * typechecking and figures out the offset to the enclosing type.
 *
 * Example:
 *	struct foo {
 *		int fielda, fieldb;
 *		// ...
 *	};
 *	struct info {
 *		int some_other_field;
 *		struct foo my_foo;
 *	};
 *
 *	static struct info *foo_to_info(struct foo *foo)
 *	{
 *		size_t off = container_off(struct info, my_foo);
 *		return (void *)((char *)foo - off);
 *	}
 */
#define container_off(containing_type, member)	\
	offsetof(containing_type, member)

/**
 * container_of_var - get pointer to enclosing structure using a variable
 * @member_ptr: pointer to the structure member
 * @container_var: a pointer of same type as this member's container
 * @member: the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * Example:
 *	static struct info *foo_to_i(struct foo *foo)
 *	{
 *		struct info *i = container_of_var(foo, i, my_foo);
 *		return i;
 *	}
 */
#if HAVE_TYPEOF
#define container_of_var(member_ptr, container_var, member) \
	container_of(member_ptr, typeof(*container_var), member)
#else
#define container_of_var(member_ptr, container_var, member)	\
	((void *)((char *)(member_ptr)	-			\
		  container_off_var(container_var, member)))
#endif

/**
 * container_off_var - get offset of a field in enclosing structure
 * @container_var: a pointer to a container structure
 * @member: the name of a member within the structure.
 *
 * Given (any) pointer to a structure and a its member name, this
 * macro does pointer subtraction to return offset of member in a
 * structure memory layout.
 *
 */
#if HAVE_TYPEOF
#define container_off_var(var, member)		\
	container_off(typeof(*var), member)
#else
#define container_off_var(var, member)			\
	((char *)&(var)->member - (char *)(var))
#endif


/**
 * struct list_node - an entry in a doubly-linked list
 * @next: next entry (self if empty)
 * @prev: previous entry (self if empty)
 *
 * This is used as an entry in a linked list.
 * Example:
 *	struct child {
 *		const char *name;
 *		// Linked list of all us children.
 *		struct list_node list;
 *	};
 */
struct list_node
{
	struct list_node *next, *prev;
};

/**
 * struct list_head - the head of a doubly-linked list
 * @h: the list_head (containing next and prev pointers)
 *
 * This is used as the head of a linked list.
 * Example:
 *	struct parent {
 *		const char *name;
 *		struct list_head children;
 *		unsigned int num_children;
 *	};
 */
struct list_head
{
	struct list_node n;
};

/**
 * list_check - check head of a list for consistency
 * @h: the list_head
 * @abortstr: the location to print on aborting, or NULL.
 *
 * Because list_nodes have redundant information, consistency checking between
 * the back and forward links can be done.  This is useful as a debugging check.
 * If @abortstr is non-NULL, that will be printed in a diagnostic if the list
 * is inconsistent, and the function will abort.
 *
 * Returns the list head if the list is consistent, NULL if not (it
 * can never return NULL if @abortstr is set).
 *
 * See also: list_check_node()
 *
 * Example:
 *	static void dump_parent(struct parent *p)
 *	{
 *		struct child *c;
 *
 *		printf("%s (%u children):\n", p->name, p->num_children);
 *		list_check(&p->children, "bad child list");
 *		list_for_each(&p->children, c, list)
 *			printf(" -> %s\n", c->name);
 *	}
 */
struct list_head *list_check(const struct list_head *h, const char *abortstr);

/**
 * list_check_node - check node of a list for consistency
 * @n: the list_node
 * @abortstr: the location to print on aborting, or NULL.
 *
 * Check consistency of the list node is in (it must be in one).
 *
 * See also: list_check()
 *
 * Example:
 *	static void dump_child(const struct child *c)
 *	{
 *		list_check_node(&c->list, "bad child list");
 *		printf("%s\n", c->name);
 *	}
 */
struct list_node *list_check_node(const struct list_node *n,
				  const char *abortstr);

#ifdef CCAN_LIST_DEBUG
#define list_debug(h) list_check((h), __func__)
#define list_debug_node(n) list_check_node((n), __func__)
#else
#define list_debug(h) (h)
#define list_debug_node(n) (n)
#endif

/**
 * LIST_HEAD_INIT - initializer for an empty list_head
 * @name: the name of the list.
 *
 * Explicit initializer for an empty list.
 *
 * See also:
 *	LIST_HEAD, list_head_init()
 *
 * Example:
 *	static struct list_head my_list = LIST_HEAD_INIT(my_list);
 */
#define LIST_HEAD_INIT(name) { { &name.n, &name.n } }

/**
 * LIST_HEAD - define and initialize an empty list_head
 * @name: the name of the list.
 *
 * The LIST_HEAD macro defines a list_head and initializes it to an empty
 * list.  It can be prepended by "static" to define a static list_head.
 *
 * See also:
 *	LIST_HEAD_INIT, list_head_init()
 *
 * Example:
 *	static LIST_HEAD(my_global_list);
 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * list_head_init - initialize a list_head
 * @h: the list_head to set to the empty list
 *
 * Example:
 *	...
 *	struct parent *parent = malloc(sizeof(*parent));
 *
 *	list_head_init(&parent->children);
 *	parent->num_children = 0;
 */
void list_head_init(struct list_head *h);

/**
 * list_add - add an entry at the start of a linked list.
 * @h: the list_head to add the node to
 * @n: the list_node to add to the list.
 *
 * The list_node does not need to be initialized; it will be overwritten.
 * Example:
 *	struct child *child = malloc(sizeof(*child));
 *
 *	child->name = "marvin";
 *	list_add(&parent->children, &child->list);
 *	parent->num_children++;
 */
void list_add(struct list_head *h, struct list_node *n);

/**
 * list_add_tail - add an entry at the end of a linked list.
 * @h: the list_head to add the node to
 * @n: the list_node to add to the list.
 *
 * The list_node does not need to be initialized; it will be overwritten.
 * Example:
 *	list_add_tail(&parent->children, &child->list);
 *	parent->num_children++;
 */
void list_add_tail(struct list_head *h, struct list_node *n);

/**
 * list_empty - is a list empty?
 * @h: the list_head
 *
 * If the list is empty, returns true.
 *
 * Example:
 *	assert(list_empty(&parent->children) == (parent->num_children == 0));
 */

int list_empty(const struct list_head *h);

/**
 * list_del - delete an entry from an (unknown) linked list.
 * @n: the list_node to delete from the list.
 *
 * Note that this leaves @n in an undefined state; it can be added to
 * another list, but not deleted again.
 *
 * See also:
 *	list_del_from()
 *
 * Example:
 *	list_del(&child->list);
 *	parent->num_children--;
 */

void list_del(struct list_node *n);

/**
 * list_del_from - delete an entry from a known linked list.
 * @h: the list_head the node is in.
 * @n: the list_node to delete from the list.
 *
 * This explicitly indicates which list a node is expected to be in,
 * which is better documentation and can catch more bugs.
 *
 * See also: list_del()
 *
 * Example:
 *	list_del_from(&parent->children, &child->list);
 *	parent->num_children--;
 */

void list_del_from(struct list_head *h, struct list_node *n);

/**
 * list_entry - convert a list_node back into the structure containing it.
 * @n: the list_node
 * @type: the type of the entry
 * @member: the list_node member of the type
 *
 * Example:
 *	// First list entry is children.next; convert back to child.
 *	child = list_entry(parent->children.n.next, struct child, list);
 *
 * See Also:
 *	list_top(), list_for_each()
 */
#define list_entry(n, type, member) container_of(n, type, member)

/**
 * list_top - get the first entry in a list
 * @h: the list_head
 * @type: the type of the entry
 * @member: the list_node member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *first;
 *	first = list_top(&parent->children, struct child, list);
 *	if (!first)
 *		printf("Empty list!\n");
 */
#define list_top(h, type, member)					\
	((type *)list_top_((h), list_off_(type, member)))

const void *list_top_(const struct list_head *h, size_t off);

/**
 * list_pop - remove the first entry in a list
 * @h: the list_head
 * @type: the type of the entry
 * @member: the list_node member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *one;
 *	one = list_pop(&parent->children, struct child, list);
 *	if (!one)
 *		printf("Empty list!\n");
 */
#define list_pop(h, type, member)					\
	((type *)list_pop_((h), list_off_(type, member)))

const void *list_pop_(const struct list_head *h, size_t off);

/**
 * list_tail - get the last entry in a list
 * @h: the list_head
 * @type: the type of the entry
 * @member: the list_node member of the type
 *
 * If the list is empty, returns NULL.
 *
 * Example:
 *	struct child *last;
 *	last = list_tail(&parent->children, struct child, list);
 *	if (!last)
 *		printf("Empty list!\n");
 */
#define list_tail(h, type, member) \
	((type *)list_tail_((h), list_off_(type, member)))

const void *list_tail_(const struct list_head *h, size_t off);

/**
 * list_for_each - iterate through a list.
 * @h: the list_head (warning: evaluated multiple times!)
 * @i: the structure containing the list_node
 * @member: the list_node member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.
 *
 * Example:
 *	list_for_each(&parent->children, child, list)
 *		printf("Name: %s\n", child->name);
 */
#define list_for_each(h, i, member)					\
	list_for_each_off(h, i, list_off_var_(i, member))

/**
 * list_for_each_rev - iterate through a list backwards.
 * @h: the list_head
 * @i: the structure containing the list_node
 * @member: the list_node member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.
 *
 * Example:
 *	list_for_each_rev(&parent->children, child, list)
 *		printf("Name: %s\n", child->name);
 */
#define list_for_each_rev(h, i, member)					\
	for (i = container_of_var(list_debug(h)->n.prev, i, member);	\
	     &i->member != &(h)->n;					\
	     i = container_of_var(i->member.prev, i, member))

/**
 * list_for_each_safe - iterate through a list, maybe during deletion
 * @h: the list_head
 * @i: the structure containing the list_node
 * @nxt: the structure containing the list_node
 * @member: the list_node member of the structure
 *
 * This is a convenient wrapper to iterate @i over the entire list.  It's
 * a for loop, so you can break and continue as normal.  The extra variable
 * @nxt is used to hold the next element, so you can delete @i from the list.
 *
 * Example:
 *	struct child *next;
 *	list_for_each_safe(&parent->children, child, next, list) {
 *		list_del(&child->list);
 *		parent->num_children--;
 *	}
 */
#define list_for_each_safe(h, i, nxt, member)				\
	list_for_each_safe_off(h, i, nxt, list_off_var_(i, member))

/**
 * list_next - get the next entry in a list
 * @h: the list_head
 * @i: a pointer to an entry in the list.
 * @member: the list_node member of the structure
 *
 * If @i was the last entry in the list, returns NULL.
 *
 * Example:
 *	struct child *second;
 *	second = list_next(&parent->children, first, list);
 *	if (!second)
 *		printf("No second child!\n");
 */
#define list_next(h, i, member)						\
	((list_typeof(i))list_entry_or_null(list_debug(h),		\
					    (i)->member.next,		\
					    list_off_var_((i), member)))

/**
 * list_prev - get the previous entry in a list
 * @h: the list_head
 * @i: a pointer to an entry in the list.
 * @member: the list_node member of the structure
 *
 * If @i was the first entry in the list, returns NULL.
 *
 * Example:
 *	first = list_prev(&parent->children, second, list);
 *	if (!first)
 *		printf("Can't go back to first child?!\n");
 */
#define list_prev(h, i, member)						\
	((list_typeof(i))list_entry_or_null(list_debug(h),		\
					    (i)->member.prev,		\
					    list_off_var_((i), member)))

/**
 * list_append_list - empty one list onto the end of another.
 * @to: the list to append into
 * @from: the list to empty.
 *
 * This takes the entire contents of @from and moves it to the end of
 * @to.  After this @from will be empty.
 *
 * Example:
 *	struct list_head adopter;
 *
 *	list_append_list(&adopter, &parent->children);
 *	assert(list_empty(&parent->children));
 *	parent->num_children = 0;
 */
void list_append_list(struct list_head *to,
				    struct list_head *from);

/**
 * list_prepend_list - empty one list into the start of another.
 * @to: the list to prepend into
 * @from: the list to empty.
 *
 * This takes the entire contents of @from and moves it to the start
 * of @to.  After this @from will be empty.
 *
 * Example:
 *	list_prepend_list(&adopter, &parent->children);
 *	assert(list_empty(&parent->children));
 *	parent->num_children = 0;
 */
void list_prepend_list(struct list_head *to,
				     struct list_head *from);

/**
 * list_for_each_off - iterate through a list of memory regions.
 * @h: the list_head
 * @i: the pointer to a memory region wich contains list node data.
 * @off: offset(relative to @i) at which list node data resides.
 *
 * This is a low-level wrapper to iterate @i over the entire list, used to
 * implement all oher, more high-level, for-each constructs. It's a for loop,
 * so you can break and continue as normal.
 *
 * WARNING! Being the low-level macro that it is, this wrapper doesn't know
 * nor care about the type of @i. The only assumtion made is that @i points
 * to a chunk of memory that at some @offset, relative to @i, contains a
 * properly filled `struct node_list' which in turn contains pointers to
 * memory chunks and it's turtles all the way down. Whith all that in mind
 * remember that given the wrong pointer/offset couple this macro will
 * happilly churn all you memory untill SEGFAULT stops it, in other words
 * caveat emptor.
 *
 * It is worth mentioning that one of legitimate use-cases for that wrapper
 * is operation on opaque types with known offset for `struct list_node'
 * member(preferably 0), because it allows you not to disclose the type of
 * @i.
 *
 * Example:
 *	list_for_each_off(&parent->children, child,
 *				offsetof(struct child, list))
 *		printf("Name: %s\n", child->name);
 */
#define list_for_each_off(h, i, off)                                    \
  for (i = list_node_to_off_(list_debug(h)->n.next, (off));             \
       list_node_from_off_((void *)i, (off)) != &(h)->n;                \
       i = list_node_to_off_(list_node_from_off_((void *)i, (off))->next, \
                             (off)))

/**
 * list_for_each_safe_off - iterate through a list of memory regions, maybe
 * during deletion
 * @h: the list_head
 * @i: the pointer to a memory region wich contains list node data.
 * @nxt: the structure containing the list_node
 * @off: offset(relative to @i) at which list node data resides.
 *
 * For details see `list_for_each_off' and `list_for_each_safe'
 * descriptions.
 *
 * Example:
 *	list_for_each_safe_off(&parent->children, child,
 *		next, offsetof(struct child, list))
 *		printf("Name: %s\n", child->name);
 */
#define list_for_each_safe_off(h, i, nxt, off)                          \
  for (i = list_node_to_off_(list_debug(h)->n.next, (off)),             \
         nxt = list_node_to_off_(list_node_from_off_(i, (off))->next,   \
                                 (off));                                \
       list_node_from_off_(i, (off)) != &(h)->n;                        \
       i = nxt,                                                         \
         nxt = list_node_to_off_(list_node_from_off_(i, (off))->next,   \
                                 (off)))


/* Other -off variants. */
#define list_entry_off(n, type, off)		\
	((type *)list_node_from_off_((n), (off)))

#define list_head_off(h, type, off)		\
	((type *)list_head_off((h), (off)))

#define list_tail_off(h, type, off)		\
	((type *)list_tail_((h), (off)))

#define list_add_off(h, n, off)                 \
	list_add((h), list_node_from_off_((n), (off)))

#define list_del_off(n, off)                    \
	list_del(list_node_from_off_((n), (off)))

#define list_del_from_off(h, n, off)			\
	list_del_from(h, list_node_from_off_((n), (off)))

/* Offset helper functions so we only single-evaluate. */
void *list_node_to_off_(struct list_node *node, size_t off);
struct list_node *list_node_from_off_(void *ptr, size_t off);

/* Get the offset of the member, but make sure it's a list_node. */
#define list_off_(type, member)					\
	(container_off(type, member) +				\
	 check_type(((type *)0)->member, struct list_node))

#define list_off_var_(var, member)			\
	(container_off_var(var, member) +		\
	 check_type(var->member, struct list_node))

#if HAVE_TYPEOF
#define list_typeof(var) typeof(var)
#else
#define list_typeof(var) void *
#endif

/* Returns member, or NULL if at end of list. */
void *list_entry_or_null(const struct list_head *h,
				       const struct list_node *n,
				       size_t off);
#endif /* CCAN_LIST_H */
