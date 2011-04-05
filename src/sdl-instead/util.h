#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

typedef int (*parser_fn)(const char *v, void *data);

struct parser {
	const char 	*cmd;
	parser_fn	fn; 
	void		*p;
	long		aux;
};

extern int is_space(int c);
extern int is_empty(const char *str);

extern int parse_ini(const char *path, struct parser *cmd_parser);
extern char *getpath(const char *d, const char *n);
extern char *strip(char *s);
char *getfilepath(const char *d, const char *n);

extern char *lookup_tag(const char *fname, const char *tag, const char *comm);
extern char *lookup_lang_tag(const char *fname, const char *tag, const char *comm);
extern int parse_esc_string(const char *v, void *data);
extern int parse_string(const char *v, void *data);
extern int parse_int(const char *v, void *data);
extern int parse_float(const char *v, void *data);

extern int parse_full_path(const char *v, void *data);
extern int process_cmd(char *n, char *v, struct parser *cmd_parser);

extern char *encode_esc_string(const char *v);
extern char *find_in_esc(const char *l, const char *s);
#ifdef _HAVE_ICONV
extern char *decode(iconv_t hiconv, const char *s);
#endif

extern void unix_path(char *path);
extern char *sdl_path(char *path);
extern int setdir(const char *path);
extern char *getdir(char *path, size_t size);
extern char *dirpath(const char *path);

extern char *parse_tag(char *line, const char *tag, const char *comm, int *brk);
extern int remove_dir(const char *path);

extern void tolow(char *p);
extern int strlowcmp(const char *s, const char *d);

#ifndef PATH_MAX
#define PATH_MAX 	4096
#endif

#define FREE(v) do { if ((v)) free((v)); v = NULL; } while(0)

#endif
