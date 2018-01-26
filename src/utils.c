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
#include "utils.h"

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
			strncat(oline, line, size - 1);
			len = strlen(line);
			if (len >= size)
				return nr;
			size -= len;
			line[0] = 0;
		} else {
			break;
		}
	}
	strncat(oline, line, size - 1);
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
	/* *p = sdl_path(*p); note: do not convert relative paths to avoid double encoding */
	unix_path(*p);
	return 0;
}
#ifdef _WIN32
static char *wchar2utf(const wchar_t *wc)
{
	char *buf;
	int size = WideCharToMultiByte (CP_UTF8, 0, wc, -1, NULL, 0, NULL, NULL);
	if (!size)
		return NULL;
	buf = malloc(size);
	WideCharToMultiByte (CP_UTF8, 0, wc, -1, buf, size, NULL, NULL);
	return buf;
}

char *w32_getdir(char *path, size_t size)
{
	wchar_t *wp;
	char *p = NULL;
	path[0] = 0;
	wp = _getcwd(NULL, 0);
	if (!wp)
		return path;
	p = wchar2utf(wp);
	free(wp);
	if (!p)
		return path;
	snprintf(path, size, "%s", p);
	free(p);
	return path;
}
#endif
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
#ifdef _WIN32
	w32_getdir(cwd, sizeof(cwd));
#else
	getdir(cwd, sizeof(cwd));
#endif
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

#ifdef __EMSCRIPTEN__
void data_sync(void)
{
	EM_ASM(FS.syncfs(function(error) {
		if (error) {
			console.log("Error while syncing:", error);
		} else {
			console.log("Config synced");
		}
	}););
}
#else
void data_sync(void)
{
}
#endif
