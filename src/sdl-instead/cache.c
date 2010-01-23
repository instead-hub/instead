#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "cache.h"

typedef struct {
	struct list_head list;
	int size;
	int max_size;
	cache_free_fn free_fn;
} __cache_t;

typedef struct {
	struct list_head list;
	char *name;
	void *data;
	int free_it;
} __cache_e_t;

cache_t cache_init(int size, cache_free_fn free_fn)
{
	__cache_t *c = malloc(sizeof(__cache_t));
	if (!c)
		return NULL;
	INIT_LIST_HEAD(&c->list);
	c->size = 0;
	c->max_size = size;
	c->free_fn = free_fn;
	return (cache_t)c;
}

static void cache_e_free(cache_t cache, __cache_e_t *cc)
{
	__cache_t *c = cache;
	if (!c)
		return;
	free(cc->name);
	if (c->free_fn && cc->free_it)
		c->free_fn(cc->data);
	list_del((struct list_head*)cc);
	free(cc);
}

void cache_zap(cache_t cache)
{
	__cache_t *c = cache;
	if (!c)
		return;
	while (!list_empty(&c->list))
		cache_e_free(cache, (__cache_e_t *)(c->list.next));
	c->size = 0;
}

void cache_free(cache_t cache)
{
	if (!cache)
		return;
	cache_zap(cache);
	free(cache);
}

static __cache_e_t *cache_lookup(cache_t cache, const char *name)
{
	struct list_head *pos;
	__cache_t *c = cache;
	__cache_e_t *cc;
	if (!c || !name)
		return NULL;
	list_for_each(pos, &c->list) {
		cc = (__cache_e_t*) pos;
		if (!strcmp(cc->name, name))
			return cc;
	}
	return NULL;
}

static __cache_e_t *cache_data(cache_t cache, void *p)
{
	struct list_head *pos;
	__cache_t *c = cache;
	__cache_e_t *cc;
	if (!c || !p)
		return NULL;
	list_for_each(pos, &c->list) {
		cc = (__cache_e_t*) pos;
		if (p == cc->data)
			return cc;
	}
	return NULL;
}

void cache_forget(cache_t cache, void *p)
{
	__cache_e_t *cc = cache_data(cache, p);
	if (cc)
		cc->free_it = 1;
}

void *cache_get(cache_t cache, const char *name)
{
	__cache_e_t *cc;
	__cache_t *c = cache;
	if (!c || !name)
		return NULL;
	cc = cache_lookup(cache, name);
	if (!cc)
		return NULL;
	cc->free_it = 0; /* need again! */
	list_move((struct list_head*)cc, &c->list); /* first place */
//	printf("%p\n", cc->data);
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

int cache_add(cache_t cache, const char *name, void *p)
{
	__cache_e_t *cc;
	__cache_t *c = cache;
	if (!c || !name)
		return -1;
	cc = cache_lookup(cache, name);
	if (cc)
		return 0;
	cc = malloc(sizeof(__cache_e_t));
	if (!cc)
		return -1;
	cc->name = strdup(name);
	if (!cc->name) {
		free(cc);
		return -1;
	}
	cc->data = p;
	cc->free_it = 0;
	list_add((struct list_head*)cc, &c->list);
	c->size ++;
//	printf("size: %d:%s\n", c->size, name);
	if (c->size > c->max_size) {
		c->size --;
		cache_e_free(cache, (__cache_e_t *)c->list.prev);
	}
	return 0;
}

