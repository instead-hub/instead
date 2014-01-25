/*
 * Copyright 2009-2014 Peter Kosyh <p.kosyh at gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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
extern void *cache_lookup(cache_t cache, const char *name);
extern int cache_add(cache_t cache, const char *name, void *p);
extern int cache_have(cache_t cache, void *p);
extern unsigned long hash_addr(void *p);
extern unsigned long hash_string(const char *str);

#endif
