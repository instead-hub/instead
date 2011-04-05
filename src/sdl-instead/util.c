#include "externals.h"
#include "config.h"
#include "util.h"

void tolow(char *p)
{
	while (*p) {
		if (*p >=  'A' && *p <= 'Z')
			*p |= 0x20;
		p ++;
	}
}

int strlowcmp(const char *s, const char *d)
{
	int rc;
	char *ss = NULL;
	char *dd = NULL;
	ss = strdup(s);
	dd = strdup(d);
	if (!ss || !dd) {
		rc = strcmp(s, d);
		goto err;
	}
	tolow(ss);
	tolow(dd);
	rc = strcmp(ss, dd);
err:
	if (ss)
		free(ss);
	if (dd)
		free(dd);
	return rc; 
}

char *getfilepath(const char *d, const char *n)
{
	int i = strlen(d) + strlen(n) + 3;
	char *p = malloc(i);
	if (p) {
		strcpy(p, d);
		strcat(p, "/");
		strcat(p, n);
		unix_path(p);
	}
	return p;
}

char *getpath(const char *d, const char *n)
{
	char *p = getfilepath(d, n);
	strcat(p, "/");
	return p;
}

int is_space(int c)
{
	return (c == ' ' || c == '\t');
}

int is_empty(const char *str)
{
	if (!str || !*str)
		return 1;
	while (*str && !is_space(*str++))
		return 0;
	return 1;
}

char *strip(char *s)
{
	char *e;
	while (is_space(*s))
		s ++;
	if (!*s)
		return s;
	e = s + strlen(s) - 1;
	while (e != s && is_space(*e)) {
		*e = 0;
		e --;
	}
	return s;
}

int process_cmd(char *n, char *v, struct parser *cmd_parser)
{
	int i;
	n = strip(n);
	v = strip(v);
	for (i = 0; cmd_parser[i].cmd; i++) {
		if (!strcmp(cmd_parser[i].cmd, n)) {
			return cmd_parser[i].fn(v, cmd_parser[i].p);
		}
	}
	return -1;
}

static int fgetsesc(char *oline, size_t size, FILE *fp)
{
	int nr = 0;
	char line[4096];
	*oline = 0;
	*line = 0;
	while (fgets(line, sizeof(line), fp)) {
		int i;
		nr ++;
		i = strcspn(line, "\n\r");
		if (!i || !line[i])
			break;
		line[i] = 0;
		if (line[i - 1] == '\\') {
			line[i - 1] = 0;
			strncat(oline, line, size);
			line[0] = 0;
			size -= strlen(line);
			if (size <= 0)
				return nr;
		} else {
			break;
		}
	}
	strncat(oline, line, size);
	return nr;
}

char *find_in_esc(const char *l, const char *s)
{
	int esc = 0;
	for (; *l; l++) {
		if (esc) {
			esc = 0;
			continue;
		}
		l += strcspn(l, s);
		if (*l == '\\') {
			esc = 1;
			continue;
		}
		if (!esc)
			return (char*)l;
	}
	return NULL;
}

static void comments_zap(char *p)
{
	char *l = find_in_esc(p, "\\;\n");
	if (l)
		*l = 0;
}

int parse_ini(const char *path, struct parser *cmd_parser)
{
	int nr;
	int rc = 0;
	int line_nr = 1;
	FILE *fp;
	char line[4096];
	fp = fopen(path, "rb");
	if (!fp)
		return -1;
	while ((nr = fgetsesc(line, sizeof(line), fp))) {
		char *p = line;
		char *val;
		int len;
		line_nr += nr;
		p += strspn(p, " \t");
		if (*p == ';')
			continue;
		len = strcspn(p, "=");
		if (p[len] != '=') /* just ignore it */
			continue;
		p[len] = 0;
		val = p + len + 1;
		len = strcspn(p, " \t");
		p[len] = 0;
//		printf("%s\n", p);
		val += strspn(val, " \t");
		comments_zap(val);
//		val[strcspn(val, ";\n")] = 0;
		if (process_cmd(p, val, cmd_parser)) {
			rc = -1;
			fprintf(stderr, "Can't process cmd '%s' on line %d in '%s': %s\n", p, line_nr - nr, path, strerror(errno));
		}
	}
	fclose(fp);
	return rc;
}

int parse_string(const char *v, void *data)
{
	char **p = ((char **)data);
	if (*p)
		free(*p);
	*p = strdup(v);
	if (!*p)
		return -1;
	return 0;	
}

char *encode_esc_string(const char *v)
{
	char *r, *p;
	if (!v)
		return NULL;
	p = r = malloc((strlen(v)*2) + 1);
	if (!r)
		return NULL;
	while (*v) {
		switch (*v) {
		case ' ':
			*p ++ = '\\';
			*p ++ = ' ';
			break;
		case '"':
			*p ++ = '\\';
			*p ++ = '"';
			break;
		case '\'':
			*p ++ = '\\';
			*p ++ = '\'';
			break;
		case '\\':
			*p ++ = '\\';
			*p ++ = '\\';
			break;
		case '\n':
			*p ++ ='\\';
			*p ++ ='\n';
			break;
		default:
			*p ++ = *v;
		}
		v ++;
	}
	*p ++ = 0;
	return r;
}

