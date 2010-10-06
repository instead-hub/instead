#ifndef __CACHE_H
#define __CACHE_H

typedef void * cache_t;
typedef void (*cache_free_fn)(void *p);

extern cache_t cache_init(int size, cache_free_fn);
extern void cache_free(cache_t cache);
extern int  cache_forget(cache_t cache, void *p);
extern void cache_zap(cache_t cache);
extern void cache_shrink(cache_t cache);
extern void *cache_get(cache_t cache, const char *name);
extern int cache_add(cache_t cache, const char *name, void *p);
extern int cache_have(cache_t cache, void *p);

#endif
