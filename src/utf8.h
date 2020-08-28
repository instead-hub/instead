#include <stdint.h>

/* is c the start of a utf8 sequence? */
#define isutf(c) (((c)&0xC0)!=0x80)

/* convert UTF-8 data to wide character */
int u8_toucs(uint32_t *dest, int sz, const char *src, int srcsz);

/* returns length of next utf-8 sequence */
int u8_seqlen(const char *s);

/* return next character, updating an index variable */
u_int32_t u8_nextchar(const char *s, int *i);

/* count the number of characters in a UTF-8 string */
int u8_strlen(const char *s);
