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

#include "externals.h"
#include "config.h"
#include "util.h"
#include "idf.h"

int get_utf8(const char *sp, unsigned long *sym_out)
{
	int i = 0, l = 0;
	unsigned long sym = 0;
	const unsigned char *p = (const unsigned char*)sp;

	if (sym_out)
		*sym_out = *p;

	if (!*p)
		return 0;

	if (!(*p & 0xc0))
		return 1;

	if ((*p & 0xe0) == 0xc0) {
		l = 1;
		sym = (*p & 0x1f);
	} else if ((*p & 0xf0) == 0xe0) {
		l = 2;
		sym = (*p & 0xf);
	} else if ((*p & 0xf8) == 0xf0) {
		l = 3;
		sym = (*p & 7);
	} else if ((*p & 0xfc) == 0xf8) {
		l = 4;
		sym = (*p & 3);
	} else if ((*p & 0xfe) == 0xfc) {
		l = 5;
		sym = (*p & 1);
	} else {
		return 1;
	}
	p ++;
	for (i = 0; i < l; i ++) {
		sym <<= 6;
		if ((*p & 0xc0) != 0x80) {
			return 1;
		}
		sym |= (*p++ & 0x3f);
	}
	if (sym_out)
		*sym_out = sym;
	return l + 1;
}

