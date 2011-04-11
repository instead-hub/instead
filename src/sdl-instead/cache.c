#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "cache.h"
extern unsigned long hash_string(const char *str);

#define HASH_SIZE 511

typedef struct {
	struct list_head list;
	struct list_head unused;
	int auto_grow;
	int size;
	int max_size;
	int used;
	cache_free_fn free_fn;
	struct list_head hash[HASH_SIZE];
	struct list_head vhash[HASH_SIZE];
} __cache_t;

typedef struct {
	struct list_head list;
	struct list_head *hash;
	struct list_head *vhash;
	char *name;
	void *data;
	int used;
} __cache_e_t;

typedef struct {
	struct list_head list;
	struct list_head *data;
} __hash_item_t;

/* #define GOLDEN_RATIO_PRIME_32 0x9e370001UL */
#include <stdio.h>

static unsigned long hash_long32(unsigned long a)
{
	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	a = (a+0x165667b1) + (a<<5);
	a = (a+0xd3a2646c) ^ (a<<9);
	a = (a+0xfd7046c5) + (a<<3);
	a = (a^0xb55a4f09) ^ (a>>16);
	return a;
}

static unsigned long hash_long64(unsigned long key)
{
	key = (~key) + (key << 21); // key = (key << 21) - key - 1;
	key = key ^ (key >> 24);
	key = (key + (key << 3)) + (key << 8); // key * 265
	key = key ^ (key >> 14);
	key = (key + (key << 2)) + (key << 4); // key * 21
	key = key ^ (key >> 28);
	key = key + (key << 31);
	return key;
}

unsigned long hash_addr(void *p)
{
	if (sizeof(long) == 8)
		return hash_long64((unsigned long)p);
	return hash_long32((unsigned long)p);
}

