#ifndef __INSTEAD_H__
#define __INSTEAD_H__

extern int  instead_init(void);
extern int  instead_load(char *game);
extern void instead_done(void);
extern char *instead_cmd(char *s);
extern int  instead_eval(char *s);
extern int  instead_clear(void);
extern char *instead_retval(int n);
extern int  instead_bretval(int n);
extern int  instead_iretval(int n);
extern int	instead_lang(void);
char 		*fromgame(const char *s);
extern int  instead_encode(const char *s, const char *d);
#endif
