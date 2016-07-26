/*
 * Copyright 2009-2016 Peter Kosyh <p.kosyh at gmail.com>
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
#include "util.h"

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

int parse_path(const char *v, void *data)
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

int parse_full_path(const char *v, void *data)
{
	char cwd[PATH_MAX];
	char **p = ((char **)data);

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

char *lookup_lang_tag(const char *fname, const char *tag, const char *comm, const char *opt_lang)
{
	char lang_tag[1024];
	char *l;
	snprintf(lang_tag, sizeof(lang_tag), "%s(%s)", tag, opt_lang);
	l = lookup_tag(fname, lang_tag, comm);
	if (!l) 
		l = lookup_tag(fname, tag, comm);
	return l;
}

char *lookup_lang_tag_idf(idf_t idf, const char *fname, const char *tag, const char *comm, const char *opt_lang)
{
	char lang_tag[1024];
	char *l;
	idff_t idff;
	if (!idf)
		return NULL;
	idff = idf_open(idf, fname);
	if (!idff)
		return NULL;
	snprintf(lang_tag, sizeof(lang_tag), "%s(%s)", tag, opt_lang);
	l = lookup_tag_all(lang_tag, comm, idff_gets, idff);
	if (!l) {
		idf_seek(idff, 0, SEEK_SET);
		l = lookup_tag_all(tag, comm, idff_gets, idff);
	}
	idf_close(idff);
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
	l = ns + strlen(ns);
	while (l != ns) {
		l --;
		if (*l != '\r' && *l != '\n')
			break;
		*l = 0;
	}
	return ns;
}

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

void data2hex(const void *d, int len, void *o)
{
	unsigned char *data = (unsigned char *)d;
	unsigned char *out = (unsigned char *)o;

	static char map[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
		'a', 'b', 'c', 'd', 'e', 'f' };

	while (len --) {
		unsigned char a = *data ++;
		*out ++ = map[a & 0xf];
		*out ++ = map[a >> 4];
	}
}

int hex2data(const void *d, void *o, int len)
{
	unsigned char *data = (unsigned char *)d;
	unsigned char *out = (unsigned char *)o;
	unsigned char b = 0;
	int rc = 0;
	len *= 2;

	while (len --) {
		unsigned char c = *data ++;
		if ((c < '0' || c > '9') && (c < 'a' || c > 'f'))
			break;
		if (c >= 'a')
			c -= ('a' - 10);
		else
			c -= '0';
		if (len & 1)
			b = c;
		else
			*out ++ = b + (c << 4);
		rc ++;
	}
	return rc;
}

void trunc_lines(char *pp, int max)
{
	int n = 0;

	if (!pp)
		return;

	if (max == 0) {
		pp[strcspn(pp, "\n\r")] = 0;
		return;
	}

	while (pp[strcspn(pp, "\n\r")]) {
		n ++;
		pp += strcspn(pp, "\n\r");
		if (n >= max) {
			*pp = 0;
			break;
		}
		pp ++;
	}
}