unsigned long hash_string(const char *str)
{
	unsigned long hash = 0;
	int c;
	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

cache_t cache_init(int size, cache_free_fn free_fn)
{
	int i = 0;
	__cache_t *c = malloc(sizeof(__cache_t));
	if (!c)
		return NULL;
	INIT_LIST_HEAD(&c->list);
	INIT_LIST_HEAD(&c->unused);
	c->auto_grow = 0;
	c->size = 0;
	c->used = 0;
	if (!size)
		c->auto_grow = 1;
	c->max_size = size;
	c->free_fn = free_fn;
	for (i = 0; i < HASH_SIZE; i++) {
			INIT_LIST_HEAD(&c->hash[i]);
			INIT_LIST_HEAD(&c->vhash[i]);
	}
	return (cache_t)c;
}

static void cache_e_free(cache_t cache, __cache_e_t *cc)
{
	__cache_t *c = cache;
	if (!c)
		return;

	list_del(cc->hash);
	list_del(cc->vhash);
	list_del(&cc->list);

	if (c->free_fn && !cc->used) {
		c->free_fn(cc->data);
	}
	free(cc->name);
	free(cc->hash);
	free(cc->vhash);
	free(cc);
}

void cache_zap(cache_t cache)
{
	__cache_t *c = cache;
	if (!c)
		return;
	while (!list_empty(&c->list))
		cache_e_free(cache, (__cache_e_t *)(c->list.next));
	while (!list_empty(&c->unused))
		cache_e_free(cache, (__cache_e_t *)(c->unused.next));
	c->size = 0;
	c->used = 0;
}

void cache_free(cache_t cache)
{
	if (!cache)
		return;
	cache_zap(cache);
	free(cache);
}

static __cache_e_t *_cache_lookup(cache_t cache, const char *name)
{
	struct list_head *pos;
	struct list_head *list;
	__cache_t *c = cache;
	__cache_e_t *cc;
	if (!c || !name)
		return NULL;	
	list = &c->hash[hash_string(name) % HASH_SIZE];
	list_for_each(pos, list) {
		cc = (__cache_e_t*)((__hash_item_t*)pos)->data;
		if (!strcmp(cc->name, name))
			return cc;
	}
	return NULL;
}

static __cache_e_t *cache_data(cache_t cache, void *p)
{
	struct list_head *pos;
	struct list_head *list;

	__cache_t *c = cache;
	__cache_e_t *cc;
	if (!c || !p)
		return NULL;
	list = &c->vhash[hash_addr(p) % HASH_SIZE];
	list_for_each(pos, list) {
		cc = (__cache_e_t*)((__hash_item_t*)pos)->data;
		if (p == cc->data)
			return cc;
	}
	return NULL;
}

int cache_forget(cache_t cache, void *p)
{
	__cache_t *c = cache;
	__cache_e_t *cc = cache_data(cache, p);
//	if (cc)
//		fprintf(stderr, "Forget %p at %d\n", p, cc->used);
	if (cc && cc->used) {
		cc->used --;
		if (!cc->used) {
			((__cache_t*)cache)->used --;
			list_move_tail(&cc->list, &c->unused);
		}
		return 0;
	}
	return -1;
}

void *cache_get(cache_t cache, const char *name)
{
	__cache_e_t *cc;
	__cache_t *c = cache;
	if (!c || !name)
		return NULL;
	cc = _cache_lookup(cache, name);
	if (!cc)
		return NULL;
	cc->used ++; /* need again! */
	if (cc->used == 1)
		((__cache_t*)cache)->used ++;
	list_move(&cc->list, &c->list); /* first place */
//	printf("cache_get %s:%p %d\n", name, cc->data, cc->used);
	return cc->data;
}

void *cache_lookup(cache_t cache, const char *name)
{
	__cache_e_t *cc;
	__cache_t *c = cache;
	if (!c || !name)
		return NULL;
	cc = _cache_lookup(cache, name);
	if (!cc)
		return NULL;
	return cc->data;
}

int cache_have(cache_t cache, void *p)
{
	__cache_e_t *cc;
	__cache_t *c = cache;
	if (!c || !p)
		return -1;
	cc = cache_data(cache, p);
	if (!cc)
		return -1;
	return 0;
}

static void __cache_shrink(__cache_t *c)
{
	while (c->size && c->size > c->max_size && !list_empty(&c->unused)) {
		__cache_e_t *cc = (__cache_e_t *)(c->unused.next);
		c->size --;
		cache_e_free(c, cc);
	}
//	fprintf(stderr,"%d/%d\n", c->size, c->used);
}

int cache_add(cache_t cache, const char *name, void *p)
{
	__cache_e_t *cc;
	__hash_item_t *hh;
	__hash_item_t *vh;
	__cache_t *c = cache;
	struct list_head *list;
	if (!c || !name)
		return -1;
	cc = _cache_lookup(cache, name);
	if (cc)
		return 0;
	cc = malloc(sizeof(__cache_e_t));
	if (!cc)
		return -1;
	hh = malloc(sizeof(__hash_item_t));
	if (!hh) {
		free(cc);
		return -1;
	}

	vh = malloc(sizeof(__hash_item_t));
	if (!vh) {
		free(hh);
		free(cc);
		return -1;
	}

	cc->name = strdup(name);
	if (!cc->name) {
		free(vh);
		free(hh);
		free(cc);
		return -1;
	}
	cc->data = p;
	cc->used = 1;
	cc->hash = (struct list_head*) hh;
	cc->vhash = (struct list_head*) vh;

	list_add((struct list_head*)cc, &c->list);
	list = &c->hash[hash_string(name) % HASH_SIZE];
	hh->data = (struct list_head*)cc;
	list_add((struct list_head*)hh, list);

	list = &c->vhash[hash_addr(p) % HASH_SIZE];
	vh->data = (struct list_head*)cc;
	list_add((struct list_head*)vh, list);

	c->size ++;
	c->used ++;
	if (c->auto_grow && c->used > c->max_size)
		c->max_size = c->used;
//	printf("cache_add %s:%p %d\n", name, cc->data, cc->used);
//	__cache_shrink(c);
	return 0;
}

void cache_shrink(cache_t cache)
{
	__cache_t *c = cache;
	if (c->auto_grow && c->max_size > 2*c->used)
		c->max_size = c->used + c->used / 2;
	__cache_shrink(c);
//	printf("size: %d:%d:%d\n", c->used, c->size, c->max_size);
}