int is_cjk(unsigned long sym)
{
	if (sym >=0x2E80 && sym <= 0x2EFF)
		return 1;
	if (sym >=0x2F00 && sym <= 0x2FDF)
		return 1;
	if (sym >= 0x2FF0 && sym <= 0x2FFF)
		return 1;
	if (sym >= 0x3000 && sym <= 0x303F)
		return 1;
	if (sym >= 0x3040 && sym <= 0x309F)
		return 1;
	if (sym >= 0x30A0 && sym <=0x30FF)
		return 1;
	if (sym >= 0x3100 && sym <=0x312F)
		return 1;
	if (sym >= 0x3130 && sym <= 0x318F)
		return 1;
	if (sym >= 0x3190 && sym <= 0x319F)
		return 1;
	if (sym >= 0x31A0 && sym <= 0x31BF)
		return 1;
	if (sym >= 0x31F0 && sym <= 0x31FF)
		return 1;
	if (sym >= 0x3200 && sym <= 0x32FF)
		return 1;
	if (sym >= 0x3300 && sym <= 0x33FF)
		return 1;
	if (sym >= 0x3400 && sym <= 0x4DBF)
		return 1;
	if (sym >= 0x4DC0 && sym <= 0x4DFF)
		return 1;
	if (sym >= 0x4E00 && sym <= 0x9FFF)
		return 1;
	if (sym >= 0xA000 && sym <= 0xA48F)
		return 1;
	if (sym >= 0xA490 && sym <= 0xA4CF)
		return 1;
	if (sym >= 0xAC00 && sym <= 0xD7AF)
		return 1;
	if (sym >= 0xF900 && sym <= 0xFAFF)
		return 1;
	if (sym >= 0xFE30 && sym <= 0xFE4F)
		return 1;
	if (sym >= 0x1D300 && sym <= 0x1D35F)
		return 1;
	if (sym >= 0x20000 && sym <= 0x2A6DF)
		return 1;
	if (sym >= 0x2F800 && sym <= 0x2FA1F)
		return 1;
	return 0;
}

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
	int i = ((d)?strlen(d):0) + ((n)?strlen(n):0) + 3;
	char *p = malloc(i);
	if (p) {
		p[0] = 0;
		if (d) {
			strcpy(p, d);
			strcat(p, "/");
		}
		if (n)
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

static int fgetsesc(char *oline, size_t size, char *(*getl)(void *p, char *s, int size), void *fp)
{
	int nr = 0;
	char line[4096];
	size_t len;
	*oline = 0;
	*line = 0;
	while (getl(fp, line, sizeof(line))) {
		int i;
		nr ++;
		i = strcspn(line, "\n\r");
		if (!i || !line[i])
			break;
		line[i] = 0;
		if (line[i - 1] == '\\') {
			line[i - 1] = 0;
			strncat(oline, line, size);
			len = strlen(line);
			if (len >= size)
				return nr;
			size -= len;
			line[0] = 0;
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

int parse_all(void *fp, char *(*getl)(void *p, char *s, int size), const char *path, struct parser *cmd_parser)
{
	int nr;
	int rc = 0;
	int line_nr = 1;

	char line[4096];
	if (!fp)
		return -1;

	while ((nr = fgetsesc(line, sizeof(line), getl, fp))) {
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
/*		printf("%s\n", p); */
		val += strspn(val, " \t");
		comments_zap(val);
/*		val[strcspn(val, ";\n")] = 0; */
		if (process_cmd(p, val, cmd_parser)) {
			rc = -1;
			fprintf(stderr, "Can't process cmd '%s' on line %d in '%s': %s\n", p, line_nr - nr, path, strerror(errno));
		}
	}
	return rc;
}

static char *file_gets(void *fd, char *s, int size)
{
	return fgets(s, size, (FILE *)fd);
}

static char *idff_gets(void *fd, char *s, int size)
{
	return idf_gets((idff_t)fd, s, size);
}

int parse_ini(const char *path, struct parser *cmd_parser)
{
	int rc = 0;
	FILE *fp;
	fp = fopen(path, "rb");
	if (!fp)
		return -1;
	rc = parse_all(fp, file_gets, path, cmd_parser);
	fclose(fp);
	return rc;
}

int parse_idff(idff_t idff, const char *path, struct parser *cmd_parser)
{
	if (!idff)
		return -1;
	return parse_all(idff, idff_gets, path, cmd_parser);
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

	if (theme_relative || 
		!strncmp(v, "blank:", 6) || 
		!strncmp(v, "box:", 4) ||
		!strncmp(v, "spr:", 4)) /* hack for special files*/
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

static char *lookup_tag_all(const char *tag, const char *comm, char *(*getl)(void *p, char *s, int size), void *fp)
{
	int brk = 0;
	char *l; char line[1024];
	while ((l = getl(fp, line, sizeof(line))) && !brk) {
		l = parse_tag(l, tag, comm, &brk);
		if (l)
			return l;
	}
	return NULL;

}

char *lookup_tag(const char *fname, const char *tag, const char *comm)
{
	char *l;
	FILE *fd = fopen(fname, "rb");
	if (!fd)
		return NULL;
	l = lookup_tag_all(tag, comm, file_gets, fd);
	fclose(fd);
	return l;
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

char *lookup_lang_tag_idf(idff_t idf, const char *tag, const char *comm)
{
	char lang_tag[1024];
	char *l;
	if (!idf)
		return NULL;
	snprintf(lang_tag, sizeof(lang_tag), "%s(%s)", tag, opt_lang);
	l = lookup_tag_all(lang_tag, comm, idff_gets, idf);
	if (!l) {
		idf_seek(idf, 0, SEEK_SET);
		l = lookup_tag_all(tag, comm, idff_gets, idf);
	}
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

/*
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
 *
 */

/*
 * char *realpath(const char *path, char *resolved);
 *
 * Find the real name of path, by removing all ".", ".." and symlink
 * components.  Returns (resolved) on success, or (NULL) on failure,
 * in which case the path which caused trouble is left in (resolved).
 */

char *getrealpath(const char *path, char *resolved)
{
	const char *q;
	char *p, *fres;
	size_t len;
#if defined(unix) && !defined(S60)
	struct stat sb;
	ssize_t n;
	int idx = 0, nlnk = 0;
	char wbuf[2][PATH_MAX];
#endif
	/* POSIX sez we must test for this */
	if (path == NULL) {
		return NULL;
	}

	if (resolved == NULL) {
		fres = resolved = malloc(PATH_MAX);
		if (resolved == NULL)
			return NULL;
	} else
		fres = NULL;


	/*
	 * Build real path one by one with paying an attention to .,
	 * .. and symbolic link.
	 */

	/*
	 * `p' is where we'll put a new component with prepending
	 * a delimiter.
	 */
	p = resolved;

	if (*path == '\0') {
		*p = '\0';
		goto out;
	}

	/* If relative path, start from current working directory. */
	if (!is_absolute_path(path)) {
		/* check for resolved pointer to appease coverity */
		if (resolved && getdir(resolved, PATH_MAX) == NULL) {
			p[0] = '.';
			p[1] = '\0';
			goto out;
		}
		unix_path(resolved);
		len = strlen(resolved);
		if (len > 1)
			p += len;
	}

loop:
	/* Skip any slash. */
	while (*path == '/')
		path++;

	if (*path == '\0') {
		if (p == resolved)
			*p++ = '/';
		*p = '\0';
		return resolved;
	}

	/* Find the end of this component. */
	q = path;
	do
		q++;
	while (*q != '/' && *q != '\0');

	/* Test . or .. */
	if (path[0] == '.') {
		if (q - path == 1) {
			path = q;
			goto loop;
		}
		if (path[1] == '.' && q - path == 2) {
			/* Trim the last component. */
			if (p != resolved)
				while (*--p != '/' && p != resolved)
					continue;
			path = q;
			goto loop;
		}
	}

	/* Append this component. */
	if (p - resolved + 1 + q - path + 1 > PATH_MAX) {
		if (p == resolved)
			*p++ = '/';
		*p = '\0';
		goto out;
	}
	if (p == resolved 
		&& is_absolute_path(path) 
			&& path[0] != '/') { /* win? */
		memcpy(&p[0], path,
		    q - path);
		p[q - path] = '\0';
		p += q - path;
		path = q;
		goto loop;
	} else {
		p[0] = '/';
		memcpy(&p[1], path,
		    /* LINTED We know q > path. */
		    q - path);
		p[1 + q - path] = '\0';
	}
#if defined(unix) && !defined(S60)
	/*
	 * If this component is a symlink, toss it and prepend link
	 * target to unresolved path.
	 */
	if (lstat(resolved, &sb) != -1 && S_ISLNK(sb.st_mode)) {
		if (nlnk++ >= 16) {
			goto out;
		}
		n = readlink(resolved, wbuf[idx], sizeof(wbuf[0]) - 1);
		if (n < 0)
			goto out;
		if (n == 0) {
			goto out;
		}

		/* Append unresolved path to link target and switch to it. */
		if (n + (len = strlen(q)) + 1 > sizeof(wbuf[0])) {
			goto out;
		}
		memcpy(&wbuf[idx][n], q, len + 1);
		path = wbuf[idx];
		idx ^= 1;

		/* If absolute symlink, start from root. */
		if (*path == '/')
			p = resolved;
		goto loop;
	}
#endif
	/* Advance both resolved and unresolved path. */
	p += 1 + q - path;
	path = q;
	goto loop;
out:
	free(fres);
	return NULL;
}

static tinymt32_t trandom;

void mt_random_init(void) 
{
	tinymt32_init(&trandom, time(NULL));
}

void mt_random_seed(unsigned long seed) 
{
	tinymt32_init(&trandom, seed);
}

unsigned long mt_random(void) 
{
	return tinymt32_generate_uint32(&trandom);
}

double mt_random_double(void)
{
	return tinymt32_generate_32double(&trandom);
}
