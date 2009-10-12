#ifndef __INSTEAD_H__
#define __INSTEAD_H__

extern int  instead_init(void);
extern int  instead_load(char *game);
extern void instead_done(void);
extern char *instead_cmd(char *s);
extern char *instead_eval(char *s);
char 		*fromgame(const char *s);
extern int  instead_encode(const char *s, const char *d);
#endif
