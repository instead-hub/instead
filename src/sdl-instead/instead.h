#ifndef __INSTEAD_H__
#define __INSTEAD_H__

#define INSTEAD_NIL  0
#define INSTEAD_NUM  1
#define INSTEAD_STR  2
#define INSTEAD_BOOL 3

struct instead_args {
	int type;
	const char *val;
};

extern int  instead_init(const char *path);
extern int  instead_load(char *game);
extern void instead_done(void);
extern char *instead_cmd(char *s);
extern int  instead_function(char *s, struct instead_args *args);
extern int  instead_eval(char *s);
extern int  instead_clear(void);
extern char *instead_retval(int n);
extern int  instead_bretval(int n);
extern int  instead_iretval(int n);
extern int	instead_lang(void);
char 		*fromgame(const char *s);
extern int  instead_encode(const char *s, const char *d);
extern int  instead_busy(void);

/* internal use */
extern int  instead_init_lua(const char *path);
extern int  instead_loadscript(char *name, int argc, char **argv, int exec);

#endif
