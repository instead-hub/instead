#ifndef _LIST_H
#define _LIST_H
#include <stdlib.h>

#define LIST_POISON1 NULL
#define LIST_POISON2 NULL

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

extern void INIT_LIST_HEAD(struct list_head *list);
extern void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next);
extern void list_add(struct list_head *new, struct list_head *head);
extern void list_add_tail(struct list_head *new, struct list_head *head);
extern void __list_del(struct list_head * prev, struct list_head * next);
extern void list_del(struct list_head *entry);
extern void list_replace(struct list_head *old,
				struct list_head *new);
extern void list_replace_init(struct list_head *old,
					struct list_head *new);
extern void list_del_init(struct list_head *entry);
extern void list_move(struct list_head *list, struct list_head *head);
extern void list_move_tail(struct list_head *list,
				  struct list_head *head);
extern int list_is_last(const struct list_head *list,
				const struct list_head *head);
extern int list_empty(const struct list_head *head);
extern void __list_splice(const struct list_head *list,
				 struct list_head *prev,
				 struct list_head *next);
extern void list_splice(const struct list_head *list,
				struct list_head *head);
extern void list_splice_tail(struct list_head *list,
				struct list_head *head);
extern void list_splice_init(struct list_head *list,
				    struct list_head *head);
extern void list_splice_tail_init(struct list_head *list,
					 struct list_head *head);
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

#endif