int parse_esc_string(const char *v, void *data)
{
	int esc = 0;
	char *ptr;
	char **p = ((char **)data);
	if (*p)
		free(*p);
	*p = strdup(v);
	if (!*p)
		return -1;
	for (ptr = *p; *v; v ++) {
		if (esc) {
			switch (*v) {
			case 'n':
				*ptr = '\n';
				break;
			case '$':
				*ptr = '$';
				break;	
			case '\\':
				*ptr = '\\';
				break;
			case ';':
				*ptr = ';';
				break;
			case 'r':
				*ptr = '\n';
				break;
			default:
				*ptr = *v;
				break;	
			}
			esc = 0;
			ptr ++;
			continue;
		} else if (*v != '\\') {
			*ptr = *v;
			ptr ++;
			continue;
		} else
			esc = 1;
	}
	*ptr = 0;
	return 0;	
}

int parse_int(const char *v, void *data)
{
	int *i = (int *)data;
	char *eptr = NULL;
	*i = strtol(v, &eptr, 0);
	if (!eptr || *eptr)
		return -1;
	return 0;	
}

int parse_float(const char *v, void *data)
{
	float *f = (float *)data;
	if (sscanf(v, "%f", f) != 1)
		return -1;
	return 0;	
}

static int parse_path(const char *v, void *data)
{
	char **p = ((char **)data);
	if (*p)
		free(*p);
	if (!v[0]) {
		*p = strdup("");
		return (*p)?0:-1;
	}
	*p = strdup(v);
	if (!*p)
		return -1;
	*p = sdl_path(*p);
	return 0;
}
extern int theme_relative; /* hack, theme layer here :( */
int parse_full_path(const char *v, void *data)
{
	char cwd[PATH_MAX];
	char **p = ((char **)data);

	if (theme_relative || !strncmp(v, "blank:", 6) || !strncmp(v, "box:", 4)) /* hack for special files*/
		return parse_path(v, data);

	if (*p)
		free(*p);
	if (!v[0]) {
		*p = strdup("");
		return (*p)?0:-1;
	}
	getdir(cwd, sizeof(cwd));
	*p = malloc(strlen(v) + strlen(cwd) + 2);
	if (!*p)
		return -1;
	strcpy(*p, cwd);
	strcat(*p,"/");
	strcat(*p, v);
	*p = sdl_path(*p);
	return 0;
}

void unix_path(char *path)
{
	char *p = path;
	if (!path)
		return;
	while (*p) { /* bad bad Windows!!! */
		if (*p == '\\')
			*p = '/';
		p ++;
	}
	return;
}

char *lookup_tag(const char *fname, const char *tag, const char *comm)
{
	int brk = 0;
	char *l; char line[1024];
	FILE *fd = fopen(fname, "rb");
	if (!fd)
		return NULL;

	while ((l = fgets(line, sizeof(line), fd)) && !brk) {
		l = parse_tag(l, tag, comm, &brk);
		if (l) {
			fclose(fd);
			return l;
		}
	}
	fclose(fd);
	return NULL;
}

char *lookup_lang_tag(const char *fname, const char *tag, const char *comm)
{
	char lang_tag[1024];
	char *l;
	snprintf(lang_tag, sizeof(lang_tag), "%s(%s)", tag, opt_lang);
	l = lookup_tag(fname, lang_tag, comm);
	if (!l) 
		l = lookup_tag(fname, tag, comm);
	return l;
}

char *parse_tag(char *line, const char *tag, const char *comm, int *brk)
{
	char *l = line;
	char *ns = NULL;
	char ftag[1024];
	snprintf(ftag, sizeof(ftag), "$%s:", tag);
	l += strspn(l, " \t");
	if (strncmp(l, comm, strlen(comm))) { /* non coment block */
		*brk = 1;
		return NULL;
	}
	l += strlen(comm); l += strspn(l, " \t");
	if (strncmp(l, ftag, strlen(ftag)))
		return NULL;
	l += strlen(ftag);
	l += strspn(l, " \t");
	ns = l;
	l = find_in_esc(l, "\\$");
	if (l)
		*l = 0;
	l = ns; ns = NULL;
	if (parse_esc_string(l, &ns))
		return NULL;
	ns[strcspn(ns, "\n\r")] = 0;
	return ns;
}
#ifdef _HAVE_ICONV
#define CHAR_MAX_LEN 4
char *decode(iconv_t hiconv, const char *s)
{
	size_t s_size, chs_size, outsz, insz;
	char *inbuf, *outbuf, *chs_buf;
	if (!s || hiconv == (iconv_t)(-1))
		return NULL;
	s_size = strlen(s) + 1; 
	chs_size = s_size * CHAR_MAX_LEN; 
	if ((chs_buf = malloc(chs_size + CHAR_MAX_LEN))==NULL)
		goto exitf; 
	outsz = chs_size; 
	outbuf = chs_buf; 
	insz = s_size; 
	inbuf = (char*)s; 
	while (insz) { 
		if (iconv(hiconv, &inbuf, &insz, &outbuf, &outsz) 
						== (size_t)(-1)) 
	   	 	goto exitf; 
	} 
	*outbuf++ = 0; 
	return chs_buf; 
exitf: 
	if(chs_buf) 
		free(chs_buf); 
	return NULL; 
}
#endif
int remove_dir(const char *path)
{
	DIR *d;
	struct dirent *de;
	if (!path)
		return 0;
	d = opendir(path);
	if (!d) {
		if (!access(path, F_OK)) {
			unlink(path);
		}
		return -1;
	}
	while ((de = readdir(d))) {
		char *p;
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		p = getfilepath(path, de->d_name);
		if (p) {
			remove_dir(p);
			free(p);
		}
	}
	closedir(d);
	rmdir(path);
	return 0;
}